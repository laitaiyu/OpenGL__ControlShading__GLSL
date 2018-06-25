

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <io.h>
#include <process.h>
#include "./GL/glew.h"
#include "./GL/glut.h"
// include DevIL to load images
#include ".\IL\il.h"
#include <string>
using std::string;
#include "Freeimage.h"
#include "bitmap.h"
#include "textfile.h"
#include "inputModule.h"
//#include "geometry.h"
#include "PLY.h"

GLuint v0, f0, p0; // part 0
GLuint v1, f1, p1; // part 1
GLuint v2, f2, p2; // part 2

GLuint v3, f3, p3; // part 3
GLuint v4, f4, p4; // part 4
GLuint v5, f5, p5; // part 5

GLint timeloc_part3;
GLint timeloc_part4;
GLint timeloc_part5, woodloc, l3dloc;


float lpos[4] = {1,0.5,1,0};
float tg = 0.0f;
float tp5 = 0.0f;
unsigned char image2[256][256][3];

extern "C" {
#define IMAGE_WIDTH  800
#define IMAGE_HEIGHT 600

typedef struct{
	float fieldOfView;
	float aspect;
	float nearPlane;
	float farPlane;
}perspectiveData;
}

extern GLfloat current_pos_bunny[];
extern GLfloat current_rot_bunny[];
extern GLfloat current_sca_bunny[];

extern int recording;
extern int texture;
int window;
int updateFlag;
static GLuint texName1;
static GLuint texName2;

PLYObject *ply = NULL;
GLuint texture_index;
perspectiveData pD;

GLuint vp, fp;

void cleanup(int sig)
{
  delete(ply);
  exit(0);
}


// ------------------------------------------------------------
//
//			Loading the image
//
// ------------------------------------------------------------

unsigned int ilLoadImage(std::string filename) {

    ILboolean success;
    unsigned int imageID;

    // init DevIL. This needs to be done only once per application
    ilInit();
    // generate an image name
    ilGenImages(1, &imageID);
    // bind it
    ilBindImage(imageID);
    // match image origin to OpenGL’s
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    // load  the image
    success = ilLoadImage((ILstring)filename.c_str());
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    // check to see if everything went OK
    if (!success) {
        ilDeleteImages(1, &imageID);
        return 0;
    }
    else return imageID;
}


int loadtexturebmp ( const char* fileName )
{
    BITMAPFILEHEADER1 texInfo;
    GLubyte* bits;
    GLuint id;

    bits = LoadBitmapFile(fileName,&texInfo);
    glGenTextures(1,&id);
    printf("id=%d......\n",id);

    glBindTexture(GL_TEXTURE_2D,id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST

    glTexImage2D(GL_TEXTURE_2D, 0, 4, texInfo.biWidth, texInfo.biHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bits );

    return id;
}


void screenShot(int x, int y, int w, int h, string filename)
{
	FIBITMAP *image=0;
	unsigned char *data = new unsigned char[w*h*3];
	unsigned char *temp = new unsigned char[w*3];

	if(!data || !temp)
	{
		fprintf(stderr, "no more memory!\n");
	}

	glReadPixels(x,y,w,h, GL_RGB, GL_UNSIGNED_BYTE, data);

	int i;
	unsigned char tempData;
	for(i=0; i<w*h*3; i+=3){
		tempData = data[i+2];
		data[i+2] = data[i];
		data[i]   = tempData;
	}

/*
	// Invert the image
	for(i=0; i < h/2; i++) {
		memmove(temp,&data[i*w*3], sizeof(unsigned char)*w*3);
		memmove(&data[i*w*3], &data[(h-i-1)*w*3],sizeof(unsigned char)*w*3);
		memmove(&data[(h-i-1)*w*3],temp, sizeof(unsigned char)*w*3);
	}
*/
	// Save image
	image = FreeImage_ConvertFromRawBits(data, w,h,w*3, 24, 0,0,0);
	if(!image)
	{
		fprintf(stderr, "Failed to convert bits!\n");
	}

	filename += ".png";
	if(FreeImage_Save(FIF_PNG, image, filename.c_str()))
		fprintf(stderr, "Wrote %s!\n", filename.c_str());
	else
		fprintf(stderr, "Failed!\n");
	FreeImage_Unload(image);

	delete [] temp;
	delete [] data;
}

void getColor(float z, float c[3])
{
   float indexC;
   float max_z =  0.15f;
   float min_z = -0.15f;

   indexC = 4.0 * (z - min_z) / ( max_z - min_z);

   if (indexC >= 4.0){
	  c[0] = 1.0;	  c[1] = 0.0;      c[2] = 0.0;
   }
   else if (indexC >= 3.0){
	  c[0] = 1.0;	  c[1] = 4.0 - indexC;	  c[2] = 0.0;
   }
   else if (indexC >= 2.0){
	  c[0] = indexC - 2.0;	  c[1] = 1.0;	  c[2] = 0.0;
   }
   else if (indexC >= 1.0){
	  c[0] = 0.0;	  c[1] = 1.0;	  c[2] = 2.0 - indexC;
   }
   else if (indexC >= 0.0){
	  c[0] = 0.0;	  c[1] = indexC;	  c[2] = 1.0;
   }
   else {
	  c[0] = 0.0; 	  c[1] = 0.0;	  c[2] = 1.0;
   }
}

void drawGrid()
{
  float i, i2, j, j2;
  float space = 0.5f;

  float color[3];

  glColor3f(0,0,0);
  
  for (i=-5.0f; i<=5.0f; i+=space){
	  for (j=-5.0f; j<=5.0f; j+=space){

		  i2 = i+space;
		  j2 = j+space;

		  glBegin(GL_LINE_LOOP);
		  //glBegin(GL_QUADS);
			glVertex3f( i,  0, j);
 			glVertex3f( i2, 0, j);
			glVertex3f( i2, 0, j2);
			glVertex3f( i,  0, j2);
		  glEnd();
	  }
  }
}


void drawPlane()
{
  glColor3f(0,0,0);

  //glBegin(GL_LINE_LOOP);
  glBegin(GL_QUADS);
    glTexCoord2f(0, 0);		glVertex3f(-5.0F,  0, -5.0F);
 	glTexCoord2f(1, 0);		glVertex3f( 5.0F,  0, -5.0F);
	glTexCoord2f(1, 1);		glVertex3f( 5.0F,  0,  5.0F);
	glTexCoord2f(0, 1);		glVertex3f(-5.0F,  0,  5.0F);
  glEnd();
}


void drawPlaneTexture()
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,loadtexturebmp("qr.bmp"));
    static float s = 0.0f;
    s+=0.000f;
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f+s, 0.0f+s); glVertex3f(-5.0f, 10.0f, -5.0f);
    glTexCoord2f(0.0f+s, 1.0f+s); glVertex3f( 5.0f, 10.0f, -5.0f);
    glTexCoord2f(1.0f+s, 1.0f+s); glVertex3f( 5.0f,  0.0f, -5.0f);
    glTexCoord2f(1.0f+s, 0.0f+s); glVertex3f(-5.0f,  0.0f, -5.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}


void gl_ortho_begin(unsigned size_x, unsigned size_y)
{
    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.375f, 0.375f, 0.0f);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, size_x, 0, size_y);
    glMatrixMode(GL_MODELVIEW);
    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}

void string_render(const char *string, GLint x, GLint y)
{
    unsigned i;
    glRasterPos2i(x + 1, y - 1);
    for (i = 0; string[i]; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
    }
}

void gl_ortho_end(void)
{
    glPopAttrib();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}



void reshape(int width, int height)
{
  int screenWidth  = width;
  int screenHeight = height;
  
  if (screenWidth <= 0 || screenHeight <=0)
	return;
  
  glViewport(0, 0, screenWidth, screenHeight);
  glutReshapeWindow(screenWidth, screenHeight);
  
  pD.aspect = (float)screenWidth / screenHeight;
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(pD.fieldOfView, pD.aspect, pD.nearPlane, pD.farPlane);
  
  // set basic matrix mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}



int loadtexture(const char *fileName)
{
	BITMAPFILEHEADER1 texInfo;
	GLubyte *bits;
	GLuint id;

	bits = LoadBitmapFile(fileName,&texInfo);
	glGenTextures(1,&id);
	printf("id=%d............\n",id);

	glBindTexture(GL_TEXTURE_2D,id);
		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR		
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_NEAREST
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, texInfo.biWidth, texInfo.biHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bits);

	return id;
}

void initDisplay()
{
  // Perspective projection parameters
  pD.fieldOfView = 45.0;
  pD.aspect      = (float)IMAGE_WIDTH/IMAGE_HEIGHT;
  pD.nearPlane   = 0.1;
  pD.farPlane    = 50.0;

  // setup context
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(pD.fieldOfView, pD.aspect, pD.nearPlane, pD.farPlane);

  // set basic matrix mode
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);

  glClearColor(1.0, 1.0, 1.0, 1.0);
  glClearIndex(0);
  glClearDepth(1);

}


void prepareTexture(int w, int h, unsigned char* data, GLuint* textureID) {

    /* Create and load texture to OpenGL */
    glGenTextures(1, textureID); /* Texture name generation */
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_NEAREST
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        w, h,
        0, GL_RGBA, GL_UNSIGNED_BYTE,
        data);
    glGenerateMipmap(GL_TEXTURE_2D);

}


void initTexture()
{

//     int w, h, id;
//     unsigned char* data;
// 
//     // load image first so that window opens with image size
//     id = ilLoadImage("wood.bmp");
//     // image not loaded
//     if (id == 0)
//         return;
// 
//     ilBindImage(id);
//     w = ilGetInteger(IL_IMAGE_WIDTH);
//     h = ilGetInteger(IL_IMAGE_HEIGHT);
//     data = ilGetData();
// 
//     prepareTexture(w, h, data, &texName1);
//     glBindTexture(GL_TEXTURE_2D, texName1);
//     glEnable(GL_TEXTURE_2D);
// 
//     // load image first so that window opens with image size
//     id = ilLoadImage("l3d.bmp");
//     // image not loaded
//     if (id == 0)
//         return;
// 
//     ilBindImage(id);
//     w = ilGetInteger(IL_IMAGE_WIDTH);
//     h = ilGetInteger(IL_IMAGE_HEIGHT);
//     data = ilGetData();
// 
//     prepareTexture(w, h, data, &texName2);
//     glBindTexture(GL_TEXTURE_2D, texName2);
//     glEnable(GL_TEXTURE_2D);


    glActiveTextureARB(GL_TEXTURE0_ARB);
    texName1 = loadtexture(".\\wood.bmp");
    glBindTexture(GL_TEXTURE_2D, texName1);
    glEnable(GL_TEXTURE_2D);
    
    glActiveTextureARB(GL_TEXTURE1_ARB);
    texName2 = loadtexture(".\\l3d.bmp");
    glBindTexture(GL_TEXTURE_2D, texName2);
    glEnable(GL_TEXTURE_2D);
}


void display(void)
{  
    glutSetWindow(window);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    setUserView();

    glPushMatrix();
    glTranslatef(11, 0, -11);
    //drawPlaneTexture();
    glPopMatrix();
//    initTexture();

    // part 0 ---------------------------
    //glUseProgramObjectARB(p1);
    glUseProgram(p0);

    // uniform

    // draw
    glPushMatrix();
    glTranslatef(-11,0,0);
    drawGrid();
    glPopMatrix();


    // part 1 ---------------------------
    //glUseProgramObjectARB(p1);
    glUseProgram(p1);

    // uniform

    // draw
    glPushMatrix();
    drawPlane();
    glTranslatef(0,-4,0);
    glScalef(4,-4,4);
    ply->draw();
    glPopMatrix();


    // part 2 ---------------------------
    //glUseProgramObjectARB(p2);
    glUseProgram(p2);

    // uniform

    // draw
    glPushMatrix();
    glTranslatef(11, 0, 0);
    drawPlane();
    glTranslatef(0,-4,0);
    glScalef(-4,-4,4);
    ply->draw();
    glPopMatrix();


    // part 3 ---------------------------
    //glUseProgramObjectARB(p1);
    glUseProgram(p3);

    // uniform
    tg += 0.01f;
    if (tg>1.1f){
        tg = 0.0f;
    }
    glUniform1f(timeloc_part3, tg);

    // draw
    glPushMatrix();
    glTranslatef(-11,0,-11);
    drawGrid();
    glPopMatrix();


    // part 4 ---------------------------
    //glUseProgramObjectARB(p2);
    glUseProgram(p4);

    // uniform
    glUniform1f(timeloc_part4, tg);

    // draw
    glPushMatrix();
    glTranslatef(0, 0, -11);
    drawPlane();
    glPopMatrix();


    // part 5 ---------------------------
    //glUseProgramObjectARB(p2);
    initTexture();
    glUseProgram(p5);

    // uniform
    tp5 += 0.05f;
    if (tp5 > 5.0f)
    {
        tp5 = -0.0f;
    }
    glUniform1f(timeloc_part5, tp5);
    glUniform1i(woodloc, 0);
    glUniform1i(l3dloc, 1);

    // draw
    glPushMatrix();
    glTranslatef(11, 0, -11);
    drawPlane();
    glPopMatrix();


    // string render
    gl_ortho_begin(IMAGE_WIDTH, IMAGE_HEIGHT);
    glColor3f(0.0f, 0.0f, 0.0f);
    string_render("Computer Graphics, How To Control Shading Using OpenGL?", 20, 580);
    string_render("v: start/end screenshot", 20, 550);
    gl_ortho_end();


    // screenshot-----------------------------------------------------
    static int count = 0;

    if (recording == 1) {
        count++;
        string filename = "buffer";

        char ext2[5];
        //glRotated(1.0, 0.0, 0.0, 1.0);
        sprintf(ext2,"%04d", count);
        screenShot(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, filename + ext2);
    }
    // screenshot-----------------------------------------------------



    glutSwapBuffers();
}

void idle_callback(void)
{
  glutPostRedisplay();
}

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void setShadersPart0() 
{
	char *vs = NULL,*fs = NULL;

	v0 = glCreateShader(GL_VERTEX_SHADER);
	f0 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("part0_opengl.vert");
	fs = textFileRead("part0_opengl.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v0, 1, &vv,NULL);
	glShaderSource(f0, 1, &ff,NULL);

	free(vs);free(fs);

	glCompileShader(v0);
	glCompileShader(f0);

	printShaderInfoLog(v0);
	printShaderInfoLog(f0);

	p0 = glCreateProgram();
	glAttachShader(p0,v0);
	glAttachShader(p0,f0);  

	glLinkProgram(p0);
	printProgramInfoLog(p0);

	//glUseProgram(p0);

	// uniform

}

void setShadersPart1() 
{
	printf("Part 1\n");
	char *vs = NULL,*fs = NULL;

	v1 = glCreateShader(GL_VERTEX_SHADER);
	f1 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("part1_spotlight.vert");
	fs = textFileRead("part1_spotlight.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v1, 1, &vv,NULL);
	glShaderSource(f1, 1, &ff,NULL);

	free(vs);free(fs);

	glCompileShader(v1);
	glCompileShader(f1);

	printShaderInfoLog(v1);
	printShaderInfoLog(f1);

	p1 = glCreateProgram();
	glAttachShader(p1,v1);
	glAttachShader(p1,f1);

	glLinkProgram(p1);
	printProgramInfoLog(p1);

	//glUseProgram(p1);

	// uniform

}


void setShadersPart2() 
{
	printf("Part 2\n");
	char *vs = NULL,*fs = NULL;

	v2 = glCreateShader(GL_VERTEX_SHADER);
	f2 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("part2_toonf2.vert");
	fs = textFileRead("part2_toonf2.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v2, 1, &vv, NULL);
	glShaderSource(f2, 1, &ff, NULL);

	free(vs);free(fs);

	glCompileShader(v2);
	glCompileShader(f2);

	printShaderInfoLog(v2);
	printShaderInfoLog(f2);

	p2 = glCreateProgram();
	glAttachShader(p2,v2);
	glAttachShader(p2,f2);

	glLinkProgram(p2);
	printProgramInfoLog(p2);

	//glUseProgram(p2);

	// uniform

}


void setShadersPart3()
{
	printf("Part 3\n");
	char *vs = NULL,*fs = NULL;

	v3 = glCreateShader(GL_VERTEX_SHADER);
	f3 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("part3_wave.vert");
	fs = textFileRead("part3_wave.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v3, 1, &vv,NULL);
	glShaderSource(f3, 1, &ff,NULL);

	free(vs);free(fs);

	glCompileShader(v3);
	glCompileShader(f3);

	printShaderInfoLog(v3);
	printShaderInfoLog(f3);

	p3 = glCreateProgram();
	glAttachShader(p3,v3);
	glAttachShader(p3,f3);

	glLinkProgram(p3);
	printProgramInfoLog(p3);

	//glUseProgram(p3);

	// uniform

	timeloc_part3 = glGetUniformLocation(p3, "t");
}


void setShadersPart4() 
{
	printf("Part 4\n");
	char *vs = NULL,*fs = NULL;

	v4 = glCreateShader(GL_VERTEX_SHADER);
	f4 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("part4_wave.vert");
	fs = textFileRead("part4_wave.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v4, 1, &vv, NULL);
	glShaderSource(f4, 1, &ff, NULL);

	free(vs);free(fs);

	glCompileShader(v4);
	glCompileShader(f4);

	printShaderInfoLog(v4);
	printShaderInfoLog(f4);

	p4 = glCreateProgram();
	glAttachShader(p4,v4);
	glAttachShader(p4,f4);

	glLinkProgram(p4);
	printProgramInfoLog(p4);

	//glUseProgram(p4);

	// uniform
	timeloc_part4 = glGetUniformLocation(p4, "t");
}

void setShadersPart5() 
{
	printf("Part 5\n");

	char *vs = NULL,*fs = NULL;

	v5 = glCreateShader(GL_VERTEX_SHADER);
	f5 = glCreateShader(GL_FRAGMENT_SHADER);

	vs = textFileRead("part5_l3d.vert");
	fs = textFileRead("part5_l3d.frag");

	const char * vv = vs;
	const char * ff = fs;

	glShaderSource(v5, 1, &vv, NULL);
	glShaderSource(f5, 1, &ff, NULL);

	free(vs);free(fs);

	glCompileShader(v5);
	glCompileShader(f5);

	printShaderInfoLog(v5);
	printShaderInfoLog(f5);

	p5 = glCreateProgram();
	glAttachShader(p5,v5);
	glAttachShader(p5,f5);

	glLinkProgram(p5);
	printProgramInfoLog(p5);

    //glUseProgram(p5);

	// uniform
	timeloc_part5 = glGetUniformLocation(p5, "t");
    woodloc = glGetUniformLocation(p5, "tex");
    l3dloc = glGetUniformLocation(p5, "l3d");


}

int main(int argc, char **argv)
{

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(IMAGE_WIDTH, IMAGE_HEIGHT);
	
  window = glutCreateWindow("How To Control Shading Using OpenGL?");


  FILE *in;
  char *filename = NULL;
  filename = "bunny.ply";

  if (!(in = fopen(filename, "r"))) {
      fprintf(stderr, "Cannot open input file %s.\n", filename);
      exit(1);
  }
  ply = new PLYObject(in);
  ply->resize();


  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(readKeyboard);
  glutMouseFunc(mouseButtHandler);
  glutMotionFunc(mouseMoveHandler);
  glutPassiveMotionFunc(mouseMoveHandler);
  glutSpecialFunc(special_callback);
  glutIdleFunc(idle_callback);

  glewInit();

  if (glewIsSupported("GL_VERSION_2_0"))
  {
      printf("Ready for OpenGL 2.0\n");
  }
  else 
  {
	  printf("OpenGL 2.0 not supported\n");
	  exit(1);
  }

  setShadersPart0();
  setShadersPart1();
  setShadersPart2();
  setShadersPart3();
  setShadersPart4();
  setShadersPart5();


  initDisplay();
  initTexture();

  glutMainLoop();
  
  delete(ply);
  return 0;             /* ANSI C requires main to return int. */
}
