## Kompilacja na PC
Testowano na systemie macOS z użyciem kompilatora GCC 15. 
- Pobierz submoduły (lvgl): `git submodule update --init --recursive`
- Zainstaluj bibliotekę SDL zgodnie ze swoim systemem operacyjnym: 
  - Linux: `sudo apt install libsdl2-dev`
  - macOS: `brew install sdl2`
  - Windows: postępuj zgodnie z instrukcjami na [stronie sterowników LVGL SDL](https://lvgl.io/docs/open/integration/pc/sdl)
- Przejdź do folderu `pc` i utwórz folder `build`: `cd pc && mkdir build`
- Wygeneruj pliki konfiguracyjne za pomocą CMake: `cd build && cmake ..`
- Skompiluj projekt: `make`
- Przenieś przykładowy plik z rozkładem jazdy `schedule.bin` do folderu kompilacji: `mv ../../schedule.bin .`
- Uruchom aplikację: `./DepartureDisplay`
- Przykładowy plik z rozkładem jazdy zawiera informacje od 7 czerwca 2026.

## Kompilacja na ESP32
Przygotowano dla [ESP32-Cheap-Yellow-Display](https://github.com/witnessmenow/ESP32-Cheap-Yellow-Display).
- Skopiuj przykładowy plik z rozkładem jazdy `schedule.bin` na kartę microSD.
- Pobierz submoduły: `git submodule update --init --recursive`
- Zainstaluj [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) (użyto wersji v6.0.1)
- Aktywuj środowisko ESP-IDF używając polecenia `eim select` i wklejając podaną komendę.
- Przejdź do folderu `esp32`: `cd esp32`
- Skompiluj i wgraj projekt: `idf.py -p PORT build flash monitor` (zamień `PORT` na port swojego urządzenia, np. `/dev/cu.usbserial-...` lub `COM3`)
