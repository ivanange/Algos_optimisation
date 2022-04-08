#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"
#include <time.h>




float f( float x ) {

	return pow(x,4) - 3*pow(x,3) + 2 ;
}

float d_f(float x){

	return 4*pow(x,3) - 9*pow(x,2) ;
}


/**
* MSE (Mean Square Error) function
* @param y_pred : The predicted values
* @param y : Real values
* @param n : y_pred and y length
* @description : This function compute the MSE beetwen the real and  predicted values
* @return: The MSE beetwen y_pred, y
**/
double MSE(double *y_pred , double *y, double n) {

	double diff, sum_sq = 0.00 ;

	for (int i = 0; i < n; ++i)
	{
		diff = y[i] - y_pred[i] ;
		sum_sq += pow(diff,2) ;

	}

	return (sum_sq / n );
}


/**
* prediction function
* @param X : double array represent the dependent variable
* @param theta : double array were thetha[0] a.k.a (a) is a slope and thetha[1] a.k.a (b) is the intercept
* @param n : X length
* @description : This function compute the independent variables with the actual theta parameters
* @return : Compute and return y = aX + b
**/
double* prediction(double* X, double *theta, int n){

	double* Y = malloc(sizeof(double)*n) ;

	for (int i = 0; i < n; ++i)
	{
		Y[i] = (theta[0] + (theta[1]*X[i]));
	}

	return Y;


}


/**
* prediction grad_descent
* @param y_pred : The predicted values
* @param y : Real values
* @param x : double array represent the dependent variable
* @param theta : double array parameters were thetha[0] a.k.a (a) is a slope and thetha[1] a.k.a (b) is the intercept
* @param n : y_pred , y and x length
* @description : the gradient descent algoritm to update the parameters
* @return : void function, he juste update the theta array parameters *
*/
void grad_descent(double *y_pred , double *y, double *x , double *theta , double n){


	double da , db , diff;
	double suma = 0.000 , sumb = 0.000 ;

	for (int i = 0; i < n; ++i)
	{
		diff = y[i] - y_pred[i];
		suma = (suma + diff) ;
		sumb = sumb + ( x[i]*(diff) );
	}

	da = ( (-2.000/(double)n) * (suma) );
	db = ( (-2.000/(double)n) * (sumb) );
	theta[0] = ( theta[0] - ((LEARNING_RATE)*da) );
	theta[1] = ( theta[1] - ((LEARNING_RATE)*db) );
	//printf("Theta0: %lf Theta1: %lf",theta[0],theta[1]);

}



double ***bacht_data(double *y, double *x , int bacht_size, int n)
{

	int num_bacht, i = -1;
	num_bacht = n / bacht_size;
	int index[n];

	for (int k = 0;  k < n; k++) {
		index[k] = k ;
	}

	randomize(index, n);

	double **bacht_y = allocate_dynamic_float_matrix(num_bacht, bacht_size);
	double **bacht_x = allocate_dynamic_float_matrix(num_bacht, bacht_size);
	double ***ret_vec = malloc(sizeof(double**)*2);

	for(int row = 0 ; row < num_bacht ; row++)
	{

		for( int col = 0 ; col < bacht_size;  col++)
		{
			i = i + 1;
			bacht_y[row][col] = y[index[col]];
			bacht_x[row][col] = x[index[col]];
			// printf("%d . y:%lf--->x:%lf\n",index[col], bacht_y[row][col], bacht_x[row][col]);

		}


	}
	ret_vec[0] = bacht_y;
	ret_vec[1] = bacht_x;
	
	return ret_vec ;
}

void randomize(int *array, int n) {
    int i;
    for(i = n-1; i > 0; i--) {
        int j = rand() % (i+1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void initialize_vect_zero(double *a, int n)
{
	for (int i = 0; i < n; i++)
	{
		a[i] = 0 ;
	}
	
}


void plot_error_iter(double *e)
{
	char * commandsForGnuplot[] = {"set title \"TITLE\"", "plot 'data.temp' w l"};
	FILE * temp = fopen("data.temp", "w");
	FILE * gnuplotPipe = popen ("gnuplot -persistent", "w");

	for (int i=0; i < EPOCHS; i++)
	{
		fprintf(temp, "%lf %lf \n", (double)i , e[i]); //Write the data to a temporary file
	}

	for (int i=0; i < NUM_COMMANDS; i++)
	{
		fprintf(gnuplotPipe, "%s \n", commandsForGnuplot[i]); //Send commands to gnuplot one by one.
	}
}



double **allocate_dynamic_float_matrix(int row, int col)
{
    double **ret_val;
    int i;

    ret_val = malloc(sizeof(double *) * row);
    if (ret_val == NULL)
    {
        perror("memory allocation failure");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < row; ++i)
    {
        ret_val[i] = malloc(sizeof(double) * col);
        if (ret_val[i] == NULL)
        {
            perror("memory allocation failure");
            exit(EXIT_FAILURE);
        }
    }

    return ret_val;
}

void deallocate_dynamic_float_matrix(float **matrix, int row)
{
    int i;

    for (i = 0; i < row; ++i)
    {
        free(matrix[i]);
    }
    free(matrix);
}
