# Wasm
Wowcube public WebAssembly examples and utils
## Installation

* [Install](https://emscripten.org/docs/getting_started/downloads.html) Emscripten Compiler Frontend
* Add to system environment path C:\your_path\emsdk\upstream\emscripten
## Build wasm
cmake cmd
```bash
mkdir build
cd build
cmake ..
cmake --build . -jYOUR_CORES_NUM
```
In follder ../bin you will find all wasm files
## Usage
* [Download](https://github.com/wowcube/wasm/releases) and unpack latest release.
* Download and install latest wow cube emulator
* Build wasm file
* Run wow cube emulator in projector mode
* Run simulatorServer.exe with command-line parameters:
```bash
simulatorServer.exe --wasm_path example.wasm --render_dir_path "C:\Users\user\Documents\WOWCube\modules" --split_terminal
```

