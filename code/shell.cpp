#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define NULLPTR (nullptr)

using namespace std;

const int MAX_INPUT = 1024;
const int MAX_ARGUMENTS = 64;

int main()
{
	while(true)
	{
		cout << "user$ ";
		
		char line[MAX_INPUT];
		{
			fgets(line, sizeof(line), stdin);
		}
		
		for(int i = 0; i < MAX_INPUT; i++)
		{
			if(line[i] == '\n')
				line[i] = '\0';
		}
		
		char* command;
		char* arguments[MAX_ARGUMENTS];
		{
			int index = 0;
			
			arguments[index] = strtok(line, " ");
			
			while((index < MAX_ARGUMENTS) && (arguments[index] != NULLPTR))
			{
				index += 1;
				
				arguments[index] = strtok(NULLPTR, " ");
			}
			
			command = arguments[0];
		}
		
		if(command == NULLPTR)
			continue;
		
		if(strcmp(command, "exit") == 0)
			break;
		
		if(strcmp(command, "cd") == 0)
		{
			if(arguments[1] == NULLPTR)
				cout << "Argument required" << '\n';
			
			else
				chdir(arguments[1]);
			
			continue;
		}
		
		if(fork() == 0)
		{
			execvp(command, arguments);
			
			cout << "Wrong command" << '\n';
			
			break;
		}
		
		else
			wait(NULLPTR);
	}
}