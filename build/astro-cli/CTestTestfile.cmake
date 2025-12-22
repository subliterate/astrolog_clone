# CMake generated Testfile for 
# Source directory: /home/noddy/astrolog_clone/astro-cli
# Build directory: /home/noddy/astrolog_clone/build/astro-cli
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(date_util_unit_test "/home/noddy/astrolog_clone/build/astro-cli/test_date_util")
set_tests_properties(date_util_unit_test PROPERTIES  _BACKTRACE_TRIPLES "/home/noddy/astrolog_clone/astro-cli/CMakeLists.txt;32;add_test;/home/noddy/astrolog_clone/astro-cli/CMakeLists.txt;0;")
add_test(smoke_test "/usr/bin/cmake" "--build" "." "--target" "smoke_test_target")
set_tests_properties(smoke_test PROPERTIES  ENVIRONMENT "ASTROLOG_BIN=/home/noddy/astrolog_clone/build/astrolog;ASTROLOG=/home/noddy/astrolog_clone/ephem" WORKING_DIRECTORY "/home/noddy/astrolog_clone/build" _BACKTRACE_TRIPLES "/home/noddy/astrolog_clone/astro-cli/CMakeLists.txt;41;add_test;/home/noddy/astrolog_clone/astro-cli/CMakeLists.txt;0;")
