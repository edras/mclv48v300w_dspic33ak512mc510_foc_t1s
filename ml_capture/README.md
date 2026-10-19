# ML data capture (pyX2Cscope over 10BASE-T1S)

Records FOC signals with X2Cscope and writes them in the MPLAB ML upload format:
one CSV per scope snapshot plus `data/data.dcli` with labels and metadata.

The scripts take no arguments. Edit the `SETTINGS` block at the top of the
file, then execute it.

## Recorded signals

| Column | Variable | Type |
|--------|----------|------|
| `iq`  | `mcApp.controlScheme.idq.q` | float, A |
| `rpm` | `mcApp.controlScheme.estimatorInterface.speedMech.RPM` | float, RPM |
| `id`  | `mcApp.controlScheme.idq.d` | float, A |
| `vq`  | `mcApp.controlScheme.vdq.q` | float, V |

`PRESCALER = 10` → 2 kHz. With the 16000-byte X2Cscope buffer one snapshot
holds 1000 samples (0.5 s, ~12 revolutions at 1500 RPM).

## Capture: `capture_ml_data.py`

1. In pyX2Cscope, load `out/AK512_FOC_T1S/default.elf` (the flashed firmware)
   and connect via TCP/IP to `192.168.0.100:12666`.
2. Edit `LABEL`, `SPEED_RPM`, `COUNT`, `TEST_SET` in the settings block.
3. Scripting tab → select `capture_ml_data.py` → **Execute**.

The script uses the GUI connection, takes remote control, sets the speed,
starts the motor and waits until it is stable. Then it waits `START_DELAY_S`
seconds so you can apply the disturbance (finger, pen in notch) and records
`COUNT` snapshots. Keep the disturbance applied until "Done." is printed.
**Stop** aborts cleanly; the motor is stopped and remote control is released.

Run it once per label and speed, e.g. `normal`, `fingertap`, `notch_short`,
`notch_long` at 1000 / 1500 / 2000 RPM, plus a few runs with `TEST_SET = True`.
All runs are appended to `data/data.dcli`.

Outside the GUI the same file runs with `python capture_ml_data.py`
(needs `pip install pyx2cscope`); it then connects to `HOST`/`TCP_PORT` itself.

## Upload: `upload_to_mplabml.py`

Set `PROJECT` and run it with the mplabml SDK environment:

```
..\..\mplabml_SDK\.venv\Scripts\python upload_to_mplabml.py
```

The API key is read from `mplabml_SDK/mplabmlapi/key.txt`.

In Model Builder use a windowing segmenter with a window that fits into one
snapshot (e.g. 512 samples) and the query columns `iq, rpm, id, vq`.
