# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# CppCheck runner

set(CMD "cppcheck \
	${CPPCHECK_TEMPLATE_ARG} ${CPPCHECK_ARGS} ${INCLUDES} ${FILES}")

exec_program(${CMD})
