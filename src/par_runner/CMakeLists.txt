set(NAME par_runner)
set(MCU_TARGET_FILES_DIR ../mcu_target_common)

add_subdirectory(orginal_fw)

add_executable(${NAME}  
${MCU_TARGET_FILES_DIR}/stm32f030xc.s   
main.cpp
)

target_link_libraries(${NAME}
        f030_drivers
        orginal_fw
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
        ${COMPILER_OPTIONS}
)

target_link_options(${NAME} PRIVATE
        #-print-multi-lib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/memory.ld
        -mcpu=cortex-m0
        -mthumb
        -mfpu=auto
        -mfloat-abi=soft
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