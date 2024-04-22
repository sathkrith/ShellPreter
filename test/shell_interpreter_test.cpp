#include <gtest/gtest.h>
#include "shellpreter.h"

// Test fixture for shell interpreter tests
class ShellInterpreterTest : public ::testing::Test {
protected:
    // Common objects or variables accessible to all test cases
    ShellInterpreter shell;
};


// Test case for executing a simple command
TEST_F(ShellInterpreterTest, ExecuteSimpleCommand) {
    // Test executing a simple command
    EXPECT_EQ(shell.executeCommand("ls"), 0);
}

// Test case for executing a command with output redirection
TEST_F(ShellInterpreterTest, ExecuteCommandWithRedirection) {
    // Test executing a command with output redirection
    EXPECT_EQ(shell.executeCommand("ls > test_output.txt"), 0);

    // Check if the output file exists
    std::ifstream outputFile("test_output.txt");
    EXPECT_TRUE(outputFile.good());
    outputFile.close();

    // Clean up: remove the output file
    std::remove("test_output.txt");
}

// Test case for executing a command with input redirection
TEST_F(ShellInterpreterTest, ExecuteCommandWithInputRedirection) {
    // Create a test input file
    std::ofstream inputFile("test_input.txt");
    inputFile << "Hello, World!" << std::endl;
    inputFile.close();

    // Test executing a command with input redirection
    EXPECT_EQ(shell.executeCommand("cat < test_input.txt"), 0);

    // Clean up: remove the input file
    std::remove("test_input.txt");
}

// Test case for executing a command with piping
TEST_F(ShellInterpreterTest, ExecuteCommandWithPiping) {
    // Test executing a command with piping
    EXPECT_EQ(shell.executeCommand("ls | wc -l"), 0);
}

// Main function to run the tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}