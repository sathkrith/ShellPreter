# Shell Interpreter

This project implements a shell interpreter in C++ that supports various features such as command execution, piping, and redirection.

## Usage

### Compilation

To compile the project, follow these steps:

1. Create a build directory:
mkdir build
cd build
2. Generate build files using CMake:
cmake ..
3. Build the project:
cmake --build .

### Running the Program

Once the project is compiled, you can run the shell interpreter using the following command:

- On Windows:
.\shell_interpreter.exe
- On UNIX:
./shell_interpreter

### Running Tests

To run tests, use the following command:

- On Windows:
ctest.exe
- On UNIX:
ctest


### Additional Notes

- The project uses CMake for building, which provides cross-platform support for compiling, running, and testing the code.
- `CMakeLists.txt` file in the root directory defines the project structure and build instructions.
- Ensure that CMake and a C++ compiler are installed on your system before compiling the project.

