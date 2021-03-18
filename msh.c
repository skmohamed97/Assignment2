#include "msh.h"

/*
msh.c
Awale Ahmed cssc2141 820347457, Said Mohamed cssc2120 819274138
CS570 Summer 2020
Assignment #2, microshell 
*/

void prompt()
{
    printf("\ncs2141%% ");
    fflush(stdin);
    fflush(stdout);
}

void pipes(char *args[])
{
    int fd1[2];
    int fd2[2];
    int num_commands = 0;
    char *command[256];
    pid_t pid;
    int end = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;

    while (args[l] != NULL) //loop through user input and count cmds
    {
        if (strcmp(args[l], "|") == 0)
        {
            num_commands++;
        }
        l++;
    }
    num_commands++; //increment for final command

    while (args[j] != NULL && end != 1)
    {
        k = 0;
        while (strcmp(args[j], "|") != 0)
        {
            command[k] = args[j];
            j++;
            if (args[j] == NULL)
            {
                end = 1;
                k++;
                break;
            }
            k++;
        }
        command[k] = NULL; //holds only commands, no pipe symbols
        j++;

        if (i % 2 != 0)
        { //pipe file descriptors depending on iteration
            pipe(fd1);
        }
        else
        {
            pipe(fd2);
        }

        pid = fork(); //create child process

        if (pid == -1)
        { //if failed to create child process
            if (i != num_commands - 1)
            {
                if (i % 2 != 0)
                {
                    close(fd1[1]);
                }
                else
                {
                    close(fd2[1]);
                }
            }
            return;
        }
        if (pid == 0)
        {
            //depending on what command we are in, we will either replace the std input
            //or std output or both

            if (i == 0)
            { //if first cmd, only change std output
                dup2(fd2[1], STDOUT_FILENO);
            }

            else if (i == num_commands - 1)
            { //if final cmd, only change std input
                if (num_commands % 2 != 0)
                {
                    dup2(fd1[0], STDIN_FILENO);
                }
                else
                {
                    dup2(fd2[0], STDIN_FILENO);
                }
            }
            else
            {
                if (i % 2 != 0)
                { //if middle cmd, change both std input and output
                    dup2(fd2[0], STDIN_FILENO);
                    dup2(fd1[1], STDOUT_FILENO);
                }
                else
                {
                    dup2(fd1[0], STDIN_FILENO);
                    dup2(fd2[1], STDOUT_FILENO);
                }
            }
            if (execvp(command[0], command) == -1)
            {
                kill(getpid(), SIGTERM);
            }
        }
        
        //close file descriptors 
        if (i == 0)
        {
            close(fd2[1]);
        }
        else if (i == num_commands - 1)
        {
            if (num_commands % 2 != 0)
            {
                close(fd1[0]);
            }
            else
            {
                close(fd2[0]);
            }
        }
        else
        {
            if (i % 2 != 0)
            {
                close(fd2[0]);
                close(fd1[1]);
            }
            else
            {
                close(fd1[0]);
                close(fd2[1]);
            }
        }
        waitpid(pid, NULL, 0);
        i++;
    }
}

void run(char **args)
{
    if ((pid = fork()) == -1)
    { //child process failed to be created
        return;
    }
    if (pid == 0)
    {
        if (execvp(args[0], args) == -1)
        { //execvp could not run command
            printf("Not a valid executable file");
            kill(getpid(), SIGTERM);
        }
    }
    waitpid(pid, NULL, 0);
}

int runCommands(char *args[])
{
    int i = 0;
    int j = 0;
    char *commands_arr[256];
    while (args[j] != NULL)
    {
        commands_arr[j] = args[j];
        j++;
    }

    if (strcmp(args[0], "exit") == 0)
        exit(0);
    else
    {
        while (args[i] != NULL)
        {
            if (strcmp(args[i], "|") == 0)
            { //pipe commands whenever we encounter '|'
                pipes(args);
                return 1;
            }
            i++;
        }
        commands_arr[i] = NULL; //signifies end of user input
        run(commands_arr);
    }
    return 1;
}

int main(int argc, char *argv[], char **envp)
{
    char line[MAX_SIZE]; //user input
    char *tokens[LIMIT]; //tokenized commands
    int numTokens;
    pid = -10;
    while (1)
    {
        prompt();
        memset(line, '\0', MAX_SIZE);  //empty buffer 
        fgets(line, MAX_SIZE, stdin);
        if ((tokens[0] = strtok(line, " \n\t")) == NULL)
            continue;
        numTokens = 1;
        while ((tokens[numTokens] = strtok(NULL, " \n\t")) != NULL)
            numTokens++;
        runCommands(tokens);
    }
    exit(0);
}