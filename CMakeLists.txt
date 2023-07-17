cmake_minimum_required(VERSION 3.15)

set(OPTI_FLAG s)
set(CMAKE_BUILD_TYPE MinSizeRelease)

set(STM32_DEFINES "-DSTM32F0 -DSTM32F030x8 -DSTM32")

set(TOOLCHAIN_PREFIX arm-none-eabi-)
set(CMAKE_TOOLCHAIN_FILE toolchain/arm-none-eabi-gcc.cmake)
include(${CMAKE_TOOLCHAIN_FILE})

project(uv-k5-custom-build C CXX ASM)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# set(COMPILER_OPTIONS -mthumb -mcpu=cortex-m0 -mfpu=auto -mfloat-abi=soft -fno-exceptions 
# -msoft-float -fdata-sections -ffunction-sections -Wall $<$<COMPILE_LANGUAGE:CXX>:-Wno-register> -Wno-unknown-pragmas 
# -O${OPTI_FLAG} $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
# )

set(COMPILER_OPTIONS -mthumb -mcpu=cortex-m0 -mfpu=auto -mfloat-abi=soft -fno-exceptions 
-msoft-float -fdata-sections -ffunction-sections -Wall $<$<COMPILE_LANGUAGE:CXX>:-Wno-register> -Wno-unknown-pragmas 
-O${OPTI_FLAG} -Wl,--gc-sections $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti> 
)

add_subdirectory(libs)
add_subdirectory(src)

message(STATUS "System Processor      : ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS
    "BUILD CONFIGURATION: ${CMAKE_BUILD_TYPE}\n"
    "---optimization level : ${OPTI_FLAG}\n"
    "---libs:\n"
    "BUILD FLAGS:\n"
    "[STM32_DEFINES]    ${STM32_DEFINES}\n"
    "[compiler options]          ${COMPILER_OPTIONS}\n"
    # "[c++ flags]        ${CMAKE_CXX_FLAGS}\n"
    # "[asm flags]        ${CMAKE_ASM_FLAGS}\n"
    # "[ld flags]         ${CMAKE_EXE_LINKER_FLAGS}\n"
)
