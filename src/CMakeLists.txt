add_library(BH1750 STATIC
    BH1750.cpp
)

target_include_directories(BH1750 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(BH1750 PUBLIC
    pico_stdlib
    hardware_i2c
    hardware_gpio
)
