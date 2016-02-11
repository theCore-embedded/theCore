# CppCheck runner

set(CMD "cppcheck \
	${CPPCHECK_TEMPLATE_ARG} ${CPPCHECK_ARGS} ${INCLUDES} ${FILES}")

exec_program(${CMD})
