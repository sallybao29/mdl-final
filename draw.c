#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"

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
void fill( struct matrix *points, int i, screen s, color amb, z_buff zb){//,
	   //struct vertex_normal **vns, struct light *light, struct constants *c,
	   //char *shading){
  // rows -> x, y, z
  double p[3][3]; 
  double tmp;
  double d0, d1, d2;
  double x0, x1, y0, y1, z0, z1;
  double dz0, dz1, dz2;
  int m, n, l;
  int fb = 0;
  int TOP = 0, MID = 1, BOT = 2;
  color ct, cm, cb;
  struct vertex_normal *vn;

	
  for (m = 0; m < 3; m++)
    for (n = 0; n < 3; n++)
      p[m][n] = points -> m[m][n + i];

  /*------------------------------ Horizontal Scanline ------------------------------------*/
  for (m = 0; m < 3; m++){
    for (n = m + 1; n < 3; n++){
      if (p[1][m] < p[1][n]){
	for (l = 0; l < 3; l++){
	  tmp = p[l][m];
	  p[l][m]= p[l][n];
	  p[l][n] = tmp;
	}
      }
    }
  }
  /*
  if (strcmp(shading, "gouraud") == 0){
    vn = lookup_vertex_normal(vns, 100, p[0][TOP], p[1][TOP], p[2][TOP]);
    ct.red = get_illumination(light -> c, vn -> normal, light -> l[0], c -> r);
    ct.green = get_illumination(light -> c, vn -> normal, light -> l[1], c -> g);
    ct.blue = get_illumination(light -> c, vn -> normal, light -> l[2], c -> b);

    vn = lookup_vertex_normal(vns, 100, p[0][MID], p[1][MID], p[2][MID]);
    ct.red = get_illumination(light -> c, vn -> normal, light -> l[0], c -> r);
    ct.green = get_illumination(light -> c, vn -> normal, light -> l[1], c -> g);
    ct.blue = get_illumination(light -> c, vn -> normal, light -> l[2], c -> b);

    vn = lookup_vertex_normal(vns, 100, p[0][BOT], p[1][BOT], p[2][BOT]);
    ct.red = get_illumination(light -> c, vn -> normal, light -> l[0], c -> r);
    ct.green = get_illumination(light -> c, vn -> normal, light -> l[1], c -> g);
    ct.blue = get_illumination(light -> c, vn -> normal, light -> l[2], c -> b);
  }
  */
  y0 = p[1][BOT];
  x0 = p[0][BOT];
  x1 = x0;
  z0 = p[2][BOT];
  z1 = z0;

  d0 = (p[0][TOP] - p[0][BOT]) / (p[1][TOP] - p[1][BOT]); //(yt-yb)/(xt-xb)
  d1 = (p[0][MID] - p[0][BOT]) / (p[1][MID] - p[1][BOT]); //(ym-yb)/(xm-xb)
  d2 = (p[0][TOP] - p[0][MID]) / (p[1][TOP] - p[1][MID]); //(yt-ym)/(xt-xm)

  //delta z / distance top to bottom
  dz0 = (p[2][TOP] - p[2][BOT]) / distance(p[0][TOP], p[1][TOP],
					   p[0][BOT], p[1][BOT]);

  //delta z / distance middle to bottom
  dz1 = (p[2][MID] - p[2][BOT]) / distance(p[0][MID], p[1][MID], p[0][BOT], p[1][BOT]);

  //delta z / distance top to middle
  dz2 = (p[2][TOP] - p[2][MID]) / distance(p[0][TOP], p[1][TOP], p[0][MID], p[1][MID]);	

  while ((int)y0 < (int)(p[1][TOP])){
	
    //printf("Drawing (%lf, %lf) to (%lf, %lf)\n", x0, y0, x0, y1);
    draw_line(x0, y0, z0, x1, y0, z1, s, amb, zb);

    y0 += 1;
    x0 += d0;
    x1 += d1;
    z0 += dz0;
    z1 += dz1;

    if (y0 >= (int)p[1][MID] && !fb){
      d1 = d2;
      dz1 = dz2;
      x1 = p[0][MID];
      z1 = p[2][MID];
      fb++; 
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

struct vertex_normal *lookup_vertex_normal(struct vertex_normal **vns, int pos,
					   double vx, double vy, double vz){

  int i;
  struct vertex_normal *vn;

  for (i = 0; i < pos; i++){
    vn = vns[i];

    if (vn -> vertex[0] == vx &&
	vn -> vertex[1] == vy &&
	vn -> vertex[2] == vz){
			
      return vn;
    }
  }
  return NULL;
}


void add_vertex(struct vertex_normal **vns, int *pos, double *normal,
		double x, double y, double z){
	
  struct vertex_normal *vn;

  vn = lookup_vertex_normal(vns, *pos, x, y, z);

  //if vertex in storage, add surface to vertex normal
  if (vn != NULL){
    vn -> normal[0] += normal[0];
    vn -> normal[1] += normal[1];
    vn -> normal[2] += normal[2];
  }
  else {
    //add new vertex
    vn = (struct vertex_normal *)malloc(sizeof(struct vertex_normal));
    vn -> vertex[0] = x;
    vn -> vertex[1] = y;
    vn -> vertex[2] = z;
	
    vn -> normal[0] = 0;
    vn -> normal[1] = 0;
    vn -> normal[2] = 0;
	
    vns[*pos] = vn;
    *pos += 1;
  }
}


struct vertex_normal **get_vertex_normals(struct matrix *polygons){
  struct vertex_normal **vns;
  double *normal;
  int i;
  int size = 100;
  int pos = 0;
	
  vns = (struct vertex_normal**)malloc(sizeof(struct vertex_normal *)*size);
	
  for( i=0; i < polygons->lastcol-2; i+=3 ) {
    if ( calculate_dot( polygons, i ) < 0 ) {

      //calculate surface normal for polygon
      normal = calculate_normal( polygons->m[0][i+1] - polygons->m[0][i],
				 polygons->m[1][i+1] - polygons->m[1][i],
				 polygons->m[2][i+1] - polygons->m[2][i],
				 polygons->m[0][i+2] - polygons->m[0][i],
				 polygons->m[1][i+2] - polygons->m[1][i],
				 polygons->m[2][i+2] - polygons->m[2][i] );

      add_vertex(vns, &pos, normal, polygons -> m[0][i],
		 polygons -> m[1][i], polygons -> m[2][i]);
      add_vertex(vns, &pos, normal, polygons -> m[0][i+1],
		 polygons -> m[1][i+1], polygons -> m[2][i+1]);
			
      add_vertex(vns, &pos, normal, polygons -> m[0][i+2],
		 polygons -> m[1][i+2], polygons -> m[2][i+2]);
    }
  }
  for (i = 0; i < pos; i++){
    normalize(normal);
  }
	
  return vns;
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
  struct vertex_normal **vns;

  if (strcmp(shading, "gouraud") == 0)
    vns = get_vertex_normals(polygons);
	
	
  for( i=0; i < polygons->lastcol-2; i+=3 ) {

    if ( calculate_dot( polygons, i ) < 0 ) {

      if (strcmp(shading, "flat") == 0){
	double ax, ay, az, bx, by, bz;
	double *normal;
	double light[3];

	//calculate A and B vectors
	ax = polygons->m[0][i+1] - polygons->m[0][i];
	ay = polygons->m[1][i+1] - polygons->m[1][i];
	az = polygons->m[2][i+1] - polygons->m[2][i];

	bx = polygons->m[0][i+2] - polygons->m[0][i];
	by = polygons->m[1][i+2] - polygons->m[1][i];
	bz = polygons->m[2][i+2] - polygons->m[2][i];

	//get the surface normal
	normal = calculate_normal( ax, ay, az, bx, by, bz );

	light[0] = (polygons->m[0][i]+polygons->m[0][i+1]+polygons->m[0][i+2]) / 3;
	light[1] = (polygons->m[1][i]+polygons->m[1][i+1]+polygons->m[1][i+2]) / 3;
	light[2] = (polygons->m[2][i]+polygons->m[2][i+1]+polygons->m[2][i+2]) / 3;
				
	col.red = get_illumination(light, normal, l -> l[0], c -> r);
	col.green = get_illumination(light, normal, l -> l[1], c -> g);
	col.blue = get_illumination(light, normal, l -> l[2], c -> b);
      }
	 
      fill(polygons, i, s, col, zb);
	   //, vns, l, c, shading);
			
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
  int x, y, d, dx, dy;
  //double z, dz;
  int z;
  double dz;

  //printf("Actual [%lf, %lf]\n", z0, z1);
  //printf("Casted [%d, %d]\n", (int)z0, (int)z1);
	
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
  
      while ( x <= x1 ) {
	plot(s, c, zb, x, y, z);

	if ( d < 0 ) {
	  x = x + 1;
	  //z = z + dz;

	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  //z = z + dz;
	  d = d + dy - dx;
	}
	z = z0 + ((double)x-x0)/(x1-x0)*(z1-z0);
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      while ( y <= y1 ) {

	plot(s, c, zb, x, y, z);
	if ( d > 0 ) {
	  y = y + 1;
	  z = z + dz;
	  d = d - dx;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  z = z + dz;
	  d = d + dy - dx;
	}
	z = z0 + ((double)y-y0)/(y1-y0)*(z1-z0);
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {

      d = dy + ( dx / 2 );
  
      while ( x <= x1 ) {

	plot(s, c, zb, x, y, z);

	if ( d > 0 ) {
	  x = x + 1;
	  z = z + dz;
	  d = d + dy;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  z = z + dz;
	  d = d + dy + dx;
	}
	z = z0 + ((double)x-x0)/(x1-x0)*(z1-z0);
      }
    }

    //slope < -1: Octant 7 (3)
    else {

      d =  (dy / 2) + dx;

      while ( y >= y1 ) {
	
	plot(s, c, zb, x, y, z);
	if ( d < 0 ) {
	  y = y - 1;
	  z = z + dz;
	  d = d + dx;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  z = z + dz;
	  d = d + dy + dx;
	}
	z = z0 + ((double)y-y0)/(y1-y0)*(z1-z0);
      }
    }
  }
}

