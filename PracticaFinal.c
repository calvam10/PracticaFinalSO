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
int CalculaMaximaPrioridad(clientes *clientes);
void eliminarClienteRed(int posicion);
void eliminarClienteApp(int posicion);
void finalizarPrograma (int signal);

/*Semáforos*/

pthread_mutex_t mutexCola;
pthread_mutex_t mutexLog;
pthread_mutex_t mutexDescanso;
pthread_mutex_t mutexAtendido;
pthread_mutex_t mutexSolicitud;

pthread_cond_t InicioAtencionDomiciliaria;
pthread_cond_t FinAtencionDomiciliaria;

int num_clie_app;  //Contador Clientes App
int num_clie_red;  //Contador Clientes Red
int numclientes; //Contador de Clientes en Total
int num_solicitudes; //Contador Solicitudes Atención Domiciliaria
int contador_ID_App; //Contador del ID de los clientes de App
int contador_ID_Red; //Contador del ID de los clientes de Red

struct sigaction e;
/*Clientes*/

typedef struct{
    int id;
    int atendido;
    int tipo;
    int prioridad;
    int solicitud;
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

/*Colas para los Clientes*/

clientes *Clientes_App;
clientes *Clientes_Red;

/*Diferentes tipos de trabajadores*/

trabajadores *Encargado;
trabajadores *Responsables;
trabajadores *Tecnicos;

/*Nombre del fichero*/

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
    if(signal(SIGINT,finalizarPrograma)==SIG_ERR){	
		//salimos del programa
		exit(-1);
	}

    /*Inicializacion de las variables del programa*/

    /*Semáforos*/

    pthread_mutex_init(&mutexCola,NULL);
    pthread_mutex_init(&mutexLog,NULL);
    pthread_mutex_init(&mutexDescanso,NULL);
    pthread_mutex_init(&mutexAtendido,NULL);
    pthread_mutex_init(&mutexSolicitud,NULL);


    /*Variables numéricas*/

    num_clie_app = 0;
    num_clie_red = 0;
    numclientes = num_clie_app + num_clie_red;
    num_solicitudes = 0;
    contador_ID_App = 0;
    contador_ID_Red = 0;

    /*Inicializamos a los clientes sin determinar si son de App o de Red*/

    clientes = ((clientes*)malloc(max_num_cola*sizeof(clientes)));
    for(int i =0;i<max_num_cola;i++){
        //mutex?
        Clientes[i].id = 0;
        Clientes[i].atendido = 0;
        Clientes[i].prioridad = 0;
        Clientes[i].solicitud = 0;
    }

    /*Inicializamos a los Tecnicos*/

    Tecnicos = (trabajadores*)malloc(num_tecnicos*sizeof(trabajadores));
    for(int i =0;i<num_tecnicos;i++){
        //mutex?
        Tecnicos[i].id =0;
        Tecnicos[i].descanso = 0;
        Tecnicos[i].ocupado = 0;
        Tecnicos[i].contador = 0;
        pthread_create(&Tecnicos[i].hilo,NULL,AccionTecnico(),(*void) Tecnicos[i]);
        if(pthread_create(&Tecnicos[i].hilo,NULL,AccionTecnico(),(*void) Tecnicos[i])!=0){
            perror("Error en la creacion del hilo");
            exit(-1);
        }
    }

    /*Inicializamos a los Responsables*/

    Responsables = (trabajadores*)malloc(num_responsables*sizeof(trabajadores));
    for(int i =0;i<num_responsables;i++){
        //mutex?
        Responsables[i].id =0;
        Responsables[i].descanso = 0;
        Responsables[i].ocupado = 0;
        Responsables[i].contador = 0;
        pthread_create(&Responsables[i].hilo,NULL,AccionesResponsables(),(*void) Responsables[i]);
        if(pthread_create(&Responsables[i].hilo,NULL,AccionesResponsables(),(*void) Responsables[i])!=0){
            perror("Error en la creacion del hilo");
            exit(-1);
        }
    }

    /*Inicializamos al Encargado*/

    Encargado = (trabajadores*)malloc(num_encargado*sizeof(trabajadores));
    for(int i =0;i<num_tecnicos;i++){
        //mutex?
        Encargado[i].id =0;
        Encargado[i].descanso = 0;
        Encargado[i].ocupado = 0;
        Encargado[i].contador = 0;
        pthread_create(&Encargado[i].hilo,NULL,AccionEncargado(),(*void) Encargado[i]);
        if(pthread_create(&Encargado[i].hilo,NULL,AccionEncargado(),(*void) Encargado[i])!=0){
            perror("Error en la creacion del hilo");
            exit(-1);
        }
    }

    /*Ficheros*/

    ficheroLog=fopen("registroTiemposLog.log","wt");
	fclose(ficheroLog);

    //Variables Atencion Domiciliaria

    //Variables Condicion
    pthread_cond_init(&InicioAtencionDomiciliaria,NULL);
    pthread_cond_init(&FinAtencionDomiciliaria,NULL);

    
    /*Bucle infinito*/

    while(1){
        pause();
    }
    return 0;
}

void nuevoClienteRed(int signal){
    if(numclientes<max_num_cola){
        pthread_mutex_lock(&mutexCola);
        
        switch(signal){
            case SIGUSR1:  //Clientes de App
                Clientes_App[numclientes].tipo = 0;
                Clientes_App[numclientes].id = contador_ID_App + 1;
                Clientes_App[numclientes].atendido = 0;
                Clientes_App[numclientes].prioridad = calculaAleatorios(1,10);
                Clientes_App[numclientes].solicitud = 0;
                num_clie_app++;
                contador_ID_App++;
            break;
            case SIGUSR2:   //Clientes de Red
                Clientes_Red[numclientes].tipo =1;
                Clientes_Red[numclientes].id = contador_ID_Red + 1;
                Clientes_Red[numclientes].atendido = 0;
                Clientes_Red[numclientes].prioridad = calculaAleatorios(1,10);
                Clientes_Red[numclientes].solicitud = 0;
                num_clie_red++;
                contador_ID_Red++;
            break;
        }
        
    
        pthread_create(&thread_id, NULL, myThreadFun, NULL);//Corregir
        pthread_mutex_unlock(&mutexCola);
    }else{
        printf("La cola de atencion esta llena\n");
    }
}

int obtenerPosicion(int indice){
    int posicion = 0;
    if(Cliente.tipo==0){ /*En el caso que el cliente sea de App*/
        for(int i=0;i<num_clie_app;i++){
            if(Cliente.id==indice){
                return posicion;
            }
            posicion++;
        }

    }else{
        for(int i=0;i<num_clie_red;i++){
            if(Cliente.id==indice){
                return posicion;
            }
            posicion++;
        }
    }

    return posicion;
}
void AccionCliente(void *Cliente){
    char MotivoSalida[100];
    char TipoCliente[100];
    char Espera[100];
    char FinEspera[100]; 
    if(Cliente.tipo==0){
        pthread_mutex_lock(&mutexLog);
        sprintf(TipoCliente,"El cliente es de Tipo App");
        writeLogMessage(logFile,TipoCliente);
        pthread_mutex_unlock(&mutexLog);
    }else{
        pthread_mutex_lock(&mutexLog);
        sprintf(TipoCliente,"El cliente es de Tipo Red");
        writeLogMessage(logFile,TipoCliente);
        pthread_mutex_unlock(&mutexLog);
    }

    pthread_mutex_lock(&mutexAtendido); /*Compruebo si el cliente esta siendo atendido*/

    while(Cliente.atendido==0){
        int dificultad =calculaAleatorios(1,100);
        if(dificultad<11){ /*Encuentran dificil la aplicacion y se van de inmediato*/
            sprintf(MotivoSalida,"El cliente encuentra dificil la aplicacion y abandona de inmediato la aplicacion");
            writeLogMessage(logFile,MotivoSalida);
            break;
        }else if(dificultad<31){  /*Se cansa de esperar y sale de la aplicacion despues de 8 segundos*/
            sleep(8);
            sprintf(MotivoSalida,"El cliente se ha cansado de esperar y abandona la aplicacion");
            writeLogMessage(logFile,MotivoSalida);
            break;
        }else{
            int aleatorio = calculaAleatorios(1,100);
            if(aleatorio<6){
                sprintf(MotivoSalida,"El cliente ha perdido la conexion y abandona la aplicacion");
                writeLogMessage(logFile,MotivoSalida);
                break;
            }else{ /*El usuario permanece en la aplicacion*/
                sleep(2);
            }
        }
    }  
    pthread_mutex_unlock(&mutexAtendido);
    while(Cliente.atendido==1){
        pthread_mutex_lock(&mutexCola);
        //el cliente atendido
        pthread_mutex_unlock(&mutexCola);
        sleep(2);
    }
    if(Cliente.tipo==1 && Cliente.solicitud==1){
        int atencion = calculaAleatorios(1,100);
        if(atencion<31){
            if(cliente.tipo ==1 && cliente.solicitud==2){ /*Si es Cliente de Red*/
            while(num_solicitudes<4){ 
                if(num_solicitudes<4){
                    pthread_mutex_lock(&mutexSolicitud);
                    num_solicitudes++;
                    pthread_mutex_unlock(&mutexSolicitud);
                    sprintf(Espera,"El cliente esta esperando a ser atendido");
                    writeLogMessage(ficheroLog,Espera);
                    pthread_mutex_lock(&mutexCola);
                    clientes.solicitud=1;
                    pthread_mutex_unlock(&mutexCola);
                    if(num_solicitudes==4){
                        pthread_mutex_lock(&mutexSolicitud);
                        pthread_cond_signal(&InicioAtencionDomiciliaria);

                        pthread_cond_wait(&FinAtencionDomiciliaria,&mutexSolicitud);
                        pthread_mutex_lock(&mutexLog);
                        sprintf(FinEspera,"El cliente ya ha sido atendido");
                        writeLogMessage(ficheroLog,FinEspera);
                        pthread_mutex_unlock(&mutexLog);
                    }
                }else{
                    sleep(3);
                }
               
            }
            
        }
    }
    }else{ /*Si es cliente de App o no realiza solicitud domiciliaria */
        if(Cliente.tipo==0){  /*Si es Cliente de App*/
            int aux = obtenerPosicion(Cliente.id);
            eliminarClienteApp(aux);
        }else{
            int aux = obtenerPosicion(Cliente.id);
            eliminarClienteRed(aux); /*Si es Cliente de Red*/
        }

    }
        
    
}

void AccionTecnico(void *Tecnico){
    while(1){
        char motivoFin[100];
        char mensajeInicio[100];
        char mensajeFinal[100];
        char descanso[100];
        char findescanso[100];
        pthread_mutex_lock(&mutexCola);
        int aux = CalculaMaximaPrioridad(Clientes_App);
        for(int i=0;i<num_clie_app;i++){
            if(Clientes_App[i].atendido==0){
                if(Clientes_App[i].prioridad==aux){
                    pthread_mutex_lock(&mutexAtendido);
                    Clientes_App[i].atendido = 1;
                    pthread_mutex_unlock(&mutexAtendido);
                    int numeroAleatorio = calculaAleatorios(1,100);
                    if(numeroAleatorio<81){
                        pthread_mutex_lock(&mutexLog);
                        sprintf(mensajeInicio,"Comienza la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeInicio);
                        sleep(calculaAleatorios(1,4));
                        sprintf(mensajeFinal,"Termina la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeFinal);
                        sprintf(motivoFin,"El cliente tiene todo en regla y abandona el programa");
                        writeLogMessage(ficheroLog,motivoFin);
                        pthread_mutex_unlock(&mutexLog);
                        
                    }else if(numeroAleatorio<91){
                        pthread_mutex_lock(&mutexLog);
                        sprintf(mensajeInicio,"Comienza la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeInicio);
                        sleep(calculaAleatorios(2,6));
                        sprintf(mensajeFinal,"Termina la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeFinal);
                        sprintf(motivoFin, "El Cliente no se ha identificado correctamente y abandona el programa");
                        writeLogMessage(ficheroLog,motivoFin);
                        pthread_mutex_unlock(&mutexLog);
                    }else{
                        pthread_mutex_lock(&mutexLog);
                        sprintf(mensajeInicio,"Comienza la atencion del Cliente");
                        writeLogMessage(ficheroLog,mensajeInicio);
                        sleep(calculaAleatorios(1,2));
                        sprintf(mensajeFinal,"Termina la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeFinal);
                        sprintf(motivoFin,"El Cliente se ha confundido de compañia y abandona el programa");
                        writeLogMessage(ficheroLog,motivoFin);
                        pthread_mutex_unlock(&mutexLog);
                    }
                    pthread_mutex_lock(&mutexAtendido);
                    Clientes_App[i].atendido = 0;
                    pthread_mutex_unlock(&mutexAtendido);
                }
            }
        }
        Tecnico.contador +=1;
        if(Tecnico.contador%5==0){
            pthread_mutex_lock(&mutexDescanso);
            pthread_mutex_lock(&mutexLog);
            sprintf(descanso,"Inicia el descando del tecnico");
            writeLogMessage(logFile,descanso);
            Tecnico.descanso = 1;
            sleep(5);
            Tecnicos->descanso = 0;
            sprintf(findescanso,"Termina el descando del tecnico");
            writeLogMessage(logFile,findescanso);
            pthread_mutex_unlock(&mutexLog);
            pthread_mutex_unlock(&mutexDescanso);
        }
        pthread_mutex_unlock(&mutexCola);
    }
   
}

/*Funcion que calcula el cliente no atendido con la mayor prioridad*/

int CalculaMaximaPrioridad(clientes *clientes){
    int maximo =0;
    int i=0;
    while(clientes[i]!=NULL){
        if(clientes[i].atendido==0){
            if(clientes[i].prioridad>maximo){
                maximo = clientes[i].prioridad;
            }
        }
        i++;
    }
    return maximo;

}
void AccionesResponsables(void *Responsable){
    while(1){
        char motivoFin[100];
	    char mensajeInicio[100];
        char mensajeFinal[100];
        char descanso[100];
        char findescanso[100];
        pthread_mutex_lock(&mutexCola);
        int aux = CalculaMaximaPrioridad(Clientes_Red);
        for(int i=0;i<num_clie_red;i++){
            if(Clientes_Red[i].atendido==0){
                if(Clientes_Red[i].prioridad==aux){
                    pthread_mutex_lock(&mutexAtendido);
                    Clientes_Red[i].atendido = 1;
                    pthread_mutex_unlock(&mutexAtendido);
                    int numeroAleatorio = calculaAleatorios(1,100);
                    if(numeroAleatorio<81){
                        pthread_mutex_lock(&mutexLog);
                        sprintf(mensajeInicio,"Comienza la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeInicio);
                        sleep(calculaAleatorios(1,4));
                        sprintf(mensajeFinal,"Termina la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeFinal);
                        int atencion = calculaAleatorios(1,10);
                        if(atencion<4){
                            Clientes_Red[i].solicitud=1;
                            sprintf(motivoFin,"El cliente tiene todo en regla y  solicita atencion domiciliaria, por lo que sigue en el programa esperando");
                            writeLogMessage(ficheroLog,motivoFin);
                        }else{
                            sprintf(motivoFin,"El cliente tiene todo en regla y no solicita atencion domiciliaria, por lo que abandona el programa ");
                            writeLogMessage(ficheroLog,motivoFin);
                        }
                        pthread_mutex_unlock(&mutexLog);
                    }else if(numeroAleatorio<91){
                        pthread_mutex_lock(&mutexLog);
                        sprintf(mensajeInicio,"Comienza la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeInicio);
                        sleep(calculaAleatorios(2,6));
                        sprintf(mensajeFinal,"Termina la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeFinal);
                        int atencion = calculaAleatorios(1,10);
                        if(atencion<4){
                            Clientes_Red[i].solicitud=1;
                            sprintf(motivoFin,"El Cliente no se ha identificado correctamente y solicita atencion domiciliaria, por lo que continua el programa esperando");
                            writeLogMessage(ficheroLog,motivoFin);
                        }else{
                            sprintf(motivoFin,"El Cliente no se ha identificado correctamente y no solicita atencion domiciliaria, por lo que abandona el programa");
                            writeLogMessage(ficheroLog,motivoFin);
                        }
                        pthread_mutex_unlock(&mutexLog);
                    }else{
                        pthread_mutex_lock(&mutexLog);
                        sprintf(mensajeInicio,"Comienza la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeInicio);
                        sleep(calculaAleatorios(1,2));
                        sprintf(mensajeFinal,"Termina la atencion del cliente");
                        writeLogMessage(ficheroLog,mensajeFinal);
                        sprintf(motivoFin,"El Cliente se ha confundido de compañia y abandona el programa");
                        writeLogMessage(ficheroLog,motivoFin);
                        pthread_mutex_unlock(&mutexLog);   
                    } 
                    pthread_mutex_lock(&mutexAtendido);
                    Clientes_Red->atendido = 0;
                    pthread_mutex_unlock(&mutexAtendido);
                }

            }
        }
        Responsable.contador +=1;
        if(Responsable.contador%6==0){
            pthread_mutex_lock(&mutexDescanso);
            Responsable.descanso = 1;
            sleep(5);
            Responsable.descanso = 0;
            pthread_mutex_unlock(&mutexDescanso);
        }
        pthread_mutex_unlock(&mutexCola);
    }
        
}


void AccionEncargado(void *Encargado){
    while(1){
        char mensajeInicio[100];
        char mensajeFinal[100];
        char motivoFin[100];

        if(num_clie_red==0){ /*En el caso que no haya Clientes de Red miramos si hay de App*/
            if(num_clie_app==0){ /*En el caso que no haya Clientes en la cola dormimos y volvemos a comprobar en 3 segundas*/
                sleep(3);
            }else{ /*En el caso que haya Clientes de App los tratamos*/
                for(int i=0;i<num_clie_app;i++){
                    int prioridadApp = CalculaMaximaPrioridad(Clientes_App);
                    if(Clientes_App[i].prioridad==prioridadApp){
                        pthread_mutex_lock(&mutexAtendido);
                        Clientes_App[i].atendido = 1;
                        pthread_mutex_unlock(&mutexAtendido);
                        int numeroAleatorio = calculaAleatorios(1,100);
                        if(numeroAleatorio<81){
                            pthread_mutex_lock(&mutexLog);
                            sprintf(mensajeInicio,"Comienza la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeInicio);
                            sleep(calculaAleatorios(1,4));
                            sprintf(mensajeFinal,"Termina la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeFinal);
                            sprintf(motivoFin,"El cliente tiene todo en regla y abandona el programa");
                            writeLogMessage(ficheroLog,motivoFin);
                            pthread_mutex_unlock(&mutexLog);
                        }else if(numeroAleatorio<91){
                            pthread_mutex_lock(&mutexLog);
                            sprintf(mensajeInicio,"Comienza la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeInicio);
                            sleep(calculaAleatorios(2,6));
                            sprintf(mensajeFinal,"Termina la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeFinal);
                            sprintf(motivoFin, "El Cliente no se ha identificado correctamente y abandona el programa");
                            writeLogMessage(ficheroLog,motivoFin);
                            pthread_mutex_unlock(&mutexLog);
                        }else{
                            pthread_mutex_lock(&mutexLog);
                            sprintf(mensajeInicio,"Comienza la atencion del Cliente");
                            writeLogMessage(ficheroLog,mensajeInicio);
                            sleep(calculaAleatorios(1,2));
                            sprintf(mensajeFinal,"Termina la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeFinal);
                            sprintf(motivoFin,"El Cliente se ha confundido de compañia y abandona el programa");
                            writeLogMessage(ficheroLog,motivoFin);
                            pthread_mutex_unlock(&mutexLog);
                        }
                    pthread_mutex_lock(&mutexAtendido);
                    Clientes_App[i].atendido = 0;
                    pthread_mutex_unlock(&mutexAtendido);
                    }
                }
            }
        }else{
            int prioridadRed = CalculaMaximaPrioridad(Clientes_Red);
            if(prioridadRed==0){  /*Si los Clientes de Red estan todos atendidos miramos los de App*/
                if(num_clie_app==0){ /*No hay clientes de App, por lo que dormimos y volvemos a comprobar en 3 segundos*/
                    sleep(3);
                }else{ /*En el caso que haya Clientes de App los tratamos*/
                    for(int i=0;i<num_clie_app;i++){
                        int prioridadApp = CalculaMaximaPrioridad(Clientes_App);
                        if(Clientes_App[i].prioridad==prioridadApp){
                            pthread_mutex_lock(&mutexAtendido);
                            Clientes_App[i].atendido = 1;
                            pthread_mutex_unlock(&mutexAtendido);
                            int numeroAleatorio = calculaAleatorios(1,100);
                            if(numeroAleatorio<81){
                                pthread_mutex_lock(&mutexLog);
                                sprintf(mensajeInicio,"Comienza la atencion del cliente");
                                writeLogMessage(ficheroLog,mensajeInicio);
                                sleep(calculaAleatorios(1,4));
                                sprintf(mensajeFinal,"Termina la atencion del cliente");
                                writeLogMessage(ficheroLog,mensajeFinal);
                                sprintf(motivoFin,"El cliente tiene todo en regla y abandona el programa");
                                writeLogMessage(ficheroLog,motivoFin);
                                pthread_mutex_unlock(&mutexLog);
                    
                            }else if(numeroAleatorio<91){
                                pthread_mutex_lock(&mutexLog);
                                sprintf(mensajeInicio,"Comienza la atencion del cliente");
                                writeLogMessage(ficheroLog,mensajeInicio);
                                sleep(calculaAleatorios(2,6));
                                sprintf(mensajeFinal,"Termina la atencion del cliente");
                                writeLogMessage(ficheroLog,mensajeFinal);
                                sprintf(motivoFin, "El Cliente no se ha identificado correctamente y abandona el programa");
                                writeLogMessage(ficheroLog,motivoFin);
                                pthread_mutex_unlock(&mutexLog);
                            }else{
                                pthread_mutex_lock(&mutexLog);
                                sprintf(mensajeInicio,"Comienza la atencion del Cliente");
                                writeLogMessage(ficheroLog,mensajeInicio);
                                sleep(calculaAleatorios(1,2));
                                sprintf(mensajeFinal,"Termina la atencion del cliente");
                                writeLogMessage(ficheroLog,mensajeFinal);
                                sprintf(motivoFin,"El Cliente se ha confundido de compañia y abandona el programa");
                                writeLogMessage(ficheroLog,motivoFin);
                                pthread_mutex_unlock(&mutexLog);
                            }
                        pthread_mutex_lock(&mutexAtendido);
                        Clientes_App[i].atendido = 0;
                        pthread_mutex_unlock(&mutexAtendido);
                        }
                    }
                }
            }else{ /*Si hay Clientes de Red sin atender los tratamos dependiendo de la prioridad */
                for(int i=0;i<num_clie_red;i++){
                    if(Clientes_Red[i].prioridad==prioridadRed){
                        pthread_mutex_lock(&mutexAtendido);
                        Clientes_Red[i].atendido = 1;
                        pthread_mutex_unlock(&mutexAtendido);
                        int numeroAleatorio = calculaAleatorios(1,100);
                        if(numeroAleatorio<81){
                            pthread_mutex_lock(&mutexLog);
                            sprintf(mensajeInicio,"Comienza la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeInicio);
                            sleep(calculaAleatorios(1,4));
                            sprintf(mensajeFinal,"Termina la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeFinal);
                            sprintf(motivoFin,"El cliente tiene todo en regla y abandona el programa");
                            writeLogMessage(ficheroLog,motivoFin);
                            pthread_mutex_unlock(&mutexLog);
                        }else if(numeroAleatorio<91){
                            pthread_mutex_lock(&mutexLog);
                            sprintf(mensajeInicio,"Comienza la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeInicio);
                            sleep(calculaAleatorios(2,6));
                            sprintf(mensajeFinal,"Termina la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeFinal);
                            sprintf(motivoFin, "El Cliente no se ha identificado correctamente y abandona el programa");
                            writeLogMessage(ficheroLog,motivoFin);
                            pthread_mutex_unlock(&mutexLog);
                        }else{
                            pthread_mutex_lock(&mutexLog);
                            sprintf(mensajeInicio,"Comienza la atencion del Cliente");
                            writeLogMessage(ficheroLog,mensajeInicio);
                            sleep(calculaAleatorios(1,2));
                            sprintf(mensajeFinal,"Termina la atencion del cliente");
                            writeLogMessage(ficheroLog,mensajeFinal);
                            sprintf(motivoFin,"El Cliente se ha confundido de compañia y abandona el programa");
                            writeLogMessage(ficheroLog,motivoFin);
                            pthread_mutex_unlock(&mutexLog);
                        }
                        pthread_mutex_lock(&mutexAtendido);
                        Clientes_App[i].atendido = 0;
                        pthread_mutex_unlock(&mutexAtendido);
                    }
                }
            }
        }
    }
}

void AccionTecnicoDomiciliario(void *Cliente){
    char ComienzoAtencion[100];
	char Atencion[100];
	char FinAtencion[100];
    char UltimoCliente[100];

    while(1) {
        pthread_mutex_lock(&mutexDescanso);
        while(num_solicitudes<4){ //Si no esta lleno    
            pthread_cond_wait(&InicioAtencionDomiciliaria, &mutexSolicitud);
        }
        pthread_mutex_lock(&mutexLog);
        sprintf(ComienzoAtencion,"Comienza la solicitud");
        writeLogMessage(ficheroLog, ComienzoAtencion);
        pthread_mutex_unlock(&mutexLog); 
        for(int i=0;i<num_solicitudes;i++){
            sleep(1);       
            pthread_mutex_lock(&mutexLog);     
            sprintf(Atencion,"Ha atendido una solicitud");
            writeLogMessage(ficheroLog, Atencion);
            pthread_mutex_unlock(&mutexLog);     
            pthread_mutex_lock(&mutexSolicitud  );
            Cliente.solicitud = 0;
            pthread_mutex_unlock(&mutexSolicitud);          
            pthread_mutex_lock(&mutexLog);
            sprintf(FinAtencion,"Ha finalizado la solicitud");
            writeLogMessage(ficheroLog, FinAtencion);
            pthread_mutex_unlock(&mutexLog);
            if(i == num_solicitudes - 1) {
                pthread_mutex_lock(&mutexSolicitud);
                num_solicitudes = 0;
                pthread_mutex_unlock(&mutexSolicitud);
                pthread_mutex_lock(&mutexLog);
                sprintf(UltimoCliente,"Se ha atendido al último cliente , y el técnico regresa.");
                writeLogMessage(ficheroLog, UltimoCliente);
                pthread_mutex_unlock(&mutexLog);
                pthread_cond_signal(&FinAtencionDomiciliaria);
            }
        }
    }
}



void manejadora(int sig1){ /*Manejadora que trata los clientes de app y clientes de red*/
    if (signal(SIGUSR1,manejadora) == SIG_ERR){
		perror("Error en la señal SIGUSR1\n");  /*Tratamiento de la señal*/
	}else if(signal(SIGUSR2,manejadora2) == SIG_ERR){
        perror("Error en la señal SIGUSR2\n"); /*Tratamiento de la señal*/
    }else{
        nuevoClienteRed(sig1);
        exit(0);
    }	
}

void manejadora2(int sig2){ /*Manejadora que termina el programa*/
    if(signal(SIGINT,manejadora2) == SIG_ERR){
        perror("Error en la señal SIGINT\n"); /*Tratamiento de la señal*/
    }else{
        exit(0);
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

void finalizarPrograma (int signal) {
    //Antes de que finalice se debe terminar de atender a todos los clientes en cola
    //Hilos tienen que terminar de forma correcta, se libera memoria, etc.
    char final[100];
    printf("\nSe ha recibido la señal SIGINT. Procediendo a finalizar el programa...\n");

    //Se escribe en el log que se va a finalizar el programa/
    pthread_mutex_lock(&mutexLog);
    sprintf(final, "El programa se va a finalizar. ");
    writeLogMessage(ficheroLog, final);
    pthread_mutex_unlock(&mutexLog);

    pthread_mutex_lock(&mutexCola);
    //Se ponen las solicitudes domiciliarias a 0
    num_solicitudes = 0;

    for(int i=0;i<num_clie_app;i++){
        pthread_mutex_lock(&mutexSolicitud);
        Clientes_App[i].solicitud = 0;
        pthread_mutex_unlock(&mutexSolicitud);
    }

    //A cada cliente se le cambia la flag de solicitud a 0 y espera a que termine de ser atendido para finalizar los hilos de los trabajadores
    for (int i = 0; i < num_clie_red; i++) {
        pthread_mutex_lock(&mutexSolicitud);
        Clientes_Red[i].solicitud = 0;
        pthread_mutex_unlock(&mutexSolicitud);
    }

    pthread_mutex_unlock(&mutexCola);

    /*Liberamos la memoria */

    free(Clientes_App);
    free(Clientes_Red);

    /*Destruimos los mutex*/

    pthread_mutex_destroy(&mutexCola);
    pthread_mutex_destroy(&mutexAtendido);
    pthread_mutex_destroy(&mutexSolicitud);
    pthread_mutex_destroy(&mutexLog);
    pthread_mutex_destroy(&mutexDescanso);
    
    printf("\nAdiós!\n");

    exit(0);
}

void eliminarClienteApp(int posicion) {
    for(int i = posicion; i < num_clie_app; i++) {
        pthread_mutex_lock(&mutexCola);
        Clientes_App[i] = Clientes_App[i+1];
        pthread_mutex_unlock(&mutexCola);
    }
    pthread_mutex_lock(&mutexCola);
    Clientes_App[num_clie_app - 1].id  = 0;
    Clientes_App[num_clie_app - 1].atendido  = 2;
    Clientes_App[num_clie_app - 1].tipo  = 2;
    Clientes_App[num_clie_app - 1].prioridad  = 0;
    Clientes_App[num_clie_app - 1].solicitud  = 0;
    num_clie_app--;
    pthread_mutex_unlock(&mutexCola);
}

void eliminarClienteRed(int posicion) {
    for(int i = posicion; i < num_clie_red; i++) {
        pthread_mutex_lock(&mutexCola);
        Clientes_Red[i] = Clientes_Red[i+1];
        pthread_mutex_unlock(&mutexCola);
    }
    pthread_mutex_lock(&mutexCola);
    Clientes_App[num_clie_red - 1].id  = 0;
    Clientes_App[num_clie_red - 1].atendido  = 2;
    Clientes_App[num_clie_red - 1].tipo  = 2;
    Clientes_App[num_clie_red - 1].prioridad  = 0;
    Clientes_App[num_clie_red - 1].solicitud  = 0;
    num_clie_red--;
    pthread_mutex_unlock(&mutexCola);    
}