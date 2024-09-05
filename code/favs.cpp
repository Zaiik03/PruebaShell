#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <unistd.h>

void writeConfig(std::string ruta){
    std::ofstream config("./code/config.txt");
    if(config.is_open())
    {
        config << ruta;
        config.close();
    }
}

void createFile(std::string ruta){
    std::ofstream file(ruta);
    writeConfig(ruta);
    std::cout << "Arhivo creado exitosamente" << "\n";
    
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    return (first == std::string::npos || last == std::string::npos) ? "" : str.substr(first, last - first + 1);
}

std::string readConfig(){
    std::ifstream config("./code/config.txt");
    std::string ruta;
    if(config.is_open())
    {
        std::getline(config, ruta);
        config.close();
    }
    return ruta;
}

void saveSessionCommands(std::string& ruta){
    std::cout << "Guardando archivo" << "\n";
    std::ofstream file(ruta, std::ios::app);
    std::ifstream tempFile("./code/currentSession.txt");
    std::vector<std::string> favCommands;
    std::string command;

    int commandNumber = 1;
    std::ifstream favFile(ruta);
    while(std::getline(favFile, command))
    {
        commandNumber++;
    }

    if(tempFile.is_open())
    {
        while(std::getline(tempFile, command))
        {
            command = trim(command);
            if(std::find(favCommands.begin(), favCommands.end(), command) == favCommands.end())
            {
                favCommands.push_back(command);
            }
        }
        tempFile.close();

    } else{
        std::cout << "Error al abrir archivo temporal de comandos" << "\n";
    }

    if(file.is_open())
    {
        for(std::string command : favCommands){
            file << commandNumber++ << ". "<< command << std::endl;
        }

        std::cout << "Comandos guardados en: " << ruta << "\n";
        file.close();
    } else{
        std::cout << "Error al abrir el archivo para guardar" << "\n";
    }

}

int main(int argc, char *argv[]){
    if(argc < 2)
    {
        std::cout << "Debe especificar un comando" << "\n";
        return 1;
    }
    std::string ruta = readConfig(); 


    if(strcmp(argv[1], "crear") == 0)
    {   
        if(argv[2] == NULL)
        {
            std::cout << "Debe especificar una ruta" << "\n";
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
            std::cout << "Primero debe crear un archivo para guardar comandos favoritos." << "\n";
            std::cout << "Puede usar <<favs crear ruta/miarchivo.txt>>" << "\n";
        }
    }



    return 0;
}