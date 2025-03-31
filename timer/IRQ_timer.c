/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include <string.h>
#include "lpc17xx.h"
#include "timer.h"
#include "../GLCD/GLCD.h" 
#include "../TouchPanel/TouchPanel.h"
#include "../joystick/joystick.h"

extern int conteggio_secondi_0;
extern int conteggio_secondi_1;


int conteggio_secondi_0 = 20;
int conteggio_secondi_1 = 5;
/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

void TIMER0_IRQHandler (void)
{
	/*	!! Write the function here !!	*/
	conteggio_secondi_0--;  // Incrementa il conteggio dei secondi
	WriteTimeLeft(conteggio_secondi_0);
   // Esegui la funzione ogni secondo passando il secondo corrente
   if (conteggio_secondi_0 == 0) {
        // Disabilita il Timer 0 quando il conteggio raggiunge 0
        disable_timer(0);
				Timer0_expired();
    }

  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void){
	disable_timer(0);
	conteggio_secondi_1 --;
	if (conteggio_secondi_1 == 0) {
		disable_timer(1);
		RitornoModalitaGiocatore();
	}
	
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
