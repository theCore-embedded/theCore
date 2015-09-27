#include <platform/usart_device.hpp>
#include <platform/irq_manager.hpp>

// TODO: move it elsewhere
std::function< void() > IRQ_manager::m_handlers[82];


// TODO: decide if to make as a class member or not
extern "C" void platform_init()
{
}
