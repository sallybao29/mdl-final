#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "gmath.h"
#include "parser.h"



/*======== double * calculate_normal() ==========
  Inputs:   double ax
	double ay
	double az
	double bx
	double by
	double bz  
  Returns: A double arry of size 3 representing the 
	cross product of <ax, ay, az> and <bx, by, bz>

  04/17/12 16:46:30
  jonalf
  ====================*/
void calculate_normal( vector normal, double ax, double ay, double az,	
											 double bx, double by, double bz ) {
	
	
	normal[X] = ay*bz - az*by;
  normal[Y] = az*bx - ax*bz;
  normal[Z] = ax*by - ay*bx;
}

void calculate_surface_normal(struct matrix *points, int i, vector normal){
	double ax, ay, az, bx, by, bz;

  //calculate A and B vectors
  ax = points->m[X][i+1] - points->m[X][i];
  ay = points->m[Y][i+1] - points->m[Y][i];
  az = points->m[Z][i+1] - points->m[Z][i];

  bx = points->m[X][i+2] - points->m[X][i];
  by = points->m[Y][i+2] - points->m[Y][i];
  bz = points->m[Z][i+2] - points->m[Z][i];

  calculate_normal(normal, ax, ay, az, bx, by, bz );
}

/*======== double calculate_dot() ==========
  Inputs:   struct matrix *points
	int i  
  Returns: The dot product of a surface normal and
	a view vector
  
  calculates the dot product of the surface normal to
  triangle points[i], points[i+1], points[i+2] and a 
  view vector (use <0, 0, -1> to start.

  04/17/12 16:38:34
  jonalf
  ====================*/
double calculate_dot( struct matrix *points, int i ) {

  vector normal;
	vector view;
  double dot;

  calculate_surface_normal(points, i, normal);

	//set up view vector
	view[X] = 0;
	view[Y] = 0;
	view[Z] = -1;

	//calculate dot product
	dot = dot_product(normal, view);

	return dot;
}



/*======== double dot_product() ==========
  Inputs:  double *a 
	double *b
	Returns: The dot product of two vectors
  
	====================*/
double dot_product(vector a, vector b){
  double dot;

  dot = a[X] * b[X] + a[Y] * b[Y] + a[Z] * b[Z];

  return dot;
}

/*======== double distance() ==========
  Inputs:  double x0
	double y0
	double x1
	double y1
	Returns: The distance between two points
  
	====================*/
double distance(double x0, double y0, double x1, double y1){
	double a, b;

	a = pow((x1 - x0), 2);
	b = pow((y1 - y0), 2);
	return sqrt(a + b);
}

/*======== double get_ambient() ==========
  Inputs:  int ca
	double ka
	Returns: ambient component of illumination

	Product of ambient color and constant of ambient reflection
  
	====================*/
double get_ambient(int ca, double ka){
	double val;

	val = ca * ka;
	return val > 0 ? val : 0;
}

/*======== double get_ambient() ==========
  Inputs: double *v 
	
	Normalizes vector v by dividing all components by v's magnitude

	====================*/
void normalize(vector v){
  double len;

  len = sqrt(v[X] * v[X] + v[Y] * v[Y] + v[Z] * v[Z]);
  v[X] /= len;
  v[Y] /= len;
  v[Z] /= len;
}

/*======== double get_diffuse() ==========
  Inputs:  double *light
	double *normal
	int cp
	double kd

	Returns: diffuse component of illumination

	Product of diffuse color, constant of diffuse reflection, 
	and dot product of light and normal vectors

	Light and normal vectors have been normalized
  
	====================*/
double get_diffuse(vector light, vector normal,
									 int cp, double kd){
  
  double val; 

  normalize(light);
  normalize(normal);
  val = cp * kd * dot_product(light, normal);

  return val > 0 ? val : 0;
}

/*======== double get_specular() ==========
  Inputs:  double *light
	double *normal
	int cp
	double ks
	
	Returns: specular component of illumination
	
	Product of specular color, constant of specular reflection, 
	and dot product of reflection and view vectors

	Light and normal vectors have been normalized
  
	====================*/
double get_specular(vector light, vector normal,
										int cp, double ks){

	vector res;
	vector view;
	double r;
	double val;

	view[X] = 0;
	view[Y] = 0;
	view[Z] = -1;

	r = 2 * dot_product(light, normal);
	res[X] = normal[X] * r - light[X];
	res[Y] = normal[Y] * r - light[Y];
	res[Z] = normal[Z] * r - light[Z];

	val = pow(dot_product(res, view), 3) * cp * ks;

	return val > 0 ? val : 0;
}

/*======== double get_illumination() ==========
  Inputs:  double *l
	double *n
	int cp
	double *constants
	
	Returns: illumination (0 - 255)
	
	Sum of ambient, diffuse, and specular components of illumination
  
	====================*/
int get_illumination(vector light, vector normal, int cp, int ca, double *constants){
	double ambient, diffuse, specular;
	int val;

	ambient = get_ambient(ca, constants[Ka]);
	diffuse = get_diffuse(light, normal, cp, constants[Kd]);
	specular = get_specular(light, normal, cp, constants[Ks]);

	val = ambient + diffuse + specular;
	return val <= 255 ? val : 255;
}
