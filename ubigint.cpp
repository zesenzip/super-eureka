//$Id: ubigint.cpp,v 1.8 2020-01-06 13:39:55-08 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <string>
using namespace std;

#include "ubigint.h"
#include "debug.h"

// ubigint myInt {5298};
ubigint::ubigint (unsigned long that) {
   //DEBUGF ('~', this << " -> " << ubig_value);
   unsigned long digit = 0;
   while( that > 0){
      digit = that % 10;
      ubig_value.push_back(digit);
      that /= 10;
   }
}

ubigint::ubigint (const string& that) {
   //DEBUGF ('~', "that = \"" << that << "\"");
   for(auto rit=that.crbegin(); rit!=that.crend(); ++rit) {
      if (not isdigit (*rit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      ubig_value.push_back(*rit - '0');
   }
   if(ubig_value.back() == 0){
      ubig_value.pop_back();   
   }
}

ubigint ubigint::operator+ (const ubigint& that) const {
   ubigint addition;
   unsigned long sum = 0;
   unsigned long carry = 0;

   auto xi = ubig_value.begin();
   auto yi = that.ubig_value.begin();
   // A and B
   while(xi != ubig_value.end() && yi != that.ubig_value.end()){
      sum = *xi + *yi + carry;
      carry = sum/10;
      addition.ubig_value.push_back( sum % 10);
      
      xi++;
      yi++;
   }
   // case: this is longer
   while(xi != ubig_value.end()){
      sum = *xi + carry;
      carry = sum/10;

      addition.ubig_value.push_back(sum % 10);
      xi++;
   }
   // case: that is longer
   while(yi != that.ubig_value.end()){
      sum = *yi + carry;
      carry = sum/10;

      addition.ubig_value.push_back(sum % 10);
      yi++;
   }
   // case: carry out in Most Sign. Digit
   if(carry != 0){
      addition.ubig_value.push_back(carry);
   }
   return addition;
}   

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");
   ubigint sub;

   unsigned long difference = 0;
   unsigned long borrow = 0;

   auto xi = ubig_value.begin();
   auto yi = that.ubig_value.begin();

   while(xi != ubig_value.end() && yi != that.ubig_value.end()){
      if(*xi < *yi){ //needs to borrow
         difference = 10 + *xi - *yi - borrow;
         borrow = 1;
      }
      else if (*xi == *yi && borrow == 1){
         difference = 10 + *xi - *yi - borrow;
         borrow = 1;
      }
      else{        // no need to borrow
         difference = *xi - *yi - borrow;
         borrow = 0;
      }
      sub.ubig_value.push_back(difference);
      xi++;
      yi++;
   }
   // only xi can be longer in terms of vector size
   while(xi != ubig_value.end()){ // this is the bigger number
      difference = *xi - borrow;
      borrow = 0;
      sub.ubig_value.push_back(difference);
      xi++;
   }
   // get rid of extra zeros in the fron of number
   while(sub.ubig_value.size() > 0 && sub.ubig_value.back() == 0){
      sub.ubig_value.pop_back();   
   }
   return sub;
}


ubigint ubigint::operator* (const ubigint& that) const {
   ubigint mult;
   if(that.ubig_value.size()==0 || ubig_value.size()==0)
      return mult;
   
   unsigned long mi = ubig_value.size();
   unsigned long ni = that.ubig_value.size();
   mult.ubig_value.reserve(mi+ni);
   mult.ubig_value.assign(mi+ni, 0);

   for (unsigned long xi = 0; xi < mi; ++xi){
      unsigned long carry = 0;

      for (unsigned long yi = 0; yi < ni; ++yi)
      {
         unsigned long product = mult.ubig_value[xi+yi];
         product += (ubig_value[xi])*(that.ubig_value[yi])+carry;
         mult.ubig_value[xi+yi] = product % 10;
         carry = product / 10;
      }
      mult.ubig_value[xi+ni] = carry;
   }
   
   while(mult.ubig_value.size()>0 && mult.ubig_value.back()==0){
      mult.ubig_value.pop_back(); 
   }
   return mult;
}

void ubigint::multiply_by_2() {
   unsigned long carry = 0;
   auto xi = ubig_value.begin();

   while(xi != ubig_value.end()){
      *xi = (*xi*2) + carry;
      carry = *xi / 10;
      *xi = *xi % 10;
      xi++;
   }
   // case: carry extra MSD
   if(carry != 0){
      ubig_value.push_back(carry);
   }
   return;
}
   
void ubigint::divide_by_2() {
   auto xi = ubig_value.begin();
   //handle first step to avoid segfault
   if(ubig_value.size() != 0){
      *xi = *xi/2;
      xi++;
   }
   while(xi != ubig_value.end()){
      // if current number is odd
      // add 5 to prev
      // does not apply to least LSD
      if((*xi) % 2 == 1){
         *(xi - 1) += 5;
      }
      *xi /= 2;
      xi++;
   }
   // case: MSD is 1 creates a 0 at the back
   if(ubig_value.size()> 0 && ubig_value.back() == 0){
      ubig_value.pop_back();
   }
   return;
}


struct quo_rem { ubigint quotient; ubigint remainder; }; // leave
quo_rem udivide (const ubigint& dividend, const ubigint& divisor_) {
   // NOTE: udivide is a non-member function.
   ubigint divisor {divisor_};
   ubigint zero {0};
   if (divisor == zero) throw domain_error ("udivide by zero");
   ubigint power_of_2 {1};
   ubigint quotient {0};
   ubigint remainder {dividend}; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient; //leave
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder; //leave
}

bool ubigint::operator== (const ubigint& that) const {
   bool eq = true;
   eq = ubig_value.size() == that.ubig_value.size();

   auto xi = ubig_value.crbegin();
   auto yi = that.ubig_value.crbegin();
   auto xend = ubig_value.crend();
   auto yend = that.ubig_value.crend();
   // nice char limit on lines
   while(xi!=xend && yi!=yend && eq){
      eq = *xi == *yi;
      xi++;
      yi++;
   }

   return eq;
}

bool ubigint::operator< (const ubigint& that) const {
   if(ubig_value.size() > that.ubig_value.size()) 
      return false;
   if(ubig_value.size() < that.ubig_value.size()){
      return true;
   }
   //(auto rit=that.crbegin(); rit!=that.crend(); ++rit)
   auto xi = ubig_value.crbegin();
   auto yi = that.ubig_value.crbegin();
   auto xend = ubig_value.crend();
   auto yend = that.ubig_value.crend();

   // only if both vectors are same size
   // only continue if this is equal
   // start at the Most sig. digitS
   while(xi!=xend && yi!=yend){
      if(*xi < *yi)
         return true;
      else if (*xi == *yi){
         xi++;
         yi++;
      }
      else 
         return false;
   }
   return false;
}
// use this get the chars
ostream& operator<< (ostream& out, const ubigint& that) { 
   unsigned int counter = 0;

   if(that.ubig_value.size() == 0){
      return out << '0';
   }
   auto xi = that.ubig_value.crbegin();

   while(xi != that.ubig_value.crend()){
      out << +*xi;
      counter++;
      if (counter == 69){
         out <<"\\\n";
         counter = 0;
      }
      xi++;
   }
   return out;

   //return out << "ubigint(" << that.uvalue << ")";
}
