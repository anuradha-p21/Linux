/***********************************************************************
* FILENAME :        simulation.cpp
*
* DESCRIPTION :
*     Linux Driver for UART on Emulated QEMU Versatile PB Board.
* AUTHOR :   Anuradha Patelkhana
*
************************************************************************/


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "uart_pl011.h"

char buf[64];
uint8_t buf_idx = 0u;
/* introductory function to type message on screen */
static void parse_cmd(void) {
    if (!strncmp("help\r", buf, strlen("help\r"))) {
        uart_write("Just type and see what happens!\n");
    } else if (!strncmp("uname\r", buf, strlen("uname\r"))) {
        uart_write("bare-metal arm 06_uart\n");
    }
}
/* function to return the operator from input  */
static char isoperator(char c){
switch (c){
case '+' : return '+';
case '-' : return '-';
case '*' : return '*';
case '/' : return '/';
default : return '\0';
}
}
/* function to return the calculation from input*/
static int calc_res(int a, int b, char op){
 switch (op){
case '+' : return a + b;
case '-' : return a - b;
case '*' : return a * b;
case '/' : return a / b;
default : return 0;
}
}

int main() {
	/* configure the UART to 8 data bits, 1 stop bit, no parity and 9600 baudrate*/
        uart_config config = {
            .data_bits = 8,
            .stop_bits = 1,
            .parity = false,
            .baudrate = 9600
        };
        uart_configure(&config);

        uart_write("Calculator\n");
        uart_write("Syntax: calc a + b\n");
bool cont =true;
    while(cont) {
  uart_write(": ");
        while (1) {
            char c;
            if (uart_getchar(&c) == UART_OK) {
              /* if (c == '\b') { 
                if(buf_idx != 0) --buf_idx;
                  continue;  }*/
              
                 uart_putchar(c);
                 if(c == 'q' || c=='Q') {cont = false; break;}
                 if(c == ' ') continue;
                 if (c == '\r') {
                  uart_write("\n");
                  buf[buf_idx % 64] = '\0';
                  break;  
                }  
          buf[buf_idx % 64] = c;
          buf_idx++; 
       }
    }
if (!cont) break;
int i=4,sign=1;

long a=0, b=0, res; 
char op;
//the input should start with calc, check this way or starting from 0 index
 if(buf_idx > 3 && buf[0]=='c' && buf[1]=='a' && buf[2]=='l' && buf[3]=='c')
  {
  //check for the sign bit of first operand
   if(i<buf_idx && buf[i]=='-') { sign = -1; i++; }
   else if(i >= buf_idx) {uart_write("invalid syntax\n"); return 5; }
      else if (buf[i]=='+') {sign = 1; i++;}
   //extract first operand
   
   while(i<buf_idx && isdigit(buf[i])){
          a = a * 10;    //a = 10, 150
          a = a + (buf[i] - '0');  //a=1,  a = 10 + 5 = 15,, 158
          i++;
         }
    if(sign < 0) a = 0-a;
    sign =1; 
//there should be operator, save it in op
//isoperator returns operator if valid else a null.
    op = isoperator(buf[i++]) ;  
    if(op == '\0') {uart_write("missing/invalid operator\n"); return 6; }  
     
//check for the sign bit of second operand
   if(i<buf_idx && buf[i]=='-') { sign = -1; i++; }
   else if(i >= buf_idx) {uart_write("invalid syntax\n"); return 5; }
      else if (buf[i]=='+') {sign = 1; i++;}

//extract the second operator 
     while(i<buf_idx && isdigit(buf[i])){
      b = b * 10;
      b = b + (buf[i] - '0');
      i++;
     }
     if(sign < 0) b = 0-b;
     sign =1;
//calc_res will calc the res
    res = calc_res(a, b, op); 
 
//convert the res into digits of charector array.
//using the same buf array for the digits of res
     if (res < 0) { res = 0-res; sign =-1; }
  
      int digit = res % 10;
      buf_idx=0u;
      while( res != 0)
       {
          buf[buf_idx % 64] = digit + '0';
          buf_idx++;
          res =  res /10;
          digit = res % 10;
          }
     uart_write("output: ");
//print sign bit if the result is -ve
     if(sign < 0) uart_putchar('-');      
//need to print the array in reverse 
//unsigned can never be lessthan zero, always check != 0
       while (--buf_idx != 0u)   // buf_idx >= 0    
       uart_putchar(buf[buf_idx]);    
       uart_putchar(buf[buf_idx]);
       uart_putchar('\n');
   }
else
  {  uart_write("invalid syntax\n"); return 5; }
}
return 0;
}
