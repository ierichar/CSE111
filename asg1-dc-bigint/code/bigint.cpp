// Ian Richardson, ierichar
// Jackson Brazeal, jbrazeal
// $Id: bigint.cpp,v 1.4 2021-09-26 10:35:44-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <vector>
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
bigint result;
bool isval1Larger = false;
if(that.uvalue < uvalue){
   isval1Larger = true;
}

bool val1negative = false;
bool val2negative = false;

if(this->is_negative == true){
   val1negative = true;
}
if(that.is_negative == true){
   val2negative = true;
}

//now that you know which value is larger, you can run the code below!

if(isval1Larger){
   if((val1negative==false)&&(val2negative==false)){
      result = {uvalue + that.uvalue};
      //add both together
   } 

   else if((val1negative==true)&&(val2negative==true)){
      result.uvalue =  {uvalue + that.uvalue};
      result.is_negative = true;
      //keep addition, put negative on after.
   }
   else if((val1negative==true)&&(val2negative==false)){
       result.uvalue = {uvalue - that.uvalue};
       result.is_negative = true;
      //run subtraction (-a+b) add negative

   }
   else{
       result.uvalue = {uvalue - that.uvalue};
      //run subtraction (a+-b), A-B stay positive

   }
}
   else{ //B is larger now
      if((val1negative==false)&&(val2negative==false)){
         result.uvalue = {that.uvalue + uvalue};

      //do addition B+A
   } 
   else if((val1negative==true)&&(val2negative==true)){
      result.uvalue = {that.uvalue + uvalue};
      result.is_negative = true;

      //do addition flip sign (-b+-a)
   }
   else if((val1negative==true)&&(val2negative==false)){
      result.uvalue = {that.uvalue - uvalue};
      result.is_negative = true;

      //run subtraction (-b+a) add negative
   }
   else{
      //run subtraction (b+-a), B-A stay positive
      result.uvalue = {that.uvalue - uvalue};
 
      }
   }
   return result;
}

bigint bigint::operator- (const bigint& that) const {
   bigint result;
   bool isval1Larger = false;
   if(that.uvalue < uvalue){
      isval1Larger = true;
   }

   bool val1negative = false;
   bool val2negative = false;
   if(this->is_negative == true){
      val1negative = true;
   }
   if(that.is_negative == true){
      val2negative = true;
   }

//now that you know which value is larger, 
//you can run the code below!

   if(isval1Larger){
      
      if((val1negative==false)&&(val2negative==false)){
         //do subtraction A-b
         result.uvalue = {uvalue - that.uvalue};
      } 
      else if((val1negative==true)&&(val2negative==true)){
         //do subtraction a-b slap on a negative
         result.uvalue = {uvalue - that.uvalue};
         result.is_negative = true;
      }
      
      else if((val1negative==true)&&(val2negative==false)){
         //addition slap on a negative (-a-b)
         result.uvalue = {uvalue + that.uvalue};
         result.is_negative = true;
      }

      else{
         //run addition leave positive (a--b)
         result.uvalue = {uvalue + that.uvalue};
      }

}
      else{ //B is larger now
         if((val1negative==false)&&(val2negative==false)){
         //do subtraction (b-a)
         result.uvalue = {that.uvalue - uvalue};

      } 
      else if((val1negative==true)&&(val2negative==true)){
         //do subtraction (-(b-a)) slap on a negative
         result.uvalue = {that.uvalue - uvalue};
         result.is_negative = true;

      }
      else if((val1negative==true)&&(val2negative==false)){
         //run addition (-b-a) put on a negative
         result.uvalue = {that.uvalue + uvalue};
         result.is_negative = true;
      }

      else{
         //run addition (b--a) keep positive
         result.uvalue = {that.uvalue + uvalue};

      }
   }
return result;
}


bigint bigint::operator* (const bigint& that) const {
   bigint result;
   int numberOfNegatives = 0;
   if(this->is_negative==true){
      numberOfNegatives = numberOfNegatives + 1;
   }
      if(that.is_negative == true){
      numberOfNegatives = numberOfNegatives + 1;
   }
   result.uvalue = {uvalue * that.uvalue};

 //if theres only 1 negative, it's negative
   if(numberOfNegatives==1){ 
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   bigint result;
   int numberOfNegatives = 0;
   if(this->is_negative==true){
      numberOfNegatives = numberOfNegatives + 1;
   }
   if(that.is_negative==true){
      numberOfNegatives = numberOfNegatives + 1;
   }
   result.uvalue = {uvalue / that.uvalue};

   //if theres only 1 negative, it's negative
   if(numberOfNegatives==1){
      result.is_negative = true;
   }
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result;
      int numberOfNegatives = 0;
   if(this->is_negative == true){
      numberOfNegatives = numberOfNegatives + 1;
   }
   if(that.is_negative == true){
      numberOfNegatives = numberOfNegatives + 1;
   }
   result.uvalue = {uvalue % that.uvalue};

   //if theres only 1 negative, it's negative
   if(numberOfNegatives==1){
      result.is_negative = true;
   }
   return result;
}

bool bigint::operator== (const bigint& that) const {
int numberOfNegatives = 0;

//check element of each one
if(this->is_negative == true){ // is there a negative
   numberOfNegatives++;
}

if(that.is_negative == true){ //is there  a negative
   numberOfNegatives++;
}
//odd number of negatives, then can't possibly be the same.
if(numberOfNegatives==1){ 
   return false;
}
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
//if left = negative and right = positive, it's true
   if((this->is_negative==true)&&(that.is_negative==false)){ 
      return true;
   }
   // left = positive, right = negative,false.
      if((this->is_negative==false)&&(that.is_negative==true)){ 
      return false;
   }
   
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << (that.is_negative ? "_" : "")
            << that.uvalue;
}

