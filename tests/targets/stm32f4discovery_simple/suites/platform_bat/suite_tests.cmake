# Suite tests
# TODO: Autogenerate this file

add_subdirectory(${TESTCASES_DIR}/gpio_bat/ ${CMAKE_CURRENT_BINARY_DIR}/gpio_bat_case/)
target_link_libraries(platform_bat gpio_bat_case)

# End of suite tests
