#include "shell_interpreter.hpp"
#include <iostream>
#include <cstdlib> // For system() function
#include <fstream> // For file operations
#include <sstream> // For string operations
#include <algorithm> // For std::transform

void ShellInterpreter::setInputFile(const std::string& filename) {
#ifdef _WIN32
        inputHandle = CreateFileA(filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
#else
        inputHandle = open(filename.c_str(), O_RDONLY);
#endif
}

void ShellInterpreter::setOutputFile(const std::string& filename) {
#ifdef _WIN32
        outputHandle = CreateFileA(filename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#else
        outputHandle = open(filename.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
}

#ifdef _WIN32
HANDLE ShellInterpreter::getDefaultInputHandle() {
    return GetStdHandle(STD_INPUT_HANDLE);
}

HANDLE ShellInterpreter::getDefaultOutputHandle() {
    return GetStdHandle(STD_OUTPUT_HANDLE);
}
#else

int ShellInterpreter::getDefaultInputHandle() {
    return STDIN_FILENO;
}

int ShellInterpreter::getDefaultOutputHandle() {
    return STDOUT_FILENO;
}
#endif


// Function to run a command in Windows environment
int ShellInterpreter::runCommandWindows(const std::string& command) {
#ifdef _WIN32
     SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    HANDLE hChildStdinRd, hChildStdinWr, hChildStdoutRd, hChildStdoutWr;

    if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0)) {
        std::cerr << "CreatePipe failed" << std::endl;
        return -1;
    }
    if (!SetHandleInformation(hChildStdinWr, HANDLE_FLAG_INHERIT, 0)) {
        std::cerr << "SetHandleInformation failed" << std::endl;
        return -1;
    }
    if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
        std::cerr << "CreatePipe failed" << std::endl;
        return -1;
    }
    if (!SetHandleInformation(hChildStdoutRd, HANDLE_FLAG_INHERIT, 0)) {
        std::cerr << "SetHandleInformation failed" << std::endl;
        return -1;
    }

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFOA siStartInfo;
    BOOL bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStdoutWr;
    siStartInfo.hStdOutput = hChildStdoutWr;
    siStartInfo.hStdInput = hChildStdinRd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    bSuccess = CreateProcessA(NULL,
                             const_cast<LPSTR>(command.c_str()),
                             NULL,
                             NULL,
                             TRUE,
                             0,
                             NULL,
                             NULL,
                             &siStartInfo,
                             &piProcInfo);
    if (!bSuccess) {
        std::cerr << "CreateProcess failed" << std::endl;
        return -1;
    }

    CloseHandle(hChildStdinRd);
    CloseHandle(hChildStdoutWr);

    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    DWORD dwExitCode = 0;
    GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return dwExitCode;
#else
    return -1;
#endif
}

// Function to run a command in UNIX environment
int ShellInterpreter::runCommandUnix(const std::string& command) {
#ifndef _WIN32
   // Parse the command string
    std::istringstream iss(command);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }

    // Check for pipe, input redirection, and output redirection
    int pipefd[2];
    bool hasPipe = false;
    bool hasInputRedirection = false;
    bool hasOutputRedirection = false;
    std::string inputFile, outputFile;
    for (size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i] == "|") {
            hasPipe = true;
            break;
        } else if (tokens[i] == "<") {
            hasInputRedirection = true;
            inputFile = tokens[i + 1];
            tokens.erase(tokens.begin() + i, tokens.end());
            break;
        } else if (tokens[i] == ">") {
            hasOutputRedirection = true;
            outputFile = tokens[i + 1];
            tokens.erase(tokens.begin() + i, tokens.end());
            break;
        }
    }

    // Execute the command
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "fork() failed" << std::endl;
        return -1;
    } else if (pid == 0) {
        // Child process
        if (hasInputRedirection) {
            int fd = open(inputFile.c_str(), O_RDONLY);
            dup2(fd, STDIN_FILENO);
            close(fd);
        }
        if (hasOutputRedirection) {
            int fd = open(outputFile.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (hasPipe) {
            // Implement pipe redirection if needed
            // You can use popen() or pipe() and fork() combination to handle pipes
        }

        // Execute the command
        execvp(tokens[0].c_str(), const_cast<char* const*>(tokens.data()));
        std::cerr << "execvp() failed" << std::endl;
        _exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
        return status;
    }
#else
    return -1;
#endif
}

// Function to run a command in the appropriate environment (Windows or UNIX)
int ShellInterpreter::runCommand(const std::string& command) {
#ifdef _WIN32
    return runCommandWindows(command);
#else
    return runCommandUnix(command);
#endif
}

// Helper function to split a string into tokens
std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}


// Function to search for a command in the paths and run it
int ShellInterpreter::executeCommand(const std::string& command) {
    #ifdef _WIN32
    return runCommandWindows(command);
#else
    return runCommandUnix(command);
#endif
}