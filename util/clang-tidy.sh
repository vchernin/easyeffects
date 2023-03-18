#!/usr/bin/env bash

# write a custom script for ease of use, meson clang-tidy target can't do fixes, and also has somewhat broken threading

# see https://github.com/platisd/clang-tidy-pr-comments

# hardcoded build directory for now, until I figure out best way to set up these scripts
# this depends on a hack in the python wrapper script to add the header files even if they are not in the compilation database, otherwise header files are not checked at all
python3 ./util/run-clang-tidy.py -p build-devel -fix -config-file=.clang-tidy src/*.cpp include/*.hpp
