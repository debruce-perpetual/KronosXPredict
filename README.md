
# KronosPredict

KronosPredict is a C++20 library for **real-time financial market prediction** with a **plugin-based architecture** and **Python bindings**.

The core library defines a generic, runtime-selectable API for:

- Online / real-time prediction via `IRealtimeModel`
- Offline training via `IModelTrainer` and `ITrainingDataIterator`
- Model configuration via `nlohmann::json`
- Dynamic loading of concrete implementations (e.g. VARX, state-space MLE, nonlinear/Torch models) as shared libraries

This repository currently ships with a **stub plugin** (`KronosPredict_stub`) used to exercise the API and tests. Real models can be implemented as additional plugins later.

---

## 1. Dependencies

### 1.1. Common requirements

- **CMake** ≥ 3.22
- **C++20 compiler**
  - Linux: `g++-11` or newer (e.g. GCC 13 on Ubuntu)
  - macOS: Apple Clang from Xcode / Command Line Tools
- **nlohmann_json** (with CMake config file)
- **pybind11** (with CMake config file)
- **Python 3.8+** (3.10+ recommended) with dev headers/libraries

You do **not** need to install GoogleTest manually; it is fetched automatically via `FetchContent`.

---

### 1.2. Ubuntu / Debian

Install toolchain and deps (adjust package names for your distro if needed):

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    python3 \
    python3-dev \
    python3-venv \
    nlohmann-json3-dev \
    pybind11-dev
```

Optional: create and activate a Python virtualenv for testing the Python module:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
# (No extra Python packages strictly required for the bindings;
#  you may add any you need for your own experiments.)
```

---

### 1.3. macOS (Homebrew)

First install Xcode Command Line Tools (if you haven’t already):

```bash
xcode-select --install
```

Then install dependencies with Homebrew:

```bash
brew install \
    cmake \
    nlohmann-json \
    pybind11 \
    python
```

If you’re using a Python virtualenv:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install --upgrade pip
```

CMake will pick up the Homebrew `nlohmann-json` and `pybind11` via their CMake config files.

---

## 2. Building the Project

From the repository root:

```bash
mkdir build
cd build

cmake .. \
  -DKRONOSPREDICT_BUILD_PYTHON=ON \
  -DKRONOSPREDICT_BUILD_TESTS=ON

cmake --build .
```

Key CMake options:

- `KRONOSPREDICT_BUILD_PYTHON` (ON/OFF, default ON)  
  Build the `kronospredict` Python extension module using pybind11.
- `KRONOSPREDICT_BUILD_TESTS` (ON/OFF, default ON)  
  Build the C++ test executables (GoogleTest).

If you want a release build:

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release \
         -DKRONOSPREDICT_BUILD_PYTHON=ON \
         -DKRONOSPREDICT_BUILD_TESTS=ON
cmake --build .
```

### 2.1. Install (optional)

To install the library and CMake config files:

```bash
cmake --install . --prefix /your/install/prefix
```

This installs:

- `libKronosPredict.so` (or `.dylib` / `.dll`)
- Public headers under `include/KronosPredict`
- CMake config/targets under `lib/cmake/KronosPredict`

Downstream CMake projects can then do:

```cmake
find_package(KronosPredict CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE KronosPredict::KronosPredict)
```

---

## 3. Running Tests

From the `build` directory:

```bash
ctest --output-on-failure
```

This runs two test suites:

- `StubModelTest.BasicEchoBehavior`  
  Directly exercises the stub model’s factories and echo behavior.
- `PluginLoaderTest.LoadStubAndPredict`  
  Exercises dynamic loading of the stub plugin and a basic prediction call.

If both pass, the C++ core + plugin loader are working correctly.

---

## 4. Using the Python Bindings

When you configure with `KRONOSPREDICT_BUILD_PYTHON=ON`, CMake builds a Python extension module named `kronospredict` in the `build` directory.

From within `build` (or with `PYTHONPATH` pointing to `build`):

```python
import kronospredict as kp
from kronospredict import TargetKind
import numpy as np
import datetime

# Path to the stub plugin relative to the build dir
plugin_path = "plugins/stub/libKronosPredict_stub.so"  # .dylib / .dll on macOS/Windows

config = {"warmup_count": 2}
model = kp.load_model(plugin_path, config)

y1 = np.array([1.0, 2.0], dtype=float)
x1 = np.array([], dtype=float)
model.ingest(y1, x1, datetime.datetime.now())

y2 = np.array([3.0, 4.0], dtype=float)
model.ingest(y2, x1, datetime.datetime.now())

print("ready:", model.ready)
if model.ready:
    res = model.predict(TargetKind.Return, steps_ahead=1)
    print("mean:", res["mean"])
```

If you prefer to run from outside the build directory, add `build` to `PYTHONPATH`, e.g.:

```bash
export PYTHONPATH=/path/to/KronosPredict/build:${PYTHONPATH}
python -c "import kronospredict; print(kronospredict)"
```

---

## 5. Directory Layout (Overview)

At a high level:

```text
KronosPredict/
  CMakeLists.txt
  cmake/
    KronosPredictConfig.cmake.in
  include/
    KronosPredict/
      api.hpp
      runtime.hpp
      training.hpp
      plugin.hpp
      plugin_loader.hpp
  src/
    runtime.cpp
    plugin_loader.cpp
  python/
    CMakeLists.txt
    bindings.cpp
  plugins/
    stub/
      CMakeLists.txt
      stub_plugin.cpp
  tests/
    CMakeLists.txt
    test_stub_model.cpp
    test_plugin_loader.cpp
```

---

## 6. Next Steps

The current stub plugin is intentionally simple and just proves out the API and dynamic loading:

- It echoes the last ingested endogenous vector as the prediction mean.
- It uses JSON config (`warmup_count`) to control readiness.
- The trainer counts samples and emits trivial parameters.

You can add real implementations as new plugins under `plugins/` (e.g. `plugins/varx_torch`, `plugins/ssm_torch`, etc.) that:

- Export the same `extern "C"` factory/destroy functions.
- Use libtorch or other libraries internally.
- Are loaded at runtime via `PluginLibrary` and `load_plugin_library`.
