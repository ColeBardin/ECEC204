// Example 1C : Counting vowels and consonants in an ASCII string
#include "msp.h"


/**
 * main.c
 */
char s[]="QUICK BROWN FOX JUMPS OVER THE LAZY DOG";
char x;
uint16_t i=0;
uint16_t vowels=0;
uint16_t consonants=0;
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
	while(*(s+i)!='\0')
	{
	    x=*(s+i);
	    if((x=='A')||(x=='E')||(x=='I')||(x=='O')||(x=='U'))
	        vowels++;
	    else if ((x>='A')&&(x<='Z'))
	        consonants++;
	    i++;
	 }
	while(1);
}
