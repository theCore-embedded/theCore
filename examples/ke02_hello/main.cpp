/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ecl/iostream.hpp>
#include <ecl/assert.h>
#include <platform/execution.hpp>

extern "C"
void board_init()
{

}

int main()
{
    ecl::cout << "Hello, Host World!" << ecl::endl << ecl::endl;

    ecl::cout << "Starting delay test..." << ecl::endl << ecl::endl;

    ecl::cout << "[--] Waiting for 2 seconds" << ecl::endl;
    ecl::spin_wait(2000);
    ecl::cout << "[--] Done waiting 2 seconds" << ecl::endl << ecl::endl;

    ecl::cout << "[----] Waiting for 4 seconds" << ecl::endl;
    ecl::spin_wait(4000);
    ecl::cout << "[----] Done waiting 4 seconds" << ecl::endl << ecl::endl;

    ecl::cout << "[--------] Waiting for 8 seconds" << ecl::endl;
    ecl::spin_wait(8000);
    ecl::cout << "[--------] Done waiting 8 seconds" << ecl::endl << ecl::endl;

    ecl_assert_msg(1 == 0, "This is an assert demonstration. Application will abort.");

    return 0;
}
