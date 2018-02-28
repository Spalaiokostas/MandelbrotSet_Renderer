#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <mandelbrot.h>

#define DEFAULT_RESOLUTION  800    /* default value for X_RESN and Y_RESN */
#define MAX_RESOLUTION      1200
#define MIN_RESOLUTION      300

#define DEFAULT_STEP        2      /* default value for step (zooming per click) */

#define DEFAULT_ITERATIONS  100    /* default value for number of maximum iterations for the set */
#define MAX_ITERATIONS      1000
#define MIN_ITERATIONS      50

void print_usage();
int main(int argc, char *argv[])
{
    int resolution  = DEFAULT_RESOLUTION;   
    int iterations  = DEFAULT_ITERATIONS;  
    int c;
    
    /* gui variables */
    GC gc;
    Window win;
    int depth, screen;
    Visual *visual;
    Display *dis;
    XImage *pixmap;
    XEvent event;
    
    
    ComplexPlane *plane = NULL;
 
    /* use getopts to parse optional arguments supplied by user */
    while (1) {
    
        int option_index = 0;
        static struct option long_options[] = {
            {"resolution",  optional_argument,  NULL,   'r'},
            {"iterations",  optional_argument,  NULL,   'i'},
            {0,             0,                  0,       0 }
        };
        
        if ( (c = getopt_long(argc, argv, "r::i::", long_options, &option_index)) == -1) 
            break;
            
        switch (c) {
            case 'r':
                if (optarg) 
                {
                    resolution = atoi(optarg);
                    if ( (resolution > MAX_RESOLUTION) | (resolution < MIN_RESOLUTION) )
                        print_usage();
                }
                break;
            case 'i':
                if (optarg) 
                {
                    iterations = atoi(optarg);
                    if ( (iterations > MAX_ITERATIONS) | (iterations < MIN_ITERATIONS) )
                        print_usage();
                }
                break; 
            default: 
                print_usage();    
        }
    }
    
    /* Create Display */
    dis=XOpenDisplay((char *)0);
	if (!dis) {
        return EXIT_FAILURE;    
    }
    screen = DefaultScreen(dis);
    depth=DefaultDepth(dis,screen);

    /* Create window */
	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,resolution,resolution,0,0,0);
    XMapWindow(dis,win);
    XFlush(dis);

    visual = DefaultVisual(dis, screen);
	XSelectInput(dis, win, KeyPressMask | ButtonPressMask);

	gc=XCreateGC(dis,win,0,NULL);
    
    /* Create Zpixmap */
    pixmap = XCreateImage(dis, visual, depth, ZPixmap, 0, NULL,
                              resolution, resolution, 32, 0);
    pixmap->data = malloc(pixmap->bytes_per_line * resolution);
    
    /* Draw Mandelbrot set on pixmap */
    plane = initComplexPlane();
    mandelbrotsetPixmap(plane, iterations, pixmap, resolution);
    
    /* Put Pixmap on top of the window */
    XPutImage(dis,win,gc,pixmap,0,0,0,0,resolution,resolution);
    
    
    /* a loop that terminates when user presses 'q' button and zooms the point that user right clicked */
    while(1) {
        XNextEvent(dis, &event);
        if (event.type == KeyPress && (XLookupKeysym(&event.xkey, 0) == XK_q))
            break;
        else if (event.type == ButtonPress) {
        	XDestroyImage(pixmap); 
            pixmap = XCreateImage(dis, visual, depth, ZPixmap, 0, NULL,
                              resolution, resolution, 32, 0);
    		pixmap->data = malloc(pixmap->bytes_per_line * resolution);
    		
            updateComlexPlane(event.xbutton.x, event.xbutton.y, plane, DEFAULT_STEP, resolution);
            mandelbrotsetPixmap(plane, iterations, pixmap, resolution);
            
            XPutImage(dis,win,gc,pixmap,0,0,0,0,resolution,resolution);
        }
    }
       
    /* Program Finished */
    XCloseDisplay(dis);
    return 0;
    
    
    
}


void print_usage()
{
    char *parameters_info   = "RESOLUTION: *Optional* Parameter that sets the resolution of the window that the set is renderd.\n"
                              "    NOTE: The window is square with ( resolution x resolution) total number of pixels. Setting this parameter to high slows rendering\n"
                              "    Default value: 100         Min value:300        Max value: 1200\n\n"
                              "ITERATIONS: *Optional* Parameter that specifies the number of maximum iterations before determining"
                              " if a point belong at the set.\n    NOTE: Setting this parameter too high will slow down the rendering\n"
                              "    Default value: 100         Min value: 50        Max value: 1000";
                                
    char *usage            = "usage: mandelbrotSet --resolution=<num> --zoom=<num> --iterations=<num>";
    
    printf("\n%s\n\n%s\n", usage, parameters_info);
    exit(0);
}                            
    

                
                    
                 
                
        

    
