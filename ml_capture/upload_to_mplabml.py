"""Upload the capture folder (CSV files + .dcli) to an MPLAB ML Model Builder project.

Run it with the Python environment that has the mplabml SDK installed, e.g.

    ..\\..\\mplabml_SDK\\.venv\\Scripts\\python upload_to_mplabml.py

There are no command line arguments: adapt the settings block below.

Can be run again after new captures were recorded: captures that already exist
in the project are skipped (re-uploading their labels/metadata would fail with
"duplicate key value violates unique constraint").
"""

import json
from pathlib import Path

from mplabml import Client

# ======================================================================
# SETTINGS
# ======================================================================

# MPLAB ML project name (created if missing)
PROJECT = "AIoT_SmartMotor"

# File with the MPLAB ML API key (single line, never commit it)
API_KEY_FILE = Path(__file__).resolve().parents[2] / "mplabml_SDK" / "mplabmlapi" / "key.txt"

# ======================================================================

DATA_DIR = Path(__file__).resolve().parent / "data"


def main():
    dcli = DATA_DIR / f"{DATA_DIR.name}.dcli"
    if not dcli.is_file():
        raise SystemExit(f"No .dcli found: {dcli}")
    if not API_KEY_FILE.is_file():
        raise SystemExit(f"API key file not found: {API_KEY_FILE}")

    client = Client(api_key=API_KEY_FILE.read_text().strip())
    client.project = PROJECT

    existing = set(client.project.captures.build_capture_list(force=True))
    entries = json.loads(dcli.read_text())
    local = [e for e in entries if (DATA_DIR / e["file_name"]).is_file()]
    new = [e for e in local if e["file_name"] not in existing]
    print(f"{len(entries)} labeled captures, {len(local)} CSV files present, "
          f"{len(existing)} already in project '{PROJECT}', {len(new)} to upload")
    if not new:
        return

    # The SDK uploads every entry of the given .dcli; give it only the new ones.
    # The file must sit next to the CSVs.
    upload_dcli = DATA_DIR / "_upload_new.dcli"
    upload_dcli.write_text(json.dumps(new, indent=2))
    try:
        client.upload_project_dcli(name=PROJECT, dcli_path=str(upload_dcli), force=True)
    finally:
        upload_dcli.unlink()

    print(client.list_captures(force=True).tail())


if __name__ == "__main__":
    main()
