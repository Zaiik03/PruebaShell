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

pair<string, char> scanToken()
{
	string token;
	
	char c;
	
	while(true)
	{
		c = getchar();
		
		if(c != ' ')
			break;
	}
	
	while((c != ' ') && (c != '\n'))
	{
		token.push_back(c);
		
		c = getchar();
	}
	
	return {token, c};
}

vector<Command> scanNextCommands()
{
	vector<Command> commands;
	
	Command command;
	
	while(true)
	{
		const auto& [token, separator] = scanToken();
		
		if(token == "|")
		{
			commands.push_back(command);
			
			command = Command();
		}
		
		else
		{
			if(command.name.empty())
				command.name = token;
			
			else
				command.arguments.push_back(token);
		}
		
		if(separator == '\n')
		{
			commands.push_back(command);
			
			break;
		}
	}
	
	return commands;
}

int main()
{
	while(true)
	{
		cout << "user$ ";
		
		vector<Command> commands = scanNextCommands();
		
		if(commands.empty())
			continue;
		
		int pipeCount = commands.size() - 1;
		
		int pipes[pipeCount][2];
		
		if(pipeCount > 0)
		{
			for(int i = 0; i < pipeCount; i++)
				pipe(pipes[i]);
		}
		
		for(int i = 0; i < commands.size(); i++)
		{
			Command& command = commands[i];
			
			if(command.name.empty())
				break;
			
			if(command.name == "exit")
				return 0;
			
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
				
				char* arguments[count + 2];
				{
					arguments[0] = command.name.data();
					arguments[count + 1] = NULLPTR;
					
					for(int i = 0; i < count; i++)
						arguments[i + 1] = command.arguments[i].data();
				}
				
				if(i > 0)
					dup2(pipes[i - 1][0], STDIN_FILENO);
				
				if(i < pipeCount)
					dup2(pipes[i][1], STDOUT_FILENO);
				
				for(int j = 0; j < commands.size(); j++)
				{
					close(pipes[j][0]);
					close(pipes[j][1]);
				}
				
				execvp(command.name.data(), arguments);
				
				cout << "Wrong command" << '\n';
				
				break;
			}
		}
		
		for(int i = 0; i < commands.size(); i++)
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		
		while(wait(NULLPTR) > 0);
	}
}