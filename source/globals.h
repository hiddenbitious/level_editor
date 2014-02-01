#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <vector>
#include <string>
#include <GL/glut.h>

using namespace std;

#define MAX_PARAMETER_LENGTH			256   /// Maximum length of a parameter.
#define MAX_SELECTIONS                8   /// Number of different tile types

#define MIN(a,b)                    (( a < b ) ? a : b)
#define MAX(a,b)                    (( a > b ) ? a : b)

GLvoid glPrint(GLint x, GLint y, const char *string, ...);		// Where The Printing Happens

/// Globals
extern int windowSize_x;
extern int windowSize_y;

extern float tileSize;
extern int xTiles;
extern int yTiles;

extern float selections[MAX_SELECTIONS+1][3];			/// Hold the colors for each tile type

#endif
