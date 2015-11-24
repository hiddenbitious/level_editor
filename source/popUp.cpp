#include "popUp.h"

popUp::popUp(const vector<string>& options, bool returns, int mouse_x, int mouse_y, int windowWidth, int windowHeight)
{
    this->options = options;
    this->elements = options.size();
    this->returns = returns;
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;

    size_y = 25;
    size_x = 0;
    /// Find width of widest option and set it as pop up's width
    for(int i = 0; i < options.size(); i++) {
        if(options[i].size() > size_x && options[i].size()) {
            size_x = options[i].size();
        }
    }
    size_x *= 11;

    x = mouse_x;
    y = mouse_y;

    /// popUp is always drawn inside the window.
    if(size_y * this->elements + y > windowHeight) {
        y = y - size_y * this->elements;
    }

    if(size_x + x > windowWidth) {
        x = x - size_x;
    }
}

popUp::popUp(const string& option, bool returns, int mouse_x, int mouse_y, int windowWidth, int windowHeight)
{
    this->returns = returns;
    this->elements = 1;
    this->options.push_back(option);
    this->windowWidth = windowWidth;
    this->windowHeight = windowHeight;

    size_y = 25;
    size_x = options[0].length();
    size_x *= 11;

    x = mouse_x;
    y = mouse_y;

    /// must reverse menu because it is printed under mouse cursor...

    /// popUp is always drawn inside the window.
    if(size_y * this->elements + y > windowHeight) {
        y = y - size_y * this->elements;
    }
    if(size_x + x > windowWidth) {
        x = x - size_x;
    }
}

popUp::~popUp(void)
{
}

void popUp::draw(int mouse_x, int mouse_y)
{
    int fy = 0;
    int fx = 0;

    /// Draw popUp
    glBegin(GL_QUADS);
    for(int i = 0; i < this->elements; i++) {
        if(this->chooseOption(mouse_x, mouse_y) - 1 == i) {
            glColor4f(0.8f, 0.8f, 1.0f, 0.6f);    //Highlight selected option
        } else {
            glColor4f(0.5f, 0.5f, 1.0f, 0.6f);
        }

        glVertex2i(x			, windowHeight - y - size_y * i);
        glVertex2i(x + size_x, windowHeight - y - size_y * i);
        glVertex2i(x + size_x, windowHeight - y - size_y * (i + 1));
        glVertex2i(x			, windowHeight - y - size_y * (i + 1));
    }
    glEnd();

    /// Allign text
    if(size_y < 0) { fy = size_y * this->elements; }
    if(size_x < 0) { fx = size_x; }

    /// Print text
    glColor3f(0.0f, 0.0f, 0.0f);
    for(int i = 0; i < this->elements; i++) {
        glPrint(x + fx, windowHeight - y - 25 * (i + 1) + fy, "%s\n", options[i].c_str());
    }
}

/// Find over which option is the mouse pointer.
int popUp::chooseOption(int mouse_x, int mouse_y)
{
    if(!this->returns) {
        return 0;
    }

    for(int i = 0; i < this->elements; i++) {
        if(mouse_x >= x && mouse_x < x + size_x &&
                (mouse_y >= y + size_y * i && mouse_y <= y + size_y * (i + 1))) {
            return i + 1;
        }
    }

    return 0;
}

bool popUp::hasOptions(void)
{ return this->returns; }

void popUp::setWindowDimensions(int w, int h)
{
    windowWidth = w;
    windowHeight = h;
}
