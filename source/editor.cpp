/**************************************
*                                     *
*   Jeff Molofee's Picking Tutorial   *
*          nehe.gamedev.net           *
*                2001                 *
*                                     *
**************************************/

#include <windows.h>											// Header File For Windows
#include <stdio.h>												// Header File For Standard Input / Output
#include <stdarg.h>												// Header File For Variable Argument Routines
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include <time.h>												// For Random Seed
#include "NeHeGL.h"												// Header File For NeHeGL
#include "popUp.h"
#include "tile.h"

#pragma comment( lib, "opengl32.lib" )							// Search For OpenGL32.lib While Linking
#pragma comment( lib, "glu32.lib" )								// Search For GLu32.lib While Linking
//#pragma comment( lib, "winmm.lib" )								// Search For WinMM Library While Linking

#ifndef		CDS_FULLSCREEN										// CDS_FULLSCREEN Is Not Defined By Some
#define		CDS_FULLSCREEN 4									// Compilers. By Defining It This Way,
#endif															// We Can Avoid Errors

popUp *popUps = NULL;

GL_Window*	g_window;
Keys*		g_keys;

// User Defined Variables
GLuint		base;												// Font Display List

#define MAX_PARAMETER_LENGTH			256	//Maximum length of a parameter. Resize it to anythinh wanted.

float tileSize = 10;						//Size of the tile (rectangle)
int mouseTile_x , mouseTile_y;				//Which tile is the mouse over.
int crosshairSize = 5;						//Size of the crosshair
int xTiles , yTiles;						//Number of tiles per each axis
tile tiles[80][50];							//Holds the tile data
int tileSelection = 1;						//Default tile type
int linesPerTile = 3;						//Number of thin lines per tile (only 2 is accurate enough).
float selections[8+1][3];
int maxSelections = 8;						//Number of different tile types.
vector < string > options;					//Text for the popUp.

string command;
bool enterCommand = false;
bool showCommand = false;



typedef struct													// Create A Structure
{
	GLubyte	*imageData;											// Image Data (Up To 32 Bits)
	GLuint	bpp;												// Image Color Depth In Bits Per Pixel.
	GLuint	width;												// Image Width
	GLuint	height;												// Image Height
	GLuint	texID;												// Texture ID Used To Select A Texture
} TextureImage;													// Structure Name

TextureImage textures[2];										// Storage For 10 Textures


void drawPopUps ( void )
{
	if ( popUps == NULL )
		return;

//	for ( int i = 0 ; i < popUps.size () ; i++ )
		popUps->draw ();
}

bool LoadTGA(TextureImage *texture, char *filename)				// Loads A TGA File Into Memory
{
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};		// Uncompressed TGA Header
	GLubyte		TGAcompare[12];									// Used To Compare TGA Header
	GLubyte		header[6];										// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;									// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;										// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;											// Temporary Variable
	GLuint		type=GL_RGBA;									// Set The Default GL Mode To RBGA (32 BPP)

	FILE *file = fopen(filename, "rb");							// Open The TGA File

	if(	file==NULL ||											// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL)										// Did The File Even Exist? *Added Jim Strong*
			return FALSE;										// Return False
		else													// Otherwise
		{
			fclose(file);										// If Anything Failed, Close The File
			return FALSE;										// Return False
		}
	}

	texture->width  = header[1] * 256 + header[0];				// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];				// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texture->width	<=0	||									// Is The Width Less Than Or Equal To Zero
		texture->height	<=0	||									// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))						// Is The TGA 24 or 32 Bit?
	{
		fclose(file);											// If Anything Failed, Close The File
		return FALSE;											// Return False
	}

	texture->bpp	= header[4];								// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;							// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture->width*texture->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture->imageData=(GLubyte *)malloc(imageSize);			// Reserve Memory To Hold The TGA Data

	if(	texture->imageData==NULL ||								// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture->imageData!=NULL)							// Was Image Data Loaded
			free(texture->imageData);							// If So, Release The Image Data

		fclose(file);											// Close The File
		return FALSE;											// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)			// Loop Through The Image Data
	{															// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texture->imageData[i];								// Temporarily Store The Value At Image Data 'i'
		texture->imageData[i] = texture->imageData[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
		texture->imageData[i + 2] = temp;						// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);												// Close The File

	// Build A Texture From The Data
	glGenTextures(1, &texture[0].texID);						// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture[0].texID);				// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	
	if (texture[0].bpp==24)										// Was The TGA 24 Bits
	{
		type=GL_RGB;											// If So Set The 'type' To GL_RGB
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture[0].imageData);

	return true;												// Texture Building Went Ok, Return True
}


GLvoid BuildFont(GLvoid)										// Build Our Font Display List
{
	base=glGenLists(95);										// Creating 95 Display Lists
	glBindTexture(GL_TEXTURE_2D, textures[9].texID);			// Bind Our Font Texture
	for (int loop=0; loop<95; loop++)							// Loop Through All 95 Lists
	{
		float cx=float(loop%16)/16.0f;							// X Position Of Current Character
		float cy=float(loop/16)/8.0f;							// Y Position Of Current Character

		glNewList(base+loop,GL_COMPILE);						// Start Building A List
			glBegin(GL_QUADS);									// Use A Quad For Each Character
				glTexCoord2f(cx,         1.0f-cy-0.120f); glVertex2i(0,0);	// Texture / Vertex Coord (Bottom Left)
				glTexCoord2f(cx+0.0625f, 1.0f-cy-0.120f); glVertex2i(16,0);	// Texutre / Vertex Coord (Bottom Right)
				glTexCoord2f(cx+0.0625f, 1.0f-cy);		  glVertex2i(16,16);// Texture / Vertex Coord (Top Right)
				glTexCoord2f(cx,         1.0f-cy);		  glVertex2i(0,16);	// Texture / Vertex Coord (Top Left)
			glEnd();											// Done Building Our Quad (Character)
			glTranslated(10,0,0);								// Move To The Right Of The Character
		glEndList();											// Done Building The Display List
	}															// Loop Until All 256 Are Built
}

GLvoid glPrint(GLint x, GLint y, const char *string, ...)		// Where The Printing Happens
{
	char		text[256];										// Holds Our String
	va_list		ap;												// Pointer To List Of Arguments

	if (string == NULL)											// If There's No Text
		return;													// Do Nothing

	va_start(ap, string);										// Parses The String For Variables
	    vsprintf(text, string, ap);								// And Converts Symbols To Actual Numbers
	va_end(ap);													// Results Are Stored In Text

	glEnable ( GL_TEXTURE_2D );
	glBindTexture(GL_TEXTURE_2D, textures[1].texID);			// Select Our Font Texture
	glPushMatrix();												// Store The Modelview Matrix
	glLoadIdentity();											// Reset The Modelview Matrix
	glTranslated(x,y,0);										// Position The Text (0,0 - Bottom Left)
	glListBase(base-32);										// Choose The Font Set
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);			// Draws The Display List Text
	glPopMatrix();												// Restore The Old Projection Matrix
	glDisable ( GL_TEXTURE_2D );
}

BOOL Initialize (GL_Window* window, Keys* keys)					
{
	g_window	= window;
	g_keys		= keys;

	srand( (unsigned)time( NULL ) );							

	if (//!LoadTGA(&textures[0],"Data/crosshair.tga") ||
		(!LoadTGA(&textures[1],"Data/Font.tga")))				
	{
		return FALSE;											
	}

	BuildFont();

	if ( !readMap () )
	{
//		tiles = new tile ( 0 , 0 ) [ xTiles * yTiles ];
//		ZeroMemory ( tiles , xTiles * yTiles * sizeof ( tile ) );
	}

	//Color for each tile type
	selections[0][0] = 1.0f;	selections[0][1] = 0.0f;	selections[0][2] = 0.0f;	//Wall
	selections[1][0] = 0.0f;	selections[1][1] = 1.0f;	selections[1][2] = 0.0f;	//Staircase
	selections[2][0] = 0.0f;	selections[2][1] = 0.0f;	selections[2][2] = 1.0f;	//Door
	selections[3][0] = 0.0f;	selections[3][1] = 0.0f;	selections[3][2] = 0.0f;	//Monster (type can be a parameter)
	selections[4][0] = 1.0f;	selections[4][1] = 0.0f;	selections[4][2] = 1.0f;	//Whatever
	selections[5][0] = 1.0f;	selections[5][1] = 1.0f;	selections[5][2] = 0.0f;
	selections[6][0] = 0.0f;	selections[6][1] = 1.0f;	selections[6][2] = 1.0f;
	selections[7][0] = 1.0f;	selections[7][1] = 1.0f;	selections[7][2] = 1.0f;
//Tile type 0 with parameter.
	selections[8][0] = 0.9f;	selections[8][1] = 0.9f;	selections[8][2] = 0.9f;


	//Text for the popUp window
	options.push_back ( "Red" );
	options.push_back ( "Green" );
	options.push_back ( "Blue" );
	options.push_back ( "Black" );
	options.push_back ( "Magenta" );
	options.push_back ( "Yellow" );
	options.push_back ( "Cyan" );
	options.push_back ( "White" );


//OGL stuff
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);				
	glClearDepth(1.0f);									
	glDepthFunc(GL_LEQUAL);								
	glEnable(GL_DEPTH_TEST);							
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	glEnable(GL_BLEND);									
//	glAlphaFunc(GL_GREATER,0.1f);						
//	glEnable(GL_ALPHA_TEST);							
	glEnable(GL_TEXTURE_2D);							
	glDisable(GL_CULL_FACE);							

	return TRUE;										
}

void Deinitialize (void)										
{
	glDeleteLists(base,95);										

	if ( popUps != NULL )
		delete popUps;
}

void Selection( UINT mouseButton )								
{
	static old_mouse_x , old_mouse_y;

//CTRL + S
	if ( g_keys->keyDown[VK_CONTROL] && mouseButton == char('S') )
	{
		saveMap ();
	}


//CTRL + L CLICK
	if ( g_keys->keyDown[VK_CONTROL] && mouseButton == LEFT )
	{
		old_mouse_x = mouseTile_x;
		old_mouse_y = mouseTile_y;

		tiles[mouseTile_x][mouseTile_y].setCoordX ( mouseTile_x );
		tiles[mouseTile_x][mouseTile_y].setCoordY ( mouseTile_y );
		command = tiles[mouseTile_x][mouseTile_y].getParameter ();
		enterCommand = true;
	}

	if ( enterCommand == true )
	{
	//BackSpace
		if ( mouseButton == 8 )
		{
			if ( command.size () )
				command.erase ( command.end () - 1 );
		}
	//key is :			a char (A-Z)					OR					a digit (0-9)				OR		spacebar
		else if ( ( mouseButton >= 65 && mouseButton <= 90 ) || ( mouseButton <= 57 && mouseButton >= 48 ) || mouseButton == 32 &&
					mouseButton != LEFT && mouseButton != MIDDLE && mouseButton != RIGHT )
		{
			command.push_back ( (char)mouseButton );
		}
	//Enter
		else if ( mouseButton == 13 )
		{
			tiles[old_mouse_x][old_mouse_y].setParameter ( command );
			command.erase ();
			enterCommand = false;
		}

	//Don't go any further
		return;
	}

	if ( mouseButton == LEFT )
	{
		if ( popUps != NULL )
		{
			if ( popUps->hasOptions () )
				tileSelection = popUps->chooseOption ();
			delete popUps;
			popUps = NULL;
		}
		else
		{
			tiles[mouseTile_x][mouseTile_y].setCoordX ( mouseTile_x );
			tiles[mouseTile_x][mouseTile_y].setCoordY ( mouseTile_y );
			tiles[mouseTile_x][mouseTile_y].setType ( tileSelection );
		}
	}
	else if ( mouseButton == RIGHT )
	{
			tiles[mouseTile_x][mouseTile_y].setType ( 0 );
			tiles[mouseTile_x][mouseTile_y].setParameter ( "" );
	}
	else if ( mouseButton == MIDDLE )
	{
		if ( popUps != NULL )
		{
			delete popUps;
			popUps = NULL;
		}

		popUps = new popUp ( options , true );
	}
}

void Update( GLint w , GLint h )								
{
	if (g_keys->keyDown[VK_ESCAPE])								
	{
		TerminateApplication (g_window);						
	}

	if (g_keys->keyDown[VK_F1])									
	{
		ToggleFullscreen (g_window);							
	}
}

void Draw(GLint w , GLint h)									
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		
	glLoadIdentity();											

	RECT window;												// Storage For Window Dimensions
	GetClientRect (g_window->hWnd,&window);						// Get Window Dimensions

	mouseTile_x = min ( mouse_x/tileSize , xTiles-1 );
	mouseTile_y = min ( (window.bottom-mouse_y)/tileSize , yTiles-1 );


//Automatically print parameters under mouse
	if ( tiles[mouseTile_x][mouseTile_y].hasParameter == true && showCommand == false )
	{
		showCommand = true;
		command = tiles[mouseTile_x][mouseTile_y].getParameter ();
	}
	else if ( tiles[mouseTile_x][mouseTile_y].hasParameter == false && showCommand == true )
	{
		showCommand = false;
		if ( popUps != NULL )
		{
			delete popUps;
			popUps = NULL;
		}
	}


	drawGrid ( );
	
	glEnable ( GL_ALPHA );

	glColor3f ( 0.0 , 0.0 , 0.0 );
	glPrint ( 0 , window.bottom-20 , "Tile x: %i Tile y: %i Type: %i" , mouseTile_x , mouseTile_y , tiles[mouseTile_x][mouseTile_y].getType () );
	if ( tileSelection > 0 )
	{
		glColor3fv ( selections[tileSelection-1] );
		glPrint ( 0 , window.bottom-40 , "Current tile type: %s" , options[tileSelection-1].c_str () );
	}
	else
	{
		glColor3f ( 0.0 , 0.0 , 0.0 );
		glPrint ( 0 , window.bottom-40 , "Current tile type: Erase" );
	}
	glColor3f ( 0.0 , 0.0 , 0.0 );
	glPrint ( 0 , window.bottom-60 , "Tile size: %g" , tileSize );



//Field for messages
	glColor3f ( 0.5 , 0.5 , 1.0 );
	glBegin ( GL_QUADS );
		glVertex2i ( 22 * tileSize , yTiles * tileSize );
		glVertex2i ( windowSize_x , yTiles * tileSize );
		glVertex2i ( windowSize_x , yTiles * tileSize + 20 );
		glVertex2i ( 22 * tileSize , yTiles * tileSize + 20 );
	glEnd ();

//Print what the user is typing
	if ( enterCommand || showCommand )
	{
		glColor3f ( 0.0 , 0.0 , 0.0 );
		glPrint ( 22 * tileSize , yTiles * tileSize + 2 , "Parameter: %s" , command.c_str () );
	}

//Prints a pop up under mouse with parameter for a tile
	if ( showCommand && popUps == NULL )
		popUps = new popUp ( tiles[mouseTile_x][mouseTile_y].getParameter () , false );

	drawPopUps ( );

//Meh...keep windows' mouse.
//If you want to use to don't forget to load the texture in Initialize func and
//hide windows mouse in CreateWindowGL with ShowCursor(FALSE)
	//drawMouse ();


	glFlush();													
}


void drawGrid ( void )
{
	float w = xTiles * tileSize;
	float h = yTiles * tileSize;
	float step = tileSize / linesPerTile;

	glBegin ( GL_LINES );
	glColor3f ( 0.9f , 0.9f , 0.9f );
//Draw thin lines
//These are bound to loose precision
	for ( float y = 0 ; y < h ; y += step )
	{
		glVertex3f ( 0 , y , 0 );
		glVertex3f ( w , y , 0 );
	}
	for ( float x = 0 ; x < w ; x += step )
	{
		glVertex3f ( x , 0 , 0 );
		glVertex3f ( x , h , 0 );
	}

//Draw bold lines
	glColor3f ( 0.7f , 0.7f , 0.7f );
	for ( x = 0 ; x < xTiles ; x++ )
	{
		for ( y = 0 ; y < yTiles ; y++ )
		{
			glVertex2f ( x * tileSize , y * tileSize );
			glVertex2f ( (x+1) * tileSize , y * tileSize );

			glVertex2f ( (x+1) * tileSize , y * tileSize );
			glVertex2f ( (x+1) * tileSize , (y+1) * tileSize );

			glVertex2f ( (x+1) * tileSize , (y+1) * tileSize );
			glVertex2f ( x * tileSize , (y+1) * tileSize );

			glVertex2f ( x * tileSize , (y+1) * tileSize );
			glVertex2f ( x * tileSize , y * tileSize );
		}
	}
	glEnd ();

//Draw marked tiles
	for ( int tile_x = 0 ; tile_x < xTiles ; tile_x++ )
	{
		for ( int tile_y = 0 ; tile_y < yTiles ; tile_y++ )
		{
			tiles[tile_x][tile_y].draw ();
		}
	}
}

void drawMouse ( void )
{
	glEnable ( GL_TEXTURE_2D );
	glEnable ( GL_ALPHA );
	glTranslated(mouse_x,windowSize_y-mouse_y,0.0f);		
	glBindTexture(GL_TEXTURE_2D, textures[0].texID);		

	glColor3f ( 1.0f , 0.0f , 0.0f );
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f,0.0f); glVertex2i(-crosshairSize , -crosshairSize );
		glTexCoord2f(1.0f,0.0f); glVertex2i( crosshairSize , -crosshairSize );
		glTexCoord2f(1.0f,1.0f); glVertex2i( crosshairSize , crosshairSize );
		glTexCoord2f(0.0f,1.0f); glVertex2i(-crosshairSize , crosshairSize );
	glEnd();

	glDisable ( GL_TEXTURE_2D );
	glDisable ( GL_ALPHA );
}


void saveMap ( void )
{
	int nTiles = 0 , x , y;

	FILE *fd = fopen ( "map.txt" , "w" );

	if ( fd == NULL )
		return;

//Count tiles
	for ( x = 0 ; x < xTiles ; x++ )
	{
		for ( y = 0 ; y < yTiles ; y++ )
		{
			if ( tiles[x][y].getType () || tiles[x][y].hasParameter )
				nTiles++;
		}
	}

	fprintf ( fd , "%d\n" , xTiles );			//Save x dimension
	fprintf ( fd , "%d\n" , yTiles );			//Save y dimension
	fprintf ( fd , "%d\n" , nTiles );			//Save how many tiles will be saved
	fprintf ( fd , "%f\n" , tileSize );			//Save tile's dimensions

	for ( x = 0 ; x < xTiles ; x++ )
	{
		for ( y = 0 ; y < yTiles ; y++ )
		{
		//Write only non zero tiles.
			if ( tiles[x][y].getType () )
			{
				fprintf ( fd , "%d %d %d" , x , y , tiles[x][y].getType () );

				if ( tiles[x][y].hasParameter )
					fprintf ( fd , " %s\n" , tiles[x][y].getParameter ().c_str() );
				else
					fprintf ( fd , "\n" );
			}
		//And the empty tiles with a parameter
			else if ( tiles[x][y].hasParameter )
			{
				fprintf ( fd , "%d %d %d" , x , y , 0 );
				fprintf ( fd , " %s\n" , tiles[x][y].getParameter ().c_str() );
			}
		}
	}

	fclose ( fd );
}

bool readMap ( void )
{
	int _xTiles , _yTiles , _tileSize , tmp , nTiles , _x , _y;
	int i , c;
	char buf[MAX_PARAMETER_LENGTH];

	FILE *fd;

	if ( (fd = fopen( "map.txt" , "r" ) ) == NULL )
		return false;

	fscanf ( fd , "%d" , &_xTiles );		//Read size on x.
	fscanf ( fd , "%d" , &_yTiles );		//Read size on y.
	fscanf ( fd , "%d" , &nTiles );			//Read number of tiles stored in file.
	fscanf ( fd , "%f" , &_tileSize );		//Read tile size (not used).

	xTiles = max ( _xTiles , xTiles );
	yTiles = max ( _yTiles , yTiles );

	for ( i = 0 ; i < nTiles ; i++ )
	{
		fscanf ( fd , "%d" , &_x );			//Read x coords
		fscanf ( fd , "%d" , &_y );			//Read y coords
		c = fscanf ( fd , "%d" , &tmp );	//Read tile type

		tiles[_x][_y].setType ( tmp );
		tiles[_x][_y].setCoordX ( _x );
		tiles[_x][_y].setCoordY ( _y );

	//There's a parameter. Read it.
		if ( !c )
		{
		//fgets doesn't stop at white spaces but it stops at '\n'
			fgets ( buf , MAX_PARAMETER_LENGTH , fd );
			tiles[_x][_y].setParameter ( buf );
		//One loop is lost everytime a parameter is read.
			i--;
		}
	}

	fclose ( fd );
	return true;
}