#ifndef SVM_H
#define SVM_H

#include <pthread.h>
#include "sample.h"

enum KernelType
{
    _linear,
    _polynomial,
    _Gaussian,
};

typedef struct SVM
{
    enum KernelType m_kernelType; // Kernel Function Type
                             // SVM parameters
    double C;                // penality paramters
    double sigma;            // sigma = 1.0/(2*sigma*sigma)  exp(-sigma*|x-j|^2))
    double eps;              // Epsilon
    int p;                   // Polynomial Kernel index: (xT*x)^p
    double _kernel_param;    // parameter for kernel function

    Sample **m_samples; // ALL samples

    double b;  // threshold
    double *a; // alpha (Lagrangian multipliers)
    double *w; // linear weight vector;

    int iterMax;

    double *error_cache;
    int is_linear_kernel;

    // sample size
    int size;
    // feature vector size
    int fsize;

} SVM;

typedef struct Args
{
    SVM *svm;
    Sample **samples;
    int size;

} Args;

SVM *NewSVM(int, double, double, enum KernelType, double);

void train(SVM *svm, Sample **samples, int size); // TRAIN
void train_parallel(SVM *svm, Sample **samples, int size, int);
void *thread(void *args_void_ptr);
int predict(SVM *svm, const Sample *x); // PREDICT

Sample *slice(Sample *v, int m, int n);
int examine_example(SVM *svm, int id1);
int take_step(SVM *svm, int id1, int id2);

double modelFunction(SVM *svm, int id);
double error(SVM *svm, int id);
double max(double x, double y);
double min(double x, double y);
double randFloat();

// Some kernel functions:
double kernel(SVM *svm, const Sample *xi, const Sample *xj);
double linearKernel(SVM *svm, const Sample *xi, const Sample *xj);
double polynomialKernel(SVM *svm, const Sample *xi, const Sample *xj);
double gaussianKernel(SVM *svm, const Sample *xi, const Sample *xj);

#endif // SVM_H
