# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Add kernel to the build if it is really requested
if (DEFINED CONFIG_OS)
    if (CONFIG_OS STREQUAL "freertos")
		add_subdirectory(${CONFIG_OS})
	endif()
endif()
