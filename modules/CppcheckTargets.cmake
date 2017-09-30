# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# - Run cppcheck on c++ source files as a custom target and a test
#
#  include(CppcheckTargets)
#  add_cppcheck(<target-name> [UNUSED_FUNCTIONS] [STYLE] [POSSIBLE_ERROR] [FORCE] [FAIL_ON_WARNINGS]) -
#    Create a target to check a target's sources with cppcheck and the indicated options
#  add_cppcheck_sources(<target-name> [UNUSED_FUNCTIONS] [STYLE] [POSSIBLE_ERROR] [FORCE] [FAIL_ON_WARNINGS]) -
#    Create a target to check standalone sources with cppcheck and the indicated options
#
# Requires these CMake modules:
#  Findcppcheck
#
# Requires CMake 2.6 or newer (uses the 'function' command)
#
# Original Author:
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2010.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)

if(__add_cppcheck)
    return()
endif()
set(__add_cppcheck YES)

if(NOT CPPCHECK_FOUND)
    find_package(cppcheck QUIET)
endif()

if(CPPCHECK_FOUND)
    if(NOT TARGET all_cppcheck)
        if(${CONFIG_RUN_CPPCHECK_DURING_BUILD})
            add_custom_target(all_cppcheck ALL)
        else()
            add_custom_target(all_cppcheck ALL)
            set_target_properties(all_cppcheck PROPERTIES EXCLUDE_FROM_ALL TRUE)
        endif()
    endif()
endif()

function(add_cppcheck _name)
    if(NOT TARGET ${_name})
        msg_fatal("add_cppcheck given a target name that does not exist: '${_name}' !")
    endif()
    if(CPPCHECK_FOUND)
        set(_cppcheck_args)
        list(APPEND _cppcheck_args
                --enable=performance --enable=portability
                --enable=missingInclude --enable=information)

        list(FIND ARGN UNUSED_FUNCTIONS _unused_func)
        if("${_unused_func}" GREATER "-1")
            list(APPEND _cppcheck_args ${CPPCHECK_UNUSEDFUNC_ARG})
        endif()

        list(FIND ARGN STYLE _style)
        if("${_style}" GREATER "-1")
            list(APPEND _cppcheck_args ${CPPCHECK_STYLE_ARG})
        endif()

        list(FIND ARGN POSSIBLE_ERROR _poss_err)
        if("${_poss_err}" GREATER "-1")
            list(APPEND _cppcheck_args ${CPPCHECK_POSSIBLEERROR_ARG})
        endif()

        list(FIND ARGN FORCE _force)
        if("${_force}" GREATER "-1")
            list(APPEND _cppcheck_args "--force")
        endif()

        list(FIND _input FAIL_ON_WARNINGS _fail_on_warn)
        if("${_fail_on_warn}" GREATER "-1")
            list(APPEND
                    CPPCHECK_FAIL_REGULAR_EXPRESSION
                    ${CPPCHECK_WARN_REGULAR_EXPRESSION})
            list(REMOVE_AT _input ${_unused_func})
        endif()

		get_target_property(_cppcheck_sources "${_name}" INTERFACE_SOURCES)

        # Try get 'real' sources
        if(NOT _cppcheck_sources)
            get_target_property(_cppcheck_sources "${_name}" SOURCES)
            set(_real_library YES)
        endif()

            set(_files)
		foreach(_source ${_cppcheck_sources})
			get_source_file_property(_cppcheck_lang "${_source}" LANGUAGE)
			get_source_file_property(_cppcheck_loc "${_source}" LOCATION)
			if(("${_cppcheck_lang}" MATCHES "CXX")
					OR ("${_cppcheck_lang}" MATCHES "C"))
				list(APPEND _files "${_cppcheck_loc}")
			endif()
		endforeach()

		add_custom_target(${_name}_cppcheck)
		set_target_properties(${_name}_cppcheck PROPERTIES EXCLUDE_FROM_ALL TRUE)

		add_dependencies(all_cppcheck ${_name}_cppcheck)
		set(white "")

        if(_real_library)
            # TODO: comment
            set(_includes
                "-I$<JOIN:$<TARGET_PROPERTY:${_name},INCLUDE_DIRECTORIES>, -I>")
        else()
            set(_includes
                    "-I$<JOIN:$<TARGET_PROPERTY:${_name},INTERFACE_INCLUDE_DIRECTORIES>, -I>")
        endif()

		add_custom_command(TARGET
			${_name}_cppcheck
			PRE_BUILD
			COMMAND
			#${CPPCHECK_EXECUTABLE}
			#echo "${CPPCHECK_TEMPLATE_ARG}"
			cmake
			ARGS
			-DCPPCHECK_QUIET_ARG="${CPPCHECK_QUIET_ARG}"
			-DCPPCHECK_TEMPLATE_ARG="${CPPCHECK_TEMPLATE_ARG}"
			-DCPPCHECK_ARGS="${_cppcheck_args}"
			-DINCLUDES=${_includes}
			-DFILES="${_files}"
			-P ${CORE_DIR}/modules/Cppcheck.cmake
			WORKING_DIRECTORY
			"${CMAKE_CURRENT_SOURCE_DIR}"
			COMMENT
			"${_name}_cppcheck: Running cppcheck on target ${_name}... "
			)
	endif()

endfunction()
