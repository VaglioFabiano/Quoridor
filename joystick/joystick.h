#include "LPC17xx.h"


void joystick_init(void);
void joystick_Select(void);
void joystick_Down(void);
void joystick_Left(void);
void joystick_Right(void);
void joystick_Up(void);
void SalvaMossa (uint16_t player, uint16_t  moveWall, uint16_t OrizVert, uint16_t y, uint16_t  x);
void ModalitaMuro(void);
void SalvaMuro (uint16_t player, uint16_t  moveWall, uint16_t OrizVert, uint16_t x, uint16_t  y);
void RitornoModalitaGiocatore(void);
void giraMuro(void);
void rigiraMuro(void); 
void Timer0_expired(void);
void CancellaMuro_CancellaMossa(void);
int check_Trap();
void scorrimento_matrice(int player);

extern int Muri_PL0;
extern int Muri_PL1;

extern int joystick_SELECT;
extern int joystick_DOWN;
extern int joystick_LEFT;
extern int joystick_RIGHT;
extern int joystick_UP;
