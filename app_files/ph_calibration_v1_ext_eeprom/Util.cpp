#include <math.h>
#include "Util.h"

void reverse(char* str, int len) {
  int i = 0, j = len - 1, temp;
  while (i < j) {
    temp = str[i];
    str[i] = str[j];
    str[j] = temp;
    i++;
    j--;
  }
}
int intToStr(int x, char* str, int d) { 
  int i = 0;
  if (x == 0) {
    str[i++] = (x % 10) + '0'; 
    x = x / 10; 
  } 
  while (x) { 
    str[i++] = (x % 10) + '0'; 
    x = x / 10; 
  } 
  // If number of digits required is more, then 
  // add 0s at the beginning 
  while (i < d) str[i++] = '0'; 
  reverse(str, i); 
  str[i] = '\0';
  return i; 
} 
void ftoa(float n, char* res, int afterpoint) {
  // Extract integer part 
  int ipart = (int)n; 
  // Extract floating part 
  float fpart = n - (float)ipart; 
  // convert integer part to string 
  int i = intToStr(ipart, res, 0); 
  // check for display option after point 
  if (afterpoint != 0) { 
    res[i] = '.'; // add dot 
    // Get the value of fraction part upto given no. 
    // of points after dot. The third parameter  
    // is needed to handle cases like 233.007 
    fpart = fpart * pow(10, afterpoint); 
    intToStr((int)fpart, res + i + 1, afterpoint); 
  } 
}