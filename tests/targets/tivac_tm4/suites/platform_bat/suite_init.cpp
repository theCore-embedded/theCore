/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Suite initialization of the TI TIVAC (TM4C) platform BAT suite.

#include <aux/generated.hpp>

void suite_board_init()
{
    gpio_init_generated();
}
