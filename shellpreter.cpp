#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

class ShellInterpreter {
public:
    ShellInterpreter() : inputHandle(getDefaultInputHandle()), outputHandle(getDefaultOutputHandle()) {}

    void execute(const std::string& command) {
        // Parse command for piping, redirection, and background process
        std::istringstream iss(command);
        std::string token;
        std::vector<std::string> tokens;
        while (iss >> token) {
            if (token == "|") {
                executeCommand(tokens);
                tokens.clear();
            } else if (token == "<") {
                iss >> token;
                setInputFile(token);
            } else if (token == ">") {
                iss >> token;
                setOutputFile(token);
            } else {
                tokens.push_back(token);
            }
        }
        if (!tokens.empty()) {
            executeCommand(tokens);
        }
    }

    void setInputFile(const std::string& filename) {
#ifdef _WIN32
        inputHandle = CreateFileA(filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
        inputHandle = open(filename.c_str(), O_RDONLY);
#endif
    }

    void setOutputFile(const std::string& filename) {
#ifdef _WIN32
        outputHandle = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
        outputHandle = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
    }

private:
#ifdef _WIN32
    HANDLE inputHandle;
    HANDLE outputHandle;

    HANDLE getDefaultInputHandle() {
        return GetStdHandle(STD_INPUT_HANDLE);
    }

    HANDLE getDefaultOutputHandle() {
        return GetStdHandle(STD_OUTPUT_HANDLE);
    }
#else
    int inputHandle;
    int outputHandle;

    int getDefaultInputHandle() {
        return STDIN_FILENO;
    }

    int getDefaultOutputHandle() {
        return STDOUT_FILENO;
    }
#endif

    void executeCommand(const std::vector<std::string>& tokens) {
        // Implementation remains the same
    }
};

int main() {
    ShellInterpreter shell;
    std::string command;
    while (true) {
        std::cout << "$ ";
        if (!std::getline(std::cin, command) || command == "exit") {
            break;
        }
        shell.execute(command);
    }
    return 0;
}