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

vector<Command> getNextCommands()
{
	vector<Command> commands;
	
	char c;
	
	string token;
	Command command;
	
	while((c = getchar()) != '\n')
	{
		switch(c)
		{
			case ' ':
			{
				if(token.empty())
					continue;
				
				if(command.name.empty())
					command.name = token;
				
				else
					command.arguments.push_back(token);
				
				token = "";
			} break;
			
			case '|':
			{
				commands.push_back(command);
				
				command = Command();
			} break;
			
			default:
			{
				token.push_back(c);
			} break;
		}
	}
	
	if(!token.empty())
	{
		if(command.name.empty())
			command.name = token;
		
		else
			command.arguments.push_back(token);
		
		commands.push_back(command);
	}
	
	return commands;
}

string trim(const string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == string::npos || last == string::npos) ? "" : str.substr(first, last - first + 1);
}

void saveCurrentCommand(string command, vector<string>& usedCommands, int& commandNumber, ofstream& tempFile){
	command = trim(command);
	if (find(usedCommands.begin(), usedCommands.end(), command) == usedCommands.end())
	{
        usedCommands.push_back(command);  
		if(tempFile.is_open())
		{
			tempFile << commandNumber++ << ". " << command << endl;
		} else{
			cout << "Error escribiendo en el archivo temporal" << "\n";
		}
    }
}

char* getFavsAbsPath(){
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

	while(true)
	{
		cout << CYAN << "user$ " << RESET;
		
		vector<Command> commands = getNextCommands();
		
		if(commands.size() == 0)
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
				cout << "Wrong number of arguments" << '\n';
			
			else
				chdir(commands[0].arguments[0].data());
			
			continue;
		}
		
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
				{
					execv(favsAbsPath, arguments);
				} else{	
					execvp(command.name.data(), arguments);
				}

				cout << "Wrong command" << '\n';
				
				break;
			} else{

				if(commands[0].name != "favs")
				{
					saveCurrentCommand(commands[0].name, usedCommands, commandNumber, tempFile);
				}
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