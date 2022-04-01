#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "svm.h"

#define TOLERANCE 0.001

SVM *NewSVM(int _iterMax, double _C, double _eps, enum KernelType _m_kernelType, double _kernel_param)
{
    SVM *svm = (SVM *)malloc(sizeof(SVM));

    svm->iterMax = _iterMax;
    svm->C = _C;
    svm->eps = _eps;
    svm->m_kernelType = _m_kernelType;
    svm->_kernel_param = _kernel_param;

    switch (svm->m_kernelType)
    {
    case _Gaussian:
        svm->sigma = (double)_kernel_param;
        break;
    case _polynomial:
        svm->p = (int)_kernel_param;
        break;
    default:
        break;
    }

    return svm;
}

void *thread(void *args_void_ptr)
{
    Args *args = (Args *)args_void_ptr;
    train(args->svm, args->samples, args->size);
    return NULL;
}

void train_parallel(SVM *svm, Sample **samples, int sample_size, int slices)
{
    // we don't take bullshit
    if (sample_size <= 0)
        return;

    svm->m_samples = samples;
    svm->size = sample_size;
    svm->fsize = samples[0]->size;
    svm->a = (double *)realloc(svm->a, sizeof(double) * (svm->size));

    int total = sample_size;
    int size = ceil(total / slices);
    int rc, end, start = 0, i = 0, j = 0;
    SVM **svm_list = (SVM **)malloc(sizeof(SVM *) * slices);
    pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * slices);
    pthread_attr_t attr;
    void *status;
    Args *args;

    // init pthread
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    // printf("start memory: %p\n", samples);
    while (start < total)
    {
        end = min(start + size, total) - 1;

        svm_list[i] = NewSVM(svm->iterMax, svm->C, svm->eps, svm->m_kernelType, svm->sigma);

        // create args
        args = (Args *)malloc(sizeof(Args));
        args->svm = svm_list[i];
        args->size = end - start + 1;
        args->samples = samples + start;
        // args->samples = (Sample**)malloc(sizeof(Sample*) * args->size);
        // for ( i = 0; i < args->size; i++)
        // {
        //     args->samples[i] = samples[i + start];
        // }
        
        // printf("start index: %d\nend index: %d\nstart memory: %p\nend memory: %p\nsize: %d\n", start, end, args->samples, args->samples + end, args->size);

        // create thread
        rc = pthread_create(threads + i, &attr, thread, (void *)args);

        start = end + 1;
        i++;
    }
    // printf("end memory: %p\n", samples + sample_size - 1);

    // join threads
    for (i = 0; i < slices; i++)
    {
        rc = pthread_join(threads[i], &status);
    }

    svm->b = 0;
    int _size = 0;
    // merge local svm->b, alphas, svm->w and samples
    for (i = 0; i < slices; i++)
    {
        svm->b += svm_list[i]->b;
        for (j = 0; j < svm->fsize; j++)
        {
            svm->w[j] += svm_list[i]->w[j];
        }

        // apend alphas to svm->alphas
        _size += svm_list[i]->size;
        for (j = 0; j < svm_list[i]->size; j++)
        {
            svm->a[_size - svm_list[i]->size + j] = svm_list[i]->a[j];
        }
    }

    svm->b /= slices;

    // free svm list
}

double max(double x, double y)
{
    return x > y ? x : y;
}

double min(double x, double y)
{
    return x < y ? x : y;
}

double randFloat()
{
    return (rand() % 1000000) / 1000000.0;
}

int predict(SVM *svm, const Sample *x)
{
    double res = -svm->b;
    int j;

    for (j = 0; j < svm->size; j++)
        res += svm->a[j] * svm->m_samples[j]->y * kernel(svm, svm->m_samples[j], x);

    return res >= 0 ? +1 : -1;
}

void train(SVM *svm, Sample **samples, int sample_size)
{

    // init paramters
    if (sample_size < 2)
        return;

    int num_changed = 0;
    int examine_all = 1;
    int i;
    int iterCount = 0;

    svm->m_samples = samples;
    svm->size = sample_size;
    svm->fsize = samples[0]->size;

    svm->a = (double *)malloc(sizeof(double) * svm->size);
    svm->b = 0.0f;

    svm->w = (double *)malloc(sizeof(double) * svm->fsize); // for linear kernel

    // set a and w to 0
    for (int i = 0; i < svm->fsize; i++)
    {
        svm->w[i] = 0;
    }
    
    for (int i = 0; i < svm->size; i++)
    {
        svm->a[i] = 0;
    }


    srand(time(NULL));

    while ((num_changed > 0 || examine_all) && iterCount < svm->iterMax)
    {
        num_changed = 0;

        if (examine_all)
        {
            for (i = 0; i < svm->size; i++)
                num_changed += examine_example(svm, i);
        }
        else
        {
            for (i = 0; i < svm->size; i++)
            {
                if (svm->a[i] != 0 && svm->a[i] != svm->C)
                    num_changed += examine_example(svm, i);
            }
        }

        examine_all = examine_all ? 0 : num_changed == 0;

        // printf("examine_all: %d\nnum_changed: %d\n", examine_all, num_changed);

        for (i = 0; i < svm->size; i++)
        {
            if (svm->a[i] < 1e-6)
                svm->a[i] = 0.0;
        }

        iterCount += 1;
    }
}

int examine_example(SVM *svm, int id1)
{
    double y1 = svm->m_samples[id1]->y;
    double alph1 = svm->a[id1];
    double e1 = 0.0;
    int k;

    e1 = error(svm, id1);

    if ((y1 * e1 < -TOLERANCE && alph1 < svm->C) || (y1 * e1 > TOLERANCE && alph1 > 0))
    {
        int id2 = -1;
        double _max = 0.0;

        // Way1: Try argmax |E1-E2|
        for (k = 0; k < svm->size; k++)
        {
            if (svm->a[k] > 0 && svm->a[k] < svm->C)
            {
                double e2 = error(svm, k);
                double tmp = fabs(e1 - e2);

                if (tmp > _max)
                {
                    _max = tmp;
                    id2 = k;
                }
            }
        }

        if (id2 >= 0)
        {
            if (take_step(svm, id1, id2))
                return 1;
        }

        // Way2: Try iterating through the non-bound examples;
        int k0 = (int)(randFloat() * svm->size);
        for (k = k0; k < svm->size + k0; k++)
        {
            id2 = k % svm->size;

            if (svm->a[id2] > 0 && svm->a[id2] < svm->C)
            {
                if (take_step(svm, id1, id2))
                    return 1;
            }
        }

        // Way3: try iterating through the entire training set;
        k0 = (int)(randFloat() * svm->size);
        for (k = k0; k < svm->size + k0; k++)
        {
            id2 = k % svm->size;

            if (id1 != id2 && take_step(svm, id1, id2))
                return 1;
        }
    }
    return 0;
}

int take_step(SVM *svm, int id1, int id2)
{
    if (id1 == id2)
        return 0;

    double alph1 = svm->a[id1]; // old values of alpha[id1]
    double alph2 = svm->a[id2]; // old values of alpha[id2]
    float e1, e2;
    int y1 = svm->m_samples[id1]->y;
    int y2 = svm->m_samples[id2]->y;

    e1 = error(svm, id1);
    e2 = error(svm, id1);

    int s = y1 * y2;

    double L, H;

    if (y1 == y2)
    {
        L = max(0, alph1 + alph2 - svm->C);
        H = min(svm->C, alph1 + alph2);
    }
    else
    {
        L = max(0, alph1 - alph2);
        H = min(svm->C, svm->C - alph1 + alph2);
    }

    if (fabs(L - H) < 1e-6) // L==H
        return 0;

    float k11 = kernel(svm, svm->m_samples[id1], svm->m_samples[id1]);
    float k12 = kernel(svm, svm->m_samples[id1], svm->m_samples[id2]);
    float k22 = kernel(svm, svm->m_samples[id2], svm->m_samples[id2]);
    double eta = 2 * k12 - k11 - k22;

    float a1_new, a2_new;
    if (eta < 0)
    {
        a2_new = alph2 + y2 * (e2 - e1) / eta;

        if (a2_new < L)
            a2_new = L;
        else if (a2_new > H)
            a2_new = H;
    }
    else
    {
        double c1 = eta / 2.0;
        double c2 = y2 * (e1 - e2) - eta * alph2;
        double Lobj = c1 * L * L + c2 * L;
        double Hobj = c1 * H * H + c2 * H;

        if (Lobj > Hobj + svm->eps)
            a2_new = L;
        else if (Lobj < Hobj - svm->eps)
            a2_new = H;
        else
            a2_new = alph2;
    }

    // If too close.
    if (fabs(a2_new - alph2) < svm->eps * (a2_new + alph2 + svm->eps))
        return 0;

    a1_new = alph1 - s * (a2_new - alph2);

    if (a1_new < 0)
    {
        a2_new += s * a1_new;
        a1_new = 0;
    }
    else if (a1_new > svm->C)
    {
        a2_new += s * (a1_new - svm->C);
        a1_new = svm->C;
    }

    // Update svm->b(threshold)
    double bnew;
    if (a1_new > 0 && a1_new < svm->C)
        bnew = svm->b + e1 + y1 * (a1_new - alph1) * k11 + y2 * (a2_new - alph2) * k12;
    else if (a2_new > 0 && a2_new < svm->C)
        bnew = svm->b + e2 + y1 * (a1_new - alph1) * k12 + y2 * (a2_new - alph2) * k22;
    else
    {
        double b1 = svm->b + e1 + y1 * (a1_new - alph1) * k11 + y2 * (a2_new - alph2) * k12;
        double b2 = svm->b + e2 + y1 * (a1_new - alph1) * k12 + y2 * (a2_new - alph2) * k22;
        bnew = (b1 + b2) * 0.5;
    }

    double delta_b = bnew - svm->b;

    svm->b = bnew;

    float t1 = y1 * (a1_new - alph1);
    float t2 = y2 * (a2_new - alph2);

    // update weight vector if linear SVM
    if (svm->m_kernelType == _linear)
    {
        // svm->w = svm->w + t1*svm->m_samples[id1] + t2*svm->m_samples[id2]
        svm->w = plus(svm->w, multiplyNum(svm->m_samples[id1], t1));
        svm->w = plus(svm->w, multiplyNum(svm->m_samples[id2], t2));
    }

    svm->a[id1] = a1_new;
    svm->a[id2] = a2_new;

    return 1;
}

double modelFunction(SVM *svm, int id) // f(x);
{
    float s = 0;

    if (svm->m_kernelType != _linear)
    {
        s = product(svm->w, svm->m_samples[id]);
    }
    else
    {
        for (int i = 0; i < svm->size; i++)
        {
            if (svm->a[i] > 0)
                s += svm->a[i] * svm->m_samples[i]->y * kernel(svm, svm->m_samples[i], svm->m_samples[id]);
        }
    }

    s -= svm->b;
    return s;
}

double error(SVM *svm, int id) // E(x);
{
    double e = modelFunction(svm, id);
    e -= svm->m_samples[id]->y;
    return e;
}

double kernel(SVM *svm, const Sample *xi, const Sample *xj)
{
    switch ((int)svm->m_kernelType)
    {
    case _linear:
        return linearKernel(svm, xi, xj);
    case _polynomial:
        return polynomialKernel(svm, xi, xj);
    case _Gaussian:
        return gaussianKernel(svm, xi, xj);
    default:
        return linearKernel(svm, xi, xj);
    }
}

double linearKernel(SVM *svm, const Sample *xi, const Sample *xj)
{
    return sampleProduct(xi, xj);
}

double polynomialKernel(SVM *svm, const Sample *xi, const Sample *xj)
{
    return pow(sampleProduct(xi, xj) + 1, svm->p);
}

double gaussianKernel(SVM *svm, const Sample *xi, const Sample *xj)
{
    return exp(-modSquared(minus(xi, xj)) * svm->sigma);
}
