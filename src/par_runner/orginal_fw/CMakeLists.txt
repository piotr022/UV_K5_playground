set(LIB_NAME orginal_fw)

set(ORGINAL_FW_BIN orginal_fw.bin)

set(ORGINAL_FW_VECTORS_BIN org_vectors.bin)
set(ORGINAL_FW_VECTORS_OBJ org_vectors.o)

set(ORGINAL_FW_REST_BIN org_rest.bin)
set(ORGINAL_FW_REST_OBJ org_rest.o)

add_custom_command(OUTPUT ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_REST_BIN}
   COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/fw_decomposer.py 48 ${CMAKE_CURRENT_SOURCE_DIR}/${ORGINAL_FW_BIN} ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_REST_BIN}
   DEPENDS ${ORGINAL_FW_BIN}
   COMMENT "parsing orginal fw ${ORGINAL_FW_BIN}"
)

add_custom_command(OUTPUT ${ORGINAL_FW_VECTORS_OBJ} ${ORGINAL_FW_REST_OBJ}
   COMMAND arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.org_vectors ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_VECTORS_OBJ}
   COMMAND arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm --rename-section .data=.org_fw ${ORGINAL_FW_REST_BIN} ${ORGINAL_FW_REST_OBJ}
   DEPENDS ${ORGINAL_FW_VECTORS_BIN} ${ORGINAL_FW_REST_BIN}
   COMMENT "generating vector table and fw object files"
)

add_custom_target(generate_obj_files
   DEPENDS ${ORGINAL_FW_VECTORS_OBJ} ${ORGINAL_FW_REST_OBJ}
)

add_library(${LIB_NAME} OBJECT)
set_target_properties(${LIB_NAME} PROPERTIES LINKER_LANGUAGE C)
add_dependencies(${LIB_NAME} generate_obj_files)

target_sources(${LIB_NAME} PRIVATE
   ${ORGINAL_FW_VECTORS_OBJ}
   ${ORGINAL_FW_REST_OBJ}
)

target_link_libraries(${LIB_NAME}
   ${CMAKE_CURRENT_BINARY_DIR}/${ORGINAL_FW_VECTORS_OBJ}
   ${CMAKE_CURRENT_BINARY_DIR}/${ORGINAL_FW_REST_OBJ}
)