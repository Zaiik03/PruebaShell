#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(){
    char input[1024];
    char* args[64];

    while(1){
        printf("test$ ");

        fgets(input, sizeof(input), stdin); // -> Leer entrada
        
        input[strcspn(input, "\n")] = 0; // -> Remueve el salto de linea del final


        // TOKENIZACION CHAT-GPTEADA
        int i = 0;
        args[i] = strtok(input, " ");
        while(args[i] != NULL && i < 64){
            i++;
            args[i] = strtok(NULL, " ");
        }
        args[i] = NULL; // -> el ultimo argumento es NULL, porque execvp necesita que el ultimo arg sea NULL
        
        // COMANDOS ESPECIFICOS
        if (args[0] != NULL && strcmp(args[0], "exit") == 0) {
            break;
        }
        

        // parece que cd no funciona junto con el fork xd
        if(args[0] != NULL && strcmp(args[0], "cd") == 0){
            if(args[1] == NULL){
                continue;
            } else{
                chdir(args[1]);
            }
        } else{
            
            // Ejecución del comando
            // ambos procesos son concurrentes, no se sabe cual de los dos
            // se ejecuta primero, depende del planificador, por eso es necesario
            // hacer que el padre espere al hijo
            // 
            if(fork() == 0){
                // Proceso hijo
                execvp(args[0], args);
                printf("Error al ejecutar el comando\n");
                break; // -> En caso de que el comando falle 
            } else{
                wait(NULL);
            }  
        }


    }
}