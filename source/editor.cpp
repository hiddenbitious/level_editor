#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "globals.h"
#include "map.h"
#include "popUp.h"
#include "tile.h"

/// Map dimensions
#define TILES_ON_X                   80
#define TILES_ON_Y                   50

popUp *popUps = NULL;
C_Map *map = NULL;
// User Defined Variables
GLuint		base;									/// Font Display List

/// Tile size (rectangle)
/// This is used for drawing the map grid.
/// It changes when the window is changed size
float tileSize = 10.0f;
int mouseTile_x, mouseTile_y;				   /// Which tile is the mouse over.
tile tiles[TILES_ON_X][TILES_ON_Y];		   /// Holds the tile data
int tileSelection = TILE_WALL;				/// Default tile type
//float selections[MAX_TILE_TYPES+1][3];    /// RGB colors of the available tile types
vector<string> options;					      /// Text for the popUp.

/// When entering a command set this to true
bool enterCommand    = false;
bool showCommand     = false;
bool enterFilename   = false;
bool showFilename    = false;
string command;
string filename;

/// Window dimensions
int windowSize_x = 800;
int windowSize_y = 600;

/// Current mouse coordinates
int mouse_x, mouse_y;
/// Mouse button pressed (-1 for none)
int mouseButton = -1;
/// Active key modifiers (ctrl, alt, shift)
int keyModifiers = 0;
unsigned char keyPressed;

void drawGrid(void);

typedef struct {
    GLubyte	*imageData;							/// Image Data (Up To 32 Bits)
    GLuint	bpp;									/// Image Color Depth In Bits Per Pixel.
    GLuint	width;								/// Image Width
    GLuint	height;								/// Image Height
    GLuint	texID;								/// Texture ID Used To Select A Texture
} TextureImage;									/// Structure Name

TextureImage textures[10];						/// Storage For 10 Textures

bool
LoadTGA(TextureImage *texture, char *filename)				// Loads A TGA File Into Memory
{
    GLubyte		TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};		// Uncompressed TGA Header
    GLubyte		TGAcompare[12];									// Used To Compare TGA Header
    GLubyte		header[6];										// First 6 Useful Bytes From The Header
    GLuint		bytesPerPixel;									// Holds Number Of Bytes Per Pixel Used In The TGA File
    GLuint		imageSize;										// Used To Store The Image Size When Setting Aside Ram
    GLuint		temp;											// Temporary Variable
    GLuint		type = GL_RGBA;									// Set The Default GL Mode To RBGA (32 BPP)

    FILE *file = fopen(filename, "rb");							// Open The TGA File

    if(file == NULL ||											// Does File Even Exist?
            fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
            memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0				||	// Does The Header Match What We Want?
            fread(header, 1, sizeof(header), file) != sizeof(header)) {			// If So Read Next 6 Header Bytes
        if(file == NULL) {									// Did The File Even Exist? *Added Jim Strong*
            return false;    // Return False
        } else {												// Otherwise
            fclose(file);										// If Anything Failed, Close The File
            return false;										// Return False
        }
    }

    texture->width  = header[1] * 256 + header[0];				// Determine The TGA Width	(highbyte*256+lowbyte)
    texture->height = header[3] * 256 + header[2];				// Determine The TGA Height	(highbyte*256+lowbyte)

    if(texture->width	<= 0	||									// Is The Width Less Than Or Equal To Zero
            texture->height	<= 0	||									// Is The Height Less Than Or Equal To Zero
            (header[4] != 24 && header[4] != 32)) {					// Is The TGA 24 or 32 Bit?
        fclose(file);											// If Anything Failed, Close The File
        return false;											// Return False
    }

    texture->bpp	= header[4];								// Grab The TGA's Bits Per Pixel (24 or 32)
    bytesPerPixel	= texture->bpp / 8;							// Divide By 8 To Get The Bytes Per Pixel
    imageSize		= texture->width * texture->height * bytesPerPixel;	// Calculate The Memory Required For The TGA Data

    texture->imageData = (GLubyte *)malloc(imageSize);			// Reserve Memory To Hold The TGA Data

    if(texture->imageData == NULL ||								// Does The Storage Memory Exist?
            fread(texture->imageData, 1, imageSize, file) != imageSize) {	// Does The Image Size Match The Memory Reserved?
        if(texture->imageData != NULL) {						// Was Image Data Loaded
            free(texture->imageData);    // If So, Release The Image Data
        }

        fclose(file);											// Close The File
        return false;											// Return False
    }

    for(GLuint i = 0; i < int(imageSize); i += bytesPerPixel) {		// Loop Through The Image Data
        // Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
        temp = texture->imageData[i];								// Temporarily Store The Value At Image Data 'i'
        texture->imageData[i] = texture->imageData[i + 2];		// Set The 1st Byte To The Value Of The 3rd Byte
        texture->imageData[i + 2] = temp;						// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
    }

    fclose(file);												// Close The File

    // Build A Texture From The Data
    glGenTextures(1, &texture[0].texID);						// Generate OpenGL texture IDs

    glBindTexture(GL_TEXTURE_2D, texture[0].texID);				// Bind Our Texture
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered

    if(texture[0].bpp == 24) {									// Was The TGA 24 Bits
        type = GL_RGB;											// If So Set The 'type' To GL_RGB
    }

    glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture[0].imageData);

    return true;												// Texture Building Went Ok, Return True
}


void
BuildFont(void)										// Build Our Font Display List
{
    base = glGenLists(95);										// Creating 95 Display Lists
    glBindTexture(GL_TEXTURE_2D, textures[9].texID);			// Bind Our Font Texture
    for(int loop = 0; loop < 95; loop++) {						// Loop Through All 95 Lists
        float cx = float(loop % 16) / 16.0f;							// X Position Of Current Character
        float cy = float(loop / 16) / 8.0f;							// Y Position Of Current Character

        glNewList(base + loop, GL_COMPILE);						// Start Building A List
        glBegin(GL_QUADS);									// Use A Quad For Each Character
        glTexCoord2f(cx,         1.0f - cy - 0.120f); glVertex2i(0, 0);	// Texture / Vertex Coord (Bottom Left)
        glTexCoord2f(cx + 0.0625f, 1.0f - cy - 0.120f); glVertex2i(16, 0);	// Texutre / Vertex Coord (Bottom Right)
        glTexCoord2f(cx + 0.0625f, 1.0f - cy);		  glVertex2i(16, 16); // Texture / Vertex Coord (Top Right)
        glTexCoord2f(cx,         1.0f - cy);		  glVertex2i(0, 16);	// Texture / Vertex Coord (Top Left)
        glEnd();											// Done Building Our Quad (Character)
        glTranslated(10, 0, 0);								// Move To The Right Of The Character
        glEndList();											// Done Building The Display List
    }															// Loop Until All 256 Are Built
}

void
glPrint(GLint x, GLint y, const char *string, ...)		// Where The Printing Happens
{
    char		text[256];										// Holds Our String
    va_list		ap;												// Pointer To List Of Arguments

    if(string == NULL) {										// If There's No Text
        return;    // Do Nothing
    }

    va_start(ap, string);										// Parses The String For Variables
    vsprintf(text, string, ap);								// And Converts Symbols To Actual Numbers
    va_end(ap);													// Results Are Stored In Text

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textures[1].texID);			// Select Our Font Texture
    glPushMatrix();												// Store The Modelview Matrix
    glLoadIdentity();											// Reset The Modelview Matrix
    glTranslated(x, y, 0);										// Position The Text (0,0 - Bottom Left)
    glListBase(base - 32);										// Choose The Font Set
    glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);			// Draws The Display List Text
    glPopMatrix();												// Restore The Old Projection Matrix
    glDisable(GL_TEXTURE_2D);
}

static bool
Initialize(const char *mapFileName)
{
    char fontFileName[] = {"Data/Font.tga"};
    if( //!LoadTGA(&textures[0],"Data/crosshair.tga") ||
        (!LoadTGA(&textures[1], fontFileName))) {
        return false;
    }

    BuildFont();
    map = new C_Map();

    if(!map->readMap(mapFileName)) {
        assert(0);
    }

    /// Text for the popUp window
    options.push_back("Red");
    options.push_back("Green");
    options.push_back("Blue");
    options.push_back("Black");
    options.push_back("Magenta");
    options.push_back("Yellow");
    options.push_back("Cyan");
    options.push_back("White");

    /// OGL stuff
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);

    return true;
}

void
Deinitialize(void)
{
    glDeleteLists(base, 95);

    if(popUps) {
        delete popUps;
    }

    if(map) {
        delete map;
    }
}

void
drawPopUps(void)
{
    if(!popUps) {
        return;
    }

    popUps->draw(mouse_x, mouse_y);
}

/// Update pop ups, tiles and interpret pressed keys
void
update(void)
{
    static int old_mouse_x, old_mouse_y;

    /// Ctrl + left button = enter comment
    if(keyModifiers == GLUT_ACTIVE_CTRL && mouseButton == GLUT_LEFT_BUTTON) {
        old_mouse_x = mouseTile_x;
        old_mouse_y = mouseTile_y;

        map->tiles[mouseTile_x][mouseTile_y].setCoordX(mouseTile_x);
        map->tiles[mouseTile_x][mouseTile_y].setCoordY(mouseTile_y);
        command = map->tiles[mouseTile_x][mouseTile_y].getParameter();
        enterCommand = true;
    }

    /// CTRL key is pressed
    if(keyModifiers == GLUT_ACTIVE_CTRL && !enterCommand) {
        switch(keyPressed) {
            /// Ctrl + s
            case 19:
                enterFilename = true;
                break;
        }
    }

    if(enterFilename) {
        if(keyPressed == 8) {
            if(filename.size()) {
                filename.erase(filename.end() - 1);
            }
        }
        /// key is :     a char (a-z)(A-Z)              OR               a digit (0-9)            OR     dot
        else if((keyPressed >= 97 && keyPressed <= 122) || (keyPressed <= 57 && keyPressed >= 48) || keyPressed == '.') {
            filename.push_back(keyPressed);
        }
        /// Enter
        else if(keyPressed == 13) {
            enterFilename = false;
        }

        if(enterFilename == false) {
            string bspFilename = filename;
            bspFilename.append(".bsp\0");
            filename.append(".txt\0");

            map->saveBspGeometryToFile(bspFilename.c_str());
            map->saveMap(filename.c_str());
            filename.erase();
        }
    }

    if(enterCommand) {
        /// BackSpace
        if(keyPressed == 8) {
            if(command.size()) {
                command.erase(command.end() - 1);
            }
        }
        /// key is :     a char (a-z)(A-Z)              OR               a digit (0-9)            OR     space bar
        else if((keyPressed >= 97 && keyPressed <= 122) || (keyPressed <= 57 && keyPressed >= 48) || keyPressed == 32) {
            command.push_back(keyPressed);
        }
        /// Enter
        else if(keyPressed == 13) {
            map->tiles[old_mouse_x][old_mouse_y].setParameter(command);
            command.erase();
            enterCommand = false;
        }

        return;
    }

    switch(mouseButton) {
        case GLUT_LEFT_BUTTON:
            if(popUps != NULL) {
                if(popUps->hasOptions()) {
                    tileSelection = popUps->chooseOption(mouse_x, mouse_y);
                }
                delete popUps;
                popUps = NULL;
            } else {
                map->tiles[mouseTile_x][mouseTile_y].setCoordX(mouseTile_x);
                map->tiles[mouseTile_x][mouseTile_y].setCoordY(mouseTile_y);
                map->tiles[mouseTile_x][mouseTile_y].setType((tileTypes_t)tileSelection);
                map->dirty = true;
            }
            break;

        case GLUT_RIGHT_BUTTON:
            map->tiles[mouseTile_x][mouseTile_y].setType(TILE_0);
            map->tiles[mouseTile_x][mouseTile_y].setParameter("");
            map->dirty = true;
            break;

        case GLUT_MIDDLE_BUTTON:
            if(popUps != NULL) {
                delete popUps;
                popUps = NULL;
            }
            popUps = new popUp(options, true, mouse_x, mouse_y, windowSize_x, windowSize_y);
            break;
    }
}


/// Detect mouse coordinates every time the mouse moves
void
passiveMouseMove(int x, int y)
{
    mouse_x = x;
    mouse_y = y;

    mouseTile_x = MIN(mouse_x / tileSize, TILES_ON_X - 1);
    mouseTile_y = MIN((windowSize_y - mouse_y) / tileSize, TILES_ON_Y - 1);

    keyModifiers = glutGetModifiers();

    /// Redraw scene only when there is an active pop up or
    /// mouse hovers over a tile with a parametere
//   if(popUps || tiles[mouseTile_x][mouseTile_y].hasParameter)
    glutPostRedisplay();
}

/// Detect mouse coordinates while a mouse button is being pressed
void
mouseMove(int x, int y)
{
    mouse_x = x;
    mouse_y = y;

    mouseTile_x = MIN(mouse_x / tileSize, TILES_ON_X - 1);
    mouseTile_y = MIN((windowSize_y - mouse_y) / tileSize, TILES_ON_Y - 1);

    keyModifiers = glutGetModifiers();

    /// Don't update screen if mouse moves but no button is pressed
//   if(mouseButton != -1)
    glutPostRedisplay();
}

/// Detect mouse coordinates and mouse button when a mouse button is pressed
void
mouseClicks(int button, int state, int x, int y)
{
    mouse_x = x;
    mouse_y = y;

    mouseTile_x = MIN(mouse_x / tileSize, TILES_ON_X - 1);
    mouseTile_y = MIN((windowSize_y - mouse_y) / tileSize, TILES_ON_Y - 1);

    /// When button is released set mouseButton to no button
    mouseButton = state == GLUT_DOWN ? button : -1;

    keyModifiers = glutGetModifiers();

    glutPostRedisplay();
}

/// Detect all keys with an ascii code
void
keyboardFunc(unsigned char key, int x, int y)
{
    keyModifiers = glutGetModifiers();

    switch(key) {
        case 27:
            Deinitialize();
            exit(0);
            break;

        default:
            keyPressed = key;
//         printf("Key pressed: %d\n", key);
            glutPostRedisplay();
            break;
    }
}

void
keyboardUpFunc(unsigned char key, int x, int y)
{
    /// Release key
    keyPressed = -1;
}

void
Draw(void)
{
    update();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    /// Automatically print parameters under mouse
    if(map->tiles[mouseTile_x][mouseTile_y].hasParameter == true && !showCommand) {
        showCommand = true;
        command = map->tiles[mouseTile_x][mouseTile_y].getParameter();
    } else if(map->tiles[mouseTile_x][mouseTile_y].hasParameter == false && showCommand) {
        showCommand = false;
        if(popUps != NULL) {
            delete popUps;
            popUps = NULL;
        }
    }

    map->drawGrid(tileSize);

    glEnable(GL_ALPHA);

    glColor3f(0.0f, 0.0f, 0.0f);
    glPrint(0, windowSize_y - 20, "Tile x: %i Tile y: %i Type: %i Area: %d",
            mouseTile_x, mouseTile_y, map->tiles[mouseTile_x][mouseTile_y].getType(), map->tiles[mouseTile_x][mouseTile_y].getArea());
    if(tileSelection > 0) {
        glColor3fv(selections[tileSelection - 1]);
        glPrint(0, windowSize_y - 40, "Current tile type: %s", options[tileSelection - 1].c_str());
    } else {
        glColor3f(0.0f, 0.0f, 0.0f);
        glPrint(0, windowSize_y - 40, "Current tile type: Erase");
    }

    glColor3f(0.0f, 0.0f, 0.0f);
    glPrint(0, windowSize_y - 60, "Tile size: %g", tileSize);

    /// Field for messages
    glColor3f(0.5f, 0.5f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2i(22 * tileSize, TILES_ON_Y * tileSize);
    glVertex2i(windowSize_x, TILES_ON_Y * tileSize);
    glVertex2i(windowSize_x, TILES_ON_Y * tileSize + 20);
    glVertex2i(22 * tileSize, TILES_ON_Y * tileSize + 20);
    glEnd();

    /// Print what the user is typing
    if(enterCommand || showCommand || enterFilename) {
        glColor3f(0.0f, 0.0f, 0.0f);
        if(enterCommand) {
            glPrint(22 * tileSize, TILES_ON_Y * tileSize + 2, "Parameter: %s", command.c_str());
        } else {
            glPrint(22 * tileSize, TILES_ON_Y * tileSize + 2, "File name: %s", filename.c_str());
        }
    }

    /// Prints a pop up under mouse with parameter for a tile
    if(showCommand && popUps == NULL) {
        popUps = new popUp(map->tiles[mouseTile_x][mouseTile_y].getParameter(), false, mouse_x, mouse_y, windowSize_x, windowSize_y);
    }

    drawPopUps();

    glFlush();
    glutSwapBuffers();
}

void
reshape(int width, int height)
{
    windowSize_x = width;
    windowSize_y = height;

    glViewport(0, 0, (GLsizei)(width), (GLsizei)(height));
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /// recalculate tileSize
    tileSize = MIN((float)width / TILES_ON_X, (float)(height - 70) / TILES_ON_Y);

    /// Update any active pop up
    if(popUps) {
        popUps->setWindowDimensions(width, height);
    }
}

int
main(int argc, char **argv)
{
    glutInit(&argc, argv);

    /// Double buffering with depth buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    /// Create window
    glutInitWindowSize(windowSize_x, windowSize_y);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("level editor");

    if(argc == 2) {
        Initialize(argv[1]);
    } else {
        Initialize("map.txt");
    }

    glutReshapeFunc(reshape);
    glutDisplayFunc(Draw);

    /// Keyboard / mouse input
    glutMouseFunc(mouseClicks);
    glutPassiveMotionFunc(passiveMouseMove);
    glutMotionFunc(mouseMove);

    glutKeyboardFunc(keyboardFunc);
    glutKeyboardUpFunc(keyboardUpFunc);

    glutMainLoop();

    return 0;
}
