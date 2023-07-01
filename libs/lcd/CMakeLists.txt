set(NAME lcd)

add_library(${NAME} STATIC)

target_include_directories(${NAME} INTERFACE
	.
)

target_include_directories(${NAME} PRIVATE
	.
)

target_sources(${NAME} PRIVATE
   lcd.cpp
)

target_link_libraries(${NAME}
   uv_k5_system
)

target_compile_options(${NAME} PRIVATE ${COMPILER_OPTIONS})

