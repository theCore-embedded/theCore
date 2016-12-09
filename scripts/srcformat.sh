#!/bin/sh

astyle  --style=1tbs \
        --indent=spaces=4 \
        --indent-switches \
        --indent-preproc-define \
        --indent-col1-comments \
        --pad-oper \
        --pad-header \
        --align-pointer=name \
        --align-reference=name \
        --add-brackets \
        --keep-one-line-blocks \
        --convert-tabs \
        --close-templates \
        --max-code-length=80 \
        $@
