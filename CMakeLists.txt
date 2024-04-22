cmake_minimum_required(VERSION 3.10)
project(ShellInterpreter)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable target
add_executable(shell_interpreter 
    src/main.cpp
    src/shell_interpreter.cpp
)

# Set include directories
target_include_directories(shell_interpreter PRIVATE include)

# Determine platform-specific commands
if(WIN32)
    # Windows commands
    set(RUN_COMMAND ".\\shell_interpreter.exe")
    set(TEST_COMMAND "ctest.exe")
else()
    # UNIX commands
    set(RUN_COMMAND "./shell_interpreter")
    set(TEST_COMMAND "ctest")
endif()

# Add custom target to run the program
add_custom_target(run
    COMMAND ${RUN_COMMAND}
    DEPENDS shell_interpreter
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Add custom target to run tests
add_custom_target(test
    COMMAND ${TEST_COMMAND}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)