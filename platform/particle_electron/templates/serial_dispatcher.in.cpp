/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Particle Serial dispatcher template

#include <aux/serial_cfgs.hpp>

/*[[[cog
import cog
]]]*/

//[[[end]]]

namespace ecl
{

void serial_tx_dispatch()
{
    // To reduce interrupt latency, interrupts are disabled per device access.

    /*[[[cog

    template_tx = '''

    // Prevent context switching while accessing shared driver
    noInterrupts();
    using uart_inst = uart_bus<uart_device::serial%d>;

    if (uart_inst::m_status & uart_inst::tx_done) {
        uart_inst::serial_event(ecl::bus_channel::tx);
    }
    interrupts();

    '''

    if 'SERIALS' in globals():
        serials = SERIALS.split(' ')

        for serial in serials:
            num = int(serial)
            cog.outl(template_tx % num)

    ]]]*/

    //[[[end]]]
}

template<uart_device dev>
void serial_rx_dispatch()
{
    noInterrupts(); // Prevent context switching while accessing shared driver

    // Pass event only if RX is requested.
    if (uart_bus<dev>::m_status & uart_bus<dev>::rx_pend) {
        uart_bus<dev>::serial_event(ecl::bus_channel::rx);
    } else {
        // Skip symbol
        (void)uart_bus<dev>::get_serial().read();
    }

    interrupts();
}

} // namespace ecl

// Overriding Particle event handlers for RX stream

/*[[[cog

template_rx = '''
void %s()
{
    ecl::serial_rx_dispatch<ecl::uart_device::serial%d>();
}
'''

devmap = {
    0 : 'serialEvent',
    1 : 'serialEvent1',
    2 : 'serialEvent2',
    4 : 'serialEvent4',
    5 : 'serialEvent5'
}

if 'SERIALS' in globals():
    serials = SERIALS.split(' ')
    cog.msg("Serials enabled: " + str(serials))

    for serial in serials:
        num = int(serial)
        cog.outl(template_rx % (devmap[num], num))

]]]*/

//[[[end]]]

