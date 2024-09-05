#include <iostream>
#include <string.h>
#include <fstream>
#include <vector>
#include <algorithm>
#include <limits.h>
#include <unistd.h>

void createFile(std::string ruta){
    std::ofstream file(ruta);
    std::ofstream config("./code/config.txt");
    if(config.is_open())
    {
        config << ruta;
        config.close();
    }
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

void showSessionCommands(){
    std::ifstream tempFile("./code/currentSession.txt");
    std::string command;
    std::cout << "Comandos usados en la sesión actual: " << "\n";
    if(tempFile.is_open())
    {
        while(std::getline(tempFile, command))
        {
            std::cout << command << "\n";
        }
    }
}

void showFavCommands(std::string ruta){
    std::ifstream fav(ruta);
    std::string command;

    fav.seekg(0, std::ios::end);
    if(fav.tellg() == 0)
    {
        std::cout << "El archivo de comandos favoritos está vacío" << "\n";
        return;
    }

    fav.seekg(0, std::ios::beg);
    std::cout << "Comandos favoritos: " << "\n";
    if(fav.is_open())
    {
        while(std::getline(fav, command))
        {
            std::cout << command << "\n";
        }
    }
}

void deleteCommands(std::string ruta){
    std::ofstream favs(ruta, std::ios::trunc);
    if(favs.is_open())
    {
        std::cout << "Comandos favoritos eliminados" << "\n";
    }
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
    std::cout << "Guardando archivo" << "\n";
    std::ofstream file(ruta, std::ios::app);
    std::ifstream tempFile("./code/currentSession.txt");
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
        std::cout << "Error al abrir archivo temporal de comandos" << "\n";
    }

    if(file.is_open())
    {
        for(std::string command : newFavCommands){
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

    if(strcmp(argv[1], "mostrar") == 0)
    {
        if(!ruta.empty())
        {
            showSessionCommands();
        } else{
            std::cout << "No hay comandos de sesión actual" << "\n";
        }
    }

    if(strcmp(argv[1], "cargar") == 0)
    {
        if(!ruta.empty())
        {
            showFavCommands(ruta);
        } else{
            std::cout << "No hay comandos favoritos" << "\n";
            std::cout << "Puede usar <<favs guardar>>" << "\n";
        }
    }

    if(strcmp(argv[1], "borrar") == 0)
    {
        if(!ruta.empty())
        {
            deleteCommands(ruta);
        } else{
            std::cout << "No hay comandos favoritos para borrar" << "\n";
        }
    }

    return 0;
}