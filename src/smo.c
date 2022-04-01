#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <time.h>
#include <sys/time.h>
#include <limits.h>

#include "svm.h"

#define MAXCHAR 10000
typedef struct timezone timezone_t;
typedef struct timeval timeval_t;

static struct timeval _t1, _t2;
static struct timezone _tz;
timeval_t t1, t2;
timezone_t tz;

static unsigned long _temps_residuel = 0;
#define top1() gettimeofday(&_t1, &_tz)
#define top2() gettimeofday(&_t2, &_tz)

void init_cpu_time(void)
{
    top1();
    top2();
    _temps_residuel = 1000000L * _t2.tv_sec + _t2.tv_usec -
                      (1000000L * _t1.tv_sec + _t1.tv_usec);
}

unsigned long cpu_time(void) /* retourne des microsecondes */
{
    return 1000000L * _t2.tv_sec + _t2.tv_usec -
           (1000000L * _t1.tv_sec + _t1.tv_usec) - _temps_residuel;
}

int main(int argc, const char *argv[])
{
    

    FILE *fp;
    char row[MAXCHAR];
    char *token;
    double number, *numbers = NULL;
    int size = 0, fsize = 1, i = 0, j = 0, threads = 8;
    clock_t t;
    double duration;
    unsigned long time;

        if (argc >= 2)
    {
        threads = atoi(argv[1]);
    }

    fp = fopen("data/spam.csv", "r");

    while (!feof(fp))
    {
        fgets(row, MAXCHAR, fp);
        if (size == 0)
        {
            token = strtok(row, ",");
            while (token != NULL)
            {
                fsize++;
                token = strtok(NULL, ",");
            }
        }

        size++;
    }

    fsize--;

    Sample **samples = (Sample **)malloc(sizeof(Sample *) * size);
    fseek(fp, 0, SEEK_SET);

    printf("Dataset\nlines: %d \ncols: %d \n", size, fsize);

    while (!feof(fp))
    {
        fgets(row, MAXCHAR, fp);
        numbers = (double *)malloc(sizeof(double) * fsize + 1);

        j = 0;
        token = strtok(row, ",");
        while (token != NULL)
        {
            sscanf(token, "%lf", &number);
            numbers[j] = number;
            j++;
            token = strtok(NULL, ",");
        }

        samples[i] = NewSample(numbers, fsize - 1, numbers[fsize - 1]);
        i++;
    }

    fclose(fp);
    
    // for (i = 0; i < fsize-1; i++)
    // {
    //     printf(" %0.2lf", samples[501]->x[i]);
    // }
    //     printf("\n\n%0.2lf\n\n", samples[501]->x[57]);

    init_cpu_time();

    // training
    SVM *classifier = NewSVM(1000, 1, 0.001f, _linear, 0);

    // sequential training
    top1();
    train(classifier, samples, size);
    top2();

    time = cpu_time();

    printf("\nSequential\n\ttraining time: %ld microseconds \n", time);

    // calculate accuracy
    // int correct = 0;
    // for (int i = 0; i < size; i++)
    // {
    //     correct += predict(classifier, samples[i]) == samples[i]->y ? 1 : 0;
    // }
    // printf("\taccuracy: %lf %\n", (double)correct *100 / size);

    // parallel
    top1();
    train_parallel(classifier, samples, size, threads);
    top2();

    time = cpu_time();

    printf("\nParallel\n\ttraining time: %ld microseconds \n", time);

    // calculate accuracy
    // correct = 0;
    // for (int i = 0; i < size; i++)
    // {
    //     correct += predict(classifier, samples[i]) == samples[i]->y ? 1 : 0;
    // }
    // printf("\taccuracy: %lf %\n", (double)correct *100 / size);
}