/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief Collection of kernel stubs.
//! This module suppplies the dummy kernel main routine in case if kernel
//! isn't present or it's main routine shouldn't be called when initiliazing
//! the Core.

extern int early_main();

// Do nothing if kernel isn't used
void kernel_main()
{
    early_main();
}
