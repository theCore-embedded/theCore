# Placeholder
add_library(sys STATIC
			sys.cpp
			# TODO: Mark as a hack
			export/sys/spi_cfgs.h
			export/sys/usart_cfgs.h
			export/sys/pin_cfgs.h
			export/iodev/iodev.hpp
			export/iowrap/iowrap.hpp)
target_include_directories(sys PUBLIC export)

target_link_libraries(sys libcpp)
target_link_libraries(sys target)
