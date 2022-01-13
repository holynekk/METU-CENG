/********************************************************
 * Kernels to be optimized for the Metu Ceng Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "Team",                     /* Team name */

    "Student Name",             /* First member full name */
    "eXXXXXXX",                 /* First member id */

    "",                         /* Second member full name (leave blank if none) */
    "",                         /* Second member id (leave blank if none) */

    "",                         /* Third member full name (leave blank if none) */
    ""                          /* Third member id (leave blank if none) */
};

/****************
 * EXPOSURE FUSION KERNEL *
 ****************/

/*******************************************************
 * Your different versions of the exposure fusion kernel go here 
 *******************************************************/

/* 
 * naive_fusion - The naive baseline version of exposure fusion
 */
char naive_fusion_descr[] = "naive_fusion: Naive baseline exposure fusion";
void naive_fusion(int dim, int *img, int *w, int *dst) {
  
    int i, j, k; 
    for(k = 0; k < 4; k++){
        for(i = 0; i < dim; i++) {
            for(j = 0; j < dim; j++) {
                    dst[i*dim+j] += w[k*dim*dim+i*dim+j] * img[k*dim*dim+i*dim+j];
            }
        }
    }
}

/* 
 * fusion - Your current working version of fusion
 * IMPORTANT: This is the version you will be graded on
 */
char fusion_descr[] = "fusion: Current working version";
void fusion(int dim, int *img, int *w, int *dst) 
{
    naive_fusion(dim, img, w, dst);
}

/*********************************************************************
 * register_fusion_functions - Register all of your different versions
 *     of the fusion kernel with the driver by calling the
 *     add_fusion_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_fusion_functions() 
{
    add_fusion_function(&naive_fusion, naive_fusion_descr);  
    add_fusion_function(&fusion, fusion_descr);  
    /* ... Register additional test functions here */
}

/***************************
 * GAUSSIAN BLUR KERNEL *
 ***************************/

/******************************************************
 * Your different versions of the Gaussian blur functions go here
 ******************************************************/

/* 
 * naive_blur - The naive baseline version of Gussian blur
 */
char naive_blur_descr[] = "naive_blur The naive baseline version of Gaussian blur";
void naive_blur(int dim, float *img, float *flt, float *dst) {
  
    int i, j, k, l;

    for(i = 0; i < dim-5+1; i++){
        for(j = 0; j < dim-5+1; j++) {
            for(k = 0; k < 5; k++){
                for(l = 0; l < 5; l++) {
                    dst[i*dim+j] = dst[i*dim+j] + img[(i+k)*dim+j+l] * flt[k*dim+l];
                }
            }
        }
    }
}

/* 
 * blur - Your current working version of Gaussian blur
 * IMPORTANT: This is the version you will be graded on
 */
char blur_descr[] = "blur: Current working version";
void blur(int dim, float *img, float *flt, float *dst) 
{
    naive_blur(dim, img, flt, dst);
}

/*********************************************************************
 * register_blur_functions - Register all of your different versions
 *     of the gaussian blur kernel with the driver by calling the
 *     add_blur_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_blur_functions() 
{
    add_blur_function(&naive_blur, naive_blur_descr); 
    add_blur_function(&blur, blur_descr);
    /* ... Register additional test functions here */
}

