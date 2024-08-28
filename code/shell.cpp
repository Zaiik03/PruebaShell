#include <iostream>
#include <string>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>

#define NULLPTR (nullptr)

using namespace std;

struct Command
{
	string name;
	
	vector<string> arguments;
};

Command getNextCommand()
{
	Command command;
	
	char c = getchar();
	string token;
	
	while((c != '|') && (c != '\n'))
	{
		if(c == ' ' && !token.empty())
		{
			if(command.name.empty())
				command.name = token;
			
			else
				command.arguments.push_back(token);
			
			token = "";
		}
		
		else
			token.push_back(c);
		
		c = getchar();
	}
	
	if(!token.empty())
	{
		if(command.name.empty())
			command.name = token;
		
		else
			command.arguments.push_back(token);
	}
	
	return command;
}

int main()
{
	while(true)
	{
		cout << "user$ ";
		
		Command command = getNextCommand();
		
		if(command.name == "")
			continue;
		
		if(command.name == "exit")
			break;
		
		if(command.name == "cd")
		{
			if(command.arguments.size() != 1)
				cout << "Wrong number of arguments" << '\n';
			
			else
				chdir(command.arguments[0].data());
			
			continue;
		}
		
		if(fork() == 0)
		{
			int count = command.arguments.size();
			
			char* cmd;
			char* args[count + 2];
			{
				cmd = command.name.data();
				args[0] = command.name.data();
				
				for(int i = 0; i < count; i++)
					args[i + 1] = command.arguments[i].data();
				
				args[count + 1] = NULLPTR;
			}
			
			execvp(cmd, args);
			
			cout << "Wrong command" << '\n';
			
			break;
		}
		
		else
			wait(NULLPTR);
	}
}