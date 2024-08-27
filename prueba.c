#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(){
    char *args[] = {"ls", NULL};
    
    if(fork() == 0){
    // PROCESO HIJO
        execvp("ls", args); 
    } else {
        
        wait(NULL);

    }

}