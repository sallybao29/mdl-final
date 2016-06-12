#ifndef DRAW_H
#define DRAW_H

#include "matrix.h"
#include "gmath.h"

#define MAX_STEPS 100


void draw_line(int x0, int y0, double z0,
							 int x1, int y1, double z1,
							 screen s, color c, z_buff zb);

void draw_line_with_shading(int x0, int y0, double z0,
														int x1, int y1, double z1,
														screen s, z_buff zb,
														char* shading, union shade_info sh);

void fill( struct matrix *points, int i, screen s, z_buff zb,
					 struct light *light, struct constants *c, color amb,
					 char* shading, struct matrix *vertices, struct matrix *normals);

void get_vertex_normals(struct matrix *polygons, struct matrix * vertices, struct matrix *normals);

void add_vertex(struct matrix *vertices, struct matrix *normals, vector normal,
								double x, double y, double z);

int lookup_vertex(struct matrix *vertices, double vx, double vy, double vz);

void lookup_normal(struct matrix *vertices, struct matrix *normals, vector normal,
									 double x, double y, double z);

void add_point( struct matrix * points, 
								double x, double y, double z);
void add_edge( struct matrix * points, 
							 double x0, double y0, double z0, 
							 double x1, double y1, double z1);
void add_polygon( struct matrix * points, 
									double x0, double y0, double z0, 
									double x1, double y1, double z1,
									double x2, double y2, double z2);
void draw_lines( struct matrix * points, screen s, color c, z_buff zb);
void draw_polygons( struct matrix * points, screen s, z_buff zb,
										struct light *l, struct constants *c, color amb, char *shading);

//advanced shapes
void add_circle( struct matrix * points, 
								 double cx, double cy, 
								 double r, double step );
void add_curve( struct matrix *points, 
								double x0, double y0,
								double x1, double y1,
								double x2, double y2,
								double x3, double y3,
								double step, int type );
void add_box( struct matrix *points,
							double x, double y, double z,
							double w, double h, double d);
void add_sphere( struct matrix * points, 
								 double cx, double cy, double cz, double r, 
								 int step );
void generate_sphere( struct matrix * points, 
											double cx, double cy, double cz, double r, 
											int step );
void add_torus( struct matrix * points, 
								double cx, double cy, double cz, double r1, double r2, 
								int step );
void generate_torus( struct matrix * points, 
										 double cx, double cy, double cz, double r1, double r2, 
										 int step );
#endif
