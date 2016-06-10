#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"

struct vertex_normal {
	double vertex[3];
	double normal[3];
};

double * calculate_normal( double a1, double a2, double a3,
													 double b1, double b2, double b3 );
double calculate_dot( struct matrix *points, int i );

double distance(double x0, double y0, double x1, double y1);

double dot_product(double *a, double *b);

void normalize(double *v);
	
double get_ambient(int ca, double ka);

double get_diffuse(double *light, double *normal, int cp, double kd);

double get_specular(double *light, double *normal, double *view,
										int cp, double ks);

int get_illumination(double *l, double *n, int cp, double *constants);

#endif
