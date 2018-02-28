#ifndef MANDELBROT_H
#define MANDELBROT_H

#include <X11/Xlib.h>

#define MPF_MIN_BIT_PERC 260 /* This the minimun precision used in GNU MP library (expressed in bits) */ 

#define         X_MIN   -2.0
#define         X_MAX    2.0
#define         Y_MIN   -2.0
#define         Y_MAX    2.0

typedef struct ComplexPlane_t ComplexPlane;
typedef struct Complex_t complex;

void mandelbrotsetPixmap(ComplexPlane *plane, int maxIterations, XImage *pixmap, int resolution);
void updateComlexPlane(int pixel_x, int pixel_y, ComplexPlane *plane, int step, int resolution);
ComplexPlane* initComplexPlane();

#endif
