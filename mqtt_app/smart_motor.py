"""
Smart Motor MQTT Dashboard

Python/Tkinter UI to monitor and control the dsPIC33AK FOC motor drive
over MQTT, replicating the Node-RED Smart Motor dashboard.

Usage:
    python smart_motor.py [broker_ip]

    broker_ip defaults to 192.168.0.5 (the T1S network MQTT broker).
"""

import sys
import math
import time
import tkinter as tk
from tkinter import ttk
import paho.mqtt.client as mqtt

BROKER = sys.argv[1] if len(sys.argv) > 1 else "192.168.0.5"
PORT = 1883
TOPIC_PREFIX = "smart_motor"

BG = "#1a1a2e"
GROUP_BG = "#2a2a3e"
GROUP_OUTLINE = "#3a3a4e"
FG = "#ffffff"
DIM = "#888888"
BLUE = "#0d47a1"
GREEN = "#1b5e20"
GRAY = "#424242"
RED = "#d50000"

COLOR_GREEN = "#00c853"
COLOR_YELLOW = "#ffd600"
COLOR_RED = "#d50000"
COLOR_ORANGE = "#ff9800"
COLOR_GRAY = "#546e7a"

SPEED_PRESETS = [400, 900, 1000, 1100, 2000]
MAX_SPEED_DISPLAY = 3500
FAILURE_LABELS = ["Normal", "Unbalanced", "Unknown", "Wrong Speed"]
FAILURE_COLORS = [COLOR_GREEN, COLOR_YELLOW, COLOR_ORANGE, COLOR_RED]


class SmartMotorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Smart Motor Dashboard")
        self.root.configure(bg=BG)
        self.root.resizable(True, True)

        self.remote = False
        self.motor_on = False
        self.online = False
        self.last_torque_time = 0
        self.speed = 0.0
        self.torque = 0.0
        self.pot = 0.0
        self.failure = 0
        self.firmware = "—"
        self.mcu_uid = "—"
        self.max_speed = "—"
        self.selected_speed = 2000

        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.client.connect_async(BROKER, PORT)
        self.client.loop_start()

        self._build_ui()
        self._poll_online()

    def _on_connect(self, client, userdata, flags, rc, properties=None):
        subs = ["speed", "torque", "pot", "on", "firmware",
                "mcu_uid", "failure", "max_speed", "switch",
                "vcc", "board_name", "aiml_status"]
        for s in subs:
            client.subscribe(f"{TOPIC_PREFIX}/{s}")

    def _on_message(self, client, userdata, msg):
        topic = msg.topic.split("/")[-1]
        payload = msg.payload.decode("utf-8", errors="replace")
        self.root.after(0, self._handle, topic, payload)

    def _handle(self, topic, payload):
        if topic == "speed":
            try:
                self.speed = abs(float(payload))
            except ValueError:
                pass
            self._update_speed_bar()
        elif topic == "torque":
            try:
                self.torque = abs(float(payload))
            except ValueError:
                pass
            self.last_torque_time = time.time()
            self._update_torque_gauge()
        elif topic == "pot":
            try:
                self.pot = float(payload)
            except ValueError:
                pass
            self._update_pot()
        elif topic == "on":
            self.motor_on = payload.startswith("true")
            self._update_start_stop()
        elif topic == "firmware":
            self.firmware = payload
            self.lbl_firmware_val.config(text=payload)
        elif topic == "mcu_uid":
            self.mcu_uid = payload
            self.lbl_uid_val.config(text=payload)
        elif topic == "max_speed":
            self.max_speed = payload
            self.lbl_maxspeed_val.config(text=f"{payload} RPM")
        elif topic == "failure":
            try:
                self.failure = int(payload)
            except ValueError:
                pass
            self._update_failure()
        elif topic == "vcc":
            self.lbl_vcc_val.config(text=f"{payload} V")
        elif topic == "board_name":
            self.lbl_board_val.config(text=payload)
        elif topic == "switch":
            self.lbl_switch_val.config(text=payload)
        elif topic == "aiml_status":
            pass

    # ── UI construction ──

    def _build_ui(self):
        main = tk.Frame(self.root, bg=BG, padx=12, pady=12)
        main.pack(fill="both", expand=True)

        top = tk.Frame(main, bg=BG)
        top.pack(fill="x", pady=(0, 8))
        self._build_info_group(top)

        mid = tk.Frame(main, bg=BG)
        mid.pack(fill="both", expand=True)
        mid.columnconfigure(0, weight=2)
        mid.columnconfigure(1, weight=1)
        self._build_telemetry_group(mid)
        self._build_status_group(mid)

    def _group(self, parent, title, grid=None, **pack_kw):
        f = tk.LabelFrame(parent, text=f"  {title}  ", bg=GROUP_BG,
                          fg=DIM, font=("Segoe UI", 9), bd=1,
                          relief="solid", highlightbackground=GROUP_OUTLINE,
                          padx=10, pady=8)
        if grid:
            f.grid(padx=4, pady=4, sticky="nsew", **grid)
        else:
            f.pack(padx=4, pady=4, **pack_kw)
        return f

    def _label_pair(self, parent, label, row, col=0):
        tk.Label(parent, text=label, bg=GROUP_BG, fg=DIM,
                 font=("Segoe UI", 9)).grid(row=row, column=col,
                 sticky="w", padx=(0, 6), pady=2)
        val = tk.Label(parent, text="—", bg=GROUP_BG, fg=FG,
                       font=("Segoe UI", 9, "bold"))
        val.grid(row=row, column=col + 1, sticky="w", pady=2)
        return val

    # ── Info Group ──

    def _build_info_group(self, parent):
        g = self._group(parent, "Board Info", fill="x")

        left = tk.Frame(g, bg=GROUP_BG)
        left.pack(side="left", fill="x", expand=True)

        self.lbl_firmware_val = self._label_pair(left, "Firmware:", 0)
        self.lbl_uid_val = self._label_pair(left, "MCU UID:", 1)
        self.lbl_maxspeed_val = self._label_pair(left, "Max Speed:", 2)
        self.lbl_board_val = self._label_pair(left, "Board:", 3)
        self.lbl_vcc_val = self._label_pair(left, "VCC:", 4)
        tk.Label(left, text="Reset the board to refresh board info fields.",
                 bg=GROUP_BG, fg=DIM, font=("Segoe UI", 8, "italic")).grid(
                 row=5, column=0, columnspan=2, sticky="w", pady=(6, 0))

        right = tk.Frame(g, bg=GROUP_BG)
        right.pack(side="right", padx=(20, 0))

        self.online_indicator = tk.Label(right, text=" OFFLINE ", bg=GRAY,
                                         fg=FG, font=("Segoe UI", 10, "bold"),
                                         padx=12, pady=4)
        self.online_indicator.pack(pady=(0, 6))

        self.btn_remote = tk.Button(
            right, text="REMOTE: OFF", bg=GRAY, fg=FG,
            activebackground=GRAY, activeforeground=FG,
            font=("Segoe UI", 10, "bold"), bd=0, padx=12, pady=4,
            cursor="hand2", command=self._toggle_remote)
        self.btn_remote.pack(pady=(0, 2))
        tk.Label(right, text="Press to control the motor remotely",
                 bg=GROUP_BG, fg=DIM, font=("Segoe UI", 7, "italic")).pack()

    # ── Telemetry Group ──

    def _build_telemetry_group(self, parent):
        g = self._group(parent, "Telemetry & Control", grid={"row": 0, "column": 0})

        top_row = tk.Frame(g, bg=GROUP_BG)
        top_row.pack(fill="x", pady=(0, 8))

        self.btn_start_stop = tk.Button(
            top_row, text="STOPPED", bg=GRAY, fg=FG,
            activebackground=GRAY, activeforeground=FG,
            font=("Segoe UI", 11, "bold"), bd=0, width=14, pady=4,
            cursor="hand2", command=self._toggle_start_stop)
        self.btn_start_stop.pack(side="left", padx=(0, 12))

        pot_frame = tk.Frame(top_row, bg=GROUP_BG)
        pot_frame.pack(side="left", padx=(0, 12))
        tk.Label(pot_frame, text="Pot:", bg=GROUP_BG, fg=DIM,
                 font=("Segoe UI", 9)).pack(side="left")
        self.lbl_pot = tk.Label(pot_frame, text="—", bg=GROUP_BG, fg=FG,
                                font=("Segoe UI", 10, "bold"))
        self.lbl_pot.pack(side="left", padx=(4, 0))

        sw_frame = tk.Frame(top_row, bg=GROUP_BG)
        sw_frame.pack(side="left")
        tk.Label(sw_frame, text="Switches:", bg=GROUP_BG, fg=DIM,
                 font=("Segoe UI", 9)).pack(side="left")
        self.lbl_switch_val = tk.Label(sw_frame, text="—", bg=GROUP_BG, fg=FG,
                                        font=("Segoe UI", 10, "bold"))
        self.lbl_switch_val.pack(side="left", padx=(4, 0))

        # Speed bar
        speed_frame = tk.Frame(g, bg=GROUP_BG)
        speed_frame.pack(fill="x", pady=(0, 6))
        tk.Label(speed_frame, text="Speed", bg=GROUP_BG, fg=DIM,
                 font=("Segoe UI", 9)).pack(side="left")
        self.lbl_speed_val = tk.Label(speed_frame, text="0 RPM", bg=GROUP_BG,
                                       fg=FG, font=("Segoe UI", 10, "bold"))
        self.lbl_speed_val.pack(side="right")

        self.speed_canvas = tk.Canvas(g, height=24, bg="#111122",
                                       highlightthickness=0)
        self.speed_canvas.pack(fill="x", pady=(0, 10))
        self.speed_bar_id = None

        # Torque gauge
        gauge_frame = tk.Frame(g, bg=GROUP_BG)
        gauge_frame.pack(pady=(0, 10))
        self.torque_canvas = tk.Canvas(gauge_frame, width=200, height=120,
                                        bg=GROUP_BG, highlightthickness=0)
        self.torque_canvas.pack()
        self._draw_torque_gauge(0)

        # Speed setpoint buttons
        sp_frame = tk.Frame(g, bg=GROUP_BG)
        sp_frame.pack(fill="x")
        tk.Label(sp_frame, text="Speed Setpoint:", bg=GROUP_BG, fg=DIM,
                 font=("Segoe UI", 9)).pack(side="left", padx=(0, 8))
        self.speed_buttons = {}
        for spd in SPEED_PRESETS:
            btn = tk.Button(
                sp_frame, text=f"{spd}", bg=GRAY, fg=FG,
                activebackground=BLUE, activeforeground=FG,
                font=("Segoe UI", 9, "bold"), bd=0, padx=10, pady=3,
                cursor="hand2",
                command=lambda s=spd: self._set_speed_setpoint(s))
            btn.pack(side="left", padx=2)
            self.speed_buttons[spd] = btn
        self._highlight_speed_btn(self.selected_speed)

    # ── Status Group ──

    def _build_status_group(self, parent):
        g = self._group(parent, "AI Motor State", grid={"row": 0, "column": 1})

        # AIML mode selector
        tk.Label(g, text="AI Model:", bg=GROUP_BG, fg=DIM,
                 font=("Segoe UI", 9)).pack(anchor="w")
        self.aiml_mode_var = tk.StringVar(value="Off")
        self.aiml_combo = ttk.Combobox(g, textvariable=self.aiml_mode_var,
                                        values=["Off", "Model #1"],
                                        state="readonly", width=12)
        self.aiml_combo.pack(anchor="w", pady=(2, 10))
        self.aiml_combo.bind("<<ComboboxSelected>>", self._on_aiml_mode_changed)

        # Failure LEDs (vertical)
        self.failure_leds = []
        for i, (label, color) in enumerate(zip(FAILURE_LABELS, FAILURE_COLORS)):
            led_frame = tk.Frame(g, bg=GROUP_BG)
            led_frame.pack(anchor="w", pady=2)
            led = tk.Canvas(led_frame, width=14, height=14, bg=GROUP_BG,
                            highlightthickness=0)
            led.pack(side="left", padx=(0, 6))
            led.create_oval(1, 1, 13, 13, fill=COLOR_GRAY, outline="")
            tk.Label(led_frame, text=label, bg=GROUP_BG, fg=DIM,
                     font=("Segoe UI", 9)).pack(side="left")
            self.failure_leds.append((led, color))
        self._update_failure()

    # ── Widget updates ──

    def _update_speed_bar(self):
        self.speed_canvas.delete("bar")
        w = self.speed_canvas.winfo_width()
        if w < 2:
            w = 400
        frac = min(self.speed / MAX_SPEED_DISPLAY, 1.0) if MAX_SPEED_DISPLAY else 0
        bar_w = int(frac * w)

        if frac < 0.33:
            color = COLOR_GREEN
        elif frac < 0.66:
            color = COLOR_YELLOW
        else:
            color = COLOR_RED

        if bar_w > 0:
            self.speed_canvas.create_rectangle(0, 0, bar_w, 24, fill=color,
                                                outline="", tags="bar")
        self.lbl_speed_val.config(text=f"{self.speed:.0f} RPM")

    def _draw_torque_gauge(self, pct):
        c = self.torque_canvas
        c.delete("all")
        cx, cy = 100, 100
        r = 80

        c.create_arc(cx - r, cy - r, cx + r, cy + r, start=0, extent=180,
                      outline="#333333", width=12, style="arc")

        if pct < 33:
            color = COLOR_GREEN
        elif pct < 66:
            color = COLOR_YELLOW
        else:
            color = COLOR_RED

        extent = min(pct / 100.0, 1.0) * 180
        if extent > 0:
            c.create_arc(cx - r, cy - r, cx + r, cy + r, start=180,
                          extent=-extent, outline=color, width=12, style="arc")

        c.create_text(cx, cy - 15, text=f"{pct:.0f}%", fill=FG,
                       font=("Segoe UI", 20, "bold"))
        c.create_text(cx, cy + 10, text="Torque", fill=DIM,
                       font=("Segoe UI", 9))

    def _update_torque_gauge(self):
        pct = min(abs(self.torque) * 100, 100)
        self._draw_torque_gauge(pct)

    def _update_start_stop(self):
        if self.motor_on:
            self.btn_start_stop.config(text="RUNNING", bg=GREEN)
        else:
            self.btn_start_stop.config(text="STOPPED", bg=GRAY)

    def _update_pot(self):
        self.lbl_pot.config(text=f"{self.pot:.0f}")

    def _update_failure(self):
        for i, (led, color) in enumerate(self.failure_leds):
            led.delete("all")
            if i == self.failure:
                led.create_oval(1, 1, 13, 13, fill=color, outline="")
            else:
                led.create_oval(1, 1, 13, 13, fill=COLOR_GRAY, outline="")

    def _highlight_speed_btn(self, active):
        for spd, btn in self.speed_buttons.items():
            if spd == active:
                btn.config(bg=BLUE)
            else:
                btn.config(bg=GRAY)

    # ── Online detection ──

    def _poll_online(self):
        was_online = self.online
        self.online = (time.time() - self.last_torque_time) < 3.0
        if self.online != was_online:
            if self.online:
                self.online_indicator.config(text=" ONLINE ", bg=GREEN)
            else:
                self.online_indicator.config(text=" OFFLINE ", bg=GRAY)
        self.root.after(1000, self._poll_online)

    # ── Controls ──

    def _toggle_remote(self):
        self.remote = not self.remote
        if self.remote:
            self.btn_remote.config(text="REMOTE: ON", bg=BLUE)
        else:
            self.btn_remote.config(text="REMOTE: OFF", bg=GRAY)
        payload = "true" if self.remote else "false"
        self.client.publish(f"{TOPIC_PREFIX}/remote_control", payload)

    def _toggle_start_stop(self):
        if not self.remote:
            return
        self.motor_on = not self.motor_on
        payload = "true" if self.motor_on else "false"
        self.client.publish(f"{TOPIC_PREFIX}/on", payload)
        self._update_start_stop()

    def _on_aiml_mode_changed(self, event):
        mode = 0 if self.aiml_mode_var.get() == "Off" else 1
        self.client.publish(f"{TOPIC_PREFIX}/aiml_mode", str(mode))

    def _set_speed_setpoint(self, speed):
        self.selected_speed = speed
        self._highlight_speed_btn(speed)
        if self.remote:
            self.client.publish(f"{TOPIC_PREFIX}/speed_setpoint", str(speed))


def main():
    root = tk.Tk()
    root.geometry("800x560")
    root.minsize(700, 480)
    app = SmartMotorApp(root)
    root.mainloop()


if __name__ == "__main__":
    main()
