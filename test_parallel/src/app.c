#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"

double x[DATA_SIZE], y[DATA_SIZE];



int main()
{

  srand(time(NULL));
  double theta[]={0.0,0.0};
  double* y_pred ;
  int epoch=0,total_epochs = EPOCHS;
	double cost;
  double *error = malloc(sizeof(double)*EPOCHS);
  int count = 0;

    /**
    *****************************
    *                           *
    *       Load THE DATA       *
    *                           *
    *****************************
    **/
  printf("Loading Dataset from dataset/theData.csv ...\n\n");
	FILE* stream = fopen("data/theData.csv", "r");
  if (stream == NULL) {
    fprintf(stderr, "Error reading file\n");
    return 1;
  }
  while (fscanf(stream, "%lf,%lf", &x[count], &y[count]) == 2) {
      count = count+1;
  }
    // Uncomment to display loaded data
    // for (int i = 0; i < (int)total_samples; i++) {
    //   printf(" x[%d]:%lf , y[%d]:%lf\n", i,x[i], i,y[i]);
    // }

    /**
    ******************************
    *                            *
    *       Trainning Phase      *
    *                            *
    ******************************
    **/

  int num_bacht = DATA_SIZE / BACHT_SIZE ;
  printf("Training parameters using Gradient Descent..\n\n");
  


    clock_t begin = clock();

    while(epoch < total_epochs){

      double ***bacht_datas = bacht_data(y, x , BACHT_SIZE, DATA_SIZE);
      double **bacht_y = bacht_datas[0];
      double **bacht_x = bacht_datas[1];
      for (int i = 0; i < num_bacht; i++) {
        y_pred = prediction(bacht_x[i], theta, BACHT_SIZE);
        cost=MSE(y_pred, bacht_y[i], (double)BACHT_SIZE);
        grad_descent(y_pred, bacht_y[i], bacht_x[i], theta, (double)BACHT_SIZE);
        break;
      }
      error[epoch] = cost;
      printf("\nEpoch: %d Theta0: %lf Theta1: %lf Cost: %lf \n",epoch,theta[0],theta[1],cost);
      epoch++;

    }

    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    printf("\n----------terminer avec un temps t = %lf s--------------\n", time_spent);

    printf("\n\n Results after %d iterations",epoch);
    printf("\n\tTheta0 (m) : %lf    Theta1 (b) :  %lf    MSE :  %lf \n",theta[0], theta[1] ,cost);



    plot_error_iter(error);



	return 0 ;

}
