/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/iostream.hpp>
#include <ecl/assert.h>

int main()
{
    ecl::cout << "Hello, Host World!" << ecl::endl;
    ecl_assert_msg(1 == 0, "This is an assert demonstration. Application will abort.");
    return 0;
}
