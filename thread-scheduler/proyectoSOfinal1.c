#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#define TAM_BUFFER 10

pthread_mutex_t mutex,mutex2;
int n_elementos=0;
pthread_cond_t vacio,lleno,done,waiter;
int n;
int arriboactual=0;
int tiempoactual=0;
int cola=0;
int afterburst=0;
int afterarribo=0;
int turno=0;
int procesa=0;
int *tiemporetorno;
int *tiempoespera;
int *tiemporespuesta;
int imready=0;
int arriboentrante=0;
int arriboanterior=0;
int bandera=0;
int front=-1,rear=-1;
int quantum;
int hilosvivos;
int tiempototal=0;
int tiempoSJF=0;
FILE* fichero;

/*Estructura tiempo: Estructura de datos para almacenar el burst y el tiempo de arribo
de cada hilo. Emplea el formato HH:MM:SS.

*/
typedef struct{

int horas;
int minutos;
int segundos;
int tiemposegundos;

}tiempo;

/*Estructura hilo: Estructura de datos que usa cada hilo para su identificacion.
Contiene informacion relevante del hilo como su identificador, tiempo de burst y de arrio, asi como su quantum si es necesario.
*/
typedef struct{

int id;
tiempo arribo;
tiempo burst;
int quantumrestante;

}hilo;

hilo *arrHilos;
int thesize;
/*Funcion enQueue: hace un push a la cola.
Recibe un valor de tipo hilo, el cual es el que sera insertado a la cola de los hilos listos.
*/
void enQueue(hilo *value) {
 // if (rear == thesize - 1){
   // printf("\nQueue is Full with size: %d!!",thesize);
//}
  //else {
    if (front == -1)
      front = 0;
    rear++;
    arrHilos[rear] = *value;
    //printf("\nInserted -> %d", value->id);
  //}
}
/*Funcion deQueue: Hace un pop a la cola de los hilos listos.
*/
void deQueue() {
  if (front == -1)
    printf("\nLa cola esta vacia");
  else {
    //printf("\nDeleted : %d", arrHilos[front].id);
    front++;
    if (front > rear)
      front = rear = -1;
  }
}
/*Funcion display2:Imprime los elementos de la cola de los hilos listos*/

void display2() {
  if (rear == -1)
    printf("\nLa cola esta vacia");
  else {
    int i;
    printf("\nElementos de la cola:\n");
    for (i = front; i <= rear; i++)
      printf("ID: %d, BURST: %d \n", arrHilos[i].id,arrHilos[i].burst.tiemposegundos);
  }
  printf("\n");
}
/*Funcion swap: usada en el algoritmo de ordenamiento bubblesort. Intercambia dos variables de tipo hilo
Recibe dos parametros de tipo hilo que son las variables que seran intercambiadas*/

void swap(hilo *xp, hilo *yp) 
{ 
    hilo temp = *xp; 
    *xp = *yp; 
    *yp = temp; 
} 

void *RRobin(void *params);
 
/*Funcion bubbleSort: ordena en orden no decreciente el arreglo de hilos por procesar.
Recibe como parametros el arreglo a ordenar y su tamanio*/ 

void bubbleSort(hilo *arr, int n) 
{ 
   	int i, j; 
	for (i = 0; i < n-1; i++){       
       		for (j = 0; j < n-i-1; j++){
           		if (arr[j].burst.tiemposegundos > arr[j+1].burst.tiemposegundos && arr[j+1].burst.tiemposegundos!=0)
              			swap(&arr[j], &arr[j+1]);
		} 
	}
}

/*Funcion FCFS: Aplica el algoritmo de FCFS para los hilos que reciba.
Recibe los hilos que seran planificados por el algoritmo FCFS. Hace uso de mutex y variables condicionales
para la correcta sincronizacion de los hilos de acuerdo al algoritmo. Cuando un hilo accede al mutex, es procesado y se calculan sus tiempos
de retorno, espera y respuesta, ademas son escritos los datos correspondientes en reporte.txt

*/
 
void *FCFS(void *params){
	hilo *p;
	p=(hilo*)params;
	int i;
	pthread_mutex_lock(&mutex);
			while(n_elementos!=p->id)
				pthread_cond_wait(&lleno,&mutex);
			printf("hilo %d entra a cola.\n"
			,p->id);	
			
			fichero = fopen("reporte.txt", "a+");
			fprintf(fichero,"hilo %d entra a cola.\n"
			,p->id);	
			fclose(fichero);			


			if(afterarribo+afterburst<p->arribo.tiemposegundos){
				afterarribo=p->arribo.tiemposegundos;
				afterburst=p->burst.tiemposegundos;
			}
			else{
				afterarribo=afterarribo+afterburst;
				afterburst=p->burst.tiemposegundos;
			}
			int tiempo1=afterarribo+p->burst.tiemposegundos-p->arribo.tiemposegundos;
			int tiempo2=afterarribo-p->arribo.tiemposegundos;
			int tiempo3=afterarribo-p->arribo.tiemposegundos;

			tiemporetorno[p->id]=tiempo1;
			tiempoespera[p->id]=tiempo2;
			tiemporespuesta[p->id]=tiempo3;

			printf("hilo %d es procesado. Entra al procesador en tiempo: %d. \n"
			,p->id,afterarribo);

			fichero = fopen("reporte.txt", "a+");
			fprintf (fichero, "hilo %d es procesado. Entra al procesador en tiempo: %d. \n"
			,p->id,afterarribo);
			fclose(fichero);
	
			n_elementos++;
			pthread_cond_broadcast(&lleno);
	pthread_mutex_unlock(&mutex);
	pthread_exit(0);
}

void *RRobin2(void *params);

/*Funcion RRobin: Recibe los hilos recien creados por el usuario y aplica el algoritmo Round Robin para su planificacion.
Hace uso de mutex y variables condicionales para la correcta sincronizacion de los hilos de acuerdo al algoritmo.
Si varios hilos llegan al mismo tiempo, estos son agregados a la cola de procesos listos, cuando un hilo llega despues se agrega 
a la cola de procesos listos y permite que el primer hilo en la cola sea procesado de acuerdo a su quantum y al tiempo que paso.

*/

void *RRobin(void *params){
	hilo *p;
	p=(hilo*)params;
	int i;
	pthread_mutex_lock(&mutex);
			while( n_elementos!=p->id )
				pthread_cond_wait(&lleno,&mutex);	
		
			printf("hilo %d entra a la cola.\n",p->id);
			fichero = fopen("reporte.txt", "a+");
			fprintf(fichero,"hilo %d entra a la cola.\n",p->id);
			fclose(fichero);	
			if(n_elementos==0)
				arriboactual=p->arribo.tiemposegundos;
	
			if(arriboactual==p->arribo.tiemposegundos){
				enQueue(p);		
			}
			else{
				if(p->arribo.tiemposegundos-arriboactual<arrHilos[front].quantumrestante){
					arrHilos[front].burst.tiemposegundos=arrHilos[front].burst.tiemposegundos-(p->arribo.tiemposegundos-arriboactual);
					arrHilos[front].arribo.tiemposegundos=p->arribo.tiemposegundos;
					procesa=1;
					enQueue(p);
					arriboanterior=arriboactual;
					arriboactual=p->arribo.tiemposegundos;	
					pthread_cond_broadcast(&done);
					pthread_mutex_unlock(&mutex);
					RRobin2(p);
				}
				else if(p->arribo.tiemposegundos-arriboactual>=arrHilos[front].quantumrestante){
					arrHilos[front].burst.tiemposegundos=arrHilos[front].burst.tiemposegundos-arrHilos[front].quantumrestante;
					arrHilos[front].arribo.tiemposegundos=p->arribo.tiemposegundos;
					procesa=1;
					enQueue(p);
					arriboanterior=arriboactual;
					arriboactual=p->arribo.tiemposegundos;	
					pthread_cond_broadcast(&done);		
					pthread_mutex_unlock(&mutex);
					RRobin2(p);
				}	
			}
			printf("Cola de procesos listos:\n");
			display2();	
			n_elementos++;
			arriboactual=p->arribo.tiemposegundos;
			if(n_elementos==n){
				procesa=1;
				//tiempototal=p->arribo.tiemposegundos;
				pthread_cond_broadcast(&done);
				pthread_mutex_unlock(&mutex);
				RRobin2(p);
			}
			else{
				pthread_cond_broadcast(&lleno);
				pthread_mutex_unlock(&mutex);
				RRobin2(p);
			}						
}
/*Funcion RRobin2: Simula una cola de procesos listos para el algoritmo de planificacion Round Robin.
Un hilo adquiere el mutex si es su tiempo de ser procesado, es decir sea el primero en la cola de procesos.
Cuando adquiere el mutex es procesado, si su burst time acabo es eliminado, sino, es agregado a la cola de proceso listos
y ejecuta una llamada recursiva.
Esta funcion contiene dos fases, cuando ya llegaron todos los procesos lanzados por el usuario y cuando no.
Esto se determina por la variable n_elementos que cuando es igual a n es que todos los procesos ya llegaron por lo menos
una vez a la cola de procesos listos. Cuando ya llegaron todos los procesos, el procedimiento para procesar los hilos es mas
sencillo ya que unicamente se le resta el quamtum a su burst y vuelve a insertar el hilo a la cola, asi hasta haber procesado los procesos restantes
*/
void *RRobin2(void *params){
	hilo *p;
	p=(hilo*)params;
	int i;
	pthread_mutex_lock(&mutex);
			while( arrHilos[front].id!=p->id || procesa==0 )
				pthread_cond_wait(&done,&mutex);
			if(n_elementos!=n){
				p->burst.tiemposegundos=arrHilos[front].burst.tiemposegundos;
				p->arribo.tiemposegundos=arriboactual;
				
				printf("hilo %d es procesado.\n",p->id);
				fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"hilo %d es procesado\n",p->id);
				fclose(fichero);
				printf("Cola de procesos listos:\n");
				display2();					
				n_elementos++;

				if(arriboactual-arriboanterior<p->quantumrestante){
					if(p->burst.tiemposegundos<=0 && n_elementos!=n){
						tiemporetorno[p->id]=arriboactual+p->burst.tiemposegundos-p->arribo.tiemposegundos;
						printf("hilo %d ha terminado.\n",p->id);
						fichero = fopen("reporte.txt", "a+");
						fprintf(fichero,"hilo %d ha terminado\n",p->id);
						fclose(fichero);
						hilosvivos--;
						deQueue();
						procesa=0;
						pthread_cond_broadcast(&lleno);
						pthread_mutex_unlock(&mutex);	
						pthread_exit(0);
					}
					else if(p->burst.tiemposegundos>0){
						p->quantumrestante=p->quantumrestante-(arriboactual-arriboanterior);
					}
				}
				else if(arriboactual-arriboanterior>=p->quantumrestante){
					if(p->burst.tiemposegundos<=0 && n_elementos!=n){
						printf("hilo %d ha terminado.\n",p->id);
						fichero = fopen("reporte.txt", "a+");
						fprintf(fichero,"hilo %d ha terminado\n",p->id);
						fclose(fichero);
						hilosvivos--;
						deQueue();
						procesa=0;
						pthread_cond_broadcast(&lleno);
						pthread_mutex_unlock(&mutex);
						pthread_exit(0);
					}
					else if(arrHilos[front].burst.tiemposegundos>0){
						printf("hilo %d se le acabo su quantum y sale del procesador.\n",p->id);
						fichero = fopen("reporte.txt", "a+");
						fprintf(fichero,"hilo %d se le acabo su quantum y sale del procesador.\n",p->id);
						fclose(fichero);
						p->quantumrestante=quantum;
						hilo devuelta=*p;
						deQueue();
						enQueue(&devuelta);
					}
				}
		
				if(n_elementos==n){
					procesa=1;
					pthread_cond_broadcast(&done);
					pthread_mutex_unlock(&mutex);
					RRobin2(p);

				}
				else{
					procesa=0;
					pthread_cond_broadcast(&lleno);
					pthread_mutex_unlock(&mutex);
					RRobin2(p);
				}
			}
			else{
				int resta;
				printf("hilo %d es procesado\n",p->id);
				fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"hilo %d es procesado\n",p->id);
				fclose(fichero);
				if(p->burst.tiemposegundos<quantum){
					arriboactual=arriboactual+p->burst.tiemposegundos;
					resta=p->burst.tiemposegundos;
				}
				else{
					resta=quantum;
					arriboactual=arriboactual+quantum;
				}

				p->burst.tiemposegundos=p->burst.tiemposegundos-quantum;
				printf("Cola de procesos listos:\n");
				display2();
				
				if(p->burst.tiemposegundos<=0){
					tiemporetorno[p->id]=arriboactual+p->burst.tiemposegundos+resta-p->arribo.segundos;
					//printf("hilo %d con tretorno:%d y arribo:%d.\n",p->id,tiemporetorno[p->id],p->arribo.segundos);
					printf("hilo %d ha terminado.\n",p->id);
					fichero = fopen("reporte.txt", "a+");
					fprintf(fichero,"hilo %d ha terminado\n",p->id);
					fclose(fichero);
					hilosvivos--;
					deQueue();
					procesa=1;
					pthread_cond_broadcast(&done);
					pthread_mutex_unlock(&mutex);	
					pthread_exit(0);
				}
				else if(p->burst.tiemposegundos>0){
					printf("hilo %d se le acabo su quantum y sale del procesador.\n",p->id);
					fichero = fopen("reporte.txt", "a+");
					fprintf(fichero,"hilo %d se le acabo su quantum y sale del procesador.\n",p->id);
					fclose(fichero);
					hilo devuelta=*p;
					deQueue();
					enQueue(&devuelta);
					procesa=1;
					pthread_cond_broadcast(&done);
					pthread_mutex_unlock(&mutex);	
					RRobin2(p);
				}
			}	
}
/*Funcion processing: simula una cola de procesos listos para SJF sin expulsion. Si un hilo adquiere el mutex, es procesado
y se calculan sus tiempos de respuesta, retorno y espera, ademas son escritos los datos correspoondientes en reporte.txt*/

void processing(hilo *p){
	int i;	
	pthread_mutex_lock(&mutex);
			while(arrHilos[bandera].id!=p->id || procesa==0){
				pthread_cond_wait(&done,&mutex);
				
			}
			if(n_elementos!=n){
				if(afterarribo+afterburst<p->arribo.tiemposegundos){
					afterarribo=p->arribo.tiemposegundos;
					afterburst=p->burst.tiemposegundos;
				}
				else{
					afterarribo=afterarribo+afterburst;
					afterburst=p->burst.tiemposegundos;
				}
				n_elementos++;
				int tiempo1=afterarribo+p->burst.tiemposegundos-p->arribo.tiemposegundos;
				int tiempo2=afterarribo-p->arribo.tiemposegundos;
				int tiempo3=afterarribo-p->arribo.tiemposegundos;

				tiemporetorno[p->id]=tiempo1;
				tiempoespera[p->id]=tiempo2;
				tiemporespuesta[p->id]=tiempo3;

				printf("hilo %d es procesado. Entra al procesador en tiempo: %d.\n"
				,p->id, afterarribo);
				fichero = fopen("reporte.txt", "a+");
				fprintf (fichero, "hilo %d es procesado. Entra al procesador en tiempo: %d.\n"
				,p->id,afterarribo);
				fclose(fichero);
				tiempoSJF=arriboactual+p->burst.tiemposegundos;
	
    				for(i=0;i<n-1;i++)
    				{
        				arrHilos[i]=arrHilos[i+1];
    				}	
				if(n_elementos==n){
					procesa=1;
					pthread_cond_broadcast(&done);
					pthread_mutex_unlock(&mutex);
				}
				else{
					procesa=0;
					pthread_cond_broadcast(&lleno);
					pthread_mutex_unlock(&mutex);
				}
		pthread_mutex_unlock(&mutex);
		pthread_exit(0);
		}
		else{
			if(afterarribo+afterburst<p->arribo.tiemposegundos){
					afterarribo=p->arribo.tiemposegundos;
					afterburst=p->burst.tiemposegundos;
				}
				else{
					afterarribo=afterarribo+afterburst;
					afterburst=p->burst.tiemposegundos;
				}

				int tiempo1=afterarribo+p->burst.tiemposegundos-p->arribo.tiemposegundos;
				int tiempo2=afterarribo-p->arribo.tiemposegundos;
				int tiempo3=afterarribo-p->arribo.tiemposegundos;

				tiemporetorno[p->id]=tiempo1;
				tiempoespera[p->id]=tiempo2;
				tiemporespuesta[p->id]=tiempo3;

				printf("hilo %d es procesado. Entra al procesador en tiempo: %d\n",p->id,afterarribo);
				fichero = fopen("reporte.txt", "a+");
				fprintf (fichero, "hilo %d es procesado. Entra al procesador en tiempo: %d.\n"
				,p->id,afterarribo);
				fclose(fichero);	
				bandera++;
				procesa=1;
				pthread_cond_broadcast(&done);
				pthread_mutex_unlock(&mutex);	
				pthread_exit(0);		
		}

}
/*Funcion SJF: Aplica el algoritmo de SJF sin expulsion para los hilos que reciba.
Recibe los hilos que seran planificados por el algoritmo SJF sin expulsion. Hace uso de mutex y variables condicionales
para la correcta sincronizacion de los hilos de acuerdo al algoritmo. Cuando un hilo accede al mutex, es agrega a la cola de procesos listos
, la cual es ordenada para simular SJF, ademas son escritos los datos correspondientes en reporte.txt*/
void *SJF(void *params){
	hilo *p;
	p=(hilo*)params;
	int i;
	pthread_mutex_lock(&mutex);
			
			while(n_elementos!=p->id)
				pthread_cond_wait(&lleno,&mutex);
			imready=0;
			printf("hilo %d entra a cola. Tiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n"
			,p->id,p->arribo.horas,p->arribo.minutos,p->arribo.segundos,p->burst.horas,p->burst.minutos,p->burst.segundos);	
			fichero = fopen("reporte.txt", "a+");
			fprintf(fichero,"hilo %d entra a cola. Tiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n"
			,p->id,p->arribo.horas,p->arribo.minutos,p->arribo.segundos,p->burst.horas,p->burst.minutos,p->burst.segundos);	
			fclose(fichero);

			if(n_elementos==0){
				arriboactual=p->arribo.tiemposegundos;
			}
		
			if(arriboactual==p->arribo.tiemposegundos){
				for(i=0;i<n;i++){
					if(arrHilos[i].burst.tiemposegundos==0 ){
						arrHilos[i]=*p;
						break;
					}
				}
			}
			else{
				
				bubbleSort(arrHilos,n);
				if(p->arribo.tiemposegundos>=tiempoSJF){
					arriboactual=p->arribo.tiemposegundos;	
					procesa=1;
					pthread_cond_broadcast(&done);
					for(i=0;i<n;i++){
					if(arrHilos[i].burst.tiemposegundos<=0 ){
						arrHilos[i]=*p;
						break;
					}
					}
					pthread_mutex_unlock(&mutex);
					processing(p);
				}
				for(i=0;i<n;i++){
					if(arrHilos[i].burst.tiemposegundos<=0 ){
						arrHilos[i]=*p;
						break;
					}
				}	
				arriboactual=p->arribo.tiemposegundos;	
			}

			bubbleSort(arrHilos,10);
			n_elementos++;
			if(n_elementos==n){
				procesa=1;
				pthread_cond_broadcast(&done);
				pthread_mutex_unlock(&mutex);
				processing(p);
			}
			else{
				pthread_cond_broadcast(&lleno);
				pthread_mutex_unlock(&mutex);
				processing(p);
			}				
}
void *SJF2(void *params);

/*Funcion SJFE: Recibe los hilos recien creados por el usuario y aplica el algoritmo SJF con expulsion para su planificacion.
Hace uso de mutex y variables condicionales para la correcta sincronizacion de los hilos de acuerdo al algoritmo.
Si varios hilos llegan al mismo tiempo, estos son agregados a la cola de procesos listos y son ordenados para simular el SJF, cuando un hilo llega despues se agrega 
a la cola de procesos listos se ordena y permite que el primer hilo en la cola sea procesado de acuerdo al tiempo transcurrido.
*/
void *SJFE(void *params){
	hilo *p;
	p=(hilo*)params;
	int i;
	//printf("hilo %d entra a saludar\n",p->id);
	//&& buscarEnArreglo(arrHilos,p->id,n)==-1
	pthread_mutex_lock(&mutex);
			
			while(n_elementos!=p->id )
				pthread_cond_wait(&lleno,&mutex);	
			imready=0;
			printf("hilo %d entra a cola.\n",p->id);
			fichero = fopen("reporte.txt", "a+");
			fprintf(fichero,"hilo %d entra a cola.\n",p->id);
			fclose(fichero);	
			if(n_elementos==0)
				arriboactual=p->arribo.tiemposegundos;

			if(arriboactual==p->arribo.tiemposegundos){
				for(i=0;i<n;i++){
					if(arrHilos[i].burst.tiemposegundos==0){
						arrHilos[i]=*p;		
						break;
					}
				}
			}
			else{
				bubbleSort(arrHilos,n);
				for(i=0;i<n;i++){
					if(arrHilos[i].burst.tiemposegundos<=0){
						arrHilos[i]=*p;
						break;
					}
				}
				arrHilos[0].burst.tiemposegundos=arrHilos[0].burst.tiemposegundos-(p->arribo.tiemposegundos-arriboactual);
				procesa=1;
				arriboactual=p->arribo.tiemposegundos;	
				pthread_cond_broadcast(&waiter);
				pthread_mutex_unlock(&mutex);
				SJF2(p);				
			}
			
			bubbleSort(arrHilos,n);
			printf("Arreglo de procesos listos:\n");
			for(i=0;i<n;i++){
				printf("burst %d: %d\n",arrHilos[i].id,arrHilos[i].burst.tiemposegundos);	
			}				
			n_elementos++;
			if(n_elementos==n){
				procesa=1;
				pthread_cond_broadcast(&waiter);
				pthread_mutex_unlock(&mutex);
				SJF2(p);
			}
			else{
				pthread_cond_broadcast(&lleno);
				pthread_mutex_unlock(&mutex);
				SJF2(p);
			}					
}
/*Funcion SJF2: Simula una cola de procesos listos para el algoritmo de planificacion SJF con expulsion.
Un hilo adquiere el mutex si es su tiempo de ser procesado, es decir sea el primero en la cola de procesos.
Cuando adquiere el mutex es procesado, si su burst time acabo es eliminado, sino, es agregado a la cola de proceso listos
y ejecuta una llamada recursiva.
Esta funcion contiene dos fases, cuando ya llegaron todos los procesos lanzados por el usuario y cuando no.
Esto se determina por la variable n_elementos que cuando es igual a n es que todos los procesos ya llegaron por lo menos
una vez a la cola de procesos listos. Cuando ya llegaron todos los procesos, el procedimiento para procesar los hilos es mas
sencillo ya que como no llegan procesos nuevos, ejecutamos los procesos de menor burst time a mayor.

*/
void *SJF2(void *params){
	hilo *p;
	p=(hilo*)params;
	int i;
	pthread_mutex_lock(&mutex);
			while( arrHilos[bandera].id!=p->id || procesa==0 )
				pthread_cond_wait(&waiter,&mutex);
			if(n_elementos!=n){
				p->burst.tiemposegundos=arrHilos[0].burst.tiemposegundos;
				printf("hilo %d es procesado. Su burst restante es:%d\n",p->id,p->burst.tiemposegundos);
				fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"hilo %d es procesado. Su burst restante es:%d\n",p->id,p->burst.tiemposegundos);
				fclose(fichero);
				printf("Arreglo de procesos listos:\n");
				for(i=0;i<n;i++){
					printf("burst %d: %d\n",arrHilos[i].id,arrHilos[i].burst.tiemposegundos);	
				}	
				for(i=0;i<n;i++){
					printf("burst %d: %d\n",arrHilos[i].id,arrHilos[i].burst.tiemposegundos);	
				}	
				n_elementos++;

				if(p->burst.tiemposegundos<=0 && n_elementos!=n){
					printf("hilo %d ha terminado.\n",p->id);
					fichero = fopen("reporte.txt", "a+");
					fprintf(fichero,"hilo %d ha terminado.\n",p->id);
					fclose(fichero);
					
					for(i=0;i<n-1;i++)
    					{
        				arrHilos[i]=arrHilos[i+1];
    					}	
					procesa=0;
					pthread_cond_broadcast(&lleno);
					pthread_mutex_unlock(&mutex);	
					pthread_exit(0);
				}
				if(n_elementos==n){
					procesa=1;
					bandera++;
					pthread_cond_broadcast(&waiter);
					pthread_mutex_unlock(&mutex);
					pthread_exit(0);
				}
				else{
					procesa=0;
					pthread_cond_broadcast(&lleno);
					pthread_mutex_unlock(&mutex);
					SJF2(p);
				}
			}
			else{
				bandera++;
				printf("hilo %d es procesado.\n",p->id);
				
				printf("Arreglo de procesos listos:\n");
				for(i=bandera;i<n;i++){
					printf("burst %d: %d\n",arrHilos[i].id,arrHilos[i].burst.tiemposegundos);	
				}	
				fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"hilo %d es procesado.\n",p->id);
				fclose(fichero);
					procesa=1;
					pthread_cond_broadcast(&waiter);
					pthread_mutex_unlock(&mutex);	
					pthread_exit(0);		
			}	
}
/*Funcion promedio: Calcula el valor promedio de los elementos de un arreglo
Recibe el arreglo para calcular el promedio
Regresa el valor promedio*/
float promedio(int *arr){
	int suma=0;
	for(int i=0;i<n;i++){
		suma=suma+arr[i];
	}
	return suma/n;
}

/*Funcion main: Inicializa algunas variables, pregunta al usuario cuantos hilos seran creados y con que algoritmo se planificaran
Posteriormente crea los hilos con todos los parametros deseados y los envia a las funciones respectivas de su algoritmo de planificacion
Al final desglosa los tiempos de cada hilo y calcula el tiempo de retorno, espera y respuesta promedio de los hilos*/
int main(){
	remove("reporte.txt");
	int ident=0;
	int i,segundos,segundos2;
	char pase;
	int choice;
	srand (time(NULL));
	printf("hola profe, mi programa es un planificador de los 4 algoritmos para un solo procesador.\nNo usé el tiempo de arribo definido en el documento porque siento que se ve mejor la funcionalidad de algunos algoritmos en un solo procesador cuando el rango de arribo puede ser igual entre varios procesos, sin embargo usted se lo puede cambiar y el burst time tambien. abajo esta comentado donde para cada algoritmo\nCalcule los tiempos para FCFS y SJF sin expulsion, sin embargo le quedo a deber los tiempos de los otros dos.\nEspero le guste, nos vemos en la tesis.\nIntroduzca cualquier tecla para continuar\n");
	 scanf("%c",&pase);
	
	printf("Numero de hilos: \n");
  	scanf("%d",&n);
	pthread_t id[n];
	hilo hilos[n];
	arrHilos = (hilo *) malloc(sizeof(hilo)*n*10);
	tiemporetorno = (int *) malloc(sizeof(int)*n);
	tiempoespera = (int *) malloc(sizeof(int)*n);
	tiemporespuesta = (int *) malloc(sizeof(int)*n);//ojo
	thesize=n;
	hilosvivos=n;
	int tiempoanterior=0;
	printf("Algoritmo de planificacion:\nFCFS: 1\nSJF sin expulsion: 2\nSJF con expulsion: 3\nRound Robin: 4\n ");
  	scanf("%d",&choice);
	switch(choice){
		case 1://FCFS
			for(i=0;i<n;i++){
				hilos[i].id=i;
				if(i==0){
					segundos=0;
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				else{
					segundos=rand()%3+tiempoanterior;//*******aqui se le cambia el rango de arribo entre procesos**************
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				hilos[i].arribo.horas=(int)(segundos/ 3600);
				hilos[i].arribo.minutos=(int)((segundos - hilos[i].arribo.horas * 3600) / 60);
				hilos[i].arribo.segundos=segundos - (hilos[i].arribo.horas* 3600 + 			hilos[i].arribo.minutos* 60);
				segundos2=rand()%7+1;//**************aqui se le cambia el burst***************
				hilos[i].burst.tiemposegundos=segundos2;
				hilos[i].burst.horas=(int)(segundos2/ 3600);
				hilos[i].burst.minutos=(int)((segundos2 - hilos[i].burst.horas * 3600) / 60);
				hilos[i].burst.segundos=segundos2 - (hilos[i].burst.horas* 3600 + 			hilos[i].burst.minutos* 60);
				printf("Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
			,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
			fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
			,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
				
				fclose(fichero);		
			}
			
	
			for(i=0;i<n;i++){
				pthread_create(&id[i],NULL,&FCFS,(void*)&hilos[i]);
			}
			for(i=0;i<n;i++){
				pthread_join(id[i],NULL);
			}
			pthread_mutex_destroy(&mutex);
			printf("\n\n");
			for(i=0;i<n;i++){
				printf("hilo:%d, Retorno: %d. Espera: %d. Respuesta: %d\n",i,tiemporetorno[i],tiempoespera[i],tiemporespuesta[i]);
				fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"hilo:%d, Retorno: %d. Espera: %d. Respuesta: %d\n",i,tiemporetorno[i],tiempoespera[i],tiemporespuesta[i]);	
				fclose(fichero);
			}
			printf("Tiempo promedio de retorno:%f   ",promedio(tiemporetorno));
			printf("Tiempo promedio de espera:%f   ",promedio(tiempoespera));
			printf("Tiempo promedio de respuesta:%f   ",promedio(tiemporespuesta));
			fichero = fopen("reporte.txt", "a+");	
			fprintf(fichero,"Tiempo promedio de retorno:%f   ",promedio(tiemporetorno));
			fprintf(fichero,"Tiempo promedio de espera:%f   ",promedio(tiempoespera));
			fprintf(fichero,"Tiempo promedio de respuesta:%f   ",promedio(tiemporespuesta));
			fclose(fichero);
			break;
		case 2://SJF SIN EXPULSION
			for(i=0;i<n;i++){
				hilos[i].id=i;
				if(i==0){
					segundos=0;
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				else{
					segundos=rand()%2+tiempoanterior;//******************aqui se le cambia el rango de arribo entre procesos**************
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				hilos[i].arribo.horas=(int)(segundos/ 3600);
				hilos[i].arribo.minutos=(int)((segundos - hilos[i].arribo.horas * 3600) / 60);
				hilos[i].arribo.segundos=segundos - (hilos[i].arribo.horas* 3600 + 			hilos[i].arribo.minutos* 60);
				segundos2=rand()%7+1;//******************aqui se le cambia el burst*****************
				hilos[i].burst.tiemposegundos=segundos2;
				hilos[i].burst.horas=(int)(segundos2/ 3600);
				hilos[i].burst.minutos=(int)((segundos2 - hilos[i].burst.horas * 3600) / 60);
				hilos[i].burst.segundos=segundos2 - (hilos[i].burst.horas* 3600 + 			hilos[i].burst.minutos* 60);
				printf("Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
			,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
				fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
				,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
				fclose(fichero);		
			}
			for(i=0;i<n;i++){
				pthread_create(&id[i],NULL,&SJF,(void*)&hilos[i]);
			}
			for(i=0;i<n;i++){
				pthread_join(id[i],NULL);
			}
			pthread_mutex_destroy(&mutex);
			printf("\n\n");
			for(i=0;i<n;i++){
				printf("hilo:%d, Retorno: %d. Espera: %d. Respuesta: %d\n",i,tiemporetorno[i],tiempoespera[i],tiemporespuesta[i]);
				fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"hilo:%d, Retorno: %d. Espera: %d. Respuesta: %d\n",i,tiemporetorno[i],tiempoespera[i],tiemporespuesta[i]);	
				fclose(fichero);
			}
			printf("Tiempo promedio de retorno:%f   ",promedio(tiemporetorno));
			printf("Tiempo promedio de espera:%f   ",promedio(tiempoespera));
			printf("Tiempo promedio de respuesta:%f   ",promedio(tiemporespuesta));
			fichero = fopen("reporte.txt", "a+");	
			fprintf(fichero,"Tiempo promedio de retorno:%f   ",promedio(tiemporetorno));
			fprintf(fichero,"Tiempo promedio de espera:%f   ",promedio(tiempoespera));
			fprintf(fichero,"Tiempo promedio de respuesta:%f   ",promedio(tiemporespuesta));
			fclose(fichero);
			break;
		case 3://SJF CON EXPULSION
			for(i=0;i<n;i++){
				hilos[i].id=i;
				if(i==0){
					segundos=0;
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				else{
					segundos=rand()%2+tiempoanterior;//*************aqui se le cambia el rango de arribo entre procesos***************
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				hilos[i].arribo.horas=(int)(segundos/ 3600);
				hilos[i].arribo.minutos=(int)((segundos - hilos[i].arribo.horas * 3600) / 60);
				hilos[i].arribo.segundos=segundos - (hilos[i].arribo.horas* 3600 + 			hilos[i].arribo.minutos* 60);
				segundos2=rand()%7+1;//***************aqui se le cambia el burst*************
				hilos[i].burst.tiemposegundos=segundos2;
				hilos[i].burst.horas=(int)(segundos2/ 3600);
				hilos[i].burst.minutos=(int)((segundos2 - hilos[i].burst.horas * 3600) / 60);
				hilos[i].burst.segundos=segundos2 - (hilos[i].burst.horas* 3600 + 			hilos[i].burst.minutos* 60);
				printf("Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
			,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
			fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
			,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
				
				fclose(fichero);		
			}
			for(i=0;i<n;i++){
				pthread_create(&id[i],NULL,&SJFE,(void*)&hilos[i]);
			}
			for(i=0;i<n;i++){
				pthread_join(id[i],NULL);
			}
			pthread_mutex_destroy(&mutex);
			break;
		case 4://ROUND ROBIN
			printf("Quantum: \n");
  			scanf("%d",&quantum);
			for(i=0;i<n;i++){
				hilos[i].id=i;
				if(i==0){
					segundos=0;
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				else{
					segundos=rand()%2+tiempoanterior;//******************aqui se le cambia el rango de arribo entre procesos****************
					hilos[i].arribo.tiemposegundos=segundos;
					tiempoanterior=segundos;
				}
				hilos[i].arribo.horas=(int)(segundos/ 3600);
				hilos[i].arribo.minutos=(int)((segundos - hilos[i].arribo.horas * 3600) / 60);
				hilos[i].arribo.segundos=segundos - (hilos[i].arribo.horas* 3600 + 			hilos[i].arribo.minutos* 60);
				segundos2=rand()%7+1;//*************aqui se le cambia el burst**************
				hilos[i].burst.tiemposegundos=segundos2;
				hilos[i].burst.horas=(int)(segundos2/ 3600);
				hilos[i].burst.minutos=(int)((segundos2 - hilos[i].burst.horas * 3600) / 60);
				hilos[i].burst.segundos=segundos2 - (hilos[i].burst.horas* 3600 + 			hilos[i].burst.minutos* 60);
				hilos[i].quantumrestante=quantum;
				printf("Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
			,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
			fichero = fopen("reporte.txt", "a+");
				fprintf(fichero,"Informacion del proceso: %d\nTiempo de arribo: %d:%d:%d. Burst time: %d:%d:%d.\n\n"
			,hilos[i].id,hilos[i].arribo.horas,hilos[i].arribo.minutos,hilos[i].arribo.segundos,hilos[i].burst.horas,hilos[i].burst.minutos,hilos[i].burst.segundos);
				
				fclose(fichero);		
			}
			for(i=0;i<n;i++){
				pthread_create(&id[i],NULL,&RRobin,(void*)&hilos[i]);
			}
			for(i=0;i<n;i++){
				pthread_join(id[i],NULL);
			}
			pthread_mutex_destroy(&mutex);
			break;
	}
	printf("\nSe ha guardado una copia del historial de planificacion en el archivo: reporte.txt\n");
	return 0;
}
