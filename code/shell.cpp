#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits.h>
#include <algorithm>
#include <cstdlib>

#include <sys/types.h>
#include <sys/wait.h>

#define NULLPTR (nullptr)
#define RESET "\033[0m"
#define RED "\033[31m"
#define CYAN "\033[36m"
#define MAGENTA "\033[35m"

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

string trim(const string& str)
{
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	
	return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

void saveCurrentCommand(string command, vector<string>& usedCommands, int& commandNumber, ofstream& tempFile)
{
	command = trim(command);
	
	if(find(usedCommands.begin(), usedCommands.end(), command) == usedCommands.end())
	{
		usedCommands.push_back(command);
		
		if(tempFile.is_open())
			tempFile << commandNumber++ << ". " << command << endl;
		
		else
			cout << RED << "Error escribiendo en el archivo temporal" << RESET << "\n";
	}
}

char* getFavsAbsPath()
{
	static char result[1024];
	realpath("./code/favs", result);
	return result;
}

int main()
{
	ofstream tempFile("/tmp/currentSession.txt");
	vector<string> usedCommands;
	int commandNumber = 1;

	char* favsAbsPath = getFavsAbsPath();
	char cwd[1024];
	
	while(true)
	{
		getcwd(cwd, sizeof(cwd));
		
		cout << CYAN << cwd << "$ " << RESET;
		
		vector<Command> commands = scanNextCommands();
		
		if(commands.empty())
			continue;
		
		if(commands[0].name == "exit")
		{
			tempFile.close();
			
			remove("/tmp/currentSession.txt");
			
			break;
		}
		
		if(commands[0].name == "cd")
		{
			if(commands[0].arguments.size() != 1)
				cout << RED << "Wrong number of arguments" << RESET << '\n';
			
			else
				chdir(commands[0].arguments[0].data());
			
			continue;
		}
		
		int pipeCount = commands.size() - 1;
		
		int pipes[pipeCount][2];
		
		for(int i = 0; i < pipeCount; i++)
			pipe(pipes[i]);
		
		for(int i = 0; i < commands.size(); i++)
		{
			Command& command = commands[i];
			
			if(command.name.empty())
				break;
			
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
				
				for(int j = 0; j < pipeCount; j++)
				{
					close(pipes[j][0]);
					close(pipes[j][1]);
				}
				
				if(commands[0].name == "favs")
					execv(favsAbsPath, arguments);
				
				else
					execvp(command.name.data(), arguments);
				
				cout << RED << "Wrong command" << RESET << '\n';
				
				return 0;
			}
			
			else
			{
				if(commands[0].name != "favs")
					saveCurrentCommand(commands[0].name, usedCommands, commandNumber, tempFile);
			}
		}
		
		for(int i = 0; i < pipeCount; i++)
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		
		while(wait(NULLPTR) > 0);
	}
}