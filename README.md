# UV_K5_playground
## src/par_runner
The idea is to run this firmware 'parallel' with the original Quencheng firmware. This can be achieved by relocating the original vector table to the end of the original firmware, and placing a new vector table at the beginning, with entities pointing to the par_runner functions that wrap the original firmware handlers.
#### flash memory layout
making "par_runner" target will result binary with sectons in the following order:
1. par_runner vector table 
2. orginal firmware 
3. relocated orginal fw vector table
4. .text and other flash sections of par_runner

To change the original firmware that will be wrapped and placed into the original firmware section, replace `./original_fw/original_fw.bin` or set the variable 
```CMakeLists.txt set(ORGINAL_FW_BIN orginal_fw.bin)```
in ./orginal_fw/CMakeLists.txt
and rebuild par_runner

## build system installation
currently tested on windows, requred:
* arm-none-eabi-gcc
* python (i have newest version)
* cmake
* ninja
* open-ocd

All folders with executables of the above programs should be added to the PATH environment variable.

for debugging:
* vs code
  * Cortex-Debug plugin
  * CMake plugin

### building
##### via terminal
```$ mkdir build```
```$ cd build```
```$ cmake ../ -G Ninja```
```$ ninja par_runner```
outputs ./build/src/par_runner/par_runner.bin / hex / elf
###### uploading
```$ ninja par_runner_flash```

##### via VS Code
Select the par_runner build target in the bottom bar and press build.
###### uploading
Enter the 'Run & Debug' tab, select 'kwaczek DBG', and press run.
