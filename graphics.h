// $Id: graphics.h,v 1.2 2019-03-19 16:18:22-07 - - $

#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <memory>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "rgbcolor.h"
#include "shape.h"

extern unordered_map<string,void*> fontcode;

class object {
   private:
      shared_ptr<shape> pshape;
      vertex center;
      rgbcolor color;
   public:
      object (shared_ptr<shape>, vertex, rgbcolor);
      void draw(size_t label);
      void outline(size_t label);
      vertex bounce();
      // use modulo to move back into the screen.
      void move (GLfloat delta_x, GLfloat delta_y);
};

class mouse {
      friend class window;
   private:
      int xpos {0};
      int ypos {0};
      int entered {GLUT_LEFT};
      int left_state {GLUT_UP};
      int middle_state {GLUT_UP};
      int right_state {GLUT_UP};
   private:
      void set (int x, int y) { xpos = x; ypos = y; }
      void state (int button, int state);
      void draw();
};


class window {
      friend class mouse;
   private:
      static int width;         // in pixels
      static int height;        // in pixels
      static vector<object> objects;
      static size_t selected_obj;
      static mouse mus;
      static GLfloat bordsize;
      static GLfloat movesize;
      static rgbcolor defcolor;
   private:
      static void close();
      static void entry (int mouse_entered);
      static void display();
      static void reshape (int width, int height);
      static void keyboard (GLubyte key, int, int);
      static void move_selected_object (GLfloat, GLfloat);
      static void special (int key, int, int);
      static void motion (int x, int y);
      static void passivemotion (int x, int y);
      static void mousefn (int button, int state, int x, int y);
      static void select_object(size_t idx){ selected_obj = idx; }
      
   public:
      static void push_back (const object& obj) {
                  objects.push_back (obj); }
      static void setwidth (int width_) { width = width_; }
      static void setheight (int height_) { height = height_; }
      static void setmoveby (float movesize_)
                  {movesize = GLfloat(movesize_);}
      static void setbordsize (float bordsize_)
                  {bordsize = GLfloat(bordsize_);}
      static void setdefcolor (rgbcolor defcolor_)
                  {defcolor = rgbcolor(defcolor_);}
      static GLfloat getbord(){return bordsize;}
      static GLfloat getmove(){return movesize;}
      static rgbcolor getcolor(){return defcolor;}
      static GLfloat getwidth(){return GLfloat(width);}
      static GLfloat getheight(){return GLfloat(height);}
      static void main();
};

#endif
