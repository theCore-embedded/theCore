#include <target/pinconfig.hpp>
#include <platform/irq_manager.hpp>

// TODO: decide if to make as a class member or not
extern "C" void target_init(void)
{
    initialize_pins();
}

