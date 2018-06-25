


#include <stdio.h>
#include <signal.h>

#include "inputModule.h"
#include "PLY.h"

extern PLYObject *ply;

static int motionMode;
static int startX;
static int startY;
static GLfloat angle = 20;    /* in degrees */
static GLfloat angle2 = 210;   /* in degrees */

GLfloat current_pos[] = {0.0, 0.0, 5.0};
GLfloat current_pos_bunny[] = {0.0, 0.0, 0.0};
GLfloat current_rot_bunny[] = {0.0, 0.0, 0.0};
GLfloat current_sca_bunny[] = {1.0, 1.0, 1.0};


int opengl = 1;
int box = 0;
int wire = 0;
int light = 0;
int flat = 0;
int texture = 0;
int ambient=1, diffuse=1, specular=1;
int recording = 0;


void readKeyboard(unsigned char key, int x, int y)
{
  switch(key){
  case  0x1B:
  case  'q':
  case  'Q':
    exit(0);
    break;
  case 'i':
  case 'I':
    if (ply)
      ply->invertNormals();
    break;
  case 'b':
  case 'B':
    box = (box + 1) % 2;
    break;
  case 'w':
  case 'W':
    wire = (wire + 1) % 2;
    break;
  case 'f':
  case 'F':
    flat = (flat + 1) % 2;
    break;
  case 'V':
  case 'v':
    recording = (recording + 1) % 2;
	break;
  case 'h':
  case 'H':
	printf("\tpress l/L to toggle light\n");
	printf("\tpress o/O to toggle OpenGL/GPU shading\n");
    printf("\tpress q/Q for quit\n");
    printf("\tpress i/I to invert normals\n");
    printf("\tpress b/B to display bounding box\n");
    printf("\tpress w/W to toggle wire frame rendering\n");
    printf("\tpress f/F to toggle flat shading\n");
    printf("\tpress e/E to revert viewpoint to initial positions\n");
	printf("\tpress z/Z to scale PLY object\n");
	printf("\tpress x/X to scale PLY object\n");
	printf("\tpress c/C to scale PLY object\n");
	printf("\tpress r/R to rotate PLY object\n");
	printf("\tpress t/T to rotate PLY object\n");
	printf("\tpress y/Y to rotate PLY object\n");
    break;
  case 'l':
  case 'L':
	light = (light + 1) %2;
	if (light)
	  printf("lighting enabled.\n");
	else
	  printf("lighting is off.\n");
	break;
  case 'o':
	opengl = !opengl;
	if (opengl)
	  printf("OpenGL illumination and shading enabled.\n");
	else
	  printf("GLSL illumination and shading enabled.\n");
	break;
  case '4':
    texture = (texture + 1) %2;
	if (texture)
	  printf("texture enabled.\n");
	else
	  printf("texture is off.\n");
	break;
  // scale ------------------------------------
  case 'z':
    // 
	break;
  case 'Z':
    // 
    break;
  case 'x':
    // 
	break;
  case 'X':
    // 
    break;
  case 'c':
    // 
	break;
  case 'C':
    // 
    break;
  // rotate ------------------------------------
  case 'r':
    // 
	break;
  case 'R':
    // 
    break;
  case 't':
    // 
	break;
  case 'T':
    // 
    break;
  case 'y':
    // 
	break;
  case 'Y':
    // 
    break;
  case 'e':
  case 'E':
    // reset initial view parameters
    angle = 20;
    angle2 = 30;
    current_pos[0] = 0.0;
    current_pos[1] = 0.0;
    current_pos[2] = 5.0;
	// reset rendering parameters
	flat = 0;
    wire = 0;
	box = light = texture = 0;
	ambient = diffuse = specular = 1;
    break;
  default:
    break;
  }
  glutPostRedisplay();
}

void mouseButtHandler(int button, int state, int x, int y)
{
  motionMode = 0;
  
  switch(button){
  case GLUT_LEFT_BUTTON:  
    if(state == GLUT_DOWN) {
      motionMode = 1;		// Rotate object
      startX = x;
      startY = y;
    }
    break;
  case GLUT_MIDDLE_BUTTON:  
    if(state == GLUT_DOWN) {
      motionMode = 2;		// Translate object
      startX = x;
      startY = y;
    }
    break;
  case GLUT_RIGHT_BUTTON: 
    if(state == GLUT_DOWN) { 
      motionMode = 3;		// Zoom
      startX = x;
      startY = y;
    }
    break;
  }
  glutPostRedisplay();
}

void special_callback(int key, int x, int y)
{
  switch (key) {
    case GLUT_KEY_DOWN:
      // 

	  printf("%f\n", current_pos_bunny[0]);
      break;
    case GLUT_KEY_UP:
      // 

	  printf("%f\n", current_pos_bunny[0]);
      break;
    case GLUT_KEY_LEFT:
      // 

	  printf("%f\n", current_pos_bunny[0]);
      break;
    case GLUT_KEY_RIGHT:
      // 

	  printf("%f\n", current_pos_bunny[0]);
      break;
  }
}

void mouseMoveHandler(int x, int y)
{
  // No mouse button is pressed... return 
  switch(motionMode){
  case 0:
    return;
    break;

  case 1: // Calculate the rotations
    angle = angle + (x - startX);
    angle2 = angle2 + (y - startY);
    startX = x;
    startY = y;
    break;

  case 2:
    current_pos[0] = current_pos[0] - (x - startX)/100.0;
    current_pos[1] = current_pos[1] - (y - startY)/100.0;
    startX = x;
    startY = y;
    break;
    
  case 3:
    current_pos[2] = current_pos[2] - (y - startY)/10.0;
    startX = x;
    startY = y;
    break;
  }
  
  glutPostRedisplay();
}


void setUserView()
{
  glLoadIdentity();

  glTranslatef(-current_pos[0], current_pos[1], -current_pos[2]);
  glRotatef(angle2, 1.0, 0.0, 0.0);
  glRotatef(angle, 0.0, 1.0, 0.0);
}
