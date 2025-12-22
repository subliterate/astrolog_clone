# CMakeLists.txt for astro-cli
enable_testing()

# Define the executable for astro-cli
add_executable(astro
    src/main.c
    src/command.c
    src/config.c
    src/engine.c
    src/parser.c
    src/date_util.c
)

# Link astro-cli to the core astrolog library
target_link_libraries(astro PRIVATE astrolog_core)

# Include directories for astro-cli
target_include_directories(astro PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Define unit tests
# date_util test
add_executable(test_date_util
    tests/test_date_util.c
    src/date_util.c # Link directly for simple unit test
)
target_include_directories(test_date_util PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
add_test(NAME date_util_unit_test COMMAND test_date_util)

# Add the smoke test as a custom target for integration testing
add_custom_target(smoke_test_target
    COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/tests/smoke.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# Add the CTest entry for the smoke test, running the custom target
add_test(NAME smoke_test
         COMMAND ${CMAKE_COMMAND} --build . --target smoke_test_target
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# Set environment variable for the smoke test to find astrolog
set_tests_properties(smoke_test PROPERTIES ENVIRONMENT "ASTROLOG_BIN=${CMAKE_BINARY_DIR}/astrolog;ASTROLOG=${CMAKE_SOURCE_DIR}/ephem")

# Ensure astro is built before running smoke test
add_dependencies(smoke_test_target astro)
