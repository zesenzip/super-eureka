// $Id: bigint.cpp,v 1.2 2020-01-06 13:39:55-08 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

bigint::bigint (const ubigint& uvalue_, bool is_negative_):
                uvalue(uvalue_), is_negative(is_negative_) {
}

bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

bigint bigint::operator+ () const {
   return *this;
}

bigint bigint::operator- () const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   
   if (is_negative == false && that.is_negative == false){
      ubigint result = uvalue + that.uvalue;
      return {result, false};
   }
   else if (is_negative == true && that.is_negative == true){
      ubigint result = uvalue + that.uvalue;
      return {result, true};
   }
   // signs are differrent
   if(uvalue == that.uvalue){
      ubigint result;
      return {result, false};
   }
   else if(uvalue > that.uvalue){
      ubigint result = uvalue - that.uvalue;
      return {result, is_negative};
   }
   else{
      ubigint result = that.uvalue - uvalue;
      return {result, that.is_negative};
   }
}

bigint bigint::operator- (const bigint& that) const {
  // this is LEFT that is RIGHT !!!!!1!!1!!1!
  // case: both postive 
   if(uvalue == that.uvalue){
      ubigint result;
      return {result, false};
   }
   else if (is_negative == false && that.is_negative == false){
      if(uvalue > that.uvalue){
         ubigint result = uvalue - that.uvalue;
         return {result, false};  
      }
      else{
         ubigint result = that.uvalue - uvalue;
         return {result, true};
      }
   }  // both negative
   else if (is_negative == true && that.is_negative == true){
      if(uvalue > that.uvalue){
         ubigint result = uvalue - that.uvalue;
         return {result, true};
      }
      else{
         ubigint result = that.uvalue - uvalue;
         return {result, false};
      }
   }
   else{
      // left is negative and sign different
      if(is_negative){
         ubigint result = that.uvalue + uvalue;
         return {result, true};
      }
      else{
         ubigint result = uvalue + that.uvalue;
         return {result, false};
      }
   }
}


bigint bigint::operator* (const bigint& that) const {
   ubigint result = uvalue * that.uvalue;
   return {result, is_negative != that.is_negative};
}

bigint bigint::operator/ (const bigint& that) const {
   ubigint result = uvalue / that.uvalue;
   return {result, is_negative != that.is_negative};
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   //return out << "bigint(" << (that.is_negative ? "-" : "+")
   //           << "," << that.uvalue << ")";
   return out << (that.is_negative ? "-" : "")
              << that.uvalue;
}
