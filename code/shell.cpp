#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include <limits.h>
#include <stdlib.h>
#include <csignal>
#include <unistd.h>
#include <cstring>


#include <sys/types.h>
#include <sys/wait.h>

#define NULLPTR (nullptr)

#define COLOR_RED "\e[1;31m"
#define COLOR_CYN "\e[1;36m"
#define COLOR_PRP "\e[1;35m"

#define COLOR_NONE "\e[0m"

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

void saveCurrentCommand(string command, set<string>& seenCommands, ofstream& tempFile)
{
	command = trim(command);
	
	if(!seenCommands.contains(command))
	{
		seenCommands.insert(command);
		
		if(tempFile.is_open())
		{
			tempFile << seenCommands.size() << ". " << command << '\n';
			
			tempFile.flush();
		}
		
		else
			cout << COLOR_RED << "Error while writing in the temporal file." << COLOR_NONE << '\n';
	}
}

char* getFavsAbsPath()
{
	static char result[1024];
	
	realpath("./exec/favs", result);
	
	return result;
}

void signal_alarma(int seconds){

}

void setAlarm(char* arguments[]) {
    for (int i = 0; i < 4; i++) {
        if (arguments[i] == NULL) {
            std::cout << COLOR_RED <<  "Se necesitan más argumentos" << COLOR_NONE << "\n";
            break;  
        }
    }

    if (strcmp(arguments[1], "recordatorio")) {
        std::cout << COLOR_RED << "Argumento incorrecto" << COLOR_NONE << "\n";
        return;
    }

    if(arguments[2]== NULL){
        std::cout << COLOR_RED << "Argumento incorrecto" << COLOR_NONE << "\n";
        return;

    }

    if (arguments[3]==NULL){
        std::cout << COLOR_RED << "Argumento incorrecto" << COLOR_NONE << "\n";
        return;

    }

    int segundos;

    try {
        segundos = stoi(arguments[2]);
        } 
        catch (const std::invalid_argument& e) {
            std::cout << COLOR_RED << "Error: '" << arguments[2] << "' no es un número válido." << COLOR_NONE << "\n";
        }

    if(segundos <= 0){
        std::cout << COLOR_RED << "Numero invalido, numero debe ser mayor que 0" << COLOR_NONE << "\n";
        return;
    }


    string mensaje;
    for (int i = 3; arguments[i] != NULL; i++) {
        mensaje += arguments[i];
        if (arguments[i + 1] != NULL) {
            mensaje += " "; 
        }
    }


    pid_t pid = fork();
    char directory[1024];
	if (pid == 0) {
		getcwd(directory, sizeof(directory));
        signal(SIGALRM, signal_alarma);
        alarm(segundos);
        pause();
        std::cout <<"\n";
        fflush(stdout);
        std::cout << COLOR_PRP << mensaje << COLOR_NONE << "\n";
		std::cout << COLOR_CYN << "[" << directory << "]" <<  COLOR_NONE << ": ";
        exit(0);
    } else if (pid < 0) {
        std::cout << COLOR_RED << "Error al crear el proceso hijo." << COLOR_NONE << "\n";
    }


}


int main()
{
	set<string> seenCommands;
	
	ofstream tempFile("/tmp/currentSession.txt");
	
	char* favsAbsPath = getFavsAbsPath();
	
	char directory[1024];
	
	while(true)
	{
		getcwd(directory, sizeof(directory));
		cout << COLOR_CYN << "[" << directory << "]" << COLOR_NONE << ": ";
		
		vector<Command> commands = scanNextCommands();
		
		if(commands.empty())
			continue;
		
		if(commands[0].name == "exit")
		{
			tempFile.close();
			
			remove("/tmp/currentSession.txt");
			
			return 0;
		}
		
		if(commands[0].name == "cd")
		{
			if(commands[0].arguments.size() != 1)
				cout << COLOR_RED << "Wrong number of arguments" << COLOR_NONE << '\n';
			
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

				if(commands[0].name == "set")
				{
					setAlarm(arguments);
					
					exit(0);
				}
				
				if(commands[0].name == "favs")
					execv(favsAbsPath, arguments);
				
				else
					execvp(command.name.data(), arguments);
				
				cout << COLOR_RED << "Wrong command" << COLOR_NONE << '\n';
				
				return 0;
			}
		}
		
		if(commands[0].name != "favs")
			saveCurrentCommand(commands[0].name, seenCommands, tempFile);
		
		for(int i = 0; i < pipeCount; i++)
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
		}
		
		while(wait(NULLPTR) > 0);
	}
	
}