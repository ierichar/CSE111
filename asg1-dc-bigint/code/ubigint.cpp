// $Id: ubigint.cpp,v 1.12 2020-10-19 13:14:59-07 - - $

#include <cctype>
#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
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
      
      for (int i = 0; i < that.length(); i++) {
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
   // DEBUGF ('u', *this << "+" << that);
   // ubigint result (ubig_value + that.ubig_value);
   // DEBUGF ('u', result);
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

   // Append zeroes to smaller ubig_value and maintain original size
   uint8_t originalSize;
   if (sizeFlag) {
      for (int i = that.ubig_value.size(); i < ubig_value.size(); i++) {
         originalSize = that.ubig_value.size();
         that.ubig_value.push_back(static_cast<uint8_t>(0));
      }
   } else {
      for (int i = ubig_value.size(); i < that.ubig_value.size(); i++) {
         originalSize = ubig_value.size();
         ubig_value.push_back(static_cast<uint8_t>(0));
      }
   }

   // Add indecies in array
   ubigint sum = (ubig_value.size() + 1, 0);
   uint8_t temp = 0, carryFlag = 0;
   for (int i = 0; i < ubig_value.size(); i++) {
      // Premptively check carry
      if (carryFlag) {
         temp += carryFlag;
         carryFlag = 0;
      }
      temp += ubig_value[i] + that.ubig_value[i];
      // Reduce temp in case it's not 0-9
      if (temp >= BASE) {
         temp -= BASE;
         carryFlag += 1;
      }
      sum.push_back(temp)
   }
   if (carryFlag > 0) {
      sum.push_back(carryFlag);
   }

   // Remove appended zeroes to appropriate operand
   if (sizeFlag) {
      for (int i = originalSize; i < ubig_value.size(); i--) {
         that.ubig_value.pop_back();
      }
   } else {
      for (int i = originalSize; i < that.ubig_value.size(); i--) {
         ubig_value.pop_back();
      }
   }

   return sum;
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

   ubigint difference = (ubig_value.size(), 0);
   uint8_t temp = 0, carryFlag;
   // assume that.ubig_value is > or == this.ubig_value
   for (int i = 0; i < ubig_value.size(); i++) {
      if (ubig_value[i] < that.ubig_value[i]) {
         carryFlag += 1;
         temp = ubig_value[i] + BASE;
         temp -= that.ubig_value[i];
      } else {
         temp += ubig_value[i] - that.ubig_value[i];
      }
      difference.push_back(temp);
   }
   // deal with carry and subtracting carry from subsequent values
   while (i < ubig_value.size()) {
      if (carryFlag > 0) {
         if (ubig_value[i] == 0) {
            difference.push_back(static_cast<uint8_t>(9));
         } else {
            difference.push_back(ubig_value[i] - carryFlag);
            carryFlag = 0;
         }
      } else {
         difference.insert(0, ubig_value[i]);
      }
      i++;
   }

   return difference;
   // return ubigint (ubig_value - that.ubig_value);
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
   ubigint product = (ubig_value.size() + that.ubig_value.size(), 0), temp_vector;
   product.push_back(0);      // initialize product to add temp_vector to it
   uint8_t operand1, operand2, temp, carryValue = 0;
   int i, j, k, power_count = 0;
   for (i = 0; i < that.ubig_value.size(); i++) {
      // set temp to the entirety of that.ubig_value
      // to multiple into each index of ubig_value
      operand1 = that.ubig_value[i];
      for (j = 0; j < ubig_value.size(); j++) {
         // maintain carry from previous temp*ubig_value[j] product
         // multiply an index of ubig_value to temp_vector
         operand2 = ubig_value[j];

         // handle carry
         if (carryValue > 0) {
            temp += carryValue;
            carryValue = 0;
         }
         temp += operand1 * operand2;
         while (temp > 9) {
            carryValue += 1;
            temp -= BASE;
         }
         temp_vector.push_back(temp);
         temp = 0;
      }
      for (k = 0; k < power_count; k++) {
         // apply 10 ^ power_count
         temp_vector.insert(0, static_cast<uint8_t>(0));
      }
      // requires operator+ to be functional
      product = product + temp_vector;
      power_count++;
   }
   // if a carry value remains outside of all loops, insert at 0 to prodcut
   if (carryValue > 0) {
      product.push_back(carryValue);
   }
   // pop off remaining zeroes
   while (ubig_value[ubig_value.size() - 1] == static_cast<uint8_t>(0)) {
      ubig_value.pop_back();
   }
   return product;
   // return ubigint (uvalue * that.uvalue);
}

void ubigint::multiply_by_2() {
   // To implement multiply_by_2, iterate from the low order digit, and double
   // each digit (remainder 10), carrying to the next higher digit. At the end,
   // if the carry is 1, use push_back.
   // uvalue *= 2;
   uint8_t temp, carryFlag = 0;
   for (int i = ubig_value.size() - 1; i >= 0; i--) {
      if (carryFlag != 0) {
         temp += carryFlag;
         carryFlag = 0;
      }
      temp += ubig_value[i] * static_cast<uint8_t>(2);
      if (temp > 9) {
         temp -= BASE;
         carryFlag = 1;
      }
      ubig_value[i] = temp;
      temp = 0;
   }
   if (carryFlag != 0) {
      ubig_value.insert(0, carryFlag);
   }
   return
}

void ubigint::divide_by_2() {
   // To implement divide_by_2, iterate from the low order digit, and divide
   // each digit by 2. Then, if the next higher digit is odd, add 5 to the 
   // current digit. Be careful of the end, and pop_back any remaining high 
   // order zeros
   // ubig_value /= 2;
   uint8_t temp = 0, carryFlag = 0;
   for (int i = 0; i < ubig_value.size(); i++) {
      if (i < ubig_value.size() - 1 && ubig_value[i + 1] % 2 == 1) {
         temp += ubig_value[i] + static_cast<uint8_t>(5);
      }
      temp = ubig_value[i] / static_cast<uint8_t>(2);
   }
   if (ubig_value[ubig_value.size() - 1] == 0) {
      ubig_value.pop_back();
   }
   return
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
   if (that.ubig_value.size() == 1) {
      // simple division
   } else if (that.ubig_value > ubig_value) {
      // divisor is longer than the dividend
      return static_cast<uint8_t>(0);
   } else {
      //
   }
   return udivide (*this, that).quotient;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return udivide (*this, that).remainder;
}

bool ubigint::operator== (const ubigint& that) const {
   bool isEqual = true;
   if (ubig_value.size() != that.ubig_value.size()) {
      isEqual = false;
   } else {
      int i = 0;
      while (i < ubig_value.size() && isEqual) {
         if (ubig_value[i] != that.ubig_value[i]) {
            isEqual = false;
         }
      }
   }
   return isEqual;
   // return ubig_value == that.ubig_value;
}

bool ubigint::operator< (const ubigint& that) const {
   bool isLess = true;
   if (ubig_value.size() < that.ubig_value.size()) {
      isLess = true;
   } else {
      isLess = false;
   }
   return isLess;
   // return ubig_value < that.ubig_value;
}

ostream& operator<< (ostream& out, const ubigint& that) {
   string output;
   for (int i = 0; i < that.ubig_value.size() - 1; i++) {
      output = static_cast<char>(that.ubig_value[i]) + output;
   }
   return out << "ubigint(" << output << ")";
}

