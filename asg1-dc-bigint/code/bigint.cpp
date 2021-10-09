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
   //is A string larger than B string?
      //yes!
         //are the signs the same?
            //yes!
               //are they positive?
                  //yes!
                     //Run addition (A+B)
                  //no!
                     //Run addition flip sign(-A+-B)
   return *this;
}

bigint bigint::operator- () const {
   // WORK IN PROGRESS
   //first step, check size of A and B

   //is A larger than B?
      //it's gonna  stay the sign of the larger #
   //is B larger than A?
      //it's gonna stay the sign of the larger #
   //else, They are the same size

   //Are they the same sign?
      //yes?
         //is the larger string first?
            //yes?
               //stays that sign
            //no?
               //flip signs
      //no? 
         //is the larger string first?
            //yes?
               //stays that sign
            //no?
               //flip sign



   //subbing two pos #'s that leads to a positive

   // subbing two pos #'s that lead to a negative

   //subbing two neg #'s that lead to a positive

   //subbing two neg #'s that lead to a negative
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   ubigint result {uvalue + that.uvalue};
   return result;
}

bigint bigint::operator- (const bigint& that) const {
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

