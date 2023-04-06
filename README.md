# ECU Display running on RPi

LVGL configured to work with /dev/fb0 on Linux. Also compiles as a windows simulator,
which generates an additional target "ecu_sensor_spoofer" - see
instructions at bottom.

When cloning this repository, also make sure to download submodules (`git submodule update --init --recursive`) otherwise you will be missing key components.

## Clone the project
```
git clone git@github.com:hptechdesign/rpi_ecu_display.git
cd rpi_ecu_display/
git submodule update --init --recursive
```

## Recommended build tools and required dependencies: 

  - MSYS2:
    - https://www.msys2.org/wiki/MSYS2-installation/ 
    - Follow the installation instructions, then open the MSYS2 terminal [from start menu, or ```C:\msys64\ucrt64.exe```]
    - Type ```pacman -Syuu``` to update all packages. If the terminal closes, run it again until all updates are complete.
    - Next install the following minimum packages:
  - MINGW64 compiler toolchain:
    - ```pacman -S mingw-w64-ucrt-x86_64-toolchain ```
  - CMAKE
    - ```pacman -S mingw-w64-x86_64-cmake```
  - SDL2 graphics drivers 
    - Required for the windows/SDL build, not needed for RPI
    - ```pacman -S mingw-w64-x86_64-SDL2```



## RPI cross-compilation toolchain

Download the pre-built windows toolchain for raspberry pi from here (select compatible version, e.g. Bullseye):
https://gnutoolchains.com/raspberry/

Set up environment variable RPIDEV_LOC, and refer to the following document:
https://www.codeproject.com/Articles/1279667/Toolset-to-Cross-Compile-Remote-Debug-Raspberry-fr

Using this information you should be able to modify the rpi_ecu_display project's launch.json and tasks.json to fit your environment. 

## Building

### For Windows:
  - Open the top-level ```CMakeLists.txt``` file and uncomment the line
    ```set(platform "win")```. 
  - Make sure that ```set(platform "rpi")``` is commented out.
  - Choose "Build" from the control panel at the bottom of VScode.

The following executables will be placed in ```{workspace_dir}/bin```:
 - win_ecu_display
   - The Windows display simulator, with a COM port serial interface
 - ecu_sensor_spoofer
   - A signal spooder which emulates the engine sensors and transmits the data via a COM port

### For RPI
  - Open the top-level ```CMakeLists.txt``` file and uncomment the line
    ```set(platform "rpi")```. 
  - Make sure that ```set(platform "win")``` is commented out.
  - Choose "Build" from the control panel at the bottom of VScode.

The ecu_sensor_spoofer will not be built. Only the rpi_ecu_display executable is built, and placed in  ```{workspace_dir}/bin```.

### Debug
Several debug launchers have been created.

On Windows, you can launch win_ecu_display, ecu_sensor_spoofer, or a compound launcher which launches both.

On RPI, you first need to ctrl-shft-'B', which transfers the binary to the PI, then you can launch the rpi_ecu_display debugger which will attach to the remote GDB session on the RPI.

----

The base display project was derived from the LVGL Framebuffer Demo project and its related sub modules:
[https://github.com/lvgl/lv_port_linux_frame_buffer.git].

See this blog post for a step by step tutorial for deriving from scratch:
[https://blog.lvgl.io/2018-01-03/linux_fb]


