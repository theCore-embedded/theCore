# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

add_subdirectory(${PLATFORM_NAME})

# Order matters. Adding `common` module after a platform allows to reference
# platform info, like IRQ count, from inner common modules.
add_subdirectory(common)
