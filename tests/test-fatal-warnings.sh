#!/usr/bin/env sh
test 0 -eq $(grep -c qwarn $1) || exit 1
