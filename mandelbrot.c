#include <mandelbrot.h>
#include <gmp.h>
#include <omp.h>
#include <stdlib.h>
#include <stdio.h>

struct Complex_t
{
     mpf_t real, imag;
};

struct ComplexPlane_t
{
    mpf_t min_y, max_y, min_x, max_x;
};

mpf_t magnification;

ComplexPlane* initComplexPlane() 
{
	ComplexPlane *plane = malloc(sizeof(ComplexPlane));
	
	mpf_init2(magnification, MPF_MIN_BIT_PERC);
    mpf_set_ui(magnification, 2);

    mpf_init2(plane->min_y, MPF_MIN_BIT_PERC);
    mpf_init2(plane->max_y, MPF_MIN_BIT_PERC);
    mpf_init2(plane->min_x, MPF_MIN_BIT_PERC);
    mpf_init2(plane->max_x, MPF_MIN_BIT_PERC);
    mpf_set_si(plane->min_y, Y_MIN);
    mpf_set_si(plane->max_y, Y_MAX);
    mpf_set_si(plane->min_x, X_MIN);
    mpf_set_si(plane->max_x, X_MAX);
    
    return plane;
}


void updateComlexPlane(int pixel_x, int pixel_y, ComplexPlane *plane, int step, int resolution)
{
    mpf_t coordinate_x, coordinate_y;
    

    mpf_init2(coordinate_x, MPF_MIN_BIT_PERC);
    mpf_init2(coordinate_y, MPF_MIN_BIT_PERC);
    
    
    /* find x and y coordinates on axis using linear interpolation */    
    /*** for coordinate X ***/
    mpf_sub(plane->max_x, plane->max_x, plane->min_x);
    mpf_mul_ui(plane->max_x, plane->max_x, pixel_x); 
    mpf_div_ui(plane->max_x, plane->max_x,resolution);
    mpf_add(coordinate_x, plane->max_x, plane->min_x);
    
    /*** for coordinate Y ***/
    mpf_sub(plane->max_y, plane->max_y, plane->min_y);
    mpf_div_ui(plane->max_y, plane->max_y, resolution);
    mpf_mul_ui(coordinate_y, plane->max_y, pixel_y);
    mpf_mul_ui(plane->max_y, plane->max_y, resolution);
    mpf_sub(coordinate_y, plane->max_y, coordinate_y);
    mpf_add(coordinate_y, coordinate_y, plane->min_y);      

    /* calculate new values for the square plane with center the (coordinate_x, coordinate_y) */
    mpf_div_ui(magnification, magnification, step);
   
    mpf_sub(plane->min_y, coordinate_y, magnification);
    mpf_add(plane->max_y, coordinate_y, magnification);
    mpf_sub(plane->min_x, coordinate_x, magnification);
    mpf_add(plane->max_x, coordinate_x, magnification);
    
     mpf_clears(coordinate_x, coordinate_y, NULL);
    
}



void mandelbrotsetPixmap(ComplexPlane *plane, int maxIterations, XImage *pixmap, int resolution)
{

    int color;

    /* Mandlebrot variables */
    int i, j, k;
    double start_time, stop_time;
    
    complex   z, c;
    mpf_t dx, dy;
    mpf_t   lengthsq, temp;
    
    mpf_init2(dx, MPF_MIN_BIT_PERC);
    mpf_init2(dy, MPF_MIN_BIT_PERC);


    /* precalculate dx and dy */    
    /* for dx */
    mpf_sub(dx, plane->max_x, plane->min_x);
    mpf_div_ui(dx, dx, resolution);
    /* for dy */
    mpf_sub(dy, plane->max_y, plane->min_y);
    mpf_div_ui(dy, dy, resolution);

    /* keep time for info */
    start_time = omp_get_wtime();
    
   

#pragma omp parallel private(j, k, z, c, temp, lengthsq,  color)
{

    /* We initialize the variables after omp preprocessing directives   */
    /* because they must be initialized locally for each thread         */
    mpf_init2(z.real, MPF_MIN_BIT_PERC);
    mpf_init2(z.imag, MPF_MIN_BIT_PERC);
    mpf_init2(c.real, MPF_MIN_BIT_PERC);
    mpf_init2(c.imag, MPF_MIN_BIT_PERC);
    mpf_init2(temp,  MPF_MIN_BIT_PERC);
    mpf_init2(lengthsq, MPF_MIN_BIT_PERC);

    
    #pragma omp for schedule(runtime)
    for(i=0; i < resolution; i++) {
        for(j=0; j < resolution; j++) {
        
            //z.real = z.imag = 0.0;
            mpf_set_si(z.real, 0);
            mpf_set_si(z.imag, 0);
            
            //c.imag = plane->max_y - i * dy;
            mpf_mul_ui(c.imag, dy, i);
            mpf_sub(c.imag, plane->max_y, c.imag);
            
            //c.real = plane->min_x + j * dx;
            mpf_mul_ui(c.real, dx, j);
            mpf_add(c.real, plane->min_x, c.real);
            
            k = 0;
            do  {    /* iterate for pixel color */
            
                mpf_mul(temp, z.real, z.imag);
                mpf_mul_ui(temp, temp, 2);
                mpf_add(temp, temp, c.imag);  
                // temp = 2.0*z.real*z.imag + c.imag;             
                
                mpf_pow_ui(z.real, z.real, 2);
                mpf_pow_ui(z.imag, z.imag, 2);
                mpf_sub(z.real, z.real, z.imag);
                mpf_add(z.real, z.real, c.real);
                //z.real = z.real*z.real - z.imag*z.imag + c.real;
                
                mpf_set(z.imag, temp);
                //z.imag = temp;
                  
                mpf_pow_ui(temp, z.real, 2);
                mpf_pow_ui(lengthsq, z.imag, 2);
                mpf_add(lengthsq, temp, lengthsq); 
                //lengthsq = z.real*z.real+z.imag*z.imag;
                
                
                k++; 
            } while ( (mpf_cmp_si(lengthsq,4) < 0) && k < maxIterations);
            
            color = k*1808; /* 1808 is an arbitrary number used for coloring scheme */
            ((unsigned *) pixmap->data)[i*resolution + j] =color;
            if ( k == maxIterations) 
                ((unsigned *) pixmap->data)[i*resolution + j] = 0; //an to shmeio anhkei sto set tote xrwmatiseto me mauro
          
        }  
    }
    mpf_clears(z.real, z.imag, c.real, c.imag, temp, lengthsq, NULL);
}
    stop_time = omp_get_wtime();

    printf("Elapsed time: %f\n", stop_time-start_time);    

}


