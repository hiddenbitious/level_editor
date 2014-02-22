#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <vector>
#include <string>
#include <assert.h>
#include <GL/glut.h>

using namespace std;

#define MIN(a,b)                    (( a < b ) ? a : b)
#define MAX(a,b)                    (( a > b ) ? a : b)

GLvoid glPrint(GLint x, GLint y, const char *string, ...);		// Where The Printing Happens

/// Globals
extern int windowSize_x;
extern int windowSize_y;

extern int xTiles;
extern int yTiles;

//Very simple struct to hold vertex information
typedef struct {
	float x , y , z;
} C_Vertex;

//Very simple struct to hold normal information
typedef struct {
	float nx , ny , nz;
} C_Normal;

//Very simple struct to hold texture coordinate information
typedef struct {
	float u , v;
} C_TexCoord;

//Very simple struct to hold color information
typedef struct {
	float r , g , b , a;
} C_Color;

//Very simple struct to hold index information
typedef struct {
	unsigned int p0 , p1 , p2;
} C_TriIndices;


struct poly_t {
	C_Vertex* pVertices;
	C_Vertex* pNorms;
	int nVertices;
	bool usedAsDivider;
};

struct brush_t {
	poly_t* pPolys;
	int nPolys;
};

#endif
