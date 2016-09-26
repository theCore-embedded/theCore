# Suite tests
# TODO: Autogenerate this file

add_subdirectory(${TESTCASES_DIR}/unity_demo/ ${CMAKE_CURRENT_BINARY_DIR}/unity_demo_case/)
target_link_libraries(unity_demo unity_demo_case)

# End of suite tests
