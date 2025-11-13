#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>
#include <wait.h>
#include <unistd.h>
#include <csignal>

using namespace std;

// Signal counters
int sigint_count = 0, sigquit_count = 0, sigtstp_count = 0;

// Signal handlers increment corresponding counters
void sigint_handler(int sig)   { sigint_count++; }
void sigquit_handler(int sig)  { sigquit_count++; }
void sigtstp_handler(int sig)  { sigtstp_count++; }

// Parse user input into individual command arguments
void parse_args(string line, vector<string> &cmds){
    stringstream liness(line);
    string token;
    while (getline(liness, token, ' ')) {
        cmds.push_back(token);
    }
}

int main(void)
{
    vector<int> recentpids; // Stores the last 5 child process IDs

    // Setup signal handlers
    signal(SIGINT, sigint_handler);   // Ctrl+C
    signal(SIGQUIT, sigquit_handler); // Ctrl+\
    signal(SIGTSTP, sigtstp_handler); // Ctrl+Z

    while (1)
    {
        // Display shell prompt
        cout << "err799s$ ";
        string cmd;
        if (!getline(cin, cmd)) { // Handle Ctrl+D (EOF)
            cout << "\nExiting shell\n";
            exit(0);
        }

        // Skip empty input
        if (cmd.empty()) continue;

        cout <<"Received user commands: " << cmd << endl;

        // Handle built-in commands
        if (cmd == "help") {
            cout << "//*********************************************************\n";
            cout << "// Ethan Rockwood>\n";
            cout << "// CSC360/660 Operating Systems\n";
            cout << "// Project #1: My Shell - Writing Your Own Shell\n";
            cout << "// This shell supports the following commands: help, exit, history\n";
            cout << "//*********************************************************\n";
            continue;
        } else if(cmd == "exit") {
            // Show signal counts before exiting
            cout << "Number of interrupts received: SIGINT(Ctrl/C)  SIGQUIT(Ctrl/\\)  SIGTSTP(Ctrl/Z)\n";
            cout << "                                      " << sigint_count 
                 << "               " << sigquit_count 
                 << "               " << sigtstp_count << endl;
            cout << "Exiting shell" << endl;
            exit(0);
        } else if (cmd == "history") {
            if (recentpids.empty()) {
                cout << "No recent child PIDs." << endl;
                continue;
            }
            cout << "Recent child PIDs: ";
            for (auto it = recentpids.rbegin(); it != recentpids.rend(); ++it)
                cout << *it << " ";
            cout << endl;
            continue;
        }

        // Parse command and arguments
        vector<string> cmd_args;
        parse_args(cmd, cmd_args);

        // Fork a child process to execute the command
        int pid = fork();
        int status;

        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {
            // In child process: reset signals to default
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            // Convert arguments to char* array for execvp
            vector<char*> argv;
            for (auto &arg : cmd_args) argv.push_back(const_cast<char*>(arg.c_str()));
            argv.push_back(NULL);

            execvp(argv[0], argv.data()); // Execute command
            cout << "Error: command not found" << endl; // execvp failed
            exit(1);
        } else {
            // In parent process: store recent child PIDs (max 5)
            if (recentpids.size() >= 5) recentpids.erase(recentpids.begin());
            recentpids.push_back(pid);

            // Wait for child to finish
            waitpid(pid, &status, 0);
        }
    }
}
