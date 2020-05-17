// $Id: interp.cpp,v 1.3 2019-03-19 16:18:22-07 - - $

#include <memory>
#include <string>
#include <vector>
using namespace std;

#include <GL/freeglut.h>

#include "debug.h"
#include "interp.h"
#include "shape.h"
#include "util.h"

unordered_map<string,interpreter::interpreterfn>
interpreter::interp_map {
   {"define" , &interpreter::do_define },
   {"draw"   , &interpreter::do_draw   },
   {"border" , &interpreter::do_border },
   {"moveby" , &interpreter::do_moveby },
};

unordered_map<string,interpreter::factoryfn>
interpreter::factory_map {
   {"text"     , &interpreter::make_text     },
   {"ellipse"  , &interpreter::make_ellipse  },
   {"circle"   , &interpreter::make_circle   },
   {"polygon"  , &interpreter::make_polygon  },
   {"rectangle", &interpreter::make_rectangle},
   {"square"   , &interpreter::make_square   },
   {"diamond"  , &interpreter::make_diamond  },
   {"triangle" , &interpreter::make_triangle },
   {"equilateral", &interpreter::make_equil    },
};

interpreter::shape_map interpreter::objmap;

interpreter::~interpreter() {
   for (const auto& itor: objmap) {
      cout << "objmap[" << itor.first << "] = "
           << *itor.second << endl;
   }
}
// main() checks params at least size 1
void interpreter::interpret (const parameters& params) {
   DEBUGF ('i', params);
   param begin = params.cbegin();
   string command = *begin;
   auto itor = interp_map.find (command);
   if (itor == interp_map.end()) throw runtime_error ("syntax error");
   interpreterfn func = itor->second;
   func (++begin, params.cend());
}
// param == vector<string>::const_iterator = parameters:const_iterator
void interpreter::do_define (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin < 3) throw runtime_error ("syntax error");
   string name = *begin;
   objmap.emplace (name, make_shape (++begin, end));
}


void interpreter::do_draw (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 4) throw runtime_error ("syntax error");
   string name = begin[1];
   shape_map::const_iterator itor = objmap.find (name);
   if (itor == objmap.end()) {
      throw runtime_error (name + ": no such shape");
   }
   rgbcolor color {begin[0]};
   vertex where {from_string<GLfloat> (begin[2]),
                 from_string<GLfloat> (begin[3])};
   cout << itor->second << endl;
   window::push_back (object (itor->second, where, color));
}
void interpreter::do_border (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 2) throw runtime_error ("syntax error");
   window::setdefcolor(rgbcolor(*begin++));
   window::setbordsize(stof(*begin));
}
void interpreter::do_moveby (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if (end - begin != 1) throw runtime_error ("syntax error");
   window::setmoveby(stof(*begin));
}
// range at least 2
shape_ptr interpreter::make_shape (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string type = *begin++;
   auto itor = factory_map.find(type);
   if (itor == factory_map.end()) {
      throw runtime_error (type + ": no such shape");
   }
   factoryfn func = itor->second;
   // range at least 1
   return func (begin, end);
}

shape_ptr interpreter::make_text (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   string font = *begin++;
   auto rit = fontcode.find(font);
   if (rit == fontcode.end()) {
     throw runtime_error (font + ": no such font");
   }
   if(end - begin < 1) throw runtime_error ("empty string");
   string textData = *begin++;

   for (;begin != end; ++begin) {
      textData += " ";  
      textData += *begin;
   }
   return make_shared<text> (rit->second, textData);
}

shape_ptr interpreter::make_ellipse (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 2) throw runtime_error ("syntax error");
   GLfloat xi = stof(*begin++);
   GLfloat yi = stof(*begin);
   return make_shared<ellipse> (xi, yi);
}

shape_ptr interpreter::make_circle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 1) throw runtime_error ("syntax error");
   GLfloat xi = stof(*begin);
   return make_shared<circle> (GLfloat(xi));
}

shape_ptr interpreter::make_polygon (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   // number of args need to be even and above 6
   if((end - begin) % 2 == 1 || end - begin < 6)
      throw runtime_error ("syntax error: odd poly");
   vertex_list verlis;
   while(begin != end){
      vertex vi;
      vi.xpos = stof(*begin++);
      vi.ypos = stof(*begin++);
      verlis.push_back(vi);
   }
   return make_shared<polygon> (verlis);
}

shape_ptr interpreter::make_rectangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 2) throw runtime_error ("syntax error");
   GLfloat xi = stof(*begin++);
   GLfloat yi = stof(*begin);
   return make_shared<rectangle> (GLfloat(xi), GLfloat(yi));
}

shape_ptr interpreter::make_square (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 1) throw runtime_error ("syntax error");
   GLfloat xi = stof(*begin);
   return make_shared<square> (GLfloat(xi));
}
shape_ptr interpreter::make_diamond (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 2) throw runtime_error ("syntax error");
   GLfloat xi = stof(*begin++);
   GLfloat yi = stof(*begin);
   return make_shared<diamond> (GLfloat(xi), GLfloat(yi));
}
shape_ptr interpreter::make_triangle (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 6) throw runtime_error ("syntax error");

   vertex_list verlis;
   for (int i = 0; i < 3; ++i) {
      vertex vi;
      vi.xpos = stof(*begin++);
      vi.ypos = stof(*begin++);
      verlis.push_back(vi);
   }
   return make_shared<triangle> (verlis);
}
shape_ptr interpreter::make_equil (param begin, param end) {
   DEBUGF ('f', range (begin, end));
   if(end - begin != 1) throw runtime_error ("syntax error");
   GLfloat xi = stof(*begin);
   return make_shared<square> (GLfloat(xi));
}
