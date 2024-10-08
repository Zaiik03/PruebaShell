#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>

#define RESET "\033[0m"
#define MAGENTA "\033[35m"
#define RED "\033[31m"

char* getFavsAbsPath(std::string ruta)
{
	static char result[1024];
	
	realpath(ruta.c_str(), result);
	
	return result;
}

void createFile(std::string ruta){
	
	ruta = getFavsAbsPath(ruta);
	std::ofstream file(ruta);
	std::ofstream config("/tmp/config.txt");
	if(config.is_open())
	{
		config << ruta;
		config.close();
	}
	std::cout << MAGENTA << "File created succesfully in "<< ruta << RESET << "\n";

	file.close();
	config.close();
}

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(' ');
	size_t last = str.find_last_not_of(' ');
	return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

std::string readConfig(){
	std::ifstream config("/tmp/config.txt");
	std::string ruta;
	if(config.is_open())
	{
		std::getline(config, ruta);
		config.close();
	}
	return ruta;
}

void showSessionCommands(){
	std::ifstream tempFile("/tmp/currentSession.txt");
	std::string command;
	std::cout << MAGENTA << "Commands used in current session: " << RESET << "\n";
	if(tempFile.is_open())
	{
		while(std::getline(tempFile, command))
		{
			std::cout << MAGENTA << command << RESET << "\n";
		}
	}

	tempFile.close();
}

bool isFavsEmpty(std::string ruta){
	std::ifstream fav(ruta);

	fav.seekg(0, std::ios::end);
	if(fav.tellg() == 0)
	{
		fav.seekg(0, std::ios::beg);
		fav.close();
		return true;
	}
	fav.close();
	return false;
}

void showFavCommands(std::string ruta){
	bool isEmpty = isFavsEmpty(ruta);

	if(isEmpty)
	{
		std::cout << RED << "No favorite commands in file to show" << RESET << "\n";
		return;
	}

	std::ifstream fav(ruta);
	
	std::string command;
	std::cout << MAGENTA << "Favorite commands in file: " << RESET << "\n";
	if(fav.is_open())
	{
		while(std::getline(fav, command))
		{
			std::cout << MAGENTA << command << RESET << "\n";
		}
	}

	fav.close();
}

void deleteCommands(std::string ruta){
	bool isEmpty = isFavsEmpty(ruta);

	if(isEmpty)
	{
		std::cout << RED << "No favorite commands to delete from persistent file" << RESET << "\n";
		return;
	}

	std::ofstream favs(ruta, std::ios::trunc);
	if(favs.is_open())
	{
		std::cout << MAGENTA << "Favorite commands deleted from persistent file" << RESET << "\n";
	}

	favs.close();
}

void deleteNumCommands(std::string commandsIndex){
	std::vector<int> numbers; 
	std::string token;

	for(char c : commandsIndex)
	{
		if(c == ',')
		{
			if(!token.empty())
			{
				try{
					numbers.push_back(std::stoi(token));
				} catch(std::invalid_argument& e){
					std::cout << RED << "Error: " << token << " is not a number" << RESET << "\n";
					return;
				}
				token.clear();
			}

		} else{
			token.push_back(c);
		}
	}

	if(!token.empty())
	{
		try{
			numbers.push_back(std::stoi(token));
		} catch(std::invalid_argument& e){
			std::cout << RED << "Error: " << token << " is not a number" << RESET << "\n";
			return;
		}
	}

	std::ifstream fav("/tmp/currentSession.txt");
	std::vector<std::string> favCommands;
	std::string line;

	while(std::getline(fav, line))
	{
		favCommands.push_back(line);
	}
	fav.close();

	std::vector<std::string> updatedFavCommands;
	for(int i = 0;i < favCommands.size();i++)
	{
		if(std::find(numbers.begin(), numbers.end(), i+1) == numbers.end())
		{
			updatedFavCommands.push_back(favCommands[i]);
		}
	}

	std::ofstream newFav("/tmp/currentSession.txt");
	for (int i = 0; i < updatedFavCommands.size(); i++) {
		newFav << (i + 1) << ". " << updatedFavCommands[i].substr(updatedFavCommands[i].find(' ') + 1) << std::endl;
	}

	newFav.close();
	
	std::cout << MAGENTA << "Commands deleted successfully from current session" << RESET << "\n";
	
}

std::string extractCommand(std::string line){
	size_t dotPos = line.find(". ");
	if(dotPos != std::string::npos)
	{
		return line.substr(dotPos + 2);
	}
	return line;
}

void saveSessionCommands(std::string& ruta){
	std::cout << MAGENTA << "Saving file" << RESET << "\n";
	std::ofstream file(ruta, std::ios::app);
	std::ifstream tempFile("/tmp/currentSession.txt");
	std::vector<std::string> currentFavCommands;
	std::vector<std::string> newFavCommands;
	std::string line;

	int commandNumber = 1;
	std::ifstream favFile(ruta);
	while(std::getline(favFile, line))
	{
		line = trim(line);
		std::string command = extractCommand(line);
		currentFavCommands.push_back(command);
		commandNumber++;
	}

	if(tempFile.is_open())
	{
		while(std::getline(tempFile, line))
		{
			line = trim(line);
			std::string command = extractCommand(line);

			if(std::find(newFavCommands.begin(), newFavCommands.end(), command) == newFavCommands.end() && 
			   std::find(currentFavCommands.begin(), currentFavCommands.end(), command) == currentFavCommands.end() )
			{
				newFavCommands.push_back(command);
			}
		}
		tempFile.close();

	} else{
		std::cout << RED <<  "Error while opening temporal archive" << RESET << "\n";
	}

	if(file.is_open())
	{
		for(std::string command : newFavCommands){
			file << commandNumber++ << ". "<< command << std::endl;
		}

		std::cout << MAGENTA << "Commands saved successfully in: " << ruta << RESET << "\n";
		file.close();
	} else{
		std::cout << RED << "Error while opening file to save" << RESET << "\n";
	}


	file.close();
	tempFile.close();
}

void searchSubString(std::string substr){
	std::ifstream fav("/tmp/currentSession.txt");
	std::vector<std::string> favCommands;
	std::string line;

	while(std::getline(fav, line))
	{
		favCommands.push_back(line);
	}
	fav.close();

	for(int i = 0;i < favCommands.size();i++)
	{
		if(favCommands[i].find(substr) != std::string::npos)
		{
			std::cout << MAGENTA << favCommands[i] << RESET << "\n";
		}
	}

	fav.close();

}

void execCommand(std::string numCommandA){
	std::ifstream fav("/tmp/currentSession.txt");
	std::string line;
	std::string numCommandB;
	std::string command;

	while(std::getline(fav, line))
	{
		size_t dotPos = line.find('.');
		if(dotPos != std::string::npos)
		{
			numCommandB = line.substr(0, dotPos);
		   
		}


		if(numCommandA == numCommandB)
		{
			command = extractCommand(line);

			char* args[] = {const_cast<char*>(command.c_str()), NULL};
			pid_t pid = fork();
			if(pid == 0)
			{
				execvp(command.c_str(), args);
				std::cout << RED << "Error while executing command" << RESET << "\n";
			} else{
				waitpid(pid, NULL, 0);
			}
		}	
  
	}
	

	fav.close();
}

void resetFavs(std::string ruta){
	remove("/tmp/config.txt");
	remove(ruta.c_str());
	std::cout << MAGENTA << "Favs reset successfully" << RESET << "\n";
	std::cout << MAGENTA << "Favorite commands file deleted" << RESET << "\n";
}

int main(int argc, char *argv[]){
	if(argc < 2)
	{
		std::cout << RED << "Must specify a command" << RESET << "\n";
		return 1;
	}

	std::string ruta = readConfig(); 

	if(strcmp(argv[1], "crear") == 0)
	{   
		if(argv[2] == NULL)
		{
			std::cout << RED << "Must specify a path" << RESET <<"\n";
		} else{
			createFile(argv[2]);
		}
	}

	if(strcmp(argv[1], "guardar") == 0)
	{
		if(!ruta.empty())
		{
			saveSessionCommands(ruta);
		} else{
			std::cout << RED << "First you need to create a file to save favorite commands" << RESET <<"\n";
			std::cout << RED << "You can use <<favs crear ruta/miarchivo.txt>>" << RESET << "\n";
		}
	}

	if(strcmp(argv[1], "mostrar") == 0)
	{
		
		showSessionCommands();
		
	}

	if(strcmp(argv[1], "cargar") == 0)
	{   
		if(!ruta.empty())
		{
			showFavCommands(ruta);
		} else{
			std::cout << RED << "No favorite commands saved in file" << RESET << "\n";
			std::cout << RED << "You can use <<favs guardar>>" << RESET <<"\n";
		}
	}

	if(strcmp(argv[1], "borrar") == 0)
	{
		if(!ruta.empty())
		{
			deleteCommands(ruta);
		} else{
			std::cout << RED << "No favorite commands in file to delete" << RESET << "\n";
		}
	}

	if(strcmp(argv[1], "eliminar") == 0)
	{
		if(!ruta.empty())
		{

			if(argv[3] != NULL)
			{
				std::cout << RED << "Correct format is 'num1,num2...'" << RESET <<"\n";
			} else{

				if(argv[2] != NULL)
				{
					deleteNumCommands(argv[2]);
				} else{ 
					std::cout << RED << "Must specify the numbers associated with the favorite commands to delete" << RESET << "\n";
				}

			}
	
		} else{
			std::cout << RED << "No favorite commands to delete" << RESET << "\n";
		}
		
	}

	if(strcmp(argv[1], "buscar") == 0)
	{

		if(argv[2] != NULL)
		{
			searchSubString(argv[2]);
		} else{
			std::cout << RED << "Must specify the substring to search" << RESET << "\n";
		}

	}

	if(strcmp(argv[1], "reiniciar") == 0)
	{
		if(!ruta.empty())
		{
			resetFavs(ruta);
		} else{
			std::cout << RED << "No favorite file to delete" << RESET << "\n"; 
		}
		
	}
	
	if(strcmp(argv[2], "ejecutar") == 0)
	{
		if(argv[1] != NULL)
		{
			std::cout << MAGENTA << "Executing from current session commands" << MAGENTA << "\n";
			execCommand(argv[1]);
		} else{ 
			std::cout << RED << "Must specify the number associated with the command to execute" << RESET << "\n";
		}
	}
	
	return 0;
}