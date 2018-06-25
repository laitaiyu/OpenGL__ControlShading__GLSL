
#ifndef INPUTMODULE_H
#define INPUTMODULE_H

#include <stdlib.h>
#include "./GL/glut.h"

extern GLfloat current_pos[];

#ifdef __cplusplus
extern "C" {
#endif
  
void readKeyboard(unsigned char key, int x, int y);
void mouseButtHandler(int button, int state, int x, int y);
void mouseMoveHandler(int x, int y);
void setUserView();
void special_callback(int key, int x, int y);

#ifdef __cplusplus
}
#endif


#endif









