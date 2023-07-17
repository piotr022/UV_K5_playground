set(NAME views)

add_library(${NAME} INTERFACE)

target_include_directories(${NAME} INTERFACE
	.
)

target_sources(${NAME} PRIVATE
   views.cpp
)

target_link_libraries(${NAME} INTERFACE
   uv_k5_system
   lcd
   keyboard
   radio
)

# target_compile_options(${NAME} PRIVATE ${COMPILER_OPTIONS})

