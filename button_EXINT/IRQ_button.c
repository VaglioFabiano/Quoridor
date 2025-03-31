#include "button.h"
#include "lpc17xx.h"
#include "../GLCD/GLCD.h" 
#include "../joystick/joystick.h"
#include "../RIT/RIT.h"

/*----------	Extern Variables visible here  ----------*/


/*----------	Variables with extra visibility  ----------*/


int press_INT0 = 0;
int press_KEY1 = 0;
int press_KEY2 = 0;




void EINT0_IRQHandler (void)	  	/* INT0														 */
{		
		//enable_RIT();	
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection: setting INT0 pin as readable input by writing 0 at bit 20 of PINSEL4. Default reset value is 00 so bit 21 is 0 */	
	press_INT0 = 1; 

	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
		
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{								
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts */			 
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	press_KEY1 =1;
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts */			 
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
	press_KEY2=1;
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}

