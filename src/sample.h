#ifndef SAMPLE_H
#define SAMPLE_H

typedef struct Sample
{
    double *x; // feature vector
    double y;
    int size;
} Sample;

Sample *NewSample(double *x, int size, double y);

double modSquared(Sample *sample);

double sampleProduct(const Sample *x1, const Sample *x2);

double product(const double *w, const Sample *x);

double *plus(const double *w, const Sample *x);

Sample *multiplyNum(const Sample *x, float t);

Sample *minus(const Sample *x1, const Sample *x2);

#endif // SAMPLE_H