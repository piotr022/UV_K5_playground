set(LIB_NAME orginal_fw)

set(ORGINAL_FW_BIN orginal_fw.bin)

set(ORGINAL_FW_VECTORS_BIN org_vectors.bin)
set(ORGINAL_FW_VECTORS_OBJ org_vectors.o)

set(ORGINAL_FW_REST0_BIN org_bootloader.bin)
set(ORGINAL_FW_REST0_OBJ org_bootloader.o)

set(ORGINAL_FW_REST1_BIN org_fw_rest.bin)
set(ORGINAL_FW_REST1_OBJ org_fw_rest.o)

add_custom_command(OUTPUT ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_REST0_BIN} ${ORGINAL_FW_REST1_BIN}
   COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/fw_decomposer.py 48 4096 ${CMAKE_CURRENT_SOURCE_DIR}/${ORGINAL_FW_BIN} ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_REST0_BIN} ${ORGINAL_FW_REST1_BIN}
   DEPENDS ${ORGINAL_FW_BIN}
   COMMENT "parsing orginal fw ${ORGINAL_FW_BIN}"
)

add_custom_command(OUTPUT ${ORGINAL_FW_VECTORS_OBJ} ${ORGINAL_FW_REST0_OBJ} ${ORGINAL_FW_REST1_OBJ}
   COMMAND arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.org_vectors ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_VECTORS_OBJ}
   COMMAND arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.org_bootloader ${ORGINAL_FW_REST0_BIN} ${ORGINAL_FW_REST0_OBJ}
   COMMAND arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.org_fw_rest ${ORGINAL_FW_REST1_BIN} ${ORGINAL_FW_REST1_OBJ}
   DEPENDS ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_REST0_BIN} ${ORGINAL_FW_REST1_BIN}
   COMMENT "generating vector table and fw object files"
)

add_custom_target(generate_obj_files
   DEPENDS ${ORGINAL_FW_VECTORS_OBJ} ${ORGINAL_FW_REST0_OBJ} ${ORGINAL_FW_REST1_OBJ}
)

add_library(${LIB_NAME} OBJECT)
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE C)
add_dependencies(${LIB_NAME} generate_obj_files)

target_sources(${LIB_NAME} PRIVATE
   ${ORGINAL_FW_VECTORS_OBJ}
   ${ORGINAL_FW_REST0_OBJ}
   ${ORGINAL_FW_REST1_OBJ}
)

target_link_libraries(${LIB_NAME}
   ${CMAKE_CURRENT_BINARY_DIR}/${ORGINAL_FW_VECTORS_OBJ}
   ${CMAKE_CURRENT_BINARY_DIR}/${ORGINAL_FW_REST0_OBJ}
   ${CMAKE_CURRENT_BINARY_DIR}/${ORGINAL_FW_REST1_OBJ}
)

set_target_properties(${LIB_NAME} PROPERTIES BOOTLOADER_BIN_PATH ${CMAKE_CURRENT_BINARY_DIR}/${ORGINAL_FW_REST0_BIN})

add_custom_target(${LIB_NAME}_bootloader_flash
   COMMAND openocd -f interface/cmsis-dap.cfg -f ${PROJECT_SOURCE_DIR}/openocd_scripts/dp32g030.cfg -c "write_image ${CMAKE_CURRENT_BINARY_DIR}/${ORGINAL_FW_REST0_BIN} 0" -c "halt" -c "shutdown"
   DEPENDS ${ORGINAL_FW_REST0_BIN}
)