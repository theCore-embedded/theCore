#!/usr/bin/env sh

script="$0"
basename="$(dirname $script)"

uncrustify -c "${basename}/srcformat.cfg" --replace $@
