#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "primitives.h"
#include "raytracing.h"
#include "raythread.h"

#define OUT_FILENAME "out.ppm"

#define ROWS 512
#define COLS 512
#define THREAD_NUM 8.0


static void write_to_ppm(FILE *outfile, uint8_t *pixels,
                         int width, int height)
{
    fprintf(outfile, "P6\n%d %d\n%d\n", width, height, 255);
    fwrite(pixels, 1, height * width * 3, outfile);
}

static double diff_in_second(struct timespec t1, struct timespec t2)
{
    struct timespec diff;
    if (t2.tv_nsec-t1.tv_nsec < 0) {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec - 1;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec + 1000000000;
    } else {
        diff.tv_sec  = t2.tv_sec - t1.tv_sec;
        diff.tv_nsec = t2.tv_nsec - t1.tv_nsec;
    }
    return (diff.tv_sec + diff.tv_nsec / 1000000000.0);
}

int main()
{
    /*thread variable*/
    pthread_t *threadx;
    struct parameter *threadpara;

    uint8_t *pixels;
    light_node lights = NULL;
    rectangular_node rectangulars = NULL;
    sphere_node spheres = NULL;
    color background = { 0.0, 0.1, 0.1 };
    struct timespec start, end;

#include "use-models.h"

    /* allocate by the given resolution */
    pixels = malloc(sizeof(unsigned char) * ROWS * COLS * 3);
    if (!pixels) exit(-1);

    printf("# Rendering scene\n");
    /* do the ray tracing with the given geometry */
    clock_gettime(CLOCK_REALTIME, &start);

    /*****create parameter which thread used****/
    threadpara =(struct parameter*)malloc(THREAD_NUM*sizeof(struct parameter));
    for(int i =0; i<THREAD_NUM; i++) {
        threadpara[i].begin_col =COLS*((double)i/THREAD_NUM);
        threadpara[i].finish_col =COLS*((double)(i+1.0)/THREAD_NUM);
        threadpara[i].pixels =pixels;
        threadpara[i].lights =lights;
        threadpara[i].rectangulars = rectangulars;
        threadpara[i].spheres =spheres;
        memcpy( threadpara[i].background,background,sizeof(color));
        threadpara[i].view = &view;
        threadpara[i].width = ROWS;
        threadpara[i].height = COLS;
    }
    threadx=(pthread_t*)malloc(THREAD_NUM*sizeof(pthread_t));

    /*
    pthread_create(&threadx[0],NULL,&raytracing,&threadpara[0]);
    pthread_create(&threadx[1],NULL,&raytracing,&threadpara[1]);
    pthread_create(&threadx[2],NULL,&raytracing,&threadpara[2]);
    pthread_create(&threadx[3],NULL,&raytracing,&threadpara[3]);


    pthread_join(threadx[0],NULL);
    pthread_join(threadx[1],NULL);
    pthread_join(threadx[2],NULL);
    pthread_join(threadx[3],NULL);
     */

    for(int i=0; i<THREAD_NUM; i++) {
        pthread_create(&threadx[i],NULL,&raytracing,&threadpara[i]);
    }
    for(int i=0; i<THREAD_NUM; i++) {
        pthread_join(threadx[i],NULL);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    {
        FILE *outfile = fopen(OUT_FILENAME, "wb");
        write_to_ppm(outfile, pixels, ROWS, COLS);
        fclose(outfile);
    }

    delete_rectangular_list(&rectangulars);
    delete_sphere_list(&spheres);
    delete_light_list(&lights);
    free(pixels);
    FILE *output;
    output= fopen("opt.txt","a");
    fprintf(output,"raytracing(): %lf \n", diff_in_second(start, end));
    fclose(output);
    printf("Done!\n");
    printf("Execution time of raytracing() : %lf sec\n", diff_in_second(start, end));
    free(threadx);
    return 0;
}
