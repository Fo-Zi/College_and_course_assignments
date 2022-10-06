#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h> 
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#include <sys/types.h>    
#include <sys/socket.h>  
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

/*****************************DEFINES*****************************/

//SERVER
#define PORT_NUM           1234  	// Numero de Port
#define IP_ADDR "127.0.0.1" 	// Direccion IP LOCALHOST
#define NCOLA 2
#define SOCKET_PROTOCOL 0

#define TIMEOUT 60 			// Global p/todos los clientes

//JUEGO
#define CARTAS_MAZO 40
#define CARTAS_MANO 3
#define NRO_MAX_CARTAS_MESA 20
#define NRO_MAX_CARTAS_REC 40

#define PASADAS_RAND 100
#define UPPER 39 
#define LOWER 0

//MEM COMP
#define NUMERO 123
#define ARCHIVO "/bin/more"

/*****************************GLOBAL*****************************/



extern int errno ;

//JUEGO------------------------------------------------
char *palos[4] = {"Basto" , "Espada" , "Copa" , "Oro"};

struct Carta{
	char palo[6];
	unsigned int val;
};
size_t TAM_CARTA = sizeof(struct Carta);

struct Jugador{
	struct Carta cartas_rec[NRO_MAX_CARTAS_REC];
	unsigned int nro_c_rec;
	char nombre[20];
	int escobas;
	struct Carta mano[3];
	unsigned int nro_c_mano;
};


struct Carta mazo[CARTAS_MAZO];	//Array de cartas -> conforma el mazo
unsigned int indice_mazo = 0;

struct mesa_compartida{
	struct Carta cartas_mesa[NRO_MAX_CARTAS_MESA];		//Array de cartas mesa -> conforma pozo
	unsigned int nro_c_mesa;
	char nombres_jug[4][20];
	int ultimo_levante;
};



typedef struct{
	long id;
	bool flag;
}mensaje1;

typedef struct{
	long id;
	struct Carta carta;
}mensaje2;

typedef struct{
	long id;
	char jugada[200];
}mensaje3;

mensaje3 msj3;


/*****************************FUNCIONES P/JUEGO*****************************/

void inic_mazo(){
	unsigned int cont=0;
	int i,j;
	for(j=0 ; j<4 ; j++){			//Inicializa de forma ordenada, palo por palo
		for(i=1 ; i<=7 ; i++){
			strcpy(mazo[cont].palo ,palos[j]);
			mazo[cont].val = i;
			cont++;
		}
		for(i=10 ; i<=12 ; i++){
			strcpy(mazo[cont].palo ,palos[j]);
			mazo[cont].val = i;
			cont++;
		}
	}
}

void mezclar_mazo(){
	srand(time(0));					//Se toma como valor semilla para el algoritmo una medición de tiempo
	int i;
	for(i=0 ; i < PASADAS_RAND ; i++){		//Se intercambia la posición de las cartas del mazo de forma aleat
		int j = (rand() % (UPPER - LOWER + 1)) + LOWER;
		int k = (rand() % (UPPER - LOWER + 1)) + LOWER;
		struct Carta carta_aux;
		carta_aux = mazo[j];
		mazo[j] = mazo[k];
		mazo[k] = carta_aux;
	}
}

void primera_mano( int nro_jug, struct mesa_compartida *mem_comp ){
	unsigned int i;
	mem_comp->nro_c_mesa = 4;
	for(i=0 ; i< mem_comp->nro_c_mesa ; i++){
		mem_comp->cartas_mesa[i] = mazo[indice_mazo];
		printf("---%d de %s--- \n",mem_comp->cartas_mesa[i].val,mem_comp->cartas_mesa[i].palo);
		fflush(stdout);
		indice_mazo++;		
	}

}

int convert_valor(int a){
	switch(a){
		case 12:
			return 10;
			break;
		case 11:
			return 9;
			break;
		case 10: 
			return 8;
			break;
		default:
			return a;
	}
}


bool levantar_cartas(struct Jugador *jug,struct mesa_compartida *mem_comp,int c_jugo , int *c_lev, int nro_c_lev ){
	int i,sum = 0;
	sprintf(msj3.jugada,"\n--%s jugó la carta: %d de %s--",jug->nombre,jug->mano[c_jugo-1].val,jug->mano[c_jugo-1].palo);
	sum += convert_valor(jug->mano[c_jugo-1].val);
	for(i=0 ; i< nro_c_lev; i++){
		char auxstr[50];
		sprintf(auxstr,"\nLevantó la carta: %d de %s",mem_comp->cartas_mesa[c_lev[i]-1].val,mem_comp->cartas_mesa[c_lev[i]-1].palo);
		strcat(msj3.jugada,auxstr);
		sum+= convert_valor(mem_comp->cartas_mesa[c_lev[i]-1].val);
	}
	if( sum == 15 ) {
		struct Carta aux;
        	aux = jug->mano[c_jugo-1];
 	        jug->mano[c_jugo-1] = jug->mano[jug->nro_c_mano - 1];
		jug->mano[jug->nro_c_mano - 1] = aux;
    	     	jug->nro_c_mano--;        	
         	jug->cartas_rec[jug->nro_c_rec] = aux;
		jug->nro_c_rec++;
		
		for(int i=0 ; i<nro_c_lev ; i++){
			struct Carta aux1;
        		aux1 = mem_comp->cartas_mesa[c_lev[i]-1];
 	        	mem_comp->cartas_mesa[c_lev[i]-1] = mem_comp->cartas_mesa[(mem_comp->nro_c_mesa)-1];        			
			mem_comp->cartas_mesa[(mem_comp->nro_c_mesa)-1] = aux1;
    	     		mem_comp->nro_c_mesa--;
         		
			jug->cartas_rec[jug->nro_c_rec] = aux1;
			jug->nro_c_rec++;
		}
		int aux2 = mem_comp -> nro_c_mesa;
		char auxstr[200];	
		switch(aux2){
            		case 0:
           			
           			sprintf(auxstr,"\nMESA: \n\n");
           			strcat(msj3.jugada,auxstr);          	
				break;
          		case 1:
                     		sprintf(auxstr,"\nMESA:---%d de %s (1)--- \n\n",mem_comp->cartas_mesa[0].val,mem_comp->cartas_mesa[0].palo);                   	
                     		strcat(msj3.jugada,auxstr); 
				break;
               	default:
                    		sprintf(auxstr,"\nMESA:---%d de %s (1)--- ",mem_comp->cartas_mesa[0].val,mem_comp->cartas_mesa[0].palo);
				strcat(msj3.jugada,auxstr); 
				for(int i=1; i< mem_comp->nro_c_mesa; i++ ){
					sprintf(auxstr,"---%d de %s (%d)---   ",mem_comp->cartas_mesa[i].val,mem_comp->cartas_mesa[i].palo,i+1);
				strcat(msj3.jugada,auxstr);	
				}
				sprintf(auxstr,"\n\n");
				strcat(msj3.jugada,auxstr);                      	
			break;
        }
		return true;
	}else{
		printf("Las cartas elegidas no suman 15. Intente nuevamente.\n");
		fflush(stdout);
		return false;
	}
}

void descartar_carta(struct Jugador *jug , int c_jugo, struct mesa_compartida *mem_comp ){
	
	struct Carta aux;
	aux = jug->mano[c_jugo-1];
	jug->mano[c_jugo-1] = jug->mano[jug->nro_c_mano - 1];
	jug->mano[jug->nro_c_mano - 1] = aux;
	jug->nro_c_mano--;
	mem_comp->nro_c_mesa++;
	mem_comp->cartas_mesa[mem_comp->nro_c_mesa-1] = aux;
	sprintf(msj3.jugada,"\n%s descartó la carta: %d de %s",jug->nombre,aux.val,aux.palo);
	int aux2 = mem_comp -> nro_c_mesa;
	printf("Nro de cartas en la mesa:%d\n",aux2);
	fflush(stdout);
	char auxstr[200];	
		switch(aux2){
            		case 0:	
           			sprintf(auxstr,"\nMESA: \n\n");
           			strcat(msj3.jugada,auxstr);          	
				break;
          		case 1:
                     		sprintf(auxstr,"\nMESA:---%d de %s (1)---\n\n",mem_comp->cartas_mesa[0].val,mem_comp->cartas_mesa[0].palo);                   	
                     		strcat(msj3.jugada,auxstr); 
				break;
               	default:
                    		sprintf(auxstr,"\nMESA:---%d de %s (1)--- ",mem_comp->cartas_mesa[0].val,mem_comp->cartas_mesa[0].palo);
				strcat(msj3.jugada,auxstr); 
				for(int i=1; i< mem_comp->nro_c_mesa; i++ ){
					sprintf(auxstr,"---%d de %s (%d)---   ",mem_comp->cartas_mesa[i].val,mem_comp->cartas_mesa[i].palo,i+1);
					strcat(msj3.jugada,auxstr);	
				}            
				sprintf(auxstr,"\n\n");
				strcat(msj3.jugada,auxstr);        	
			break;
        }
}


void cargar_mano(struct Jugador *jug, char *buf_tx){
	int nro_c = jug-> nro_c_mano;
	char auxstr[200];
	sprintf(buf_tx,"\nMANO: ");  
	for(int i=0 ; i<nro_c; i++){
		sprintf(auxstr,"---%d de %s (%d)---   ",jug->mano[i].val,jug->mano[i].palo,i+1);
		strcat(buf_tx,auxstr);
	}

}

void cargar_mesa(struct mesa_compartida *mem_comp,char *buf_tx){
	int nro_c = mem_comp -> nro_c_mesa;
	printf("Nro de cartas en la mesa:%d\n",nro_c);
	fflush(stdout);
	char auxstr[200];	
	switch(nro_c){
       	case 0:	
          		sprintf(buf_tx,"\nMESA: ");          	
			break;
               default:
                    	sprintf(buf_tx,"\nMESA:---%d de %s (1)--- ",mem_comp->cartas_mesa[0].val,mem_comp->cartas_mesa[0].palo);
			for(int i=1; i< mem_comp->nro_c_mesa; i++ ){
			  sprintf(auxstr,"---%d de %s (%d)---   ",mem_comp->cartas_mesa[i].val,mem_comp->cartas_mesa[i].palo,i+1);
			  strcat(buf_tx,auxstr);	
			}                     	
			break;
        }

}

void cargar_c_levantadas(struct Jugador *jug , char *buf_tx){
	int aux = jug->nro_c_rec;
	char auxstr[200];	
	switch(aux){
       	case 0:	
          		sprintf(buf_tx,"\n--- CARTAS RECOLECTADAS:   ---");          	
			break;
               default:
                    	sprintf(buf_tx,"\n--- CARTAS RECOLECTADAS:---%d de %s (1)--- ",jug->cartas_rec[0].val,jug->cartas_rec[0].palo);
			for(int i=1; i< aux; i++ ){
			  sprintf(auxstr,"---%d de %s (%d)---   ",jug->cartas_rec[i].val,jug->cartas_rec[i].palo,i+1);
			  strcat(buf_tx,auxstr);	
			}                     	
			break;
        }

};

void cargar_escobas(struct Jugador *jug , char *buf_tx){
	int aux = jug->escobas;
	char auxstr[200];	
	switch(aux){
       	case 0:	
          		sprintf(auxstr,"\n--- ESCOBAS REALIZADAS: 0  :(  ---");
          		strcat(buf_tx,auxstr);          	
			break;
               default:
                    	sprintf(auxstr,"\n---   ESCOBAS REALIZADAS: %d    --- ",jug->escobas);
               	strcat(buf_tx,auxstr);
			break;
        }

};


/*****************************MAIN*****************************/


int main(int argc, char *argv[]){

  unsigned int         server_s;        // Descriptor del socket
  unsigned int         connect_s;       // Connection socket descriptor
  struct sockaddr_in   server_addr;     // Estructura con los datos del servidor
  struct sockaddr_in   client_addr;     // Estructura con los datos del cliente
  struct in_addr       client_ip_addr;  // Client IP address
  int                  addr_len;        // Tamaño de las estructuras
  char                 buf_tx[200];     // Buffer de 1500 bytes para los datos a transmitir
  char                 buf_rx[200];     // Buffer de 1500 bytes para los datos a transmitir
  int                  bytesrecibidos, bytesaenviar, bytestx;  // Contadores
  int                  i=0;             //contador de mensajes
  pid_t pid_n;
  char arch[8];

  unsigned short int f_count=0;	// Contador de archivos escritos
  
  /***Variables - Cola msjes***/
  key_t Buf_Key;
  int Buf_Id,id;
  int Id_Cola_Mensajes;
  mensaje1 msj1;
  mensaje2 msj2;
  msj1.flag = false;	
  
  /***Variables mem compartida***/
  key_t Clave;
  int Id_Memoria1;
  
  
  
  /***Inicialización de cola de msjes***/
  Buf_Key = ftok ("/bin/ls", 33);
  if (Buf_Key == (key_t)-1){
	printf("Failed to get key\n");
	exit(1);
  }
	
  Id_Cola_Mensajes = msgget (Buf_Key, 0600 | IPC_CREAT);
  if (Id_Cola_Mensajes == -1){
	printf("Error al obtener identificador para cola mensajes" "\n");
	exit (-1);
  }
  
  /***Inicialización de memoria compartida***/
  Clave = ftok (ARCHIVO, NUMERO);
  if (Clave == -1){
		printf("No consegui  clave para memoria compartida\n");
		exit(1);
  }

  Id_Memoria1 = shmget (Clave, sizeof(struct mesa_compartida), 0666 | IPC_CREAT);
  if (Id_Memoria1 == -1){
	printf("No consegui Id para memoria compartida\n");
	exit (2);
  }
  
  struct mesa_compartida *mem_comp; 
  mem_comp = (struct mesa_compartida*)shmat (Id_Memoria1, (const void *)0, 0);
  if (mem_comp == NULL){
	printf("No consegui asociar la memoria compartida a una variable\n");
	exit (3);
  }

  /***Crea el socket***/
  server_s = socket(AF_INET, SOCK_STREAM, SOCKET_PROTOCOL);
  if (server_s==-1)
    {
    perror("socket");
    return 1;
  }
  
  printf("Cree el descriptor del socket %d\n",server_s);
  
 /***SETEO DE ATRIBUTOS DEL STRUCT DE DATOS DEL SERVER***/

  server_addr.sin_family      = AF_INET;            // Familia TCP/IP

  server_addr.sin_port        = htons(PORT_NUM);    // Número de Port, htons() lo convierte al orden de la red

  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // INADDR_ANY = cualquier direccion IP, htonl() lo convierte al orden de la red
  
  /***Se asocian  los datos de la estructura con el descriptor de socket***/
  bind(server_s, (struct sockaddr *)&server_addr, sizeof(server_addr));
  printf("asocie el descriptor %u con el port %u acepta conexiones desde %u\n", server_s,PORT_NUM, INADDR_ANY) ;
  
  /*Socket en apertura pasiva y asocio una cola  NCOLA posiciones*/
  listen(server_s, NCOLA);
  addr_len = sizeof(client_addr);
  
  /***Se abre el archivo para mantener registro de conexiones***/
  FILE *registro;
  registro=fopen("registro_conexiones.txt","w");
  
  //Var aux
  unsigned int nro_jug_c = 0 , nro_jug_max = 1; //Nro jug max se inicializa en 1 por conveniencia, se reescribe luego con
  						  //input del jugador
  						  
/************************LOOP PRINCIPAL DEL PROGRAMA************************/
  
  //PROCESO PADRE
  bool terminar_padre = false;
  while(!terminar_padre){   
    
    /*** Espera bloqueante de conexiones entrantes ***/
    if( (nro_jug_c < nro_jug_max) ){
    connect_s = accept(server_s, (struct sockaddr *)&client_addr, &addr_len);

    if (connect_s==-1){	//Error handling de accept
      perror("accept");
      return 2;
    }
    //Var contadora de jugadores conectados
    nro_jug_c++;
    
    //Flujo para el PRIMER JUGADOR
    if(nro_jug_c ==1 ){
  	
  	sprintf(buf_tx,"---Bienvenido al juego---\nUsted es el primer jugador, por favor ingrese cuántos jugadores integrarán la partida: ");
	bytesaenviar=strlen(buf_tx); //me aseguro que la cantidad de bytes a enviar sea la correcta  
 	bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
  	memset(buf_tx,0,strlen(buf_tx));
  	
  	bytesrecibidos=recv(connect_s, buf_rx, sizeof(buf_rx), 0);
 	printf("%s\n", buf_rx); 
  	fflush(stdout);

  	sscanf(buf_rx,"%d",&nro_jug_max);
  	
    }
    
    /*** Se muestra info del proc padre y cliente ***/
    printf("Soy el padre %d y digo que el IP del cliente es: %s y el port del cliente es %hu \n",getpid(),    
    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    }

/*****************************PROCESO/S HIJO/S*****************************/    
    if((pid_n=fork())==0) {
     	
     	close (server_s); // El socket server_s no lo necesita el proceso hijo, sólo el connect_s
        
        /*** Imprimo la información del cliente y del hijo ***/
        printf("El hijo %d dice que el IP del cliente es: %s y el port del cliente es %hu \n",getpid(),    
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port)); 
        
        /*** Seteo de timeout para socket cliente ***/
        struct timeval tv;
  	tv.tv_sec = TIMEOUT;
  	tv.tv_usec = 0;
  	setsockopt(connect_s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        
        
        /*** Se envía un mensaje de confirmación al cliente ***/
        int nro_jugador = nro_jug_c;
	printf("nro jug: %d\n",nro_jugador);
	fflush(stdout);
	if(nro_jugador!=1){
		
		//Msje de aviso al jug-
		sprintf(buf_tx,"---Bienvenido al juego!---\nUsted es el jugador número: %d\n",nro_jugador); 
        	bytesaenviar=strlen(buf_tx); //me aseguro que la cantidad de bytes a enviar sea la correcta  
        	bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
		memset(buf_tx,0,strlen(buf_tx));
        
		if (bytestx==-1){	//Se hace error handling del envío
			perror ("send");
		 	return 3;
		}
        
		printf("Envie el mensaje %s con %d bytes\n", buf_tx, bytestx); //Msje de control para debugging
	
	}
       
        //Se inicializa al jugador y sus variables
        struct Jugador jug;
        jug.nro_c_mano = 0;
        jug.nro_c_rec = 0;
        jug.escobas = 0;
       
	//Msje de aviso al jug-       
     	sprintf(buf_tx,"\nIngrese a continuación su nombre: "); 
     	bytesaenviar=strlen(buf_tx); //me aseguro que la cantidad de bytes a enviar sea la correcta  
    	bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
	memset(buf_tx,0,strlen(buf_tx));
	
	//Recibe el nombre y almacena en struct Jugador
	bytesrecibidos=recv(connect_s, buf_rx, sizeof(buf_rx), 0);
  	printf("Cliente %d respondió: %s\n",nro_jugador,buf_rx);
	fflush(stdout);
	
	sscanf(buf_rx,"%s",jug.nombre);
	
	//Msje de aviso a jug-
	sprintf(buf_tx,"\n\n--La partida comenzará cuando se conecte el resto de los jugadores--\n "); 
     	bytesaenviar=strlen(buf_tx); //me aseguro que la cantidad de bytes a enviar sea la correcta  
    	bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
	memset(buf_tx,0,strlen(buf_tx));
       	
//--------------------------------LOOP PRINCIPAL-------------------------------------------------------       
        
        
    //Este loop corresponde a una RONDA -> 3 manos
    bool terminar1 = false;
    while(!terminar1){
             
        //Jug recibe las cartas para nueva mano
	for(int i=0 ; i<3 ; i++){
        	msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj2, sizeof(msj2) - sizeof(msj2.id), nro_jugador, 0);
   		jug.mano[i] = msj2.carta;
   		jug.nro_c_mano++;
        }
        
        //Confirma a proceso padre que recibió todas las cartas correctamente
	msj1.id = nro_jug_max + nro_jugador;
	msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1)-sizeof(msj1.id),0);
        
        //Confirma al cliente que queda al menos una ronda más
	sprintf(buf_tx,"\n---A continuación se iniciará la ronda---\n"); 
        bytesaenviar =  strlen(buf_tx)+1;
    	buf_tx[bytesaenviar]= '\0'; 
        bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
        memset(buf_tx,0,strlen(buf_tx));
        
        //Este loop corresponde a las 3 manos de cada ronda
        for(int i=0 ; i < 3 ; i++){
        	
        	//Carga la mano en buf_tx y se la envía al cliente
		cargar_mano(&jug,buf_tx);
		bytesaenviar =  strlen(buf_tx)+1;
    		buf_tx[bytesaenviar]= '\0'; 
        	bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
		memset(buf_tx,0,strlen(buf_tx));

		//Espera que el proc padre habiite acceso a mem compartida
		printf("nro jug: %d\n",nro_jugador);
		fflush(stdout);
		msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj1, sizeof(msj1) - sizeof(msj1.id), nro_jugador, 0);
		
		//Temporal
		strcpy(mem_comp-> nombres_jug[nro_jugador-1] , jug.nombre);
		
		//Carga la mesa como texto en buf_tx y envía al jugador 
		cargar_mesa(mem_comp,buf_tx);
		bytesaenviar =  strlen(buf_tx)+1;
		buf_tx[bytesaenviar]= '\0'; 
		bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
		memset(buf_tx,0,strlen(buf_tx));
	
		//Envía confirmación de que terminó de acceder a M.Compartida
		msj1.id = nro_jug_max + nro_jugador;
		msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1)-sizeof(msj1.id),0);	
												
		//Msje de aviso al jug-
		strcpy(buf_tx,"\n\n--Espere y observe el juego hasta que le toque su turno--\n\n"); 
		bytesaenviar =  strlen(buf_tx)+1;
		buf_tx[bytesaenviar]= '\0'; 
		bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
		memset(buf_tx,0,strlen(buf_tx));									
		
		//Jugador recibe las jugadas de jug PREVIOS a sí (Orden de conexión)
		for(int i=1 ; i < nro_jugador ; i++) {	
			msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj3, sizeof(msj3) - sizeof(msj3.id), nro_jugador, 0); 
			
			strcpy(buf_tx,msj3.jugada); 
			bytesaenviar =  strlen(buf_tx)+1;
			buf_tx[bytesaenviar]= '\0'; 
			bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
			memset(buf_tx,0,strlen(buf_tx));

		}

		//Jugada		
		printf("--Juega jugador %d--\n", nro_jugador);
		bool terminar2 = false;
		do{		
			int aux1,aux2,c_jugo;
			
			//Carga la mano en buf_tx y se la envía al cliente
			cargar_mano(&jug,buf_tx);
			bytesaenviar =  strlen(buf_tx)+1;
    			buf_tx[bytesaenviar]= '\0'; 
        		bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
			memset(buf_tx,0,strlen(buf_tx));
			
			//Carga la mesa como texto en buf_tx y envía al jugador 
			cargar_mesa(mem_comp,buf_tx);
			bytesaenviar =  strlen(buf_tx)+1;
			buf_tx[bytesaenviar]= '\0'; 
			bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
			memset(buf_tx,0,strlen(buf_tx));
			
			//Loopea con el cliente hasta que ingrese una JUGADA VÁLIDA
			bool jugada = true;
			while(jugada){
			
			//Msje de aviso al jug-
			sprintf(buf_tx,"\nIngrese (1) si desea jugar,o (2) si desea descartar: "); 
			bytesaenviar =  strlen(buf_tx)+1;
			buf_tx[bytesaenviar]= '\0'; 
			bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
			memset(buf_tx,0,strlen(buf_tx));
			
			//Recibe la decisión del jug y almacena
			bytesrecibidos=recv(connect_s, buf_rx, sizeof(buf_rx), 0);
			sscanf(buf_rx,"%d",&aux2);
			
			
			//LEVANTE
			if( aux2 == 1 ){
				bool carta_valida = true;

				do{
					//Msje de aviso al jug-
					sprintf(buf_tx,"Indique QUÉ carta va a jugar:"); 
					bytesaenviar =  strlen(buf_tx)+1;
					buf_tx[bytesaenviar]= '\0'; 
					bytestx=send(connect_s, buf_tx, bytesaenviar, 0);			
					memset(buf_tx,0,strlen(buf_tx));
			
					//Recibe qué carta va a jugar(de su mano) y almacena
					bytesrecibidos=recv(connect_s, buf_rx, sizeof(buf_rx), 0);
					sscanf(buf_rx,"%d",&c_jugo);
				
					if(c_jugo<0 || c_jugo > jug.nro_c_mano){
						carta_valida  = false;
						sprintf(buf_tx,"Ingresó una carta no válida, reingrese por favor.\n"); 
						bytesaenviar =  strlen(buf_tx)+1;
						buf_tx[bytesaenviar]= '\0'; 
						bytestx=send(connect_s, buf_tx, bytesaenviar, 0);			
						memset(buf_tx,0,strlen(buf_tx));
				
					}else{
						carta_valida=true;
					}
				}while(!carta_valida);
				
				//Msje de aviso al jug-
				sprintf(buf_tx,"Indique CUÁNTAS cartas va a levantar:"); 
				bytesaenviar =  strlen(buf_tx)+1;
				buf_tx[bytesaenviar]= '\0'; 
				bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
				memset(buf_tx,0,strlen(buf_tx));
				
				//Recibe la cant de cartas a lev y almacena
				bytesrecibidos=recv(connect_s, buf_rx, sizeof(buf_rx), 0);
				sscanf(buf_rx,"%d",&aux1);
				
				int c_lev[aux1];
				bool misma_carta = false;	
				do{
					for(int j=0 ; j<aux1 ; j++){
						do{
							//Mensaje a jug -> Cumple una función meramente estética
							sprintf(buf_tx,"Carta %d: ",j+1); 
							bytesaenviar =  strlen(buf_tx)+1;
							buf_tx[bytesaenviar]= '\0'; 
							bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
							memset(buf_tx,0,strlen(buf_tx));	
							//Se recibe msje con qué carta de la mesa levanta y almacena
							bytesrecibidos=recv(connect_s, buf_rx, sizeof(buf_rx), 0);
							sscanf(buf_rx,"%d",&c_lev[j]);
							//Se evalúa que la carta ingresada sea válida
							if( c_lev[j]<0 || c_lev[j]>(mem_comp->nro_c_mesa) ){
								carta_valida = false;
								
								sprintf(buf_tx,"\nCarta no válida, reingrese por favor "); 
								bytesaenviar =  strlen(buf_tx)+1;
								buf_tx[bytesaenviar]= '\0'; 
								bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
								memset(buf_tx,0,strlen(buf_tx));	
							}else{
								carta_valida = true;
							}
						}while(!carta_valida);
					}
				
					//Se evalúa que no se haya ingresado ninguna carta repetida	
					for(int j=0 ; j<aux1 ; j++){
						int aux = c_lev[j];	
						for(int k= 0 ; k< aux1 ; k++){
							if(j!=k && c_lev[k]==aux){
								misma_carta = true;
							}		
						
						}
					}
					//Se avisa al jugador que jugó incorrectamente y se setea booleana para repetir jugada.
					if( misma_carta ){
						sprintf(buf_tx,"\nIngresó dos cartas iguales. Reintente nuevamente.\n"); 
						bytesaenviar =  strlen(buf_tx)+1;
						buf_tx[bytesaenviar]= '\0'; 
						bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
						memset(buf_tx,0,strlen(buf_tx));		
						misma_carta=false;
					}
				
				}while( misma_carta );

				//Evalúa si las cartas suman 15
				if(levantar_cartas(&jug, mem_comp , c_jugo , c_lev,aux1)){
 					
 					mem_comp->ultimo_levante = nro_jugador; //Llevo cuenta de quién fue el último en levantar
					int c_mesa = mem_comp->nro_c_mesa;	 //Var aux con numero de cartas en mesa
					
					//Evalúa si el jugador hizo escoba
					if(c_mesa==0){
						//Se avisa al jug que hizo una escoba y se aumenta var contadora
						jug.escobas++;
						sprintf(buf_tx,"\n\n------Felicitaciones! Hizo una escoba!------\n\n"); 
						bytesaenviar =  strlen(buf_tx)+1;
						buf_tx[bytesaenviar]= '\0'; 
						bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
						memset(buf_tx,0,strlen(buf_tx));
			
					}else{					
						//Msje de aviso al jug-
						sprintf(buf_tx,"--Jugada correcta, levanta--\n"); 
						bytesaenviar =  strlen(buf_tx)+1;
						buf_tx[bytesaenviar]= '\0'; 
						bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
						memset(buf_tx,0,strlen(buf_tx));
					}
					
					//Se avisa al proc padre que el jugador terminó de jugar
					msj3.id = nro_jug_max + nro_jugador;
					msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj3,sizeof(msj3)-sizeof(msj3.id),0);
					terminar2 = true;
					jugada=false;
					
				}else{
					//Msje de aviso al jug -> NO se altera valor de terminar2 -> Se loopea la jugada nuevamente
					sprintf(buf_tx,"--Las cartas NO suman 15, por favor revise su jugada y reingrésela--\n"); 
					bytesaenviar =  strlen(buf_tx)+1;
					buf_tx[bytesaenviar]= '\0'; 
					bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
					memset(buf_tx,0,strlen(buf_tx));
				}
				
			
			//DESCARTE
			}else if( aux2 == 2 ){
			bool carta_valida =true;
			do{	
				//Msje de aviso al jug
				sprintf(buf_tx,"\nIndique qué carta va a descartar: "); 
				bytesaenviar =  strlen(buf_tx)+1;
				buf_tx[bytesaenviar]= '\0'; 
				bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
				memset(buf_tx,0,strlen(buf_tx));	
				
				//Se recibe respuesta y almacena en var
				bytesrecibidos=recv(connect_s, buf_rx, sizeof(buf_rx), 0);
				sscanf(buf_rx,"%d",&c_jugo);
				
				//Se evalúa que la carta sea válida	
				if( c_jugo < 0 || c_jugo > (jug.nro_c_mano) ){
					carta_valida = false;
					sprintf(buf_tx,"\nCarta no válida, reingrese por favor: "); 
					bytesaenviar =  strlen(buf_tx)+1;
					buf_tx[bytesaenviar]= '\0';
					bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
					memset(buf_tx,0,strlen(buf_tx));	
				}else{
					carta_valida = true;
				}
								
				
			}while(!carta_valida);
			
			//Llamado a función para descarte
			descartar_carta(&jug , c_jugo , mem_comp );

			//Se avisa al proc padre que el jugador terminó de jugar
			msj3.id = nro_jug_max + nro_jugador;
			msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj3,sizeof(msj3)-sizeof(msj3.id),0);
			terminar2 = true;
			jugada=false;	
			
			}

			}
		}while(!terminar2);
		
		//terminar2 = false;	//Se reinicia var aux de control de las jugadas para la sig iteración
		printf("\n\n");	
		
		//Mensaje de aviso para el jugador - 
		strcpy(buf_tx,"\n\n--Espere y observe el juego hasta que le toque su turno--\n\n"); 
		bytesaenviar =  strlen(buf_tx)+1;
		buf_tx[bytesaenviar]= '\0'; 
		bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
		memset(buf_tx,0,strlen(buf_tx));
		
		//Jugador recibe las jugadas de los jug POSTERIORES a sí (Orden de conexión)
		for(int j = 0 ; j < nro_jug_max - nro_jugador ; j++){
			//Recibe jugada por msg queue
			msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj3, sizeof(msj3) - sizeof(msj3.id), nro_jugador, 0);
			//Envía por socket la jugada
			strcpy(buf_tx,msj3.jugada); 
        		bytesaenviar =  strlen(buf_tx)+1;
    			buf_tx[bytesaenviar]= '\0'; 
        		bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
			memset(buf_tx,0,strlen(buf_tx));

		}	

	}; 
	
	//Recibe msje de proceso padre confirmando si se juega otra mano o no
     	msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj1, sizeof(msj1) - sizeof(msj1.id), nro_jugador, 0);
     	if(msj1.flag == true){
     		terminar1 = true;
     	}
	
	};
      
     	//Se avisa al jugador que no se jugarán mas rondas
      	sprintf(buf_tx,"\n\n --- EL JUEGO HA TERMINADO --- \n\n A continuación se muestran las cartas que levantó\n"); 
     	bytesaenviar =  strlen(buf_tx)+1;
      	buf_tx[bytesaenviar]= '\0'; 
      	bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
      	memset(buf_tx,0,strlen(buf_tx));
      
      	//Espera que el proc padre habiite acceso a mem compartida
      	printf("nro jug: %d\n",nro_jugador);
      	fflush(stdout);
      	msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj1, sizeof(msj1) - sizeof(msj1.id), nro_jugador, 0);
	
	//Evalúa si fue el último jugador en levantar
      	if(nro_jugador == mem_comp->ultimo_levante){
      		//Evalúa si quedan cartas en la mesa
      		if(mem_comp->nro_c_mesa != 0){
             		//Se lleva las cartas de la mesa
             		for(int i =0 ; i< mem_comp->nro_c_mesa ; i++){
				struct Carta aux;
        			aux = mem_comp->cartas_mesa[i];
      		
				jug.cartas_rec[jug.nro_c_rec] = aux;
				jug.nro_c_rec++;
			}	
      	  	}
     	}
	
      		
        
      	//Se cargan como texto las cartas recolectadas por el jugador, las escobas que realizó y se las envía como texto
      	cargar_c_levantadas(&jug,buf_tx);
      	cargar_escobas(&jug,buf_tx);
      	bytesaenviar =  strlen(buf_tx)+1;
      	buf_tx[bytesaenviar]= '\0'; 
      	bytestx=send(connect_s, buf_tx, bytesaenviar, 0);
      	memset(buf_tx,0,strlen(buf_tx));  
      
        //Envía confirmación de que terminó de acceder a M.Compartida, y envió el último mensaje al jug -> Ya se puede cerrar
        //el socket y liberar mem comp, cola de msjes,etc.
      	msj1.id = nro_jug_max + nro_jugador;
      	msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1)-sizeof(msj1.id),0);
      	
      	//Termina conex socket
      	close(connect_s);
      	
      	//Termina hijo
      	return 0; 
      
     }else{
     	
     	printf("Soy el padre %d, recibi un pedido de conexión, la derive a mi hijo %d\n", getpid(),pid_n);//Msje de control 
      	
      	//Si la cantidad de jug conectados no es igual a la cant de jug que jugarán la partida
      	//el proc padre vuelve al flujo del programa previo a crear nuevos hijos, y queda bloqueado
      	//esperando nuevas conexiónes
      	if(nro_jug_c == nro_jug_max){
	
		//Se setea esta var de forma que al terminar el juego no se quede el server esperando conexiones.	
		terminar_padre=true;
		//Se crea el mazo(inic), se ordenan aleatoriam las cartas(mezclar), y se dan 4 cartas para la mesa(primera mano)
		inic_mazo();
		mezclar_mazo();
      		primera_mano( nro_jug_max , mem_comp );
      		
      		//Loop correspondiente al desarrollo del juego
     	 	bool terminar = false;
		while(!terminar){	      			
      			
      			//Se reparten las cartas a c/jug (Comienzo de ronda)
      			for(int i=1 ; i<= nro_jug_max ; i++){
      				msj2.id = i;
      				for(int j=0 ; j<3 ; j++){
      					msj2.carta = mazo[indice_mazo];
      					indice_mazo++;
      					msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj2,sizeof(msj2)-sizeof(msj2.id),0);
				}      		
      			}
      		
      			//Espera de confirmación
      			//Loopea confirmación de todos los jug
      			for(int i=1 ; i <= nro_jug_max ; i++){
      				msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj1, sizeof(msj1) - sizeof(msj1.id), nro_jug_max + i, 0);
      			}
		
			//3 manos, que equivalen a una ronda
			for(int k=0 ; k < 3 ; k++){
			
				//Habilita a c/jugador a acceder a la MESA (Está en memoria compartida)
				for(int i=1 ; i <= nro_jug_max ; i++){
					//Aviso a jug de que tiene permitido acceder a mem comp
					msj1.id = i;
					msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1)-sizeof(msj1.id),0);
					
					//printf("Ola ke aseh\n");	
					
					//Espera confirmación del jug, indicando que terminó de utilizar recursos de mem compartida
					msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1) - sizeof(msj1.id), nro_jug_max + i, 0);
				}

      				//JUGADAS
      				//Este loop corresponde a cada turno de cada jugador 		
      				for(int j=1 ; j <= nro_jug_max ; j++){
      					char auxstr[100];
      					
      					//Recibe la jugada del jugador
      					msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj3,sizeof(msj3) - sizeof(msj3.id), nro_jug_max + j, 0);
      					//Agrega(como texto)a la jugada, quién es el próximo que juega 
      					if(j!=nro_jug_max){
      						sprintf(auxstr,"\n ---  A CONTINUACIÓN JUGARÁ: %s  ---\n\n",mem_comp->nombres_jug[j]);
      						strcat(msj3.jugada,auxstr);
      					}else{
      						sprintf(auxstr,"\n ---A CONTINUACIÓN JUGARÁ: %s",mem_comp->nombres_jug[0]);
      					}
      					
      					//Envía la jugada al resto de jugadores
      					//Jugadores ANTERIORES
      					if(j!=1){
      						for(int i=1; i < j ; i++) {	
							msj3.id = i;
							msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj3,sizeof(msj3)-sizeof(msj3.id),0); 
						}
					}
      					//Jugadores POSTERIORES
      					if(j!=nro_jug_max){
      						for(int i = j+1 ; i <= nro_jug_max ; i++){					
      							msj3.id = i;
							msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj3,sizeof(msj3)-sizeof(msj3.id),0);
						}
					}
				
				
				
      				}	
      			}
      			
      			//Evalúa si se dieron todas las cartas del mazo
      			if(indice_mazo == CARTAS_MAZO){
      				
      				//En caso de ser así avisa a todos los jug que no se jugarán mas rondas
      				terminar = true;
      				for(int i=1 ; i <= nro_jug_max ; i++){
					msj1.id = i;
					msj1.flag = true;
					msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1)-sizeof(msj1.id),0);
				}
      				
      				//Habilita a c/jugador a acceder a la MESA (Está en memoria compartida)
				for(int i=1 ; i <= nro_jug_max ; i++){
					//Aviso a jug de que tiene permitido acceder a mem comp
					msj1.id = i;
					msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1)-sizeof(msj1.id),0);
					
				
					
					//Espera confirmación del jug, indicando que terminó de utilizar recursos de mem compartida
					msgrcv(Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1) - sizeof(msj1.id), nro_jug_max + i, 0);
				}
			     	}else{
      			
      				for(int i=1 ; i <= nro_jug_max ; i++){
					msj1.id = i;
					msgsnd (Id_Cola_Mensajes, (struct msgbuf *)&msj1,sizeof(msj1)-sizeof(msj1.id),0);
				}
      			
      			}
      	
		}
            } 
    }  //PADRE vuelve al accept a esperar otra conexión
  
  }
	
  close(connect_s);
  close(server_s);
  msgctl (Id_Cola_Mensajes, IPC_RMID, (struct msqid_ds *)NULL);
  shmdt ((const void *) mem_comp);
  shmctl (Id_Memoria1, IPC_RMID, (struct shmid_ds *)NULL);
  return 0; // FIN DE PROGRAMA
}
