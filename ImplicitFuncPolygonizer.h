#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#define TET	0  /* use tetrahedral decomposition */
#define NOTET	1  /* no tetrahedral decomposition  */


typedef struct point 
{		   /* a three-dimensional point */
    double x, y, z;		   /* its coordinates */
} POINT;


typedef struct vertex 
{		   /* surface vertex */
    POINT position, normal;	   /* position and surface normal */
} VERTEX;

typedef struct vertices 
{	   /* list of vertices in polygonization */
    int count, max;		   /* # vertices, max # allowed */
    VERTEX *ptr;		   /* dynamically allocated */
} VERTICES;



typedef double(*ImplicitFunction)(double, double, double);
typedef int (*triangleProc)(int, int, int, VERTICES);

char *polygonize(ImplicitFunction function, double size, int bounds,
                 double x, double y, double z, triangleProc triproc, int mode);
#ifdef __cplusplus
}
#endif

