#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ml6.h"
#include "display.h"
#include "symtab.h"
#include "matrix.h"
#include "gmath.h"
#include "draw.h"

/*======== void add_polygon() ==========
  Inputs:   struct matrix *surfaces
  double x0
  double y0
  double z0
  double x1
  double y1
  double z1
  double x2
  double y2
  double z2
  Returns:
  Adds the vertices (x0, y0, z0), (x1, y1, z1)
  and (x2, y2, z2) to the polygon matrix. They
  define a single triangle surface.

  04/16/13 13:05:59
  jdyrlandweaver
  ====================*/
void add_polygon( struct matrix *polygons,
									double x0, double y0, double z0,
									double x1, double y1, double z1,
									double x2, double y2, double z2 ) {
  add_point(polygons, x0, y0, z0);
  add_point(polygons, x1, y1, z1);
  add_point(polygons, x2, y2, z2);
}


/*======== void fill() ==========
  Inputs:   struct matrix *polygons
  int i
  screen s
  color c
  Returns:
  Fills in the polygon

  05/13/16 13:13:27

  ====================*/
void fill( struct matrix *points, int i, screen s, z_buff zb,
					 struct light *light, struct constants *c, color amb,
					 char* shading, struct matrix *vertices, struct matrix *normals){

	double p[3][3];
	double tmp;
	double d0, d1, d2;
	double x0, x1, y0, y1, z0, z1;
	double dz0, dz1, dz2;
	double dist0, dist1, dist2;
	int m, n, l;
	int fb = 0;
	int TOP = 0, MID = 1, BOT = 2;
	union shade_info sh;


	for (m = 0; m < 3; m++)
		for (n = 0; n < 3; n++)
			p[m][n] = points -> m[m][n + i];

	/*------------------------------ Horizontal Scanline ------------------------------------*/
	for (m = 0; m < 3; m++){
		for (n = m + 1; n < 3; n++){
			if (p[Y][m] < p[Y][n]){
				for (l = 0; l < 3; l++){
					tmp = p[l][m];
					p[l][m]= p[l][n];
					p[l][n] = tmp;
				}
			}
		}
	}

	y0 = p[Y][BOT];
	x0 = p[X][BOT];
	x1 = x0;
	z0 = p[Z][BOT];
	z1 = z0;

	dist0 = (p[Y][TOP] - p[Y][BOT]);
	dist1 = (p[Y][MID] - p[Y][BOT]);
	dist2 = (p[Y][TOP] - p[Y][MID]);

	d0 = (p[X][TOP] - p[X][BOT]) / dist0; //(yt-yb)/(xt-xb)
	d1 = (p[X][MID] - p[X][BOT]) / dist1; //(ym-yb)/(xm-xb)
	d2 = (p[X][TOP] - p[X][MID]) / dist2; //(yt-ym)/(xt-xm)

	dz0 = (p[Z][TOP] - p[Z][BOT]) / dist0;
	dz1 = (p[Z][MID] - p[Z][BOT]) / dist1;
	dz2 = (p[Z][TOP] - p[Z][MID]) / dist2;

	color ct, cm, cb;
	double c0r, c0g, c0b,
		c1r, c1g, c1b;
	double dc0r, dc0g, dc0b,
		dc1r, dc1g, dc1b,
		dc2r, dc2g, dc2b;
	vector lt, lm, lb, normal;

	if (strcmp(shading, "gouraud") == 0){

		//calculate light vectors from light source to each point
		lt[X] = p[X][TOP] - light -> c[X];
		lt[Y] = p[Y][TOP] - light -> c[Y];
		lt[Z] = p[Z][TOP] - light -> c[Z];

		lm[X] = p[X][MID] - light -> c[X];
		lm[Y] = p[Y][MID] - light -> c[Y];
		lm[Z] = p[Z][MID] - light -> c[Z];

		lb[X] = p[X][BOT] - light -> c[X];
		lb[Y] = p[Y][BOT] - light -> c[Y];
		lb[Z] = p[Z][BOT] - light -> c[Z];
		
		//get colors for top, middle, and bottom vertex
		lookup_normal(vertices, normals, normal, p[X][TOP], p[Y][TOP], p[Z][TOP]);
		ct = get_illumination(lt, normal, light -> l, amb, c);

		lookup_normal(vertices, normals, normal, p[X][MID], p[Y][MID], p[Z][MID]);
		cm = get_illumination(lm, normal, light -> l, amb, c);

		lookup_normal(vertices, normals, normal, p[X][BOT], p[Y][BOT], p[Z][BOT]);
		cb = get_illumination(lb, normal, light -> l, amb, c);

		//draw outline of polygon
		sh.gouraud.c0[0] = ct.red;
		sh.gouraud.c0[1] = ct.green;
		sh.gouraud.c0[2] = ct.blue;
		sh.gouraud.c1[0] = cm.red;
		sh.gouraud.c1[1] = cm.green;
		sh.gouraud.c1[2] = cm.blue;
		
		draw_line_with_shading(p[X][TOP], p[Y][TOP], p[Z][TOP],
		p[X][MID], p[Y][MID], p[Z][MID], s, zb, shading, sh);
		sh.gouraud.c1[0] = cb.red;
		sh.gouraud.c1[1] = cb.green;
		sh.gouraud.c1[2] = cb.blue;
		
		draw_line_with_shading(p[X][TOP], p[Y][TOP], p[Z][TOP],
		p[X][BOT], p[Y][BOT], p[Z][BOT], s, zb, shading, sh);
		sh.gouraud.c0[0] = cm.red;
		sh.gouraud.c0[1] = cm.green;
		sh.gouraud.c0[2] = cm.blue;
		
		draw_line_with_shading(p[X][MID], p[Y][MID], p[Z][MID],
		p[X][BOT], p[Y][BOT], p[Z][BOT], s, zb, shading, sh);
		
		//calculate change for each component of color for each vertex
		dc0r = (ct.red - cb.red) / dist0;
		dc0g = (ct.green - cb.green) / dist0;
		dc0b = (ct.blue - cb.blue) /dist0;

		//if bottom y and middle y are the same
		if (dist1 == 0){
			dc1r = (cm.red - cb.red) / (fabs(p[X][MID] - p[X][BOT]));
			dc1g = (cm.green - cb.green) / (fabs(p[X][MID] - p[X][BOT]));
			dc1b = (cm.blue - cb.blue) / (fabs(p[X][MID] - p[X][BOT]));
		}
		else {
			dc1r = (cm.red - cb.red) / dist1;
			dc1g = (cm.green - cb.green) /dist1;
			dc1b = (cm.blue - cb.blue) / dist1;
		}

		//if top y and middle y are the same
		if (dist2 == 0){
			dc2r = (ct.red - cm.red) / (fabs(p[X][TOP] - p[X][MID]));
			dc2g = (ct.green - cm.green) / (fabs(p[X][TOP] - p[X][MID]));
			dc2b = (ct.blue - cm.blue) / (fabs(p[X][TOP] - p[X][MID]));
		}
		else {
			dc2r = (ct.red - cm.red) / dist2;
			dc2g = (ct.green - cm.green) / dist2;
			dc2b = (ct.blue - cm.blue) / dist2;
		}

		//set initial color values
		c0r = cb.red;
		c0g = cb.green;
		c0b = cb.green;

		c1r = c0r;
		c1g = c0g;
		c1b = c0b;

		//initialize union
		sh.gouraud.c0[0] = c0r;
		sh.gouraud.c0[1] = c0g;
		sh.gouraud.c0[2] = c0b;
  
		sh.gouraud.c1[0] = c1r;
		sh.gouraud.c1[1] = c1g;
		sh.gouraud.c1[2] = c1b;
	}

	vector nt, nm, nb, n0, n1;
	vector dn0, dn1, dn2;
	
	if (strcmp(shading, "phong") == 0){
		lookup_normal(vertices, normals, nt, p[X][TOP], p[Y][TOP], p[Z][TOP]);
		lookup_normal(vertices, normals, nm, p[X][MID], p[Y][MID], p[Z][MID]);
		lookup_normal(vertices, normals, nb, p[X][BOT], p[Y][BOT], p[Z][BOT]);
		/*
		//draw outline of polygon
		sh.phong.n0[X] = nt[X]; 
		sh.phong.n0[Y] = nt[Y]; 
		sh.phong.n0[Z] = nt[Z]; 
		sh.phong.n1[X] = nm[X]; 
		sh.phong.n1[Y] = nm[Y]; 
		sh.phong.n1[Z] = nm[Z]; 

		draw_line_with_shading(p[X][TOP], p[Y][TOP], p[Z][TOP],
				       p[X][MID], p[Y][MID], p[Z][MID], s, zb, shading, sh);
	
		sh.phong.n1[X] = nb[X]; 
		sh.phong.n1[Y] = nb[Y]; 
		sh.phong.n1[Z] = nb[Z]; 

		draw_line_with_shading(p[X][TOP], p[Y][TOP], p[Z][TOP],
		p[X][BOT], p[Y][BOT], p[Z][BOT], s, zb, shading, sh);
		sh.phong.n0[X] = nm[X]; 
		sh.phong.n0[Y] = nm[Y]; 
		sh.phong.n0[Z] = nm[Z];

		draw_line_with_shading(p[X][MID], p[Y][MID], p[Z][MID],
		p[X][BOT], p[Y][BOT], p[Z][BOT], s, zb, shading, sh);
		*/
		dn0[X] = (nt[X] - nb[X]) / dist0;
		dn0[Y] = (nt[Y] - nb[Y]) / dist0;
		dn0[Z] = (nt[Z] - nb[Z]) / dist0;
	
		//if bottom y and middle y are the same
		if (dist1 == 0){
			dn1[X] = (nm[X] - nb[X]) / (fabs(p[X][MID] - p[X][BOT]));
			dn1[Y] = (nm[Y] - nb[Y]) / (fabs(p[X][MID] - p[X][BOT]));
			dn1[Z] = (nm[Z] - nb[Z]) / (fabs(p[X][MID] - p[X][BOT]));
		}
		else{
			dn1[X] = (nm[X] - nb[X]) / dist1;
			dn1[Y] = (nm[Y] - nb[Y]) / dist1;
			dn1[Z] = (nm[Z] - nb[Z]) / dist1;
		}
		
		//if top y and middle y are the same
		if (dist2 == 0){
			dn2[X] = (nt[X] - nm[X]) / (fabs(p[X][TOP] - p[X][MID]));
			dn2[Y] = (nt[Y] - nm[Y]) / (fabs(p[X][TOP] - p[X][MID]));
			dn2[Z] = (nt[Z] - nm[Z]) / (fabs(p[X][TOP] - p[X][MID]));
		}
		else {
			dn2[X] = (nt[X] - nm[X]) / dist2;
			dn2[Y] = (nt[Y] - nm[Y]) / dist2;
			dn2[Z] = (nt[Z] - nm[Z]) / dist2;
		}

		//set initial vertex normal values
		n0[X] = nb[X];
		n0[Y] = nb[Y];
		n0[Z] = nb[Z];
		
		n1[X] = n0[X];
		n1[Y] = n0[Y];
		n1[Z] = n0[Z];

		//initialize union
		sh.phong.n0[X] = n0[X];
		sh.phong.n0[Y] = n0[Y];
		sh.phong.n0[Z] = n0[Z];
	
		sh.phong.n1[X] = n1[X];
		sh.phong.n1[Y] = n1[Y];
		sh.phong.n1[Z] = n1[Z];

		sh.phong.l = light;
		sh.phong.c = c;
		sh.phong.amb = amb;
	}

	while (y0 <= (int)(p[Y][TOP])){
		/*
			if (y0 == (int)p[Y][TOP] &&
			p[Y][TOP] == p[Y][MID]){
			
			x0 = p[Y][TOP];
			x1 = p[Y][MID];
			y0 = p[Y][TOP];
			z0 = p[Z][TOP];
			z1 = p[Z][MID];

			if (strcmp(shading, "gouraud") == 0){
			c0r = ct.red;
			c0g = ct.green;
			c0b = ct.blue;
				
			c1r = cm.red;
			c1g = cm.green;
			c1b = cm.blue;

			sh.gouraud.c0[0] = c0r;
			sh.gouraud.c0[1] = c0g;
			sh.gouraud.c0[2] = c0b;
  
			sh.gouraud.c1[0] = c1r;
			sh.gouraud.c1[1] = c1g;
			sh.gouraud.c1[2] = c1b;
			}

			if (strcmp(shading, "phong") == 0){
			n0[X] = nt[X];
			n0[Y] = nt[Y];
			n0[Z] = nt[Z];

			n1[X] = nm[X];
			n1[Y] = nm[Y];
			n1[Z] = nm[Z];

			sh.phong.n0[X] = n0[X];
			sh.phong.n0[Y] = n0[Y];
			sh.phong.n0[Z] = n0[Z];
	
			sh.phong.n1[X] = n1[X];
			sh.phong.n1[Y] = n1[Y];
			sh.phong.n1[Z] = n1[Z];
			}
			}*/
		if (strcmp(shading, "gouraud") == 0 ||
				strcmp(shading, "phong") == 0)
			
			draw_line_with_shading(x0, y0, z0, x1, y0, z1, s, zb, shading, sh);
		
		else
			draw_line(x0, y0, z0, x1, y0, z1, s, amb, zb);

		y0 += 1;
		x0 += d0;
		x1 += d1;
		z0 += dz0;
		z1 += dz1;

		if (strcmp(shading, "gouraud") == 0){
			c0r += dc0r;
			c0g += dc0g;
			c0b += dc0b;

			c1r += dc1r;
			c1g += dc1g;
			c1b += dc1b;
		
			sh.gouraud.c0[0] = c0r;
			sh.gouraud.c0[1] = c0g;
			sh.gouraud.c0[2] = c0b;
  
			sh.gouraud.c1[0] = c1r;
			sh.gouraud.c1[1] = c1g;
			sh.gouraud.c1[2] = c1b;
		}

		if (strcmp(shading, "phong") == 0){
			n0[X] += dn0[X];
			n0[Y] += dn0[Y];
			n0[Z] += dn0[Z];

			n1[X] += dn1[X];
			n1[Y] += dn1[Y];
			n1[Z] += dn1[Z];

			sh.phong.n0[X] = n0[X];
			sh.phong.n0[Y] = n0[Y];
			sh.phong.n0[Z] = n0[Z];
	
			sh.phong.n1[X] = n1[X];
			sh.phong.n1[Y] = n1[Y];
			sh.phong.n1[Z] = n1[Z];
		}

		if (y0 >= (int)p[Y][MID] && !fb){
			d1 = d2;
			dz1 = dz2;
			x1 = p[X][MID];
			z1 = p[Z][MID];
			fb++;

			if (strcmp(shading, "gouraud") == 0){
				c1r = cm.red;
				c1g = cm.green;
				c1b = cm.blue;

				dc1r = dc2r;
				dc1g = dc2g;
				dc1b = dc2b;
			}

			if (strcmp(shading, "phong") == 0){
				n1[X] = nm[X];
				n1[Y] = nm[Y];
				n1[Z] = nm[Z];

				dn1[X] = dn2[X];
				dn1[Y] = dn2[Y];
				dn1[Z] = dn2[Z];
			}
		}
	}

	/*------------------------------ Vertical Scanline ------------------------------------*//*
		fb = 0;

		for (m = 0; m < 3; m++){
		for (n = m + 1; n < 3; n++){
		if (p[0][m] < p[0][n]){
		for (l = 0; l < 3; l++){
		tmp = p[l][m];
		p[l][m]= p[l][n];
		p[l][n] = tmp;
		}
		}
		}
		}

		y0 = p[1][BOT];
		y1 = y0;
		x0 = p[0][BOT];
		z0 = p[2][BOT];
		z1 = z0;

		d0 = (p[1][TOP] - p[1][BOT]) / (p[0][TOP] - p[0][BOT]); //(yt-yb)/(xt-xb)
		d1 = (p[1][MID] - p[1][BOT]) / (p[0][MID] - p[0][BOT]); //(ym-yb)/(xm-xb)
		d2 = (p[1][TOP] - p[1][MID]) / (p[0][TOP] - p[0][MID]); //(yt-ym)/(xt-xm)

		//delta z / distance top to bottom
		dz0 = (p[2][TOP] - p[2][BOT]) / distance(p[0][TOP], p[1][TOP], p[0][BOT], p[1][BOT]);

		//delta z / distance middle to bottom
		dz1 = (p[2][MID] - p[2][BOT]) / distance(p[0][MID], p[1][MID], p[0][BOT], p[1][BOT]);

		//delta z / distance top to middle
		dz2 = (p[2][TOP] - p[2][MID]) / distance(p[0][TOP], p[1][TOP], p[0][MID], p[1][MID]);

		while ((int)x0 < (int)(p[0][TOP])){

		//printf("Drawing (%lf, %lf) to (%lf, %lf)\n", x0, y0, x0, y1);
		draw_line(x0, y0, z0, x0, y1, z1, s, c, zb);

		x0 += 1;
		y0 += d0;
		y1 += d1;
		z0 += dz0;
		z1 += dz1;

		if (x0 >= (int)p[0][MID] && !fb){
		d1 = d2;
		dz1 = dz2;
		y1 = p[1][MID];
		z1 = p[2][MID];
		fb++;
		}
		}
																																													 */
}

void lookup_normal(struct matrix *vertices, struct matrix *normals, vector normal,
									 double x, double y, double z){
	int index;

	index = lookup_vertex(vertices, x, y, z);

	normal[X] = normals -> m[X][index];
	normal[Y] = normals -> m[Y][index];
	normal[Z] = normals -> m[Z][index];
}

int lookup_vertex(struct matrix *vertices, double vx, double vy, double vz){

	int i;

	for (i = 0; i <= vertices -> lastcol; i++){

		if (vertices -> m[X][i] == vx &&
				vertices -> m[Y][i] == vy &&
				vertices -> m[Z][i] == vz){
			return i;
		}
	} 
	return -1;
}


void add_vertex(struct matrix *vertices, struct matrix *normals, vector normal,
								double x, double y, double z){
	int index;

	index = lookup_vertex(vertices, x, y, z);

	//add vertex
	if (index == -1){

		add_point(vertices, x, y, z);
		add_point(normals, normal[X], normal[Y], normal[Z]);
	}

	//if vertex in storage, add surface to vertex normal
	else {
		normals -> m[X][index] += normal[X];
		normals -> m[Y][index] += normal[Y];
		normals -> m[Z][index] += normal[Z];
	}
}


void get_vertex_normals(struct matrix *polygons, struct matrix * vertices, struct matrix *normals){

	vector normal;
	int i;
	double len;

	for( i=0; i < polygons->lastcol-2; i+=3 ) {
		//if ( calculate_dot( polygons, i ) < 0 ) {

		//calculate surface normal for polygon
		calculate_surface_normal(polygons, i, normal);

		add_vertex(vertices, normals, normal, polygons -> m[X][i],
							 polygons -> m[Y][i], polygons -> m[Z][i]);
		add_vertex(vertices, normals, normal, polygons -> m[X][i+1],
							 polygons -> m[Y][i+1], polygons -> m[Z][i+1]);

		add_vertex(vertices, normals, normal, polygons -> m[X][i+2],
							 polygons -> m[Y][i+2], polygons -> m[Z][i+2]);
		//}
	}
	for (i = 0; i < normals -> lastcol; i++){
		len = sqrt(normals -> m[X][i] * normals -> m[X][i] +
							 normals -> m[Y][i] * normals -> m[Y][i] +
							 normals -> m[Z][i] * normals -> m[Z][i]);

		normals -> m[X][i] /= len;
		normals -> m[Y][i] /= len;
		normals -> m[Z][i] /= len;

		//printf("normalized <%lf, %lf, %lf> \n",
		//normals -> m[X][i], normals -> m[X][i], normals -> m[X][i]);
	}
}

/*======== void draw_polygons() ==========
	Inputs:   struct matrix *polygons
	screen s
	color c
	Returns:
	Goes through polygons 3 points at a time, drawing
	lines connecting each points to create bounding
	triangles

	04/16/13 13:13:27
	jdyrlandweaver
	====================*/
void draw_polygons( struct matrix *polygons, screen s, z_buff zb,
										struct light *l, struct constants *c, color amb, char *shading){


	int i;
	color col;
	vector normal;
	vector light;
	double mid[3];
	struct matrix *vertices, *normals;

	col = amb;

	if (l == NULL){
		printf("No light source specified\n");
		exit(0);
	}

	if (c == NULL){
		printf("No constants specified\n");
		exit(0);
	}

	if (shading == NULL){
		printf("No shading method specified\n");
		exit(0);
	}

	if (strcmp(shading, "gouraud") == 0 ||
			strcmp(shading, "phong") == 0){

		vertices = new_matrix(4, 100);
		normals = new_matrix(4, 100);
		get_vertex_normals(polygons, vertices, normals);

		//printf("Vertices: %d\n", vertices -> lastcol);
		//printf("Normals: %d\n", normals -> lastcol);
		
	}

	for( i=0; i < polygons->lastcol-2; i+=3 ) {

		if ( calculate_dot( polygons, i ) < 0 ) {

			if (strcmp(shading, "flat") == 0){

				calculate_surface_normal(polygons, i, normal);

				mid[X] = (polygons->m[X][i]+polygons->m[X][i+1]+polygons->m[X][i+2]) / 3;
				mid[Y] = (polygons->m[Y][i]+polygons->m[Y][i+1]+polygons->m[Y][i+2]) / 3;
				mid[Z] = (polygons->m[Z][i]+polygons->m[Z][i+1]+polygons->m[Z][i+2]) / 3;

				light[X] = mid[X] - l -> c[X];
					light[Y] = mid[Y] - l -> c[Y];
					light[Z] = mid[Z] - l -> c[Z];

					col = get_illumination(light, normal, l -> l, amb, c);

					draw_line( polygons->m[0][i],
										 polygons->m[1][i],
										 polygons->m[2][i],
										 polygons->m[0][i+1],
										 polygons->m[1][i+1],
										 polygons->m[2][i+1],
										 s, col, zb);
					draw_line( polygons->m[0][i+1],
										 polygons->m[1][i+1],
										 polygons->m[2][i+1],
										 polygons->m[0][i+2],
										 polygons->m[1][i+2],
										 polygons->m[2][i+2],
										 s, col, zb);
					draw_line( polygons->m[0][i+2],
										 polygons->m[1][i+2],
										 polygons->m[2][i+2],
										 polygons->m[0][i],
										 polygons->m[1][i],
										 polygons->m[2][i],
										 s, col, zb); 
				}

				if (strcmp(shading, "wireframe") == 0){
					draw_line( polygons->m[0][i],
										 polygons->m[1][i],
										 polygons->m[2][i],
										 polygons->m[0][i+1],
										 polygons->m[1][i+1],
										 polygons->m[2][i+1],
										 s, col, zb);
					draw_line( polygons->m[0][i+1],
										 polygons->m[1][i+1],
										 polygons->m[2][i+1],
										 polygons->m[0][i+2],
										 polygons->m[1][i+2],
										 polygons->m[2][i+2],
										 s, col, zb);
					draw_line( polygons->m[0][i+2],
										 polygons->m[1][i+2],
										 polygons->m[2][i+2],
										 polygons->m[0][i],
										 polygons->m[1][i],
										 polygons->m[2][i],
										 s, col, zb); 
				}
			
				else
					fill(polygons, i, s, zb, l, c, col, shading, vertices, normals);
			}
		}
		if (strcmp(shading, "gouraud") == 0 ||
				strcmp(shading, "phong") == 0){
			free_matrix(vertices);
			free_matrix(normals);
		}
	}

	/*======== void add_sphere() ==========
		Inputs:   struct matrix * points
		double cx
		double cy
		double r
		double step
		Returns:

		adds all the points for a sphere with center
		(cx, cy) and radius r.

		should call generate_sphere to create the
		necessary points

		jdyrlandweaver
		====================*/
	void add_sphere( struct matrix * points,
									 double cx, double cy, double cz, double r,
									 int step ) {

		struct matrix * temp;
		int lat, longt;
		int index;
		int num_steps, num_points;
		double px0, px1, px2, px3;
		double py0, py1, py2, py3;
		double pz0, pz1, pz2, pz3;

		num_steps = MAX_STEPS / step;
		num_points = num_steps * (num_steps + 1);

		temp = new_matrix( 4, num_points);
		//generate the points on the sphere
		generate_sphere( temp, cx, cy, cz, r, step );

		int latStop, longStop, latStart, longStart;
		latStart = 0;
		latStop = num_steps;
		longStart = 0;
		longStop = num_steps;

		num_steps++;

		for ( lat = latStart; lat < latStop; lat++ ) {
			for ( longt = longStart; longt < longStop; longt++ ) {

				index = lat * num_steps + longt;

				px0 = temp->m[0][ index ];
				py0 = temp->m[1][ index ];
				pz0 = temp->m[2][ index ];

				px1 = temp->m[0][ (index + num_steps) % num_points ];
				py1 = temp->m[1][ (index + num_steps) % num_points ];
				pz1 = temp->m[2][ (index + num_steps) % num_points ];

				px3 = temp->m[0][ index + 1 ];
				py3 = temp->m[1][ index + 1 ];
				pz3 = temp->m[2][ index + 1 ];

				if (longt != longStop - 1) {
					px2 = temp->m[0][ (index + num_steps + 1) % num_points ];
					py2 = temp->m[1][ (index + num_steps + 1) % num_points ];
					pz2 = temp->m[2][ (index + num_steps + 1) % num_points ];
				}
				else {
					px2 = temp->m[0][ (index + 1) % num_points ];
					py2 = temp->m[1][ (index + 1) % num_points ];
					pz2 = temp->m[2][ (index + 1) % num_points ];
				}

				if (longt != 0)
					add_polygon( points, px0, py0, pz0, px1, py1, pz1, px2, py2, pz2 );
				if (longt != longStop - 1)
					add_polygon( points, px2, py2, pz2, px3, py3, pz3, px0, py0, pz0 );
			}
		}
	}

	/*======== void generate_sphere() ==========
		Inputs:   struct matrix * points
		double cx
		double cy
		double r
		double step
		Returns:

		Generates all the points along the surface of a
		sphere with center (cx, cy) and radius r

		Adds these points to the matrix parameter

		03/22/12 11:30:26
		jdyrlandweaver
		====================*/
	void generate_sphere( struct matrix * points,
												double cx, double cy, double cz, double r,
												int step ) {


		int circle, rotation;
		double x, y, z, circ, rot;

		int rotStart = step * 0;
		int rotStop = MAX_STEPS;
		int circStart = step * 0;
		int circStop = MAX_STEPS;

		for ( rotation = rotStart; rotation < rotStop; rotation += step ) {
			rot = (double)rotation / MAX_STEPS;
			for ( circle = circStart; circle <= circStop; circle+= step ) {

				circ = (double)circle / MAX_STEPS;
				x = r * cos( M_PI * circ ) + cx;
				y = r * sin( M_PI * circ ) *
					cos( 2 * M_PI * rot ) + cy;
				z = r * sin( M_PI * circ ) *
					sin( 2 * M_PI * rot ) + cz;

				add_point( points, x, y, z);
			}
		}
	}


	/*======== void add_torus() ==========
		Inputs:   struct matrix * points
		double cx
		double cy
		double r1
		double r2
		double step
		Returns:

		adds all the points required to make a torus
		with center (cx, cy) and radii r1 and r2.

		should call generate_torus to create the
		necessary points

		03/22/12 13:34:03
		jdyrlandweaver
		====================*/
	void add_torus( struct matrix * points,
									double cx, double cy, double cz, double r1, double r2,
									int step ) {

		struct matrix * temp;
		int lat, longt;
		int index;
		int num_steps;

		num_steps = MAX_STEPS / step;

		temp = new_matrix( 4, num_steps * num_steps );
		//generate the points on the torus
		generate_torus( temp, cx, cy, cz, r1, r2, step );
		int num_points = temp->lastcol;

		int latStop, longtStop, latStart, longStart;
		latStart = 0;
		longStart = 0;
		latStop = num_steps;
		longtStop = num_steps;
		for ( lat = latStart; lat < latStop; lat++ )
			for ( longt = longStart; longt < longtStop; longt++ ) {

				index = lat * num_steps + longt;

				if ( longt != num_steps-1) {
					add_polygon( points, temp->m[0][index],
											 temp->m[1][index],
											 temp->m[2][index],
											 temp->m[0][(index+num_steps+1) % num_points],
											 temp->m[1][(index+num_steps+1) % num_points],
											 temp->m[2][(index+num_steps+1) % num_points],
											 temp->m[0][index+1],
											 temp->m[1][index+1],
											 temp->m[2][index+1] );
					add_polygon( points, temp->m[0][index],
											 temp->m[1][index],
											 temp->m[2][index],
											 temp->m[0][(index+num_steps) % num_points],
											 temp->m[1][(index+num_steps) % num_points],
											 temp->m[2][(index+num_steps) % num_points],
											 temp->m[0][(index+num_steps) % num_points + 1],
											 temp->m[1][(index+num_steps) % num_points + 1],
											 temp->m[2][(index+num_steps) % num_points + 1]);
				}
				else {
					add_polygon( points, temp->m[0][index],
											 temp->m[1][index],
											 temp->m[2][index],
											 temp->m[0][(index+1) % num_points],
											 temp->m[1][(index+1) % num_points],
											 temp->m[2][(index+1) % num_points],
											 temp->m[0][index+1-num_steps],
											 temp->m[1][index+1-num_steps],
											 temp->m[2][index+1-num_steps] );
					add_polygon( points, temp->m[0][index],
											 temp->m[1][index],
											 temp->m[2][index],
											 temp->m[0][(index+num_steps) % num_points],
											 temp->m[1][(index+num_steps) % num_points],
											 temp->m[2][(index+num_steps) % num_points],
											 temp->m[0][(index+1) % num_points],
											 temp->m[1][(index+1) % num_points],
											 temp->m[2][(index+1) % num_points]);
				}

			}
	}

	/*======== void generate_torus() ==========
		Inputs:   struct matrix * points
		double cx
		double cy
		double r
		double step
		Returns:

		Generates all the points along the surface of a
		tarus with center (cx, cy) and radii r1 and r2

		Adds these points to the matrix parameter

		03/22/12 11:30:26
		jdyrlandweaver
		====================*/
	void generate_torus( struct matrix * points,
											 double cx, double cy, double cz, double r1, double r2,
											 int step ) {

		double x, y, z, circ, rot;
		int circle, rotation;

		double rotStart = step * 0;
		double rotStop = MAX_STEPS;
		double circStart = step * 0;
		double circStop = MAX_STEPS;

		for ( rotation = rotStart; rotation < rotStop; rotation += step ) {

			rot = (double)rotation / MAX_STEPS;
			for ( circle = circStart; circle < circStop; circle+= step ) {

				circ = (double)circle / MAX_STEPS;
				x = cos( 2 * M_PI * rot ) *
					( r1 * cos( 2 * M_PI * circ ) + r2 ) + cx;
				y = r1 * sin( 2 * M_PI * circ ) + cy;
				z = sin( 2 * M_PI * rot ) *
					( r1 * cos( 2 * M_PI * circ ) + r2 ) + cz;

				add_point( points, x, y, z );
			}
		}
	}

	/*======== void add_box() ==========
		Inputs:   struct matrix * points
		double x
		double y
		double z
		double width
		double height
		double depth
		Returns:

		add the points for a rectagular prism whose
		upper-left corner is (x, y, z) with width,
		height and depth dimensions.

		jdyrlandweaver
		====================*/
	void add_box( struct matrix * polygons,
								double x, double y, double z,
								double width, double height, double depth ) {

		double x2, y2, z2;
		x2 = x + width;
		y2 = y - height;
		z2 = z - depth;
		//front
		add_polygon( polygons,
								 x, y, z,
								 x, y2, z,
								 x2, y2, z);
		add_polygon( polygons,
								 x2, y2, z,
								 x2, y, z,
								 x, y, z);
		//back
		add_polygon( polygons,
								 x2, y, z2,
								 x2, y2, z2,
								 x, y2, z2);
		add_polygon( polygons,
								 x, y2, z2,
								 x, y, z2,
								 x2, y, z2);
		//top
		add_polygon( polygons,
								 x, y, z2,
								 x, y, z,
								 x2, y, z);
		add_polygon( polygons,
								 x2, y, z,
								 x2, y, z2,
								 x, y, z2);
		//bottom
		add_polygon( polygons,
								 x2, y2, z2,
								 x2, y2, z,
								 x, y2, z);
		add_polygon( polygons,
								 x, y2, z,
								 x, y2, z2,
								 x2, y2, z2);
		//right side
		add_polygon( polygons,
								 x2, y, z,
								 x2, y2, z,
								 x2, y2, z2);
		add_polygon( polygons,
								 x2, y2, z2,
								 x2, y, z2,
								 x2, y, z);
		//left side
		add_polygon( polygons,
								 x, y, z2,
								 x, y2, z2,
								 x, y2, z);
		add_polygon( polygons,
								 x, y2, z,
								 x, y, z,
								 x, y, z2);
	}

	/*======== void add_circle() ==========
		Inputs:   struct matrix * points
		double cx
		double cy
		double y
		double step
		Returns:


		03/16/12 19:53:52
		jdyrlandweaver
		====================*/
	void add_circle( struct matrix * points,
									 double cx, double cy,
									 double r, double step ) {

		double x0, y0, x, y, t;

		x0 = cx + r;
		y0 = cy;

		for ( t = step; t <= 1; t+= step ) {

			x = r * cos( 2 * M_PI * t ) + cx;
			y = r * sin( 2 * M_PI * t ) + cy;

			add_edge( points, x0, y0, 0, x, y, 0 );
			x0 = x;
			y0 = y;
		}

		add_edge( points, x0, y0, 0, cx + r, cy, 0 );
	}

	/*======== void add_curve() ==========
		Inputs:   struct matrix *points
		double x0
		double y0
		double x1
		double y1
		double x2
		double y2
		double x3
		double y3
		double step
		int type
		Returns:

		Adds the curve bounded by the 4 points passsed as parameters
		of type specified in type (see matrix.h for curve type constants)
		to the matrix points

		03/16/12 15:24:25
		jdyrlandweaver
		====================*/
	void add_curve( struct matrix *points,
									double x0, double y0,
									double x1, double y1,
									double x2, double y2,
									double x3, double y3,
									double step, int type ) {

		double x, y, t;
		struct matrix * xcoefs;
		struct matrix * ycoefs;

		//generate the coeficients
		if ( type == BEZIER_MODE ) {
			ycoefs = generate_curve_coefs(y0, y1, y2, y3, BEZIER_MODE);
			xcoefs = generate_curve_coefs(x0, x1, x2, x3, BEZIER_MODE);
		}

		else {
			xcoefs = generate_curve_coefs(x0, x1, x2, x3, HERMITE_MODE);
			ycoefs = generate_curve_coefs(y0, y1, y2, y3, HERMITE_MODE);
		}

		/*
			printf("a = %lf b = %lf c = %lf d = %lf\n", xcoefs->m[0][0],
			xcoefs->m[1][0], xcoefs->m[2][0], xcoefs->m[3][0]);
		*/

		for (t=step; t <= 1; t+= step) {

			x = xcoefs->m[0][0] * t * t * t + xcoefs->m[1][0] * t * t
				+ xcoefs->m[2][0] * t + xcoefs->m[3][0];

			y = ycoefs->m[0][0] * t * t * t + ycoefs->m[1][0] * t * t
				+ ycoefs->m[2][0] * t + ycoefs->m[3][0];

			add_edge(points, x0, y0, 0, x, y, 0);
			x0 = x;
			y0 = y;
		}

		free_matrix(xcoefs);
		free_matrix(ycoefs);
	}

	/*======== void add_point() ==========
		Inputs:   struct matrix * points
		int x
		int y
		int z
		Returns:
		adds point (x, y, z) to points and increment points.lastcol
		if points is full, should call grow on points
		====================*/
	void add_point( struct matrix * points, double x, double y, double z) {

		if ( points->lastcol == points->cols )
			grow_matrix( points, points->lastcol + 100 );

		points->m[0][points->lastcol] = x;
		points->m[1][points->lastcol] = y;
		points->m[2][points->lastcol] = z;
		points->m[3][points->lastcol] = 1;

		points->lastcol++;
	}

	/*======== void add_edge() ==========
		Inputs:   struct matrix * points
		int x0, int y0, int z0, int x1, int y1, int z1
		Returns:
		add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
		should use add_point
		====================*/
	void add_edge( struct matrix * points,
								 double x0, double y0, double z0,
								 double x1, double y1, double z1) {
		add_point( points, x0, y0, z0 );
		add_point( points, x1, y1, z1 );
	}

	/*======== void draw_lines() ==========
		Inputs:   struct matrix * points
		screen s
		color c
		Returns:
		Go through points 2 at a time and call draw_line to add that line
		to the screen
		====================*/
	void draw_lines( struct matrix * points, screen s, color c, z_buff zb) {

		int i;

		if ( points->lastcol < 2 ) {

			printf("Need at least 2 points to draw a line!\n");
			return;
		}

		for ( i = 0; i < points->lastcol - 1; i+=2 ) {

			draw_line( points->m[0][i], points->m[1][i], points->m[2][i],
								 points->m[0][i+1], points->m[1][i+1], points->m[2][i+1],
								 s, c, zb);
		}
	}


	void draw_line(int x0, int y0, double z0,
								 int x1, int y1, double z1,
								 screen s, color c, z_buff zb) {

		//printf("Drawing the line\n");
		int x, y, z, d, dx, dy;
		double dz;
 
		x = x0;
		y = y0;
		z = z0;

		//swap points so we're always drawing left to right
		if ( x0 > x1 ) {
			x = x1;
			y = y1;
			z = z1;
			x1 = x0;
			y1 = y0;
			z1 = z0;
		}

		//need to know dx and dy for this version
		dx = (x1 - x) * 2;
		dy = (y1 - y) * 2;
		//dz = (z1 - z) / distance(x, y, x1, y1);

		//positive slope: Octants 1, 2 (5 and 6)
		if ( dy > 0 ) {

			//slope < 1: Octant 1 (5)
			if ( dx > dy ) {
			
				d = dy - ( dx / 2 );
				dz = (z1 - z) / (x1 - x);
			
				while ( x <= x1 ) {
			
					plot(s, c, zb, x, y, z);

					if ( d < 0 ) {
						x = x + 1;
						d = d + dy;
					}
					else {
						x = x + 1;
						y = y + 1;
						d = d + dy - dx;
					}
					z = z + dz;
				}
			}

			//slope > 1: Octant 2 (6)
			else {
			
				d = ( dy / 2 ) - dx;
				dz = (z1 - z) / (y1 - y);
			
				while ( y <= y1 ) {
			 
					plot(s, c, zb, x, y, z);
					if ( d > 0 ) {
						y = y + 1;
						d = d - dx;
					}
					else {
						y = y + 1;
						x = x + 1;
						d = d + dy - dx;
					}
					z = z + dz;
				}
			}
		}

		//negative slope: Octants 7, 8 (3 and 4)
		else {

			//slope > -1: Octant 8 (4)
			if ( dx > abs(dy) ) {

				d = dy + ( dx / 2 );
				dz = (z1 - z) / (x1 - x);

				while ( x <= x1 ) {
				
					plot(s, c, zb, x, y, z);

					if ( d > 0 ) {
						x = x + 1;
						d = d + dy;
					}
					else {
						x = x + 1;
						y = y - 1;
						d = d + dy + dx;
					}
					z = z + dz;
				}
			}

			//slope < -1: Octant 7 (3)
			else {

				d =  (dy / 2) + dx;
				dz = (z1 - z) / (y - y1);

				while ( y >= y1 ) {
				
					plot(s, c, zb, x, y, z);
					if ( d < 0 ) {
						y = y - 1;
						d = d + dx;
					}
					else {
						y = y - 1;
						x = x + 1;
						d = d + dy + dx;
					}
					z = z + dz;

				}
			}
		}
	}


	void draw_line_with_shading(int x0, int y0, double z0,
															int x1, int y1, double z1,
															screen s, z_buff zb,
															char* shading, union shade_info sh){


		int x, y, z, d, dx, dy;
		double dz;

		double *c0, *c1;
		double dcr, dcg, dcb;

		double *n0, *n1;
		vector dn;

		struct light *l;
		struct constants *cons;
		vector light;
		color c, amb;
	
		x = x0;
		y = y0;
		z = z0;

		if (strcmp(shading, "gouraud") == 0){
			c0 = sh.gouraud.c0;
			c1 = sh.gouraud.c1;
		}

		if (strcmp(shading, "phong") == 0){
			n0 = sh.phong.n0;
			n1 = sh.phong.n1;

			l = sh.phong.l;
			cons = sh.phong.c;
			amb = sh.phong.amb;

		}

		//swap points so we're always drawing left to right
		if ( x0 > x1 ) {
			x = x1;
			y = y1;
			z = z1;
			x1 = x0;
			y1 = y0;
			z1 = z0;

			if (strcmp(shading, "gouraud") == 0){
				c0 = sh.gouraud.c1;
				c1 = sh.gouraud.c0;
			}

			if (strcmp(shading, "phong") == 0){
				n0 = sh.phong.n1;
				n1 = sh.phong.n0;
			}
		}

		//need to know dx and dy for this version
		dx = (x1 - x) * 2;
		dy = (y1 - y) * 2;
	
		//positive slope: Octants 1, 2 (5 and 6)
		if ( dy > 0 ) {

			//slope < 1: Octant 1 (5)
			if ( dx > dy ) {
				d = dy - ( dx / 2 );

				dz = (z1 - z) / (x1 - x);
			
				if (strcmp(shading, "gouraud") == 0){
	
					dcr = (c1[0] - c0[0]) / (x1 - x);
					dcg = (c1[1] - c0[1]) / (x1 - x);
					dcb = (c1[2] - c0[2]) / (x1 - x);
				}

				if (strcmp(shading, "phong") == 0){
			
					dn[X] = (n1[X] - n0[X]) / (x1 - x);
					dn[Y] = (n1[Y] - n0[Y]) / (x1 - x);
					dn[Z] = (n1[Z] - n0[Z]) / (x1 - x);
				}

				while ( x <= x1 ) {
				
					if (strcmp(shading, "gouraud") == 0){
						c.red = c0[0];
						c.green = c0[1];
						c.blue = c0[2];
					}

					if (strcmp(shading, "phong") == 0){
					
						light[X] = x - l -> c[X];
						light[Y] = y - l -> c[Y];
						light[Z] = z - l -> c[Z];
					
						c = get_illumination(light, n0, l -> l, amb, cons);
					}
				
					plot(s, c, zb, x, y, z);

					if ( d < 0 ) {
						x = x + 1;
						d = d + dy;
					}
					else {
						x = x + 1;
						y = y + 1;
						d = d + dy - dx;
					}
				
					z = z + dz;
					if (strcmp(shading, "gouraud") == 0){
						c0[0] += dcr;
						c0[1] += dcg;
						c0[2] += dcb;
					}

					if (strcmp(shading, "phong") == 0){
			 
						n0[X] += dn[X];
						n0[Y] += dn[Y];
						n0[Z] += dn[Z];
					}
				}
			}

			//slope > 1: Octant 2 (6)
			else {
				d = ( dy / 2 ) - dx;
				dz = (z1 - z) / (y1 - y);
			
				if (strcmp(shading, "gouraud") == 0){
	
					dcr = (c1[0] - c0[0]) / (y1 - y);
					dcg = (c1[1] - c0[1]) / (y1 - y);
					dcb = (c1[2] - c0[2]) / (y1 - y);
				}

				if (strcmp(shading, "phong") == 0){
			
					dn[X] = (n1[X] - n0[X]) / (y1 - y);
					dn[Y] = (n1[Y] - n0[Y]) / (y1 - y);
					dn[Z] = (n1[Z] - n0[Z]) / (y1 - y);
				}
			
				while ( y <= y1 ) {
				
					if (strcmp(shading, "gouraud") == 0){
						c.red = c0[0];
						c.green = c0[1];
						c.blue = c0[2];
					}

					if (strcmp(shading, "phong") == 0){
			
						light[X] = x - l -> c[X];
						light[Y] = y - l -> c[Y];
						light[Z] = z - l -> c[Z];
					
						c = get_illumination(light, n0, l -> l, amb, cons);
					}
				
					plot(s, c, zb, x, y, z);
					if ( d > 0 ) {
						y = y + 1;
						d = d - dx;
					}
					else {
						y = y + 1;
						x = x + 1;
						d = d + dy - dx;
					}
					z = z + dz;
					if (strcmp(shading, "gouraud") == 0){
						c0[0] += dcr;
						c0[1] += dcg;
						c0[2] += dcb;
					}

					if (strcmp(shading, "phong") == 0){
				
						n0[X] += dn[X];
						n0[Y] += dn[Y];
						n0[Z] += dn[Z];
					}
				}
			}
		}

		//negative slope: Octants 7, 8 (3 and 4)
		else {

			//slope > -1: Octant 8 (4)
			if ( dx > abs(dy) ) {
				d = dy + ( dx / 2 );
				dz = (z1 - z) / (x1 - x);

				if (strcmp(shading, "gouraud") == 0){
	
					dcr = (c1[0] - c0[0]) / (x1 - x);
					dcg = (c1[1] - c0[1]) / (x1 - x);
					dcb = (c1[2] - c0[2]) / (x1 - x);
				}

				if (strcmp(shading, "phong") == 0){
			
					dn[X] = (n1[X] - n0[X]) / (x1 - x);
					dn[Y] = (n1[Y] - n0[Y]) / (x1 - x);
					dn[Z] = (n1[Z] - n0[Z]) / (x1 - x);
				}

				while ( x <= x1 ) {
				
					if (strcmp(shading, "gouraud") == 0){
						c.red = c0[0];
						c.green = c0[1];
						c.blue = c0[2];
					}

					if (strcmp(shading, "phong") == 0){
					
						light[X] = x - l -> c[X];
						light[Y] = y - l -> c[Y];
						light[Z] = z - l -> c[Z];
					
						c = get_illumination(light, n0, l -> l, amb, cons);
					}
				
					plot(s, c, zb, x, y, z);

					if ( d > 0 ) {
						x = x + 1;
						d = d + dy;
					}
					else {
						x = x + 1;
						y = y - 1;
						d = d + dy + dx;
					}
					z = z + dz;
					if (strcmp(shading, "gouraud") == 0){
						c0[0] += dcr;
						c0[1] += dcg;
						c0[2] += dcb;
					}

					if (strcmp(shading, "phong") == 0){
	  
						n0[X] += dn[X];
						n0[Y] += dn[Y];
						n0[Z] += dn[Z];
					}
				}
			}

			//slope < -1: Octant 7 (3)
			else {
				d =  (dy / 2) + dx;
				dz = (z1 - z) / (y - y1);
			
				if (strcmp(shading, "gouraud") == 0){
	
					dcr = (c1[0] - c0[0]) / (y - y1);
					dcg = (c1[1] - c0[1]) / (y - y1);
					dcb = (c1[2] - c0[2]) / (y - y1);
				}

				if (strcmp(shading, "phong") == 0){
			
					dn[X] = (n1[X] - n0[X]) / (y - y1);
					dn[Y] = (n1[Y] - n0[Y]) / (y - y1);
					dn[Z] = (n1[Z] - n0[Z]) / (y - y1);
				}

				while ( y >= y1 ) {
				
					if (strcmp(shading, "gouraud") == 0){
						c.red = c0[0];
						c.green = c0[1];
						c.blue = c0[2];
					}

					if (strcmp(shading, "phong") == 0){
					
						light[X] = x - l -> c[X];
						light[Y] = y - l -> c[Y];
						light[Z] = z - l -> c[Z];
					
						c = get_illumination(light, n0, l -> l, amb, cons);
					}
				
					plot(s, c, zb, x, y, z);
					if ( d < 0 ) {
						y = y - 1;
						d = d + dx;
					}
					else {
						y = y - 1;
						x = x + 1;
						d = d + dy + dx;
					}
				
					z = z + dz;
					if (strcmp(shading, "gouraud") == 0){
						c0[0] += dcr;
						c0[1] += dcg;
						c0[2] += dcb;
					}

					if (strcmp(shading, "phong") == 0){
			
						n0[X] += dn[X];
						n0[Y] += dn[Y];
						n0[Z] += dn[Z];
					}
				}
			}
		}
	}
