// $Id: bigint.cpp,v 1.4 2021-09-26 10:35:44-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "ubigint.h"

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
//check for negatives in each one, subtract 1 respectively


   //are strings same size?
      //create a flag, isBLarger? yes, run first loop! no? run second loop.

   //is B string larger than A string?
      //same code as below but flip B and A when calling value!

   //is A string larger than B string?
      //yes!
         //are the signs the same?
            //yes!
               //are they positive?
                  //yes!
                     //Run addition (A+B) (A+B, keep +)
                  //no!
                     //Run addition flip sign(-A+-B) (A+B, keep -)
            //no!
               //is A negative?
                  //yes!
                     //run subtraction (-a+b) A is larger, A-B, keep -
                  //no!
                     //run subtraction (a+-b)  A is larger, A-B
            
               
      //no!

   ubigint result {uvalue + that.uvalue};
   return result;
}

bigint bigint::operator- (const bigint& that) const {
   //are strings same size?
      //create a flag, isBLarger? yes, run first loop! no? run second loop.

   //is B string larger than A string?
      //same code as below but flip B and A when calling value!

//is A string larger than B string?
   //Yes!
      //are the signs the same?
         //yes!
            //are they positive?
               //yes!
                  //run subtraction (a-b)
               //no
                  //rub subtraction(-a--b) = (-(a-b))
         //no!
            //is A negative?
               //yes!
                  //run addition (-a-b) (-16-13 = 29)
               //no!
                  //run addition (a--b) (32--16 = 48)



   ubigint result {uvalue - that.uvalue};
   return result;
}


bigint bigint::operator* (const bigint& that) const {
   bigint result {uvalue * that.uvalue};
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result {uvalue / that.uvalue};
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result {uvalue % that.uvalue};
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
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}

