#include "shell_interpreter.hpp"
#include <iostream>
int main() {
    ShellInterpreter shell;
    std::string command;
    while (true) {
        std::cout << "$ ";
        if (!std::getline(std::cin, command) || command == "exit") {
            break;
        }
        shell.executeCommand(command);
    }
    return 0;
}