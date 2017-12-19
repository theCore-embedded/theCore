/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//! \file
//! \brief General the Core version library.
#ifndef LIB_VERSION_HPP_
#define LIB_VERSION_HPP_

namespace ecl
{

//! Returns constant version string of theCore
static inline const char *version()
{
    extern const char *ver_str;
    return ver_str;
}

//! Returns string with SHA of git commit
static inline const char *git_commit()
{
    extern const char *ver_commit;
    return ver_commit;
}

//! Returns git branch
static inline const char *git_branch()
{
    extern const char *ver_branch;
    return ver_branch;
}

//! Returns remote URL of theCore
static inline const char *git_url()
{
    extern const char *ver_url;
    return ver_url;
}

} // namespace ecl

#endif // LIB_VERSION_HPP_
