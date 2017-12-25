/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#cmakedefine THECORE_VER4GIT_FOUND @THECORE_VER4GIT_FOUND@

namespace ecl
{
#ifdef THECORE_VER4GIT_FOUND

#if @PROJECT_GIT_DIRTY@
#define DIRTY "-dirty"
#else
#define DIRTY ""
#endif

const char *ver_str = "@PROJECT_NAME@ v@PROJECT_VERSION@ " "@PROJECT_GIT_SHORT@" DIRTY;
const char *ver_commit = "@PROJECT_GIT_COMMIT@";
const char *ver_branch = "@PROJECT_GIT_BRANCH@";
const char *ver_url = "@PROJECT_GIT_URL@";

#else // ! THECORE_VER4GIT_FOUND

const char *ver_str     = "v0.3.0";
const char *ver_commit  = "";
const char *ver_branch  = "";
const char *ver_url     = "";

#endif // THECORE_VER4GIT_FOUND
} // namespace ecl
