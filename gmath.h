#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"
#include "ml6.h"
#include "symtab.h"

#define X 0
#define Y 1
#define Z 2

typedef double vector[3];

struct vertex_normal {
	double vertex[3];
	double normal[3];
};

union cv {
	color c[2];
	vector v[2];
};


void calculate_normal( vector normal, double a1, double a2, double a3,
											 double b1, double b2, double b3 );

void calculate_surface_normal(struct matrix *points, int i, vector normal);

double calculate_dot( struct matrix *points, int i );

double distance(double x0, double y0, double x1, double y1);

double dot_product(vector a, vector b);

void normalize(vector v);
	
double get_ambient(int ca, double ka);

double get_diffuse(vector light, vector normal, int cp, double kd);

double get_diffuse(vector light, vector normal, int cp, double kd);

color get_illumination(vector light, vector normal, double *cp, color ca, struct constants *cons);

#endif
