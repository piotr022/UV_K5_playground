set(NAME ocl_loader)
set(MCU_TARGET_FILES_DIR ../mcu_target_common)


add_executable(${NAME}  
        main.c
)

target_link_libraries(${NAME}
   mcu_drivers
)

target_include_directories(${NAME} PUBLIC
        ./
        Drivers/CMSIS/Device/ST/STM32G0xx/Include
        Drivers/CMSIS/DSP/Include
        Drivers/CMSIS/Include
)

target_compile_definitions(${NAME} PRIVATE
        ${STM32_DEFINES}
        $<$<CONFIG:Debug>:DEBUG_ENABLED>
)

target_compile_options(${NAME} PRIVATE
        -mthumb -mcpu=cortex-m0plus -mfpu=auto -mfloat-abi=soft -fno-exceptions 
        -msoft-float -Wall $<$<COMPILE_LANGUAGE:CXX>:-Wno-register> -Wno-unknown-pragmas 
        -O${OPTI_FLAG} $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
        $<$<CONFIG:Debug>:--debug -DDEBUG>
        -ffunction-sections -fdata-sections -gdwarf-3
        -nostartfiles -fno-strict-aliasing
)

target_link_options(${NAME} PRIVATE
        #-print-multi-lib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/ram.lds #${CMAKE_SOURCE_DIR}/
        -mcpu=cortex-m0plus
        -mthumb
        -mfpu=auto
        -mfloat-abi=soft #lets go fpu
        -specs=nosys.specs
        -specs=nano.specs
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        -Wl,--print-memory-usage
        -Wstack-usage=128
        -Wno-register
)

add_custom_command(TARGET ${NAME}
        POST_BUILD
        COMMAND arm-none-eabi-size ${NAME}
)
#convert to hex
add_custom_command(TARGET ${NAME}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${NAME} ${NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${NAME} ${NAME}.bin
)