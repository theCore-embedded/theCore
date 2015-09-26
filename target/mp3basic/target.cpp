#include <target/pinconfig.hpp>

// TODO: decide if to make as a class member or not
extern "C" void target_init(void)
{
	initializePins();
}

