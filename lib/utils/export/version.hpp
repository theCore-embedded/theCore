/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief General the Core version library.
#ifndef LIB_VERSION_HPP_
#define LIB_VERSION_HPP_

extern "C" {
const char *ver_str;
const char *ver_commit;
const char *ver_branch;
const char *ver_url;
};

#endif // LIB_VERSION_HPP_
