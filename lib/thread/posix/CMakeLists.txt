# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# TODO: Issue #138 - GCC 6.2 fails to compile thread.cpp module
add_compile_options(-Wno-error=strict-aliasing)

add_library(thread_impl mutex.cpp semaphore.cpp thread.cpp)
target_include_directories(thread_impl PUBLIC export)
target_link_libraries(thread_impl PUBLIC utils types dbg pthread)
