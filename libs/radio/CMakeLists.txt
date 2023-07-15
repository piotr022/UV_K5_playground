set(NAME radio)

add_library(${NAME} INTERFACE)

target_include_directories(${NAME} INTERFACE
	.
)


target_sources(${NAME} PRIVATE
   radio.cpp
)

target_link_libraries(${NAME} INTERFACE
   uv_k5_system
)


