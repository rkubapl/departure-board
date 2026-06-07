## Build for PC
Tested on macOS using GCC 15. 
- Pull submodules (lvgl): `git submodule update --init --recursive` or clone lvgl repository `git clone https://github.com/lvgl/lvgl`
- Install SDL according to your platform: 
  - Linux: `sudo apt install libsdl2-dev`
  - macOS: `brew install sdl2`
  - Windows: follow instructions on [LVGL SDL Drivers site](https://lvgl.io/docs/open/integration/pc/sdl)
- Go to `pc` folder and create `build` folder: `cd pc && mkdir build`
- Generate compilation files using CMake: `cd build && cmake ..`
- Compile project: `make`
- Move the example schedule file `schedule.bin` inside the build folder: `mv ../../schedule.bin .`
- Start the app using `./DepartureDisplay`
- Example file has information from 7 czerwiec 2026.

## Build for ESP32
Made for [ESP32-Cheap-Yellow-Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display).
- Move the example schedule file `schedule.bin` into the microSD card.
- Pull submodules: `git submodule update --init --recursive`
- Install [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) (I used v6.0.1)
- Activate ESP-IDF environment using `eim select` and pasting the provided command
- Go to the `esp32` folder: `cd esp32`
- Compile and flash the project: `idf.py -p PORT build flash monitor` (replace `PORT` with your device port, e.g., `/dev/cu.usbserial-...` or `COM3`)