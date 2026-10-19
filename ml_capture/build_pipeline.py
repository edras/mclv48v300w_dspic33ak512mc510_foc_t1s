"""Build and train the predictive-maintenance pipeline in MPLAB ML Model Builder.

Classes: normal, fingertap, notch_short, notch_long (captures uploaded with
upload_to_mplabml.py). Run with the Python environment that has the mplabml SDK:

    ..\\..\\mplabml_SDK\\.venv\\Scripts\\python build_pipeline.py

No command line arguments: adapt the settings block below.

Design decisions (from the offline analysis of ml_capture/data):
- Columns iq, vq, rpm. id carries no class information (Fisher ratio ~0) and only
  costs RAM on the target.
- Time-domain statistics only. The disturbances are short torque pulses whose
  level and shape separate the classes; rotation harmonics / FFT bands were
  clearly worse and a 512-point FFT would cost several KB of RAM.
- Window 512 samples (256 ms @ 2 kHz, >= 2 revolutions at 500 RPM), delta 244,
  i.e. 3 windows per 1000-sample capture.
- Validation by capture (segment_uuid) while only the blue adapter is used:
  the windows of one capture stay together. This is optimistic, because
  consecutive captures of one run are almost identical. Once the black adapter
  is added, validate by adapter (= by recording run) for an honest estimate.
- Decision tree ensemble trained as random forest: small, fast, robust on a few
  dozen scaled features. (xGBoost on the server only supports 2 classes.)
"""

import time
from pathlib import Path

from mplabml import Client

# ======================================================================
# SETTINGS
# ======================================================================

PROJECT = "AIoT_SmartMotor"
PIPELINE = "SmartMotor_PredMaint"
QUERY = "SmartMotor_PredMaint_training"

# Session name used in the .dcli upload (capture_ml_data.py)
SESSION = "Session_1"
LABEL_COLUMN = "Label"
COLUMNS = ["iq", "vq", "rpm"]
METADATA_COLUMNS = ["speed_rpm", "adapter", "dataset_type"]
# Only the blue adapter for now; "" uses all captures
QUERY_FILTER = "[adapter] IN [blue]"

SAMPLE_RATE_HZ = 2000
WINDOW_SIZE = 512
WINDOW_DELTA = 244

FEATURE_GENERATORS = [
    "Mean",
    "Standard Deviation",
    "Minimum",
    "Maximum",
    "Global Peak to Peak",
    "Interquartile Range",
    "25th Percentile",
    "75th Percentile",
    "Skewness",
    "Kurtosis",
    "Mean Crossing Rate",
    "Average Demeaned Energy",
]
NUMBER_OF_FEATURES = 16

# "segment_uuid": validate on other captures of the same runs (optimistic)
# "adapter": train on one recording run, validate on the other (honest,
#            needs QUERY_FILTER = "" and VALIDATION_FOLDS = 2)
VALIDATION_METADATA = "segment_uuid"
VALIDATION_FOLDS = 3

CLASSIFIER = "Decision Tree Ensemble"
TRAINING_ALGORITHM = "Random Forest"
# Applied only if the server's Random Forest accepts these parameter names
TRAINING_PARAMS = {"max_depth": 6, "n_estimators": 20}

API_KEY_FILE = Path(__file__).resolve().parents[2] / "mplabml_SDK" / "mplabmlapi" / "key.txt"
CACHE_TIMEOUT_S = 900

# ======================================================================


def function_inputs(client, name):
    """Parameter names the server accepts for a function, None if it does not exist."""
    try:
        return {p["name"] for p in client.functions.get_function_by_name(name).input_contract}
    except Exception:
        return None


def check_function(client, name, params):
    """Abort before anything is created if a block or parameter is unknown.

    The SDK silently drops parameters that are not in the server's input contract.
    """
    known = function_inputs(client, name)
    if known is None:
        raise SystemExit(f"Function '{name}' is not available on the server")
    unknown = set(params) - known
    if unknown:
        raise SystemExit(f"'{name}' does not accept {sorted(unknown)}; valid: {sorted(known)}")


def available_generators(client):
    """Feature generators that exist on the server; missing ones are skipped with a note."""
    names = []
    for name in FEATURE_GENERATORS:
        if function_inputs(client, name) is None:
            print(f"  ! feature generator '{name}' not available, skipped")
        else:
            names.append(name)
    return names


def training_params(client):
    """TRAINING_PARAMS restricted to what the server's training algorithm accepts."""
    known = function_inputs(client, TRAINING_ALGORITHM)
    if known is None:
        raise SystemExit(f"Training algorithm '{TRAINING_ALGORITHM}' is not available on the server")
    dropped = set(TRAINING_PARAMS) - known
    if dropped:
        print(f"  ! '{TRAINING_ALGORITHM}' does not accept {sorted(dropped)}, server defaults used; "
              f"valid: {sorted(known)}")
    return {k: v for k, v in TRAINING_PARAMS.items() if k in known}


def build_steps(client, generators):
    return {
        "Windowing": {"window_size": WINDOW_SIZE, "delta": WINDOW_DELTA,
                      "enable_train_delta": False, "return_segment_index": False},
        **{name: {"columns": COLUMNS} for name in generators},
        "Tree-based Selection": {"number_of_features": NUMBER_OF_FEATURES},
        "Min Max Scale": {"min_bound": 0, "max_bound": 255},
        "Stratified Metadata k-fold": {"number_of_folds": VALIDATION_FOLDS,
                                       "metadata_name": VALIDATION_METADATA},
        "Recall": {},
        CLASSIFIER: {},
        TRAINING_ALGORITHM: training_params(client),
    }


def print_dataset(client):
    captures = client.list_captures()
    print(f"Project '{PROJECT}': {len(captures)} captures")
    if SESSION not in client.list_segmenters()["name"].values:
        raise SystemExit(f"Session '{SESSION}' not found in the project")


def build_query(client):
    query = client.create_query(
        name=QUERY,
        columns=COLUMNS,
        metadata_columns=METADATA_COLUMNS,
        metadata_filter=QUERY_FILTER,
        segmenter=SESSION,
        label_column=LABEL_COLUMN,
        force=True,
    )
    print("Building query cache ...")
    query.cache_query()
    deadline = time.monotonic() + CACHE_TIMEOUT_S
    while True:
        status = str(query.cache_query_status().get("status", ""))
        if any(s in status for s in ("SUCCESS", "COMPLETE")):
            break
        if any(s in status for s in ("FAILURE", "REVOKED")):
            raise SystemExit(f"Query cache failed: {status}")
        if time.monotonic() > deadline:
            raise SystemExit("Query cache timeout")
        time.sleep(10)
    print(query.statistics_segments().groupby(LABEL_COLUMN).size().rename("segments"))
    return query


def build_pipeline(client, steps, generators, validation):
    client.pipeline = PIPELINE
    p = client.pipeline
    p.reset()
    p.set_input_query(QUERY, use_session_preprocessor=False)
    p.add_segmenter("Windowing", params=steps["Windowing"])
    p.add_feature_generator([{"name": n, "params": steps[n]} for n in generators])
    p.add_feature_selector([{"name": "Tree-based Selection", "params": steps["Tree-based Selection"]}])
    p.add_transform("Min Max Scale", params=steps["Min Max Scale"])
    p.set_validation_method(validation, params=steps[validation])
    p.set_classifier(CLASSIFIER, params=steps[CLASSIFIER])
    p.set_training_algorithm(TRAINING_ALGORITHM, params=steps[TRAINING_ALGORITHM])
    p.set_tvo({"validation_seed": 42})
    return p


def report(results, metrics_set):
    # execute() returns a ModelResultSet, or a list of them for some server responses
    result_set = results[0] if isinstance(results, list) else results
    if result_set is None or not result_set.configurations:
        raise SystemExit("Training returned no model, see the server log above")
    print(result_set.summarize(metrics_set=metrics_set))
    model = result_set.configurations[0].models[0]
    print(model)
    return model.knowledgepack


def main():
    if not API_KEY_FILE.is_file():
        raise SystemExit(f"API key file not found: {API_KEY_FILE}")
    client = Client(api_key=API_KEY_FILE.read_text().strip())
    client.project = PROJECT
    print_dataset(client)

    generators = available_generators(client)
    steps = build_steps(client, generators)
    for name, params in steps.items():
        check_function(client, name, params)

    #build_query(client)

    # 1) Validation estimate, grouped by VALIDATION_METADATA
    p = build_pipeline(client, steps, generators, "Stratified Metadata k-fold")
    p.describe()
    print(f"\n=== Validation (grouped by '{VALIDATION_METADATA}') ===")
    results, _ = p.execute(silent=False)
    report(results, "validation")

    # 2) Final model for deployment: same pipeline trained on all captures
    p = build_pipeline(client, steps, generators, "Recall")
    print("\n=== Final model (trained on all data) ===")
    results, _ = p.execute(silent=False)
    kp = report(results, "train")
    print("\nSelected features:")
    print(kp.feature_summary)
    print("\nResource cost:")
    print(kp.cost_resource_summary())
    kp.save(f"{PIPELINE}_rf")
    print(f"\nKnowledge Pack saved as '{PIPELINE}_rf'")


if __name__ == "__main__":
    main()
