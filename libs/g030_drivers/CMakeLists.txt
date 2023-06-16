#Inertial github:piotr022
set(NAME mcu_drivers)

add_library(${NAME} INTERFACE)

target_include_directories(${NAME} INTERFACE
   ./include
   
   ./CMSIS/Device/ST/STM32G0xx/Include
   ./CMSIS/Include
   ./STM32G0xx_HAL_Driver/Inc
)

target_compile_definitions(${NAME} INTERFACE ${STM32_DEFINES})