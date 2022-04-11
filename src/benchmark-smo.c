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
    

    init_cpu_time();

    SVM *classifier = NewSVM(1000, 1, 0.001f, _linear, 0);
    FILE * temp = fopen("smo.dat", "w");

    for (i = 100; i < 1001; i+=100)
    {
		fprintf(temp, "%lf", (double)i);

        top1();
        train(classifier, samples, i);
        top2();

        time = cpu_time();

        fprintf(temp, "\t%lf", (double)time/1000);

        for (j = 2; j < 9; j+=2)
        {
            top1();
            train_parallel(classifier, samples, i, j);
            top2();

            time = cpu_time();

		    fprintf(temp, "\t%lf", (double)time/1000);
        }

		fprintf(temp, "\n");
    }

    fclose(fp);

}