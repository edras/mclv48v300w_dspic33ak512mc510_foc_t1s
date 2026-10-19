"""Capture labeled motor data over X2Cscope/T1S for MPLAB ML Model Builder.

Run it from the pyX2Cscope Scripting tab (Execute) or with `python capture_ml_data.py`.
There are no command line arguments: adapt the settings block below before each run.

Each scope snapshot is written as one CSV file (one column per channel, no
label column). All captures of the output folder are listed in a .dcli file
that labels every file with one segment over its full length. Upload the
folder afterwards with upload_to_mplabml.py.
"""

import csv
import json
import re
import time
from datetime import datetime
from pathlib import Path

from pyx2cscope.x2cscope import X2CScope

# ======================================================================
# SETTINGS - adapt before each run
# ======================================================================

# Class label of this run: "normal", "fingertap", "notch_short", "notch_long"
LABEL = "notch_long"

# Mechanical speed setpoint in RPM (500..3500)
SPEED_RPM = 2000

# Number of snapshots (CSV files) to record
COUNT = 30

# True: captures go into the test set, False: training set
TEST_SET = False

# Free text metadata describing the shaft adapter (letters, digits, '_')
ADAPTER = "black"

# Scope sample time factor, 1 = every ISR call (20 kHz). 10 -> 2 kHz
PRESCALER = 10

# True: script sets the speed and starts/stops the motor (remote control).
# False: record whatever the motor is currently doing.
CONTROL_MOTOR = True

# Seconds to wait after the motor settled, e.g. to apply the finger/pen
START_DELAY_S = 5

# Only used when the script is NOT run from the pyX2Cscope GUI
HOST = "192.168.0.100"
TCP_PORT = 12666

# ======================================================================

SCRIPT_DIR = Path(__file__).resolve().parent
ELF_FILE = SCRIPT_DIR.parent / "out" / "AK512_FOC_T1S" / "default.elf"
OUT_DIR = SCRIPT_DIR / "data"

# X2Cscope_Update() runs in the 20 kHz ADC ISR (src/main.c)
ISR_FREQUENCY_HZ = 20000

# CSV column name -> firmware variable (all float)
CHANNELS = {
    "iq": "mcApp.controlScheme.idq.q",
    "rpm": "mcApp.controlScheme.estimatorInterface.speedMech.RPM",
    "id": "mcApp.controlScheme.idq.d",
    "vq": "mcApp.controlScheme.vdq.q",
}

LABELS = ("normal", "fingertap", "notch_short", "notch_long")

SETTLE_TOLERANCE_RPM = 50.0
SETTLE_TIMEOUT_S = 15.0

VALID_NAME = re.compile(r"^[A-Za-z0-9_]+$")

# Injected by the pyX2Cscope Scripting tab; fall back when run standalone
stop_requested = globals().get("stop_requested", lambda: False)


class Stopped(Exception):
    pass


def check_settings():
    if LABEL not in LABELS:
        raise ValueError(f"LABEL must be one of {LABELS}")
    if not 500 <= SPEED_RPM <= 3500:
        raise ValueError("SPEED_RPM must be within 500..3500 (motor limits in src/mc/motor/act57blf02.h)")
    if not VALID_NAME.match(ADAPTER):
        raise ValueError("ADAPTER may only contain letters, digits and '_'")
    if PRESCALER < 1 or COUNT < 1:
        raise ValueError("PRESCALER and COUNT must be >= 1")


def wait(seconds):
    deadline = time.monotonic() + seconds
    while time.monotonic() < deadline:
        if stop_requested():
            raise Stopped()
        time.sleep(0.1)


def read(var):
    value = var.get_value()
    if value is None:
        raise RuntimeError(f"Reading {var.info.name} failed")
    return value


def check_connection(x2c):
    """pyx2cscope swallows communication errors (reads return None, writes do nothing)."""
    if x2c.get_variable("mcInitDone").get_value() is None:
        raise RuntimeError(f"No connection to the board ({HOST}:{TCP_PORT}). "
                           "Check that pyX2Cscope is connected and 'ping 192.168.0.100' works.")

    # Constant set at init (MAXIMUM_SPEED_RPM in src/mc/motor/act57blf02.h). Any other
    # value means the ELF loaded in pyX2Cscope is not the firmware that is flashed,
    # so every variable address is wrong.
    max_speed = x2c.get_variable("mcApp.motor.MaxSpeed").get_value()
    if max_speed != 3500.0:
        raise RuntimeError(f"mcApp.motor.MaxSpeed reads {max_speed} instead of 3500.0: the ELF loaded in "
                           "pyX2Cscope does not match the flashed firmware. Flash "
                           "out/AK512_FOC_T1S/default.elf and load exactly this file in pyX2Cscope.")


def start_motor(x2c):
    """Take remote control, set the speed and start the motor."""
    rpm = x2c.get_variable(CHANNELS["rpm"])

    # With remote_control set, the potentiometer no longer overwrites targetSpeed
    # and SW1/SW2 are ignored (src/main.c, src/mc/mc_app.c).
    x2c.get_variable("remote_control").set_value(1)
    x2c.get_variable("mcApp.targetSpeed").set_value(float(SPEED_RPM))
    x2c.get_variable("mcApp.runCmdBuffer").set_value(1)

    print(f"Waiting for {SPEED_RPM} RPM ...")
    deadline = time.monotonic() + SETTLE_TIMEOUT_S
    stable_since = None
    while time.monotonic() < deadline:
        measured = read(rpm)
        if abs(abs(measured) - SPEED_RPM) < SETTLE_TOLERANCE_RPM:
            stable_since = stable_since or time.monotonic()
            if time.monotonic() - stable_since > 1.0:
                print(f"Speed settled at {measured:.0f} RPM")
                return
        else:
            stable_since = None
        wait(0.1)
    print_motor_state(x2c)
    raise RuntimeError(f"Motor did not reach {SPEED_RPM} RPM within {SETTLE_TIMEOUT_S:.0f} s")


# MCAPP_STATE_T in src/mc/mc_app.h
APP_STATES = ("INIT", "CMD_WAIT", "OFFSET", "LOAD_START_READY_CHECK", "RUN", "STOPPING", "STOP", "FAULT")


def print_motor_state(x2c):
    """Print the firmware state that decides whether the motor runs."""
    names = (
        "remote_control", "mcApp.runCmdBuffer", "mcApp.runCmd", "mcApp.targetSpeed",
        "mcApp.appState", "mcApp.controlScheme.focState", "mcApp.controlScheme.faultStatus",
        "mcApp.controlScheme.ctrlParam.targetSpeed", "mcApp.controlScheme.ctrlParam.speedRef",
        CHANNELS["rpm"], "mcApp.motorInputs.measureVdc.value",
    )
    print("Motor state:")
    for name in names:
        value = x2c.get_variable(name).get_value()
        if name == "mcApp.appState" and value is not None and value < len(APP_STATES):
            value = f"{value} ({APP_STATES[value]})"
        print(f"  {name} = {value}")


def stop_motor(x2c):
    x2c.get_variable("mcApp.runCmdBuffer").set_value(0)
    x2c.get_variable("remote_control").set_value(0)
    print("Motor stopped, remote control released")


def setup_scope(x2c):
    x2c.clear_all_scope_channel()
    for name in CHANNELS.values():
        if x2c.add_scope_channel(x2c.get_variable(name)) < 0:
            raise RuntimeError(f"Could not add scope channel {name}")
    x2c.set_sample_time(PRESCALER)
    # Auto mode: steady-state captures do not need a trigger event
    x2c.reset_scope_trigger()


def capture_once(x2c, poll_timeout_s=10.0):
    x2c.request_scope_data()
    deadline = time.monotonic() + poll_timeout_s
    while not x2c.is_scope_data_ready():
        if time.monotonic() > deadline:
            raise RuntimeError("Scope data not ready (timeout)")
        wait(0.05)
    data = x2c.get_scope_channel_data()
    columns = {col: data[var] for col, var in CHANNELS.items()}
    lengths = {len(v) for v in columns.values()}
    if len(lengths) != 1 or 0 in lengths:
        raise RuntimeError(f"Inconsistent channel lengths: {lengths}")
    return columns


def write_csv(path, columns):
    with path.open("w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(columns.keys())
        writer.writerows(zip(*columns.values()))


def update_dcli(entries):
    """Merge new entries into <OUT_DIR>/<OUT_DIR name>.dcli (keyed by file name)."""
    dcli_path = OUT_DIR / f"{OUT_DIR.name}.dcli"
    existing = json.loads(dcli_path.read_text()) if dcli_path.exists() else []
    merged = {e["file_name"]: e for e in existing}
    merged.update({e["file_name"]: e for e in entries})
    dcli_path.write_text(json.dumps(list(merged.values()), indent=2))
    return dcli_path


def dcli_entry(file_name, n_samples, sample_rate_hz):
    meta = {
        "speed_rpm": str(int(SPEED_RPM)),
        "sampling_rate_hz": str(int(round(sample_rate_hz))),
        "dataset_type": "test" if TEST_SET else "training",
        "adapter": ADAPTER,
    }
    return {
        "file_name": file_name,
        "metadata": [{"name": k, "value": v} for k, v in meta.items()],
        "sessions": [{
            "session_name": "Session_1",
            "segments": [{"name": "Label", "value": LABEL, "start": 0, "end": n_samples - 1}],
        }],
    }


def main(x2c):
    check_settings()
    check_connection(x2c)
    sample_rate_hz = ISR_FREQUENCY_HZ / PRESCALER
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    print(f"Run: label={LABEL}, {SPEED_RPM} RPM, {COUNT} captures, "
          f"{'test' if TEST_SET else 'training'}, {sample_rate_hz:.0f} Hz")

    entries = []
    try:
        if CONTROL_MOTOR:
            start_motor(x2c)
        setup_scope(x2c)

        if START_DELAY_S > 0:
            print(f"Recording starts in {START_DELAY_S} s - apply '{LABEL}' now")
            wait(START_DELAY_S)

        stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        for i in range(COUNT):
            columns = capture_once(x2c)
            n = len(next(iter(columns.values())))
            file_name = f"{LABEL}_{SPEED_RPM}rpm_{stamp}_{i:03d}.csv"
            write_csv(OUT_DIR / file_name, columns)
            entries.append(dcli_entry(file_name, n, sample_rate_hz))
            print(f"[{i + 1}/{COUNT}] {file_name}: {n} samples")
        print("Done.")
    except Stopped:
        print("Stopped by user.")
    finally:
        if entries:
            print(f"{len(entries)} captures labeled in {update_dcli(entries)}")
        if CONTROL_MOTOR:
            stop_motor(x2c)


# Use the connection of the pyX2Cscope GUI if available, otherwise connect ourselves
_injected = globals().get("x2cscope")
if _injected is not None:
    main(_injected)
else:
    print(f"Connecting to {HOST}:{TCP_PORT} ...")
    # pyx2cscope's default TCP timeout of 0.05 s is too short over T1S
    _x2c = X2CScope(elf_file=str(ELF_FILE), host=HOST, tcp_port=TCP_PORT, timeout=1.0)
    try:
        main(_x2c)
    finally:
        _x2c.disconnect()
