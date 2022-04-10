#ifndef DEF_UTILS
#define DEF_UTILS

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define DATA_SIZE 10000
#define BACHT_SIZE 5
#define EPOCHS 450
#define LEARNING_RATE 0.005
#define NUM_COMMANDS 2




void grad_descent(double *y_pred , double *y, double *x , double *theta , double n);

double* prediction(double* X, double *theta, int n);

double MSE(double *y_pred , double *y, double n) ;

double **allocate_dynamic_float_matrix(int row, int col);

void deallocate_dynamic_float_matrix(float **matrix, int row);

double ***bacht_data(double *y, double *x , int bacht_size, int n ) ;

void plot_error_iter(double *e) ;

void randomize(int *array, int n) ;

float f( float x ) ;

float d_f(float x) ;

void initialize_vect_zero(double *a, int n);







#endif
