#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "sample.h"

Sample *NewSample(double *x, int size, double y)
{
    Sample *sample = (Sample *)malloc(sizeof(Sample));
    sample->x = x;
    sample->y = y;
    sample->size = size;

    return sample;
}

double modSquared(Sample *sample)
{
    double res = 0;
    for (unsigned int i = 0; i < sample->size; i++)
    {
        res += sample->x[i] * sample->x[i];
    }
    return res;
}

double sampleProduct(const Sample *x1, const Sample *x2)
{
    double res = 0;

    int i = 0;
    int j = 0;

    if (x1->size < x2->size)
    {
        for (i = 0; i < x1->size; i++)
        {
            res += x1->x[i] * x2->x[i];
        }
    }
    else
    {
        for (i = 0; i < x2->size; i++)
        {
            res += x1->x[i] * x2->x[i];
        }
    }

    return res;
}

double product(const double *w, const Sample *x)
{
    double res = 0;

    for (unsigned int i = 0; i < x->size; i++)
    {

        res += w[i] * x->x[i];
    }

    return res;
}

double *plus(const double *w, const Sample *x)
{
    double *res = (double *)malloc(sizeof(double) * x->size);

    for (unsigned int i = 0; i < x->size; i++)
    {
        int id = x->x[i];
        res[id] = w[id] + x->x[i];
    }

    return res;
}

Sample *multiplyNum(const Sample *x, float t)
{
    // Sample *res = (Sample *)malloc(sizeof(Sample));
    Sample *res = NewSample((double *)malloc(sizeof(double) * x->size), x->size, 0);

    for (unsigned int i = 0; i < x->size; i++)
    {
        res->x[i] = t * x->x[i];
    }

    return res;
}

Sample *minus(const Sample *x1, const Sample *x2)
{
    Sample *tmp = NewSample((double *)malloc(sizeof(double) * x1->size), x1->size, 0);

    int i = 0;
    int j = 0;
    if (x1->size < x2->size)
    {
        for (i = 0; i < x1->size; i++)
        {
            tmp->x[i] = (x1->x[i] - x2->x[i]);
        }
    }
    else
    {
        for (i = 0; i < x2->size; i++)
        {
            tmp->x[i] = (x1->x[i] - x2->x[i]);
        }
    }

    return tmp;
}
