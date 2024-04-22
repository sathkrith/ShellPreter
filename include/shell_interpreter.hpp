#include <string>
#include <vector>
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
    int executeCommand(const std::string& command);
private:
    void setOutputFile(const std::string& filename);
    void setInputFile(const std::string& filename);
#ifdef _WIN32
    HANDLE inputHandle;
    HANDLE outputHandle;

    HANDLE getDefaultInputHandle();
    HANDLE getDefaultOutputHandle();
#else
    int inputHandle;
    int outputHandle;

    int getDefaultInputHandle();
    int getDefaultOutputHandle();
#endif
    int runCommandWindows(const std::string& command);
    int runCommandUnix(const std::string& command);
    int runCommand(const std::string& command);
};