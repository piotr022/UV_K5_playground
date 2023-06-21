#Inertial github:piotr022
set(NAME f030_drivers)

add_library(${NAME} INTERFACE)

target_include_directories(${NAME} INTERFACE
   ./include
   
   ./CMSIS/Device/ST/STM32F0xx/Include
   ./CMSIS/Include
   ./STM32F0xx_HAL_Driver/Inc
)

target_compile_definitions(${NAME} INTERFACE ${STM32_DEFINES})