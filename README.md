# UV_K5_playground
## src/par_runner
The idea is to run this firmware 'parallel' with the original Quencheng firmware. This can be achieved by relocating the original vector table to the end of the original firmware, and placing a new vector table at the beginning, with entities pointing to the par_runner functions that wrap the original firmware handlers.
#### flash memory layout
When building the "par_runner" target, the resulting binary has sections in the following order (warning! deprecated):
![memory layout](./docs/memory-map.png)
Currently, the par_runner target creates a binary with a layout as shown on the left side of the image. However, this approach is incorrect because I found that the 'flash masking' on these Chinese devices not only remaps the first few sectors (depending on the selection in flash registers), but it actually remaps the entire code section memory so that 0x0 points to 0x1000, as shown in the image above.

#### TODO:
Unfortunately, I managed to brick my radio :D. Here are some notes for later:
Instead of building a single target, it is necessary to build two separate targets. The first target will be the stock bootloader, located at addresses 0x0 to 0x1000. The second target will be the main firmware, which will start from address 0x0 but will be flashed at address 0x1000. Additionally, this second target can be encoded as an 'encrypted' binary to work with the original Quasheng flasher tool.

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
