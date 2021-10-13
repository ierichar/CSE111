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
//uvalue(); that.uvalue();
bool val1negative = false;
int val1RealSize = uvalue.size();
bool val2negative = false;
int val2RealSize = that.uvalue.size();

//check element of each one
if(uvalue[val1RealSize] == "_"){ //jump to end of vector 1, check for _
   val1negative = true;
   val1RealSize = val1RealSize - 1;
}

if(that.uvalue[val2RealSize] == "_"){ //jump to end of vector 2, check for _
   val2negative = true;
   val2RealSize = val2RealSize - 1;
}

bool isval1Larger = false;
int temp1 = 0;
int temp2 = 0;

if (val1RealSize == val2RealSize){
   if(val1negative){
      temp1 = uvalue[uvalue.size()-1];
   }
   else{
      temp1 = uvalue[uvalue.size()];
   }
   if(val2negative){
      temp2 = that.uvalue[that.uvalue.size()-1];
   }
   else{
      temp2 = that.uvalue[that.uvalue.size()];
   }

   if(temp1 > temp2){
      isval1Larger = true;
   }

   //now that you know which value is larger, you can run the code below!
}

if(isval1Larger){
   if((val1negative==false)&&(val2negative==false)){
      ubigint result {uvalue + that.uvalue};

   } 
   if((val1negative==true)&&(val2negative==true)){
      ubigint result {uvalue + that.uvalue};
      result.ubig_value.push_back ("_");
   }
   if((val1negative==true)&&(val2negative==false)){
       ubigint result {uvalue - that.uvalue};
       result.ubig_value.push_back ("_");
      //run subtraction (-a+b) add negative
      
   }
   else{
       ubigint result {uvalue - that.uvalue};
      //run subtraction (a+-b), A-B stay positive
   }
}
   else{ //B is larger now
      if((val1negative==false)&&(val2negative==false)){
         ubigint result {that.uvalue + uvalue};
      //do addition B+A
   } 
   if((val1negative==true)&&(val2negative==true)){
      ubigint result {that.uvalue + uvalue};
      result.ubig_value.push_back ("_");
      
      //do addition flip sign (-b+-a)
   }
   if((val1negative==true)&&(val2negative==false)){
      ubigint result {that.uvalue - uvalue};
      result.ubig_value.push_back ("_");
      
      //run subtraction (-b+a) add negative
   }
   else{
      //run subtraction (b+-a), B-A stay positive
      ubigint result {that.uvalue - uvalue};
   }
}
//both of those subtract 1 from a size to account for the negative taking up 1 space.

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

   //ubigint result {uvalue + that.uvalue};
   return result;
}

bigint bigint::operator- (const bigint& that) const {
//uvalue(); that.uvalue();
bool val1negative = false;
int val1RealSize = uvalue.size();
bool val2negative = false;
int val2RealSize = that.uvalue.size();

//check element of each one
if(uvalue[val1RealSize] == "_"){ //jump to end of vector 1, check for _
   val1negative = true;
   val1RealSize = val1RealSize - 1;
}

if(that.uvalue[val2RealSize] == "_"){ //jump to end of vector 2, check for _
   val2negative = true;
   val2RealSize = val2RealSize - 1;
}

bool isval1Larger = false;
int temp1 = 0;
int temp2 = 0;

if (val1RealSize == val2RealSize){
   if(val1negative){
      temp1 = uvalue[uvalue.size()-1];
   }
   else{
      temp1 = uvalue[uvalue.size()];
   }
   if(val2negative){
      temp2 = that.uvalue[that.uvalue.size()-1];
   }
   else{
      temp2 = that.uvalue[that.uvalue.size()];
   }

   if(temp1 > temp2){
      isval1Larger = true;
   }
   //now that you know which value is larger, you can run the code below!
}

if(isval1Larger){
   if((val1negative==false)&&(val2negative==false)){
      //do subtraction A-b
      ubigint result {uvalue - that.uvalue};
   } 
   if((val1negative==true)&&(val2negative==true)){
      //do subtraction a-b slap on a negative
      ubigint result {uvalue - that.uvalue};
      result.ubig_value.push_back ("_");
   }
   if((val1negative==true)&&(val2negative==false)){
      //addition slap on a negative (-a-b)
      ubigint result {uvalue + that.uvalue};
      result.ubig_value.push_back ("_");
   }
   else{
      //run addition leave positive (a--b)
      ubigint result {uvalue + that.uvalue};
   }
}
   else{ //B is larger now
      if((val1negative==false)&&(val2negative==false)){
      //do subtraction (b-a)
      ubigint result {that.uvalue - uvalue};
   } 
   if((val1negative==true)&&(val2negative==true)){
      //do subtraction (-(b-a)) slap on a negative
      ubigint result {that.uvalue - uvalue};
      result.ubig_value.push_back ("_");
   }
   if((val1negative==true)&&(val2negative==false)){
      //run addition (-b-a) put on a negative
      ubigint result {that.uvalue + uvalue};
      result.ubig_value.push_back ("_");
   }
   else{
      //run addition (b--a) keep positive
      ubigint result {that.uvalue  uvalue};
   }
}
   ubigint result {uvalue - that.uvalue};
   // SUBTRACTION
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
   return result;
}


bigint bigint::operator* (const bigint& that) const {
   int numberOfNegatives = 0;
   if(uvalue[uvalue.size()] == "_"){
      numberOfNegatives = numberOfNegatives + 1;
   }
      if(that.uvalue[uvalue.size()] == "_"){
      numberOfNegatives = numberOfNegatives + 1;
   }
   ubigint result {uvalue * that.uvalue};

   if(numberOfNegatives==1){
      result.ubig_value.push_back ("_");
   }
   return result;
}

bigint bigint::operator/ (const bigint& that) const {
   int numberOfNegatives = 0;
   if(uvalue[0] == "_"){
      numberOfNegatives = numberOfNegatives + 1;
   }
   if(that.uvalue[0] == "_"){
      numberOfNegatives = numberOfNegatives + 1;
   }
   ubigint result {uvalue / that.uvalue};

   if(numberOfNegatives==1){
      result.ubig_value.push_back ("_");
   }
   return result;
}

bigint bigint::operator% (const bigint& that) const {
      int numberOfNegatives = 0;
   if(uvalue[0] == "_"){
      numberOfNegatives = numberOfNegatives + 1;
   }
   if(that.uvalue[0] == "_"){
      numberOfNegatives = numberOfNegatives + 1;
   }
   ubigint result {uvalue % that.uvalue};

   if(numberOfNegatives==1){
      result.ubig_value.push_back ("_");
   }
   return result;
}

bool bigint::operator== (const bigint& that) const {
int numberOfNegatives = 0;

//check element of each one
if(uvalue[uvalue.size()] == "_"){ // is there a negative
   numberOfNegatives++;
}

if(that.uvalue[that.uvalue.size()] == "_"){ //is there  a negative
   numberOfNegatives++;
}
if(numberOfNegatives==1){ //if there is an odd number of negatives, they can't possibly be the same.
   return false;
}

   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
bool val1negative = false;
bool val2negative = false;

if(uvalue[uvalue.size()] == "_"){ // is there a negative
   val1negative = true;
}

if(that.uvalue[that.uvalue.size()] == "_"){ //is there  a negative
bool val2negative = true;
}
   if((val1negative==true)&&(val2negative==false){ //if left is negative and right is positive, it's true no matter what
      return true;
   }
      if((val1negative==false)&&(val2negative==true){ // if left is positive and right is negative, its false no matter what.
      return false;
   }
   
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

ostream& operator<< (ostream& out, const bigint& that) {
   return out << "bigint(" << (that.is_negative ? "-" : "+")
              << "," << that.uvalue << ")";
}

