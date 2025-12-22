cmake_minimum_required(VERSION 3.10)
project(Astrolog C CXX)
enable_testing()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(
    . # For astrolog.h
)

# --- Core Astrolog Library (libastrolog) ---
# This will contain the calculation engine and core logic.
# The original astrolog.cpp contains main(), so we need to exclude it from the library build.
# The `astrolog.h` header defines many macros that control features; for the library,
# we might need to set some defaults or make them configurable in the future.

# Define source files for the core library
set(LIB_ASTROLOG_SRCS
    calc.cpp
    data.cpp
    express.cpp
    general.cpp
    intrpret.cpp
    io.cpp
    matrix.cpp
    placalc.cpp
    placalc2.cpp
    atlas.cpp # Added atlas.cpp
    swecl.cpp
    swedate.cpp
    swehouse.cpp
    swejpl.cpp
    swemmoon.cpp
    swemplan.cpp
    sweph.cpp
    swephlib.cpp
    xcharts0.cpp
    xcharts1.cpp
    xcharts2.cpp
    xdata.cpp
    xdevice.cpp
    xgeneral.cpp
    xscreen.cpp
    wdialog.cpp
    wdriver.cpp
)

add_library(astrolog_core STATIC ${LIB_ASTROLOG_SRCS})

# --- Original Astrolog Executable ---
# Build the original astrolog executable, linking it against the new core library.
# We need to ensure that the original astrolog.cpp (which contains main) is compiled.
# Other files are for UI/output generation.
set(ORIGINAL_ASTROLOG_SRCS
    astrolog.cpp
    charts0.cpp
    charts1.cpp
    charts2.cpp
    charts3.cpp
    xcharts0.cpp
    xcharts1.cpp
    xcharts2.cpp
    xdata.cpp
    xdevice.cpp
    xgeneral.cpp
    xscreen.cpp
    wdialog.cpp
    wdriver.cpp
)

add_executable(astrolog ${ORIGINAL_ASTROLOG_SRCS})
target_compile_options(astrolog PRIVATE -g) # Add debug symbols
target_link_libraries(astrolog PRIVATE astrolog_core)

# For X11 (if needed by original astrolog or specific charts)
find_package(X11 QUIET)
if(X11_FOUND)
    target_include_directories(astrolog PRIVATE ${X11_INCLUDE_DIR})
    target_link_libraries(astrolog PRIVATE ${X11_LIBRARIES} pthread dl m)
    message(STATUS "X11 found and linked to astrolog executable.")
else()
    message(STATUS "X11 not found. astrolog will be built without X11 support.")
    # If X11 is critical, you might add target_compile_definitions here to disable X11 features
endif()

# --- Astro-CLI Integration ---
# Add the astro-cli as a subdirectory build
add_subdirectory(astro-cli)
