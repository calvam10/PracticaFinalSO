//Librerias para el funcionamiento de la practica 
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h> 
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#define max_num_cola 20

//Declaración de las funciones del programa
int calculaAleatorios(int min,int max);
void writeLogMessage ( char * id , char * msg );



//Variables Del programa
FILE *logFile;
FILE *logFileName;


int main(int argc, char* argv[]){
    srand(time(NULL));
    return 0;
}


int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}
void writeLogMessage ( char * id , char * msg ) { 
    time_t now = time (0) ;
    struct tm * tlocal = localtime (& now );
    char stnow [25];
    strftime ( stnow , 25 , " %d/ %m/ %y %H: %M: %S", tlocal );
    // Escribimos en el log
    logFile = fopen ( logFileName , "a");
    fprintf ( logFile , "[ %s] %s: %s\n", stnow , id , msg ) ;
    fclose ( logFile );
  }
