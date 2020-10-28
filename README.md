# Wasm
Wowcube public WebAssembly examples and utils
## Installation

* [Install](https://emscripten.org/docs/getting_started/downloads.html) Emscripten Compiler Frontend

## Build wasm
Open command line and run emcc env script
```bash
C:\Users\user\emsdk\emsdk_env.bat
```
Run build
```bash
emcc -o CubeApiTest.wasm  CubeApiTest.cpp -s STANDALONE_WASM -s ERROR_ON_UNDEFINED_SYMBOLS=0 -O3 --no-entry -g3
```
## Usage
* [Download](https://github.com/wowcube/wasm/releases) and unpack latest release.
* Build wasm file
* Run wasm3_qt with command-line parameters:
```bash
wasm3_qt.exe -wasm_path CubeApiTest.wasm -cube_data_path -module_number 0 -render_dir_path "C:\Users\user\Documents\WOWCube\modules\0"
```

