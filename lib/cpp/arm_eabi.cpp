/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Required for ARM architecture
extern "C" int  __aeabi_atexit(void* object, void (*destroyer)(void*), void* dso_handle)
{
    (void) object;
    (void) destroyer;
    (void) dso_handle;
    return 0;
}
