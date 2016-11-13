//! \file
//! \brief Console class implementation for the host platform
#include "platform/console.hpp"

namespace ecl
{

// Static member declarations

const uint8_t *platform_console::m_tx;
uint8_t *platform_console::m_rx;
size_t platform_console::m_tx_size;
size_t platform_console::m_rx_size;
std::aligned_storage_t<sizeof(typename platform_console::handler_fn),
    alignof(typename platform_console::handler_fn)> platform_console::m_fn_storage;

}
