//Example 1D: Bitwise and Boolean operators
#include "msp.h"


uint8_t a,b,x,y,c0,c1,c2,c3,c4,c5,c6,c7,c8,c9,c10,c11,c12,c13,c14;
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
	a=0b00001011;
	b=0b00000101;
	c0=~a;//bitwise NOT aka one's complement
	c1=a|b;//bitwise OR
	c2=a&b;//bitwise AND
	c3=a^b;//bitwise XOR
	c4=~(a|b);// bitwise NOR
	c5=~(a&b);//bitwise NAND
	c6=~(a^b);//bitwise XNOR
	c7=(a==b);// EQUAL TO
	c8=(a!=b);// NOT EQUAL TO
	c9=(a<b);// LESS THAN
	c10=(a<=b);// LESS THAN OR EQUAL TO
	c11=(a>b);//  GREATER THAN
	c12=(a>=b);// GREATER THAN OR EQUAL TO
	x=0b10101010;
	y=0b01010101;
	c13=(a>b)&&(x<y);// Boolean AND
	c14=(a>b)||(x<y);// Boolean OR
	while(1);





	// stop watchdog timer
}
