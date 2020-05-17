// $Id: shape.cpp,v 1.2 2019-02-28 15:24:20-08 - - $
#include <cmath>
#include <typeinfo>
#include <unordered_map>
using namespace std;

#include "shape.h"
#include "util.h"
#include "graphics.h"

static unordered_map<void*,string> fontname {
   {GLUT_BITMAP_8_BY_13       , "Fixed-8x13"    },
   {GLUT_BITMAP_9_BY_15       , "Fixed-9x15"    },
   {GLUT_BITMAP_HELVETICA_10  , "Helvetica-10"  },
   {GLUT_BITMAP_HELVETICA_12  , "Helvetica-12"  },
   {GLUT_BITMAP_HELVETICA_18  , "Helvetica-18"  },
   {GLUT_BITMAP_TIMES_ROMAN_10, "Times-Roman-10"},
   {GLUT_BITMAP_TIMES_ROMAN_24, "Times-Roman-24"},
};

unordered_map<string,void*> fontcode {
   {"Fixed-8x13"    , GLUT_BITMAP_8_BY_13       },
   {"Fixed-9x15"    , GLUT_BITMAP_9_BY_15       },
   {"Helvetica-10"  , GLUT_BITMAP_HELVETICA_10  },
   {"Helvetica-12"  , GLUT_BITMAP_HELVETICA_12  },
   {"Helvetica-18"  , GLUT_BITMAP_HELVETICA_18  },
   {"Times-Roman-10", GLUT_BITMAP_TIMES_ROMAN_10},
   {"Times-Roman-24", GLUT_BITMAP_TIMES_ROMAN_24},
};


ostream& operator<< (ostream& out, const vertex& where) {
   out << "(" << where.xpos << "," << where.ypos << ")";
   return out;
}

shape::shape() {
   DEBUGF ('c', this);
}
// using initialization list
text::text (void* glut_bitmap_font_, const string& textdata_):
      glut_bitmap_font(glut_bitmap_font_), textdata(textdata_) {
   DEBUGF ('c', this);
}

ellipse::ellipse (GLfloat width, GLfloat height):
dimension ({width, height}) {
   DEBUGF ('c', this);
}

circle::circle (GLfloat diameter): ellipse (diameter, diameter) {
   DEBUGF ('c', this);
}


// minimum 3
polygon::polygon (const vertex_list& vertices_): vertices(vertices_) {
   DEBUGF ('c', this);
}

rectangle::rectangle (GLfloat width, GLfloat height): polygon({
                  {0.0,0.0},
                  {0.0,height},
                  {width,height},
                  {width,0.0}}) {
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}

square::square (GLfloat width): rectangle (width, width) {
   DEBUGF ('c', this<< "(" << width << ")");
}
diamond::diamond(GLfloat width, GLfloat height):
            polygon({{width/2.0f, 0.0},
                     {0.0,height/2.0f},
                     {width/2.0f,height},
                     {width,height/2.0f}}){
   DEBUGF ('c', this << "(" << width << "," << height << ")");
}
triangle::triangle(const vertex_list& vertices_): polygon(vertices_) {
   DEBUGF ('c', this);
}
equilateral::equilateral(GLfloat width):triangle({
                        {0.0,0.0},
                        {width/2.0f,sqrt(3.0f)*width/2.0f},
                        {0.0,width}}){
   DEBUGF ('c', this<< "(" << width << ")");
}
// draw functions are called 
void text::draw 
      (bool bord,const vertex& center, const rgbcolor& color, size_t)
         const{
   DEBUGF ('d', this << "(" << center << "," << color << ")");
   DEBUGF ('d', textdata << endl);

   auto u_str = reinterpret_cast<const GLubyte*> 
                       (textdata.c_str());
   if(bord == false){
      glColor3ubv (color.ubvec);
      glRasterPos2f (center.xpos, center.ypos);
      glutBitmapString (glut_bitmap_font, u_str);
   }
   else{
      float wid = glutBitmapLength (glut_bitmap_font, u_str);
      float hei = glutBitmapHeight (glut_bitmap_font);
      GLfloat bor = window::getbord();
      glLineWidth(bor);
      bord /= 2;
      glBegin (GL_LINE_LOOP);
      glColor3ubv(window::getcolor().ubvec);
      glVertex2f (center.xpos-bor, center.ypos-bor);
      glVertex2f (center.xpos-bor, center.ypos+hei+bor);
      glVertex2f (center.xpos+wid+bor, center.ypos+hei+bor);
      glVertex2f (center.xpos+wid+bor, center.ypos-bor);
      glEnd();
   }
}
static void label(const vertex& center, size_t number){
   auto u_str = reinterpret_cast<const GLubyte*> 
                  (to_string(number).c_str());
   // find the center of the text to really center the label
   float wid = glutBitmapLength (GLUT_BITMAP_TIMES_ROMAN_24, u_str);
   float hei = glutBitmapHeight (GLUT_BITMAP_TIMES_ROMAN_24);

   float xpos = center.xpos - (wid / 2.0f);
   float ypos = center.ypos - (hei / 4.0f);
   
   rgbcolor white = rgbcolor({255},{255},{255});
   glColor3ubv (white.ubvec);
   glRasterPos2f (xpos, ypos);
   glutBitmapString (GLUT_BITMAP_TIMES_ROMAN_24, u_str);

   return;
}
// vertex dimension ({width, height})
void ellipse::draw
   (bool bord, const vertex& center, const rgbcolor& color, 
      size_t idx)const{

   DEBUGF ('d', this << "(" << center << "," << color << ")");
   if(bord == true){
      glLineWidth(window::getbord());
      glBegin (GL_LINE_LOOP);
      glColor3ubv(window::getcolor().ubvec);
   }
   else{
      glBegin (GL_POLYGON);
      glColor3ubv (color.ubvec);
   }
   // delta = quality
   const GLfloat delta = 2 * M_PI / 64;
   const GLfloat width = dimension.xpos;
   const GLfloat height = dimension.ypos;
   for (GLfloat theta = 0; theta < 2 * M_PI; theta += delta) {
      GLfloat xpos = width * cos (theta) + center.xpos;
      GLfloat ypos = height * sin (theta)+ center.ypos;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   label(center,idx);
}
// normalize in draw, or you have to normalize in all the constructors.
void polygon::draw 
         (bool bord ,const vertex& center, const rgbcolor& color,
         size_t idx)const{

   DEBUGF ('d', this << "(" << center << "," << color << ")");
   if(bord == true){
      glLineWidth(window::getbord());
      glBegin (GL_LINE_LOOP);
      glColor3ubv(window::getcolor().ubvec);
   }
   else{
      glBegin (GL_POLYGON);
      glColor3ubv (color.ubvec);
   }
   GLfloat delta_y = {0};
   GLfloat delta_x = {0};
   for(auto rit: vertices){
      delta_x += rit.xpos;
      delta_y += rit.ypos;
   }
   delta_x /= vertices.size();
   delta_y /= vertices.size();
   for(auto rit:vertices){
      GLfloat xpos = center.xpos + rit.xpos - delta_x;
      GLfloat ypos = center.ypos + rit.ypos - delta_y;
      glVertex2f (xpos, ypos);
   }
   glEnd();
   label(center,idx);
}

void shape::show (ostream& out) const {
   out << this << "->" << demangle (*this) << ": "<< endl;
}

void text::show (ostream& out) const {
   shape::show (out);
   out << glut_bitmap_font << "(" << fontname[glut_bitmap_font]
       << ") \"" << textdata << "\"";
}

void ellipse::show (ostream& out) const {
   shape::show (out);
   out << "{" << dimension << "}";
}

void polygon::show (ostream& out) const {
   shape::show (out);
   out << "{" << vertices << "}";
}

ostream& operator<< (ostream& out, const shape& obj) {
   obj.show (out);
   return out;
}

