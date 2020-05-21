#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define _CRT_SECURE_NO_WARNINGS

#define TRUE				1
#define FALSE				0

// Commads
#define EXIT_COMMAND		"quit"
#define SET_COMMAND			"set"

// Standards
#define BUFFER_SIZE			512
#define FORK_CONCURRENTLY	3
#define FORK_SEQUENTIAL		2

char command_splitor, arg_splitor;
int fork_type;

void replaceChar(char* input, char oldChar, char newChar, int len)
{
	int i;
	if (len > 0)
	{
		for (i = 0; i < len; i++)
			if (input[i] == oldChar)
				input[i] = newChar;
	}
	else
	{
		i = -1;
		while (input[++i])
			if (input[i] == oldChar)
				input[i] = newChar;
	}
}

int splitPartCount(const char* input, char splitor)
{
	char* _copy, *temp;
	int _count = 0;

	_copy = (char*)malloc(sizeof(char) * strlen(input));
	strcpy(_copy, input);

	temp = strtok(_copy, &splitor);

	do {
		_count++;
		temp = strtok(NULL, &splitor);
	} while (temp);

	return _count;
}

char** splitor(const char* input, char splitor)
{
	char** result, *copy, *temp;
	int count, i;

	copy = (char*)malloc(sizeof(char) * strlen(input));
	strcpy(copy, input);

	count = splitPartCount(input, splitor) + 1;
	result = (char**)malloc(sizeof(char*) * count);

	temp = strtok(copy, &splitor);

	i = 0;
	do {
		result[i++] = temp;
		temp = strtok(NULL, &splitor);
	} while (temp);

	result[i] = (char*)NULL;

	return result;
}

void setFuthers(char** command)
{
	if (strcmp(command[0], SET_COMMAND) == 0)
	{
        if(command[1] != NULL)
        {
            if (strcmp(command[1], "sequential") == 0)
            {
                fork_type = FORK_SEQUENTIAL;
                puts(" ~ process creation mode has been changed to sequential !");
                return;
            }
            else if (strcmp(command[1], "concurrently") == 0)
            {
                fork_type = FORK_CONCURRENTLY;
                puts(" ~ process creation mode has been changed to concurrently !");
                return;
            }
        }

		puts(" ~ E: set command accept sequential/concurrently at its argument only .");
	}
}

int exeCommand(char** command, int fork_type)
{
	int i, status;
	pid_t id;


	if (!command[0])
		return FALSE;

	if (strcmp(command[0], EXIT_COMMAND) == 0)
	{
		return TRUE;
	}
	else if (strcmp(command[0], SET_COMMAND) == 0)
	{
		setFuthers(command);
	}
	else
	{
        if(!command[0])
			return FALSE;

        id = fork();

        if(id == 0)
        {
            char* file_location = (char*) malloc(sizeof(char) * (strlen(command[0]) + strlen("/bin/")));
            strcpy(file_location, "/bin/");
            strcat(file_location, command[0]);
            execv(file_location, command);
            puts(file_location);
            puts(" ~ E: Sorry, but this command can not be executed !");
            _Exit(0);
        }
        else if(id < 0)
        {
            puts(" ~ E: Sorry, but an other progress can not be created !");
        }
        else if(fork_type == FORK_SEQUENTIAL)
        {
            status = 0;
            wait(&status);
        }
	}

	return FALSE;
}

int main(int argc, char** argv)
{
	int command_count, i, returnVal;

	char buffer[BUFFER_SIZE];
	FILE* location;
	int must_exit;

	// defult values
	fork_type = FORK_SEQUENTIAL;
	must_exit = FALSE;

	command_splitor = ';';
	arg_splitor = ' ';

    if(argc == 2)
    {
        location = fopen(argv[1], "r");
    }
    else
    {
        location = stdin;
    }

	while(!must_exit)
	{
        printf(" > ");
		fgets(buffer, BUFFER_SIZE, location);
		replaceChar(buffer, '\n', '\0', NULL);

		char** commands = splitor(buffer, command_splitor);

		i = -1;
		while (commands[++i])
		{
			returnVal = exeCommand(splitor(commands[i], arg_splitor), fork_type);

			if(must_exit == FALSE) must_exit = returnVal;
		}

		if(argc == 2)
            if(feof(location))
                break;
	}

	return 0;
}
