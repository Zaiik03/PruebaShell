#include <iostream>
#include <string>
#include <vector>

#include <csignal>
#include <unistd.h>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>

#define NULLPTR (nullptr)

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


void signal_alarma(int seconds){
	

}
void set(char* arguments[]) {
    // Verificación de argumentos
    for (int i = 0; i < 4; i++) {
        if (arguments[i] == NULL) {
            std::cout << "Se necesitan más argumentos" << std::endl;
            break;  // Salir de la función
        }
    }

    // Comparación de cadenas utilizando strcmp
    if (strcmp(arguments[1], "recordatorio")) {
        std::cout << "Argumento incorrecto" << std::endl;
        return;
    }
	
    if(arguments[2]== NULL){
		std::cout << "Argumento incorrecto" << std::endl;
        return;

	}

    if (arguments[3]==NULL){
        std::cout << "Argumento incorrecto" << std::endl;
        return;

    }

    int segundos;
    try {
        segundos = stoi(arguments[2]);
    } catch (...) {
        std::cout << "Tiempo inválido." << std::endl;
        return;
    }

	signal(SIGALRM, signal_alarma); 
    alarm(segundos); 
	pause();

    string mensaje;
    for (int i = 3; arguments[i] != NULL; i++) {
        mensaje += arguments[i];
        if (arguments[i + 1] != NULL) {
            mensaje += " "; 
        }
    }
	std::cout << mensaje <<std::endl;
        
    
}


int main()
{
	


	while(true)
	{
		cout << "user$ ";
		
		vector<Command> commands = getNextCommands();
		
		if(commands.size() == 0)
			continue;
		
		if(commands[0].name == "exit")
			break;
		
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
				if(commands[0].name =="set"){
					set(arguments);
					exit(0);
				}
				
				execvp(command.name.data(), arguments);
				
				cout << "Wrong command" << '\n';
				
				break;

				
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