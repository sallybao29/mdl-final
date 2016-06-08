#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "matrix.h"
#include "gmath.h"



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
double * calculate_normal( double ax, double ay, double az,	
													 double bx, double by, double bz ) {
  
  double *normal;
  normal = (double *)malloc(3 * sizeof(double));

  normal[0] = ay*bz - az*by;
  normal[1] = az*bx - ax*bz;
  normal[2] = ax*by - ay*bx;

  return normal;
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

  double ax, ay, az, bx, by, bz;
  double *normal;
  double vx, vy, vz;
  double dot;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  normal = calculate_normal( ax, ay, az, bx, by, bz );

  //set up view vector
  vx = 0;
  vy = 0;
  vz = -1;

  //calculate dot product
  dot = normal[0] * vx + normal[1] * vy + normal[2] * vz;

  free(normal);  
  return dot;
}

double dot_product(double* a, double* b){
  double dot;

  dot = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];

  return dot;
}

double distance(double x0, double y0, double x1, double y1){
	double a, b;

	a = pow((x1 - x0), 2);
	b = pow((y1 - y0), 2);
	return sqrt(a + b);
}


double get_ambient(int ca, double ka){
  return ca * ka;
}

void *normalize(double *v){
  double len;

  len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

double get_diffuse(double *light, double *normal,
		   int cp, double kd){
  
  double val; 

  normalize(light);
  normalize(normal);
  val = cp * kd * dot_product(light, normal);

  return val;
}

double get_specular(int cp, int ks){

}

int get_illumination(struct constants *c, struct light *l,){
  double ambient, diffuse, specular;
  double l[3], n[3];
  int val;

  ambient = get_ambient();
  diffuse = get_diffuse(l, n, , );
  specular = get_specular();

  val = ambient + diffuse + specular;
  return val <= 255 ? val : 255;
}
