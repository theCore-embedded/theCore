/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief FreeRTOSS mock header

#ifndef FREERTOS_FREERTOS_MOCK_H_
#define FREERTOS_FREERTOS_MOCK_H_

#include <limits.h>

#define pdTRUE                  true
#define pdFALSE                 false
#define portMAX_DELAY           INT_MAX
#define portTICK_PERIOD_MS      1

#endif // FREERTOS_FREERTOS_MOCK_H_
