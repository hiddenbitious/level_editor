/********************
*                   *
*   NeHeGL Header   *
*                   *
*********************/

#ifndef GL_FRAMEWORK__INCLUDED
#define GL_FRAMEWORK__INCLUDED

#include <windows.h>								// Header File For Windows
#include <vector>
using namespace std;

//Some easier defines for my little brain
#define LEFT	WM_LBUTTONDOWN		//Left mouse button
#define RIGHT	WM_RBUTTONDOWN		//Right mouse button
#define MIDDLE	WM_MBUTTONDOWN		//Middle mouse button

typedef struct {									// Structure For Keyboard Stuff
	BOOL keyDown [256];								// Holds TRUE / FALSE For Each Key
} Keys;												// Keys

typedef struct {									// Contains Information Vital To Applications
	HINSTANCE		hInstance;						// Application Instance
	const char*		className;						// Application ClassName
} Application;										// Application

typedef struct {									// Window Creation Info
	Application*		application;				// Application Structure
	char*				title;						// Window Title
	int					width;						// Width
	int					height;						// Height
	int					bitsPerPixel;				// Bits Per Pixel
	BOOL				isFullScreen;				// FullScreen?
} GL_WindowInit;									// GL_WindowInit

typedef struct {									// Contains Information Vital To A Window
	Keys*				keys;						// Key Structure
	HWND				hWnd;						// Window Handle
	HDC					hDC;						// Device Context
	HGLRC				hRC;						// Rendering Context
	GL_WindowInit		init;						// Window Init
	BOOL				isVisible;					// Window Visible?
	DWORD				lastTickCount;				// Tick Counter
} GL_Window;										// GL_Window

void TerminateApplication (GL_Window* window);		// Terminate The Application

void ToggleFullscreen (GL_Window* window);			// Toggle Fullscreen / Windowed Mode

// These Are The Function You Must Provide
BOOL Initialize (GL_Window* window, Keys* keys);	// Performs All Your Initialization

void Deinitialize (void);							// Performs All Your DeInitialization

void Update ( GLint w , GLint h );					// Perform Motion Updates

void Draw (GLint w , GLint h);									// Perform All Your Scene Drawing
void drawGrid ( void);
void drawMouse ( void );
GLvoid glPrint(GLint x, GLint y, const char *string, ...);		// Where The Printing Happens

void Selection ( UINT );								// Perform Selection


void saveMap ( void );
bool readMap ( void );

//Globals
extern int mouse_x;
extern int mouse_y;

extern int windowSize_x;
extern int windowSize_y;

extern float tileSize;
extern int xTiles;
extern int yTiles;

extern float selections[8+1][3];					//Hold the colors for each tile type

#endif												// GL_FRAMEWORK__INCLUDED
