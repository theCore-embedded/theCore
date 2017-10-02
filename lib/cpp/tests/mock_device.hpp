/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOCK_DEVICE_HPP
#define MOCK_DEVICE_HPP

#include <iostream>
#include <CppUTestExt/MockSupport.h>

class mock_device
{
public:
    size_t idx = 0;
    std::string test_input_str;
    bool error_mode = false;

    ssize_t write(const uint8_t *buf, size_t size)
    {
        if (error_mode) {
            return mock().actualCall("write").returnIntValue();
        }

        test_input_str.append(reinterpret_cast<const char*>(buf),
                              size);

        return size;

    }

    ssize_t read(uint8_t *buf, size_t size)
    {
        if (error_mode) {
            return mock().actualCall("read").returnIntValue();
        }

        size_t chars_left = test_input_str.size() - idx;

        if (!chars_left) {
            return 0;
        }

        size_t to_copy = size < chars_left ? size : chars_left;

        for (size_t i = 0; i < to_copy; ++i) {
            buf[i] = test_input_str[i + idx];
        }

        idx += to_copy;
        return to_copy;
    }
};

#endif
