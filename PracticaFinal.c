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

/*Semáforos*/
pthread_mutex_t mutexCola;
pthread_mutex_t mutexLog;
pthread_mutex_t mutexDescanso;
pthread_cond_t mutexSolicitud;

int num_clie_app;  //Contador Clientes App
int num_clie_red;  //Contador Clientes Red
int num_solicitudes; //Contador Solicitudes Atención Domiciliaria
int contador_ID_App //Contador del ID de los clientes de App
int contador_ID_Red //Contador del ID de los clientes de Red
/*Clientes*/

typedef struct{
    int id;
    int atendido;
    int tipo;
    int prioridad;
    int solicitud;
    time_t horaLlegada;
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
    contador_ID_App = 0;
    contador_ID_Red = 0;

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
    pthread_cond_init(&mutexSolicitud, NULL);
    
    /*Bucle infinito*/
    while(true){
        pause();
    }
    return 0;


void nuevoClienteRed(int signal){
    if(numclientes<max_num_cola){
        pthread_mutex_lock(&mutexCola);
        
        Clientes[numclientes].atendido = 0;
        Clientes[numclientes].tipo = signal;
        Clientes[numclientes].prioridad = 0;
        Clientes[numclientes].solicutud = 0;
        numclientes++;
        contadorID++;
        pthread_mutex_unlock(&mutexCola);
        if(signal==0){
            Clientes[numclientes].id = contador_ID_App + 1; //
            num_clie_app++;
        }else{
            Clientes[numclientes].id = contador_ID_Red + 1; 
            num_clie_red++;
        }
        pthread_create(&thread_id, NULL, myThreadFun, NULL);//Corregir
        
    }else{
        printf("La cola de atencion esta llena\n");
    }
}

/*
Calcula la prioridad con un numero aleatorio entre 1 (más baja) y 10 (más alta) , y en caso de empate, el que haya llegado antes
Clientes[numclientes].prioridad = calculaAleatorios(1,10);
if(indiceCliente == -1 || (Clientes[i].prioridad > Clientes[indiceCliente].prioridad) || (Clientes[i].prioridad == Clientes[indiceCliente].prioridad && difftime(Clientes[i].horaLlegada, Clientes[indiceCliente].horaLlegada) < 0)){
    indiceCliente = i;
}
*/


void AccionCliente(){
    //log entrada
    //log tipo cliente

    /*Compueblo si el cliente esta siendo atendido*/
    if(Clientes[].atendido==0){
        int dificultad;
        int cansancio;
        int conexion;
        if(dificultad==1 || cansancio==1 || conexion){
            printf("El cliente ha abandonado la cola por una de las siguientes razones: Dificultad, pérdida de conexión o cansancio");
        }else{
            sleep(2);
        }

    }else{
        if(clientes[].tipo ==2 && clientes[].solicutud==1){ /*Si es Cliente de Red*/
            if(num_solicitudes<4){
                num_solicitudes++;
                //log
                clientes[].solicutud=1;
                if()
            }else{
                sleep(3);
            }
        }else{ /*Si es cliente de App o no realiza solicitud domiciliaria*/
            //liberar
        }
    }
}

void AccionTecnico(){
    int i;
    /* Variable que representa el id del usuario a tratar. */
	int solicitudElegida;

    /* Se guarda para escribir en el Log el id del tecnico. */
	int identificadorTecnico = (intptr_t)ptr;
	char idTecnico[100];
	char mensajeLog[100];
	char mensaje[100];
	char descanso[100];
	char findescanso[100];

    while(1){    
		srand(time(NULL));
		switch(solicitud){
            /* Caso de que sea un atendedor de invitaciones. */
			case 1:
			    pthread_mutex_lock(&mutexCola);
			    identificadorTecnico = (intptr_t)ptr;
			    sprintf(idTecnico, "Tecnico_%d", identificadorTecnico+1);
                /* Se buscan solicitudes de tipo 0 (Invitacion). */
			    for(i=0;i<numSolicitudes;i++){
                    identificadorTecnico = (intptr_t)ptr;
				    sprintf(idTecnico, "Tecnico_%d", identificadorTecnico+1);

				    if(listaTecnicos[i].tipo==0 && listaTecnicos[i].ocupado==0 && listaAtendedores[identificadorTecnico].ocupado==0){
                        /* Cambiamos el flag de ocupado del atendedor. */
					    /* Cambiamos el flag de si esta ocupado del tecnico. */

					    listaTecnicos[identificadorTecnico].ocupado=1;
					    listaTecnicos[identificadorTecnico].contador++;

                        /* Calculamos el tipo de atencion. */
					    int numAle = calculaAleatorios(0,100);

                        /* Primer caso, 80% de que la solicitud tenga todo en orden. */
					    if(numAle < 80){
						    sleep(calculaAleatorios(1,4));
						    sprintf(mensajeLog,"La solicitud_%d tiene todo en orden.", Clientes[i].idSolicitud);
						    printf("La solicitud_%d tiene todo en orden.\n", Clientes[i].idSolicitud);
						    Clientes[i].tipo=0;

                        /* Segundo caso, 10% de que la solicitud tenga algún error en los datos personales. */
					    } else if(numAle < 90) {
						    sleep(calculaAleatorios(2,6));
						    sprintf(mensajeLog,"La solicitud_%d tiene errores en los datos personales.",Clientes[i].idSolicitud);
						    printf("La solicitud_%d tiene errores en los datos personales.\n",Clientes[i].idSolicitud);
						    Clientes[i].tipo=1;
                        /* Tercer y ultimo caso, 10% de que se corresponda con un usuario de otra compañia. */
					    } else {
						    sleep(calculaAleatorios(1,2));
						    sprintf(mensajeLog,"La solicitud_%d es erronea por confusion de compañia.",Clientes[i].idSolicitud);
						    printf("La solicitud_%d es erronea por confusion de compañia.\n",Clientes[i].idSolicitud);
						    Clientes[i].tipo=2;
					    }


                        /* Log del momento de fin de atencion. */
					    printf("La solicitud_%d ha sido atendida correctamente por el %s.\n", Clientes[i].idSolicitud, idTecnico);
					    sprintf(mensaje, "La solicitud_%d ha sido atendida correctamente", Clientes[i].idSolicitud);
					    escribirEnLog(idTecnico, mensaje);

                        /* Log de como ha sido la atencion. */                              
					    escribirEnLog(idTecnico, mensajeLog);

                        /* Se cambia el flag de ocupado de la solicitud y se desbloquea el mutex. */
					    Clientes[i].ocupado=1;
					    pthread_mutex_unlock(&mutexCola);

                        /* Comprobacion si le toca descanso. */
					    if(listaTecnicos[0].contador != 0){
						    if(listaTecnicos[0].contador%5 == 0){

                                pthread_mutex_lock(&mutexDescanso);
                                if (descanso==0){
                            	    descanso=1;
                            	    pthread_mutex_unlock(&mutexDescanso);
                            	    printf("El tecnico se va al descanso. \n");
								    sprintf(descanso, "El tecnico se al descanso.");
								    escribirEnLog(idTecnico, descanso);
	                                /* Duerme 5 segundos */
								    sleep(5);

	                                /* Se registra la salida al descanso. */
								    printf("El tecnico regresa del descanso.\n");
								    sprintf(findescanso, "El tecnico regresa del descanso.");
								    escribirEnLog(idTecnico, findescanso);
								    pthread_mutex_lock(&mutexDescanso);
								    descanso=0;
	                            }
	                        pthread_mutex_unlock(&mutexDescanso);
						    }
					    }
				    }
			    }

                for(i=0;i<numClientesRed;i++){

				    identificadorTecnico = (intptr_t)ptr;
				    sprintf(idTecnico, "tecnico_%d", identificadorTecnico+1);

				    if(Clientes[i].ocupado==0 && Tecnicos[identificadorTecnico].ocupado==0){
                        /* Cambiamos el flag de si esta ocupado del tecnico. */
					    listaTecnicos[identificadorTecnico].ocupado=1;
					    listaTecnicos[identificadorTecnico].contador++;

                        /* Calculamos el tipo de atencion. */
					    int numAle = calculaAleatorios(0,100);

                        /* Primer caso, 80% de que la solicitud tenga todo en orden. */
					    if(numAle < 80){
						    sleep(calculaAleatorios(1,4));
						    sprintf(mensajeLog,"La solicitud_%d tiene todo en orden.", Clientes[i].idSolicitud);
						    printf("La solicitud_%d tiene todo en orden.\n", Clientes[i].idSolicitud);
						    Clientes[i].tipoAtencion=0;

                        /* Segundo caso, 10% de que la solicitud tenga algún error en los datos personales. */
					    } else if(numAle < 90) {
						    sleep(calculaAleatorios(2,6));
						    sprintf(mensajeLog,"La solicitud_%d tiene errores en los datos personales.",Clientes[i].idSolicitud);
						    printf("La solicitud_%d tiene errores en los datos personales.\n",Clientes[i].idSolicitud);
						    Clientes[i].tipoAtencion=1;

                        /* Tercer y ultimo caso, 10% de que se corresponda con un usuario de otra compañia. */
					    } else {
						    sleep(calculaAleatorios(1,2));
						    sprintf(mensajeLog,"La solicitud_%d es erronea por confusion de compañia.",Clientes[i].idSolicitud);
						    printf("La solicitud_%d es erronea por confusion de compañia.\n",Clientes[i].idSolicitud);
						    Clientes[i].tipoAtencion=2;
					    }


                        /* Log del momento de fin de atencion. */
					    printf("La solicitud_%d ha sido atendida correctamente por el %s.\n", Clientes[i].idSolicitud, idTecnico);
					    sprintf(mensaje, "La solicitud_%d ha sido atendida correctamente", Clientes[i].idSolicitud);
					    escribirEnLog(idTecnico, mensaje);

                        /* Log de como ha sido la atencion. */                              
					    escribirEnLog(idTecnico, mensajeLog);

                        /* Se cambia el flag de ocupado de la solicitud y se desbloquea el mutex. */
					    Clientes[i].ocupado=1;
					    pthread_mutex_unlock(&mutexCola);

                        /* Comprobacion si le toca descanso. */
					    if(listaTecnicos[0].contador != 0){
						    if(listaTecnicos[0].contador%5 == 0){
                                pthread_mutex_lock(&mutexDescanso);
                                if (descanso==0){
                            	    descanso=1;
                            	    pthread_mutex_unlock(&mutexDescanso);
                            	    printf("El tecnico se va al descanso. \n");
								    sprintf(descanso, "El tecnico se al descanso.");
								    escribirEnLog(idTecnico, descanso);
	                                /* Duerme 5 segundos */
								    sleep(5);
	                                /* Se registra la salida al descanso. */
								    printf("El tecnico regresa del descanso.\n");
								    sprintf(findescanso, "El tecnico regresa del descanso.");
								    escribirEnLog(idTecnico, findescanso);
								    pthread_mutex_lock(&mutexDescanso);
								    descanso=0;
	                            }
	                        pthread_mutex_unlock(&mutexDescanso);
						    }
					    }
				    }
			    }
                /* En caso de que no encuentre ninguna solicitud que atender libera el mutex. */
                default:    
    			    pthread_mutex_unlock(&mutexCola);                    
			        break;


        }
    }
}

void *AccionEncargado(void *ptr){
    // Se obtiene el identificador del encargado
    intptr_t identificadorEncargado = (intptr_t)ptr;
    // Se crea una cadena de caracteres con el identificador del encargado
    char idEncargado[100];
    sprintf(idEncargado, "encargado_%d", identificadorEncargado+1);
    while(1){
        // Se bloquea el mutex de la cola para realizar operaciones en ella
        pthread_mutex_lock(&mutexCola);
        int indiceCliente = -1;
        int indiceTecnico = -1;
        int indiceResponsable = -1;
        // Se recorre la cola de clientes
        for(int i = 0; i < max_num_cola; i++){
            // Si el cliente no ha sido atendido
            if(Clientes[i].atendido == 0){
                // Si es de tipo 0 (problemas en la app) y no se ha encontrado un cliente de ese tipo aún
                if(Clientes[i].tipo == 0 && indiceTecnico == -1){
                    indiceTecnico = i;
                }
                  // Si es de tipo 1 (problemas en la red) y no se ha encontrado un cliente de ese tipo aún
                if(Clientes[i].tipo == 1 && indiceResponsable == -1){
                    indiceResponsable = i;
                }
                // Si no se ha encontrado ningún cliente aún o el cliente actual tiene mayor prioridad que el último encontrado
                if(indiceCliente == -1 || Clientes[i].prioridad > Clientes[indiceCliente].prioridad){
                    indiceCliente = i;
                }
            }
        }
        if(indiceResponsable != -1){
            Clientes[indiceResponsable].atendido = 1;
            pthread_mutex_unlock(&mutexCola);
            char mensaje[100];
            sprintf(mensaje, "El encargado_%d está atendiendo al cliente_%d", identificadorEncargado+1, Clientes[indiceResponsable].id);
            writeLogMessage(idEncargado, mensaje);
            sleep(calculaAleatorios(1,4));
        }else if(indiceTecnico != -1){
            Clientes[indiceTecnico].atendido = 1;
            pthread_mutex_unlock(&mutexCola);
            char mensaje[100];
            sprintf(mensaje, "El encargado_%d está atendiendo al cliente_%d", identificadorEncargado+1, Clientes[indiceTecnico].id);
            writeLogMessage(idEncargado, mensaje);
            sleep(calculaAleatorios(1,4));
        }else{
            pthread_mutex_unlock(&mutexCola);
            sleep(1);
        }
    }
}

void AccionTecnicoDomiciliario(){
    pthread_mutex_lock(&mutexDescanso);
    while(num_solicitudes<4){ //Si no esta lleno
        pthread_cond_wait(&mutexSolicitud) ; // Se bloquea
        //mutex log     
        
    }
    for(int i=0;i<num_solicitudes;i++){
        printf("Durmiedo un segundo por la peticion: \n")
        sleep(1);       
        pthread_mutex_lock(&mutexDescanso);     
        printf("Ha atendido a la solicitud\n");
        pthread_mutex_unlock(&mutexDescanso);     
     
        pthread_mutex_lock(&mutexInventado);
        Clientes[].solicutud = 0;
        pthread_mutex_unlock(&mutexInventado);
    
        //mutex log
    }
    pthread_cond_signal(&mutexSolicitud);
    pthread_exit(1);
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


