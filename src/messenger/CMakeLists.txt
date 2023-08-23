set(NAME messenger)
set(MCU_TARGET_FILES_DIR ../mcu_target_common)

add_executable(${NAME}  
        main.cpp
        hardware/hardware.cpp
        dp32g030.s
)

target_link_libraries(${NAME}
        orginal_fw
        uv_k5_system
        lcd
        keyboard
        radio
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
        -flto
)

target_link_options(${NAME} PRIVATE
        #-print-multi-lib
        -T ${CMAKE_CURRENT_SOURCE_DIR}/memory.ld
        -flto
        -mcpu=cortex-m0
        -mthumb
        -specs=nano.specs
        # -static-libgcc# -lc
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

get_target_property(BOOTLOADER_BIN_PATH orginal_fw BOOTLOADER_BIN_PATH)
add_custom_command(TARGET ${NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "generating full binary with bootloader to ${NAME}_with_bootloader.bin"
        COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/fw_merger.py ${BOOTLOADER_BIN_PATH} ${NAME}.bin ${NAME}_with_bootloader.bin
)

add_custom_target(${NAME}_flash
	COMMAND openocd -f interface/cmsis-dap.cfg -f ${PROJECT_SOURCE_DIR}/openocd_scripts/dp32g030:.cfg -c "write_image ${PROJECT_SOURCE_DIR}/build/src/rssi_printer/rssi_printer.bin 0x1000" -c "halt" -c "shutdown"
	DEPENDS ${NAME}
)

add_custom_target(${NAME}_encoded
	COMMAND python ${PROJECT_SOURCE_DIR}/tools/fw_tools/python-utils/fw_pack.py ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bin ${CMAKE_CURRENT_SOURCE_DIR}/../orginal_fw/k5_26_encrypted_18to1300MHz.ver.bin ${CMAKE_CURRENT_BINARY_DIR}/${NAME}_encoded.bin
	DEPENDS ${NAME}
)

if(NOT DEFINED ${SERIAL_PORT})
	set(SERIAL_PORT /dev/ttyUSB0)
endif()

add_custom_target(${NAME}_flash_uart
	COMMAND ${CMAKE_BINARY_DIR}/tools/k5prog -F -p ${SERIAL_PORT} -b ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.bin -Y -Y -Y
	DEPENDS ${NAME}_encoded
	DEPENDS k5prog
)
