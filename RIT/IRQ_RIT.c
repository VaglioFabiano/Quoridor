/*--------------------------------- Includes ---------------------------------*/
#include "lpc17xx.h"
#include "RIT.h"
#include "../button_EXINT/button.h"
#include "../joystick/joystick.h"
#include "../GLCD/GLCD.h"
#include "../timer/timer.h"



/*----------	ASM function to be called here  ----------*/
//extern *type* ASM_funct_name(*arguments*);

extern int press_INT0;
extern int press_KEY1;
extern int press_KEY2;



/*----------	Variables with extra visibility  ----------*/

typedef struct {
    uint32_t x;
    uint32_t y;
		uint32_t player;
} CoordinatePlayer;

volatile int cont =0;
volatile int flag1 =0;
volatile int flag2 =0;

volatile CoordinatePlayer Inizio = {3, 0, 0};


void RIT_IRQHandler (void)
{	

	static int joystick_SELECT = 0;
	static int joystick_DOWN = 0;
	static int joystick_LEFT = 0;
	static int joystick_RIGHT = 0;
	static int joystick_UP = 0;

		
	
	/*	START check for buttons De-Bouncing	*/
	if(press_INT0!=0){  /*	De-Bouncing INT0 */
		//press_INT0 ++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
			switch(press_INT0){
				case 1:
						LCD_Clear(Black);
						Pre_Start();
						SalvaMossa(0,0,0,3,0);
						SalvaMossa(1,0,0,3,6);
						ConsiglioMossa(Inizio);
						WriteWallsNumber(0,Muri_PL0);
						WriteWallsNumber(1,Muri_PL1);
						enable_timer(0);
						WriteTimeLeft(20); 
						press_INT0=10;
						
					break;
				default:
				break;
			}
		} else {	
			press_INT0=1;
			NVIC_EnableIRQ(EINT0_IRQn);						/* Riabilito INT0 a interrompere */
			LPC_PINCON->PINSEL4 |= (1 << 20);     /* Riattivo la modalita' di interrupt sul pin del bottone INT0*/
		}
	}
	
	if(press_KEY1!=0){  /*	De-Bouncing KEY1	*/
		
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
		
			switch(press_KEY1){
				case 1:
					if(press_INT0 != 0  && flag1 == 0){
						ModalitaMuro();
						press_KEY1=0;
						flag1 = 1;
						NVIC_EnableIRQ(EINT1_IRQn); 						/* enable Button interrupts		*/	
						LPC_PINCON->PINSEL4    |= (1 << 22);
					}else {
						flag1 = 0;
						press_KEY1 = 0;
						press_KEY2 = 0;
						RitornoModalitaGiocatore();
						NVIC_EnableIRQ(EINT1_IRQn); 						/* enable Button interrupts		*/	
						LPC_PINCON->PINSEL4    |= (1 << 22);
					}
				break;
				/*case 2: 
					if(press_INT0 != 0 ){
						
						press_KEY1 = 0;
						if(press_KEY2 == 1){
							NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			
							LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection
						}
						press_KEY2 = 0;
						RitornoModalitaGiocatore();
						NVIC_EnableIRQ(EINT1_IRQn); 						/* enable Button interrupts		
						LPC_PINCON->PINSEL4    |= (1 << 22);
						
						
							
					}
				break;*/	
			default:
				break;
			}
		} else {	/* KEY1 released */
			press_KEY1 = 0;
			NVIC_EnableIRQ(EINT1_IRQn); 						/* enable Button interrupts		*/	
			LPC_PINCON->PINSEL4    |= (1 << 22);    /* External interrupt 0 pin selection */
		}
	}
		
	if(press_KEY2!=0){  /*	De-Bouncing KEY2	*/
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
			//reset_RIT();
			switch(press_KEY2){
			case 1:
				/*	!! Write the first press function for KEY2 here !!	*/
				if(flag1 != 0 && flag2 == 0){
					giraMuro();
					press_KEY2=0;
					flag2 = 1;
					NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts	*/	
					LPC_PINCON->PINSEL4    |= (1 << 24);
				}else {
					rigiraMuro();
					press_KEY2 = 0;
					flag2 = 0;
					NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts	*/	
					LPC_PINCON->PINSEL4    |= (1 << 24);
				}
				break;
			/*case 2: 
				if(press_KEY1 != 0){
					rigiraMuro();
					press_KEY2 = 0;
						NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts		
						LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection
						NVIC_EnableIRQ(EINT1_IRQn); 						/* enable Button interrupts		
						LPC_PINCON->PINSEL4    |= (1 << 22);    /* External interrupt 0 pin selection 
				}
				break;*/
			
			default:				
				break;
			}
		} else {	/* KEY2 released */
			//down2=0;
			press_KEY2=0;
			//reset_RIT();
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts*/			
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection*/
		}
	}
	
	
	//END De-Bouncing check
	
	
	/*	Joystick Interrupt Action	*/
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	/*	Joystick SELECT	*/
		/*	!! Write the function for JOYSTICK SELECT here !!	*/
		joystick_SELECT++;
		switch(joystick_SELECT){
			case 1:
			
     		joystick_Select();
			break;
			default:
			break;
		}
	} else {	/* JOYSTICK SELECT released */
		joystick_SELECT=0;

	}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	/*	Joystick DOWN	*/
		/*	!! Write the function for JOYSTICK DOWN here !!	*/
		joystick_DOWN++;
				switch(joystick_DOWN){
					case 1:
					
						joystick_Down();
					break;
					default:
					break;
		}
	} else {	/* joystick_DOWN released */
		joystick_DOWN=0;

	}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	/*	Joystick LEFT	*/
		/*	!! Write the function for JOYSTICK LEFT here !!	*/
		joystick_LEFT++;
						switch(joystick_LEFT){
							case 1:
							
								joystick_Left();
							break;
							default:
							break;
				}
			} else {	/* joystick_DOWN released */
				joystick_LEFT=0;

	}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	/*	Joystick RIGHT	*/
		/*	!! Write the function for JOYSTICK RIGHT here !!	*/
		joystick_RIGHT++;
		switch(joystick_RIGHT){
			case 1:
				/*	!! Write the first press function for JOYSTICK UP here !!	*/
				joystick_Right();
			break;
			default:
			break;
		}
	} else {	/* JOYSTICK UP released */
		joystick_RIGHT=0;
	}
	
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){	/*	Joystick UP, with De-Bouncing control	*/
		joystick_UP++;
		switch(joystick_UP){
			case 1:
				/*	!! Write the first press function for JOYSTICK UP here !!	*/
				joystick_Up();
			break;
			default:
			break;
		}
	} else {	/* JOYSTICK UP released */
		joystick_UP=0;
	}
		
	
	
	/**					TOUCH PANEL					
	if (getDisplayPoint(&display, Read_Ads7846(), &matrix)) {
		// operazioni da svolgere, display.x  e display.y sono le coordinate del punto toccato
	}**/
	
	
	/**					ADC	handler					
	ADC_start_conversion();**/
	
	
	//reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
