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
#define num_tecnicos 2
#define num_responsables 2
#define num_encargado 1

//Declaración de las funciones del programa
int calculaAleatorios(int min,int max);
void writeLogMessage ( char * id , char * msg );
void manejadora(int sig);
void manejadora2(int sig2);
void salirPrograma();

/*Semáforos*/
pthread_mutex_t mutexCola;
pthread_mutex_t mutexLog;
pthread_mutex_t mutexDescanso;

int num_clie_app;  //Contador Clientes App
int num_clie_red;  //Contador Clientes Red
int num_solicitudes; //Contador Solicitudes Atención Domiciliaria
int contadorID //Contador del ID de los clientes

/*Clientes*/

typedef struct{
    int id;
    int atendido;
    int tipo;
    int prioridad;
    int solicutud;
    //pthread_t hilo;
} clientes;

/*Tecnicos App y Red*/

typedef struct {
    int id; 
    int descanso;
    int ocupado; 
    int contador; 
    
    pthread_t hilo;
} trabajadores;

clientes *Clientes;

trabajadores *Encargado;
trabajadores *Responsables;
trabajadores *Tecnicos;

FILE *ficheroLog;


int main(int argc, char* argv[]){

    srand(time(NULL));
    e.sa_handler = &manejadora;
    sigaction(SIGUSR1,&e,NULL);
    if((sigaction(SIGUSR1,&e,NULL))==-1){
        perror("Error al recibir la llamada SIGUSR1\n");
        exit(-1);
    } 
    e.sa_handler = &manejadora2;
    sigaction(SIGUSR2,&e,NULL);
    if((sigaction(SIGUSR2,&e,NULL))==-1){
        perror("Error al recibir la llamada SIGUSR2\n");
        exit(-1);
        } 
	}
    if(signal(SIGINT,salirPrograma)==SIG_ERR){	
		//salimos del programa
		exit(-1);
	}
    /*Inicializacion de las variables del programa*/

    pthread_mutex_init(&mutexCola,NULL);
    pthread_mutex_init(&mutexLog,NULL);
    pthread_mutex_init(&mutexDescanso,NULL);

    num_clie_app = 0;
    num_clie_red = 0;
    numclientes = num_clie_app + num_clie_red;
    contadorID = 0;

    /*Inicializamos a los clientes sin determinar si son de App o de Red*/

    Clientes = (clientes*)malloc(max_num_cola*sizeof(clientes));
    for(int i =0;i<max_num_cola;i++){
        //mutex?
        Clientes[i].id = 0;
        Clientes[i].atendido = 0;
        Clientes[i].prioridad = 0;
        Clientes[i].solicutud = 0;
    }

    /*Inicializamos a los Tecnicos*/

    Tecnicos = (trabajadores*)malloc(num_tecnicos*sizeof(trabajadores));
    for(int i =0;i<num_tecnicos;i++){
        //mutex?
        Tecnicos[i].id =0;
        Tecnicos[i].descanso = 0;
        Tecnicos[i].ocupado = 0;
        Tecnicos[i].contador = 0;
    }

    /*Inicializamos a los Responsables*/

    Responsables = (trabajadores*)malloc(num_responsables*sizeof(trabajadores));
    for(int i =0;i<num_responsables;i++){
        //mutex?
        Responsables[i].id =0;
        Responsables[i].descanso = 0;
        Responsables[i].ocupado = 0;
        Responsables[i].contador = 0;
    }

    /*Inicializamos al Encargado*/

    Encargado = (trabajadores*)malloc(num_encargado*sizeof(trabajadores));
    for(int i =0;i<num_tecnicos;i++){
        //mutex?
        Encargado[i].id =0;
        Encargado[i].descanso = 0;
        Encargado[i].ocupado = 0;
        Encargado[i].contador = 0;
    }

    //Ficheros
    ficheroLog=fopen(NOMBRE_FICHERO_LOG,"wt");
	fclose(ficheroLog)

    //Variables Atencion Domiciliaria

    //Variables Condicion

    
    /*Bucle infinito*/
    while(true){
        pause();
    }
    return 0;
}




void nuevoClienteRed(int signal){
    if(numclientes<max_num_cola){
        pthread_mutex_lock(&mutexCola);
        Clientes[numclientes].id = contadorID + 1;
        Clientes[numclientes].atendido = 0;
        Clientes[numclientes].tipo = 
        Clientes[numclientes].prioridad = 0;
        Clientes[numclientes].solicutud = 0;
        numclientes++;
        contadorID++;
        pthread_mutex_unlock(&mutexCola);
        
        pthread_create(&thread_id, NULL, myThreadFun, NULL);
        
    }else{
        printf("La cola de atencion esta llena\n");
    }
}

void AccionCliente(){

}

void AccionTecnico(){

}

void AccionEncargado(){
 
}

void AccionTecnicoDomiciliario(){

}


----------------------







void manejadora(int sig1){ /*Manejadora que trata los clientes de app y clientes de red*/
    if (signal(SIGUSR1,manejadora) == SIG_ERR){
		perror("Error en la señal SIGUSR1\n");  /*Tratamiento de la señal*/
	}else if(signal(SIGUSR2,manejadora2) == SIG_ERR){
        perror("Error en la señal SIGUSR2\n"); /*Tratamiento de la señal*/
    }else{
        nuevoClienteRed(sig1);
        exit();
    }	
}

void manejadora2(int sig2){ /*Manejadora que termina el programa*/
    if(signal(SIGINT,manejadora2) == SIG_ERR){
        perror("Error en la señal SIGINT\n"); /*Tratamiento de la señal*/
    }else{
        exit();
    }
}

/*Funcion que escribe los logs*/

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

/*Funcion que calcula numeros aleatorios*/

int calculaAleatorios(int min, int max){
	return rand() % (max-min+1) + min;
}


