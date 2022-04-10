#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include <pthread.h>
#define NUM_THREADS 10

void sequentiel();
int load_data();
void *parallelGd(void *threadmess);
int parallel();

struct thread_param {
int k ;
double *x ;
double *y ;
double *theta;
};
struct thread_param t_thread_param [ NUM_THREADS ];
pthread_mutex_t mutex_theta ;
double tf[]={0.0,0.0};

int bacht_size = 1, seq = 1;
double x[DATA_SIZE], y[DATA_SIZE];
double theta[]={0.0,0.0};
int total_samples = DATA_SIZE;
double* y_pred ;
int randombacht;
double cost;


int main(int argc, char *argv[])
{

  
	load_data();

  	if(argc < 2)
	{
		printf("Options:\n");
    	printf("\t arg1 = <bacht_size>\n");
    	return 0;
	}

  
  	bacht_size =  atoi(argv[1]);
  	seq = atoi(argv[2]);

  	if (seq == 1)
  	{
    	sequentiel();
  	}
  	else parallel(); 

	return 0 ;

}



//----------------------sequentiel---------------------------------
void sequentiel(){

	srand(time(NULL));


 	int num_bacht = DATA_SIZE / bacht_size ;
  	double ***bacht_datas ;
  	double **bacht_y = bacht_datas[0];
  	double **bacht_x = bacht_datas[1];
	int epoch=0,total_epochs = EPOCHS;
  	double *error = malloc(sizeof(double)*EPOCHS);

	clock_t begin = clock();

    while(epoch < total_epochs){

    	bacht_datas = bacht_data(y, x , bacht_size, DATA_SIZE);
      	bacht_y = bacht_datas[0];
      	bacht_x = bacht_datas[1];
      	for (int i = 0; i < num_bacht; i++) {
      		y_pred = prediction(bacht_x[i], theta, bacht_size);
      		cost=MSE(y_pred, bacht_y[i], (double)bacht_size);
      		grad_descent(y_pred, bacht_y[i], bacht_x[i], theta, (double)bacht_size);
      		break;
      	}
      	error[epoch] = cost;
      	printf("\nEpoch: %d Theta0: %lf Theta1: %lf Cost: %lf \n",epoch,theta[0],theta[1],cost);
      	epoch++;
    }
    printf("\n\n\n \t *****************Results after %d iterations********************",epoch);
    printf("\n\n \tTheta0 (m) : %lf    Theta1 (b) :  %lf    MSE :  %lf \n",theta[0], theta[1] ,cost);

  	clock_t end = clock();
  	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  	printf("\n\t----------terminer avec un temps t = %lf s--------------\n", time_spent);

    plot_error_iter(error);

}

//---------------------------------------PARALLEL-----------------------------------

int parallel()
{

  	pthread_t threads[NUM_THREADS];
	pthread_attr_t attr;
	int rc;
   
	int bacht_size = DATA_SIZE / NUM_THREADS;
	double ***bacht_datas = bacht_data(y, x , bacht_size, DATA_SIZE);
	double **bacht_y = bacht_datas[0];
	double **bacht_x = bacht_datas[1];
	int num_bacht = DATA_SIZE / BACHT_SIZE;
	void *status;
	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	clock_t begin = clock();
	for(int i=0; i<NUM_THREADS ; i++) {
		t_thread_param[i].x = bacht_x[i] ;
		t_thread_param[i].y = bacht_y[i] ;
		t_thread_param[i].k = NUM_THREADS;
		t_thread_param[i].theta = theta;
		initialize_vect_zero(theta, 2);
		num_bacht = num_bacht - 1 ;
		//printf("Main: creating thread %d\n", i);
		rc  = pthread_create(&threads[i] , &attr, parallelGd , (void*) & t_thread_param[i]) ;
		if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);

		}
	}
	/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(int i=0; i< NUM_THREADS; i++) {

		rc = pthread_join(threads[i], &status);
		if (rc) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
		}
		//printf("Main completed join with thread %d having a status of %ld \n  ",i,(long)status);

	}

	printf("\n\n\n \t *****************Results after %d iterations (version parallel) ********************",EPOCHS);
    printf("\n\n \tTheta0 (m) : %lf    Theta1 (b) :  %lf  \n",tf[0], tf[1]);

  	clock_t end = clock();
  	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  	printf("\n\t----------terminer avec un temps t = %lf s--------------\n", time_spent);
	pthread_exit(NULL);
}

int load_data()
{
	int count = 0;
  	printf("Loading Dataset from dataset/theData.csv ...\n\n");
	FILE* stream = fopen("data/theData.csv", "r");
  	if (stream == NULL) {
    	fprintf(stderr, "Error reading file\n");
    	return 1;
  	}
  	while (fscanf(stream, "%lf,%lf", &x[count], &y[count]) == 2) {
      count = count+1;
  	}
	return 0;
    // Uncomment to display loaded data
    // for (int i = 0; i < (int)total_samples; i++) {
    //   printf(" x[%d]:%lf , y[%d]:%lf\n", i,x[i], i,y[i]);
    // }
}


void *parallelGd(void *threadmess){

    struct thread_param * mes_param ;
    mes_param = ( struct thread_param *) threadmess ;
    int epoch = 0;
    int r ;
    int bacht_size = DATA_SIZE / NUM_THREADS;
    while(epoch < EPOCHS){

      double *y_pred = prediction(mes_param->x, mes_param->theta, bacht_size);
      grad_descent(y_pred, mes_param->y, mes_param->x, mes_param->theta, (double)bacht_size);     
      epoch++;

    }

    r = pthread_mutex_lock(&mutex_theta);
    if (r!=0) { perror ("ERREUR pthread_mutex_lock()") ; exit (EXIT_FAILURE);}
    // Début de la section critique
    tf[0] = tf[0] + mes_param->theta[0] / mes_param->k;
    tf[1] = tf[1] + mes_param->theta[1] / mes_param->k;
    r = pthread_mutex_unlock (&mutex_theta);
    if (r!=0) { perror ("ERREUR pthread_mutex_ unlock()") ; exit ( EXIT_FAILURE ) ;} 
    pthread_exit ( NULL ) ;
}


