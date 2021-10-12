// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "debug.h"
#include "relops.h"
#include "ubigint.h"

ubigint::ubigint (unsigned long that): ubig_value (that) {
   DEBUGF ('~', this << " -> " << ubig_value)
}

ubigint::ubigint (const string& that): ubig_value(0) {
   DEBUGF ('~', "that = \"" << that << "\"");
   for (char digit: that) {
      if (not isdigit (digit)) {
         throw invalid_argument ("ubigint::ubigint(" + that + ")");
      }
      //uvalue = uvalue * 10 + digit - '0';
      int l = that.length();
      int n = 0;
      for(int (l-1)!=n){
         ubig_value.push_back(that.n);
         n++;
      }
      
      for (int i = len(that); i > 0; i--) {
         ubig_value.push_back(that[i])
      }
      // str = "2 5 6"
      // v[] = [2]
      // v[] = [2][5]
      // v[] = [2][5][6]
   }
}

// Notes on bigint to ubigint
// operand A, B: assume A >= B
// bigint ---  ubigint ---
// A + B 		(+) call +
// A + -B 		call - 
// -A + B 		(-) call to ubigint A - B
// -A + -B 	   (-) call to ubigint A + B

// A - B 		call - 
// A - -B 		call to ubigint A + B
// -A - B 		(-) A + B
// -A - -B 	   (-) A - B

ubigint ubigint::operator+ (const ubigint& that) const {
   DEBUGF ('u', *this << "+" << that);
   ubigint result (ubig_value + that.ubig_value);
   DEBUGF ('u', result);

   // new code
   // vector<int> leftInput;

   //  [9 9 6 8 1 9]
   //      [5 2 9 0]
   //    [0 5 2 9 0]
   //  [0 0 5 2 9 0]

   //[1 0 6 2 1 0 9]

   // Checks for larger operand and set flag
   bool sizeFlag;
   if (ubig_value.size() > that.ubig_value.size()) {
      sizeFlag = true;
   } else {
      sizeFlag = false;
   }

   // If flag is not set add one way, else add another way
   ubigint sum;
   uint8_t temp = 0, carryFlag = 0;  // max size 1024
   if (sizeFlag) {

      // FLAG is TRUE: ubig_value is greater than that.ubig_value
      int i;
      for (i = 0; i < that.ubig_value.size(); i++) {
         if (carryFlag) {
            temp += carryFlag;
            carryFlag = 0;
         }
         temp = ubig_value[ubig_value.size() - i] + that.ubig_value[that.ubig_value.size() - i];
         if (temp > 9) {
            temp -= 10;
            carryFlag = 1;
         }
         result.push_back(temp);
      }
      // append zeroes after main loop
      while (i < ubig_value.size()) {
         if (carryFlag) {
            temp += carryFlag;
            carryFlag = 0;
         }
         temp = ubig_value[ubig_value.size() - i]
         if (temp > 9) {
            temp -= 10;
            carryFlag = 1;
         }
         // REMEMBER FINAL CARRY
         result.insert(0, temp);
         i++;
      }

   } else {

      // FLAG is FALSE: that.ubig_value is greater than ubig_value
      int i;
      for (i = 0; i < ubig_value.size(); i++) {
         if (carryFlag) {
            temp += carryFlag;
            carryFlag = 0;
         }
         temp = that.ubig_value[ubig_value.size() - i] + ubig_value[that.ubig_value.size() - i];
         if (temp > 9) {
            temp -= 10;
            carryFlag = 1;
         }
         result.push_back(temp);
      }
      // append zeroes and potential carry after main loop
      while (i < that.ubig_value.size()) {
         if (carryFlag) {
            temp += carryFlag;
            carryFlag = 0;
         }
         temp = that.ubig_value[ubig_value.size() - i];
         if (temp > 9) {
            temp -= 10;
            carryFlag = 1;
         }
         result.insert(0, temp);
         i++;
      }

   }
   // if carry remains after previous calculations, add remaining carry
   if (carryFlag) {
      result.insert(0, carryFlag);
      carryFlag = 0;
   }

   return result;
}

ubigint ubigint::operator- (const ubigint& that) const {
   // a is predetermined to be less than b, otherwise throw error
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");

   // bigint is given:
   // OG: -24500 + 110
   //   : -230032 - 123123 (pass to operator+)
   // result is negative so..
   // pass +24500 - 110
   // operator-
   // bigint passing: a >= b
   // a - b
   // 24500 - 110
   // result is passed back to bigint
   // bigint knew was negative
   // OG: a negative result 

   // [1 0 0 0 0 0]
   //-[0 0 0 0 9 9]

   ubigint difference;
   uint8_t temp, carryFlag;
   // assume that.ubig_value is > or == this.ubig_value
   for (int i = 0; i < ubig_value; i++) {
      if (ubig_value[ubig_value.size() - i] < 
            that.ubig_value[ubig_value.size() - i]) {
         carryFlag += 1;
         temp = ubig_value[ubig_value.size() - i] + 10;
         temp -= that.ubig_value[ubig_value.size() - i];
      } else {
         temp = ubig_value[ubig_value.size() - i] - 
                that.ubig_value[ubig_value.size() - i];
      }
      result.push_back(temp);
   }

   return ubigint (ubig_value - that.ubig_value);
}

ubigint ubigint::operator* (const ubigint& that) const {
   // vector will never be more than twice the combined # of elements
   // double for loop
   // [1 2 5 4]
   // x   [1 5]
   // temp = [5] x [1254] x 10^0 (append a zero to the back) (1)
   // temp = [1] x [1254] x 10^1 
   // product += temp (calling ubigint operater+)

   // (1)
   // 4 x 5 = 20 (while temp > 9) carry++ temp-=10
   // 2 + 5 x 5 = 27 (while temp > 9) carry++ temp-=10
   return ubigint (uvalue * that.uvalue);
}

void ubigint::multiply_by_2() {
   uvalue *= 2;
}

void ubigint::divide_by_2() {
   uvalue /= 2;
}


struct quo_rem { ubigint quotient; ubigint remainder; };
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
   DEBUGF ('/', "quotient = " << quotient);
   DEBUGF ('/', "remainder = " << remainder);
   return {.quotient = quotient, .remainder = remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
   return ubig_value < that.ubig_value;
}

ostream& operator<< (ostream& out, const ubigint& that) { 
   return out << "ubigint(" << that.ubig_value << ")";
}

