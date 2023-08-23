set(NAME k5prog)
set(k5prog_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/k5prog)

project(${NAME})

set(CMAKE_C_COMPILER "gcc")
add_executable(${NAME} ${k5prog_SRC_DIR}/k5prog.c)
target_include_directories(${NAME} PUBLIC ${k5prog_SRC_DIR})
