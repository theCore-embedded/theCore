add_library(bh1750 INTERFACE)
target_link_libraries(bh1750 INTERFACE types)
target_link_libraries(bh1750 INTERFACE thread)
target_include_directories(bh1750 INTERFACE export)