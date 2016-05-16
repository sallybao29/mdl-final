/*========== my_main.c ==========

  This is the only file you need to modify in order
  to get a working mdl project (for now).

  my_main.c will serve as the interpreter for mdl.
  When an mdl script goes through a lexer and parser, 
  the resulting operations will be in the array op[].

  Your job is to go through each entry in op and perform
  the required action from the list below:

  frames: set num_frames (in misc_headers.h) for animation

  basename: set name (in misc_headers.h) for animation

  vary: manipluate knob values between two given frames
  over a specified interval

  set: set a knob to a given value
  
  setknobs: set all knobs to a given value

  push: push a new origin matrix onto the origin stack
  
  pop: remove the top matrix on the origin stack

  move/scale/rotate: create a transformation matrix 
  based on the provided values, then 
  multiply the current top of the
  origins stack by it.

  box/sphere/torus: create a solid object based on the
  provided values. Store that in a 
  temporary matrix, multiply it by the
  current top of the origins stack, then
  call draw_polygons.

  line: create a line based on the provided values. Store 
  that in a temporary matrix, multiply it by the
  current top of the origins stack, then call draw_lines.

  save: call save_extension with the provided filename

  display: view the image live
  
  jdyrlandweaver
  =========================*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include "parser.h"
#include "symtab.h"
#include "y.tab.h"

#include "misc_headers.h"
#include "matrix.h"
#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "stack.h"
#include "gmath.h"

/*======== void first_pass()) ==========
  Inputs:   
  Returns: 

  Checks the op array for any animation commands
  (frames, basename, vary)
  
  Should set num_frames and basename if the frames 
  or basename commands are present

  If vary is found, but frames is not, the entire
  program should exit.

  If frames is found, but basename is not, set name
  to some default value, and print out a message
  with the name being used.

  jdyrlandweaver
  ====================*/
void first_pass() {
  int i;
  int v = 0,
    b = 0,
    n = 0;

  for (i = 0; i < lastop; i++){
    switch (op[i].opcode){
    case FRAMES:
      num_frames = op[i].op.frames.num_frames;
      n = 1;
      break;
    case BASENAME:
      strcpy(name, op[i].op.basename.p -> name);
      b = 1;
      break;
    case VARY: v = 1;
      break;
    default:
      break;
    }
  }
  if (v && !num_frames){
    printf("Must specify number of frames\n");
    exit(0);
  }
  if (num_frames && !b){
    strncpy(name, "test", sizeof(name));
    printf("No filename specified. Using %s\n", name);
  }
}


/*======== struct vary_node ** second_pass()) ==========
  Inputs:   
  Returns: An array of vary_node linked lists

  In order to set the knobs for animation, we need to keep
  a seaprate value for each knob for each frame. We can do
  this by using an array of linked lists. Each array index
  will correspond to a frame (eg. knobs[0] would be the first
  frame, knobs[2] would be the 3rd frame and so on).

  Each index should contain a linked list of vary_nodes, each
  node contains a knob name, a value, and a pointer to the
  next node.

  Go through the opcode array, and when you find vary, go 
  from knobs[0] to knobs[frames-1] and add (or modify) the
  vary_node corresponding to the given knob with the
  appropirate value. 

  05/17/12 09:29:31
  jdyrlandweaver
  ====================*/
struct vary_node ** second_pass() {
  struct vary_node ** knobs;
  struct vary_node * knob;
  int i, j, num_steps, start_frame, end_frame;
  double count, step, start_val, end_val;
  char* s;
	
  knobs = (struct vary_node **)malloc(sizeof(struct vary_node*)*num_frames);
  for (i = 0; i < lastop; i++){
    if (op[i].opcode == VARY){
      if (op[i].op.vary.start_frame < 0){
	printf("Invalid starting frame\n");
	exit(0);
      }
      if (op[i].op.vary.end_frame >= num_frames){
	printf("Invalid ending frame\n");
	exit(0);
      }
      if (op[i].op.vary.start_frame > op[i].op.vary.end_frame){
	printf("End frame cannot be less than start frame\n");
	exit(0);
      }
      start_frame = op[i].op.vary.start_frame;
      end_frame = op[i].op.vary.end_frame;
      start_val = op[i].op.vary.start_val;
      end_val = op[i].op.vary.end_val;
      s = op[i].op.vary.scale;

      count = start_val;
      num_steps = end_frame - start_frame;
      step = (end_val - start_val) / num_steps;
      for (j = start_frame; j < end_frame + 1; j++){
				
	knob = (struct vary_node*)malloc(sizeof(struct vary_node));
	strncpy(knob -> name, op[i].op.vary.p -> name, sizeof(knob -> name));
	knob -> value = count;
	knob -> next = knobs[j];
	knobs[j] = knob;
	count += step;
      }
    }
  }
  return knobs;
}


/*======== void print_knobs() ==========
  Inputs:   
  Returns: 

  Goes through symtab and display all the knobs and their
  currnt values

  jdyrlandweaver
  ====================*/
void print_knobs() {
  
  int i;

  printf( "ID\tNAME\t\tTYPE\t\tVALUE\n" );
  for ( i=0; i < lastsym; i++ ) {

    if ( symtab[i].type == SYM_VALUE ) {
      printf( "%d\t%s\t\t", i, symtab[i].name );

      printf( "SYM_VALUE\t");
      printf( "%6.2f\n", symtab[i].s.value);
    }
  }
}


/*======== void my_main() ==========
  Inputs:
  Returns: 

  This is the main engine of the interpreter, it should
  handle most of the commadns in mdl.

  If frames is not present in the source (and therefore 
  num_frames is 1, then process_knobs should be called.

  If frames is present, the enitre op array must be
  applied frames time. At the end of each frame iteration
  save the current screen to a file named the
  provided basename plus a numeric string such that the
  files will be listed in order, then clear the screen and
  reset any other data structures that need it.

  Important note: you cannot just name your files in 
  regular sequence, like pic0, pic1, pic2, pic3... if that
  is done, then pic1, pic10, pic11... will come before pic2
  and so on. In order to keep things clear, add leading 0s
  to the numeric portion of the name. If you use sprintf, 
  you can use "%0xd" for this purpose. It will add at most
  x 0s in front of a number, if needed, so if used correctly,
  and x = 4, you would get numbers like 0001, 0002, 0011,
  0487

  05/17/12 09:41:35
  jdyrlandweaver
  ====================*/
void my_main( int polygons ) {

  int i, f, j;
  double step;
  double xval, yval, zval, knob_value;
  struct matrix *transform;
  struct matrix *tmp;
  struct stack *s;
  screen t;
  color g;

  struct vary_node **knobs;
  struct vary_node *vn;
  char frame_name[128];

  s = new_stack();
  tmp = new_matrix(4, 1000);
  clear_screen(t);

  step = 5;
  num_frames = 1;

  g.red = 0;
  g.green = 255;
  g.blue = 255;

  first_pass();

  if (num_frames > 1){
    knobs = second_pass();
    mkdir("./anim", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  }
  for (j = 0; j < num_frames; j++){
		
    if (num_frames > 1)		{
      for (vn = knobs[j]; vn != NULL; vn = vn -> next)
	set_value(lookup_symbol(vn -> name), vn -> value);

      print_knobs();
    }
    for (i = 0; i < lastop; i++){
      switch (op[i].opcode){

      case AMBIENT:
	g.red = op[i].op.ambient.c[0];
	g.green = op[i].op.ambient.c[1];
	g.blue = op[i].op.ambient.c[2]; 
	break;
				
      case SPHERE:
	add_sphere( tmp,op[i].op.sphere.d[0], //cx
		    op[i].op.sphere.d[1],  //cy
		    op[i].op.sphere.d[2],  //cz
		    op[i].op.sphere.r,
		    step);
	//apply the current top origin
	matrix_mult( s->data[ s->top ], tmp );
	draw_polygons( tmp, t, g );
	tmp->lastcol = 0;
	break;

      case TORUS:
	add_torus( tmp, op[i].op.torus.d[0], //cx
		   op[i].op.torus.d[1],     //cy
		   op[i].op.torus.d[2],    //cz
		   op[i].op.torus.r0,
		   op[i].op.torus.r1,
		   step);
	matrix_mult( s->data[ s->top ], tmp );
	draw_polygons( tmp, t, g );
	tmp->lastcol = 0;
	break;

      case BOX:
	add_box( tmp, op[i].op.box.d0[0],
		 op[i].op.box.d0[1],
		 op[i].op.box.d0[2],
		 op[i].op.box.d1[0],
		 op[i].op.box.d1[1],
		 op[i].op.box.d1[2]);
	matrix_mult( s->data[ s->top ], tmp );
	draw_polygons( tmp, t, g );
	tmp->lastcol = 0;
	break;

      case LINE:
	add_edge( tmp, op[i].op.line.p0[0],
		  op[i].op.line.p0[1],
		  op[i].op.line.p0[1],
		  op[i].op.line.p1[0],
		  op[i].op.line.p1[1],
		  op[i].op.line.p1[1]);
	draw_lines( tmp, t, g );
	tmp->lastcol = 0;
	break;

      case MOVE:
	//get the factors
	xval = op[i].op.move.d[0];
	yval =  op[i].op.move.d[1];
	zval = op[i].op.move.d[2];

	if (op[i].op.move.p != NULL){
	  knob_value = op[i].op.move.p -> s.value;
	  xval *= knob_value;
	  yval *= knob_value;
	  zval *= knob_value;
	}
      
	transform = make_translate( xval, yval, zval );
	//multiply by the existing origin
	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case SCALE:

	xval = op[i].op.scale.d[0];
	yval = op[i].op.scale.d[1];
	zval = op[i].op.scale.d[2];

	if (op[i].op.scale.p != NULL){
	  knob_value = op[i].op.scale.p -> s.value;
	  xval *= knob_value;
	  yval *= knob_value;
	  zval *= knob_value;
	}

	transform = make_scale( xval, yval, zval );
	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case ROTATE:

	xval = op[i].op.rotate.degrees * ( M_PI / 180 );

	if (op[i].op.rotate.p != NULL){
	  knob_value = op[i].op.rotate.p -> s.value;	  
	  xval *=  knob_value;
	}

	//get the axis
	if ( op[i].op.rotate.axis == 0 ) 
	  transform = make_rotX( xval );
	else if ( op[i].op.rotate.axis == 1 ) 
	  transform = make_rotY( xval );
	else if ( op[i].op.rotate.axis == 2 ) 
	  transform = make_rotZ( xval );

	matrix_mult( s->data[ s->top ], transform );
	//put the new matrix on the top
	copy_matrix( transform, s->data[ s->top ] );
	free_matrix( transform );
	break;

      case SET:
	set_value(lookup_symbol(op[i].op.set.p -> name), op[i].op.set.val);
	break;
 
      case SETKNOBS:
	for (f = 0; f < lastsym; f++)
	  set_value(&symtab[i], op[i].op.setknobs.value);
				
	break;
      case PUSH:
	push( s );
	break;
      case POP:
	pop( s );
	break;
      case SAVE:
	save_extension( t, op[i].op.save.p->name );
	break;
      case DISPLAY:
	display( t );
	break;
      }
    }
    if (num_frames > 1){
      sprintf(frame_name, "anim/%s%03d.png", name, j);
      printf("Generating %s\n", frame_name);
      save_extension(t, frame_name);
      clear_screen(t);
      while (s -> top > 0)
	pop(s);
    }
  }
		
  free_stack( s );
  free_matrix( tmp );
	
  if (num_frames > 1){
    for (i = 0; i < num_frames; i++){
      while (knobs[i] != NULL){
	vn = knobs[i];
	knobs[i] = knobs[i] -> next;
	free(vn);
      }
    }
    free(knobs);
  }
}
