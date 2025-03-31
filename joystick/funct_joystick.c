/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           funct_joystick.h
** Last modified Date:  2018-12-30
** Last Version:        V1.00
** Descriptions:        High level joystick management functions
** Correlated files:    lib_joystick.c, funct_joystick.c
**--------------------------------------------------------------------------------------------------------       
*********************************************************************************************************/

#include "lpc17xx.h"
#include "joystick.h"
#include "../GLCD/GLCD.h" 
#include "../button_EXINT/button.h"
#include "../timer/timer.h"

/*----------	Variables  ----------*/
extern int Muri_PL0;
extern int Muri_PL1;

extern int joystick_SELECT;
extern int joystick_DOWN;
extern int joystick_LEFT;
extern int joystick_RIGHT;
extern int joystick_UP;
extern int flag1;
extern int flag2;

int joystick_SELECT=0;
int joystick_DOWN=0;
int joystick_LEFT=0;
int joystick_RIGHT=0;
int joystick_UP=0;

int Muri_PL0 = 8;
int Muri_PL1 = 8;

//Contatori
volatile int contatore_mosse = 0;
volatile int contatore_muri = 0;
volatile int flag = 1;
volatile int trasso=1;
volatile	int trap1 = 0; // se trap = 1 non è intrappolato se == 0 è intrappolato il primo giocatore
volatile	int trap2 = 0;
volatile int joystick_MOVE = 0;
volatile int controllo_MURI = 0;
volatile int possibili[7][7] = {{0}}; 

volatile uint32_t VettoreMosse[500];
volatile uint32_t VettoreMuri[16];

typedef struct {
    uint32_t x;
    uint32_t y;
		uint32_t player;
} CoordinatePlayer;

typedef struct {
    uint32_t x;
    uint32_t y;
		uint32_t ov; //0 per i muri orizzontali 1 per i muri veriticali
		uint32_t player;
} CoordinateMuro;

volatile CoordinatePlayer NuovaPosizione = {0, 0, 0};
volatile CoordinatePlayer VettorePosizione;
volatile CoordinatePlayer PosizioneAttuale;
volatile CoordinatePlayer Consiglio;
volatile CoordinatePlayer PosizioneDaConsigliare;
volatile CoordinatePlayer PL;
volatile CoordinatePlayer PosizioneTrap;
volatile uint32_t playerDaMuovere = 0;
volatile uint32_t playerDaConsigliare = 0;
volatile CoordinateMuro MuroDaConfermare;
volatile CoordinateMuro MuroConfermato;
volatile CoordinateMuro Muro;

/*---------- Functions  ----------*/

CoordinatePlayer RecuperaPosizione(uint32_t player);
CoordinateMuro TrovaMuroLibero(CoordinateMuro Muro);

CoordinateMuro SkipMuroDown(CoordinateMuro Muro);
CoordinateMuro SkipMuroLeft(CoordinateMuro Muro);
CoordinateMuro SkipMuroRight(CoordinateMuro Muro);
CoordinateMuro SkipMuroUp(CoordinateMuro Muro);

uint32_t RecuperaCoordinatay (uint32_t mossa);
uint32_t RecuperaCoordinatax (uint32_t mossa);
uint32_t RecuperaPlayer(uint32_t mossa);
uint32_t RecuperaVerticaleOrizzontale(uint32_t mossa);
int ControlloMossa(CoordinatePlayer coord);
int ControlloMossaMuroDown();
int ControlloMossaMuroLeft();
int ControlloMossaMuroRight();
int ControlloMossaMuroUp();
void ConsiglioMossa(CoordinatePlayer coord);
void CancellaConsiglioMossa(CoordinatePlayer coord);
void scorrimento_matrice(int player);



void joystick_Down(){
	if(press_INT0 != 0 && joystick_MOVE == 0 && flag1 == 0 ){
		int x;
		int y;
		x = RecuperaCoordinatax(VettoreMosse[contatore_mosse - 1]);
		y = RecuperaCoordinatay(VettoreMosse[contatore_mosse - 1]);
		playerDaMuovere = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);//dovrai aggiungere il controllo sulla mossa inventata allo scadere del timer
    PosizioneAttuale = RecuperaPosizione(playerDaMuovere);
		playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 1]);
		PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
		NuovaPosizione.x = PosizioneAttuale.x;
		NuovaPosizione.y = PosizioneAttuale.y + 1;
		if(NuovaPosizione.y == y && NuovaPosizione.x == x && ControlloMossaMuroDown(NuovaPosizione) == 1){
			NuovaPosizione.y = NuovaPosizione.y +1;
		}
		NuovaPosizione.player = PosizioneAttuale.player;
		//controllo se mossa è concessa poi passaggio dopo pensa al fatto che se la mossa non è salvata viene poi 
		//cancellata e ridisegna la pedina al posto precedente
		if(ControlloMossa(NuovaPosizione) == 1 && ControlloMossaMuroDown(NuovaPosizione) == 1){
			CancellaConsiglioMossa(PosizioneAttuale);
			DrawDelete(PosizioneAttuale.x,PosizioneAttuale.y, Black);
			DrawPawn(NuovaPosizione.x, NuovaPosizione.y, NuovaPosizione.player);
			//ConsiglioMossa(PosizioneDaConsigliare);
			joystick_MOVE = 1;
		}
	}
	if(press_INT0 != 0 && flag1 == 1){
		MuroDaConfermare.y = MuroDaConfermare.y+1;
		if(ControlloMuro(MuroDaConfermare) == 1){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y-1, Black); //Cancello il muro vecchio
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y-1, Black); //Cancello il muro vecchio
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
		}else if (ControlloMuro(MuroDaConfermare) == 0){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y-1, Black);
				MuroDaConfermare = SkipMuroDown(MuroDaConfermare);
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y-1, Black);
				MuroDaConfermare = SkipMuroDown(MuroDaConfermare);
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 

			}
		}
	}
}

CoordinateMuro SkipMuroDown(CoordinateMuro Muro){
	CoordinateMuro MuroI;
	int i ;
	MuroI = Muro;
	MuroI.y = MuroI.y-1;
		for(i=1 ; Muro.y+i<6; i++){
			Muro.y = Muro.y+i;
			if(ControlloMuro(Muro) == 1){
				return Muro;
			}
		}
		return MuroI;
}
void joystick_Left(){
	if(press_INT0 != 0 && joystick_MOVE == 0 && flag1 == 0 ){
		int x;
		int y;
		x = RecuperaCoordinatax(VettoreMosse[contatore_mosse - 1]);
		y = RecuperaCoordinatay(VettoreMosse[contatore_mosse - 1]);
		playerDaMuovere = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);
    PosizioneAttuale = RecuperaPosizione(playerDaMuovere);
		playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 1]);
		PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
		NuovaPosizione.x = PosizioneAttuale.x - 1 ;
		NuovaPosizione.y = PosizioneAttuale.y;
		if(NuovaPosizione.x == x && NuovaPosizione.y == y && ControlloMossaMuroLeft(NuovaPosizione) == 1){
			NuovaPosizione.x = NuovaPosizione.x - 1;
		}
		NuovaPosizione.player = PosizioneAttuale.player;
		if(ControlloMossa(NuovaPosizione) == 1 && ControlloMossaMuroLeft(NuovaPosizione) == 1){
			CancellaConsiglioMossa(PosizioneAttuale);
			DrawDelete(PosizioneAttuale.x,PosizioneAttuale.y, Black);
			DrawPawn(NuovaPosizione.x, NuovaPosizione.y, NuovaPosizione.player);
			//ConsiglioMossa(PosizioneDaConsigliare);
			joystick_MOVE = 1;
		}
	}
	if(press_INT0 != 0 && flag1 == 1){
		MuroDaConfermare.x = MuroDaConfermare.x-1;
		if(ControlloMuro(MuroDaConfermare) == 1){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x+1, MuroDaConfermare.y, Black); //Cancello il muro vecchio
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x+1, MuroDaConfermare.y, Black); //Cancello il muro vecchio
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
			}
		}else if (ControlloMuro(MuroDaConfermare) == 0){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x+1, MuroDaConfermare.y, Black);
				MuroDaConfermare = SkipMuroLeft(MuroDaConfermare);
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x+1, MuroDaConfermare.y, Black);
				MuroDaConfermare = SkipMuroLeft(MuroDaConfermare);
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
			}
		}
	}
}
CoordinateMuro SkipMuroLeft(CoordinateMuro Muro){
	CoordinateMuro MuroI;
	int i ;
	MuroI = Muro;
		for(i=0; Muro.x-i>0; i++){
			Muro.x = Muro.x-i;
			if(ControlloMuro(Muro) == 1){
				return Muro;
			}
		}
	return MuroI;
}
void joystick_Right(){
	if(press_INT0 != 0 && joystick_MOVE == 0 && flag1 == 0 ){
		int x;
		int y;
		x = RecuperaCoordinatax(VettoreMosse[contatore_mosse - 1]);
		y = RecuperaCoordinatay(VettoreMosse[contatore_mosse - 1]);
		playerDaMuovere = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);
    PosizioneAttuale = RecuperaPosizione(playerDaMuovere);
		playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 1]);
		PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
		NuovaPosizione.x = PosizioneAttuale.x + 1 ;
		NuovaPosizione.y = PosizioneAttuale.y;
		if(NuovaPosizione.x == x && NuovaPosizione.y == y && ControlloMossaMuroRight(NuovaPosizione) == 1 ){
			NuovaPosizione.x = NuovaPosizione.x +1;
		}
		NuovaPosizione.player = PosizioneAttuale.player;
		if(ControlloMossa(NuovaPosizione) == 1 && ControlloMossaMuroRight(NuovaPosizione) == 1){
			CancellaConsiglioMossa(PosizioneAttuale);
			DrawDelete(PosizioneAttuale.x,PosizioneAttuale.y, Black);
			DrawPawn(NuovaPosizione.x, NuovaPosizione.y, NuovaPosizione.player);
			
			joystick_MOVE = 1;
		}
	}
	if(press_INT0 != 0 && flag1 == 1){
		MuroDaConfermare.x = MuroDaConfermare.x+1;
		if(ControlloMuro(MuroDaConfermare) == 1){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x-1, MuroDaConfermare.y, Black); //Cancello il muro vecchio
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x-1, MuroDaConfermare.y, Black); //Cancello il muro vecchio
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
		}else if (ControlloMuro(MuroDaConfermare) == 0){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x-1, MuroDaConfermare.y, Black);
				MuroDaConfermare = SkipMuroRight(MuroDaConfermare);
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x-1, MuroDaConfermare.y, Black);
				MuroDaConfermare = SkipMuroRight(MuroDaConfermare);
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
			}
		}
	}
}
CoordinateMuro SkipMuroRight(CoordinateMuro Muro){
	CoordinateMuro MuroI;
	int i ;
	MuroI = Muro;
		for(i=1; Muro.x+i<6; i++){
			Muro.x = Muro.x+1;
			if(ControlloMuro(Muro) == 1){
				return Muro;
			}
		}
	return MuroI;
}
void joystick_Up(){
	if(press_INT0 != 0 && joystick_MOVE == 0 && flag1 == 0 ){
		int x;
		int y;
		x = RecuperaCoordinatax(VettoreMosse[contatore_mosse - 1]);
		y = RecuperaCoordinatay(VettoreMosse[contatore_mosse - 1]);
		playerDaMuovere = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);
    PosizioneAttuale = RecuperaPosizione(playerDaMuovere);
		playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 1]);
		PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
		NuovaPosizione.x = PosizioneAttuale.x;
		NuovaPosizione.y = PosizioneAttuale.y-1;
		if(NuovaPosizione.y == y && NuovaPosizione.x == x && ControlloMossaMuroUp(NuovaPosizione) == 1){
			NuovaPosizione.y = NuovaPosizione.y -1;
		}
		NuovaPosizione.player = PosizioneAttuale.player;
		if(ControlloMossa(NuovaPosizione) == 1 && ControlloMossaMuroUp(NuovaPosizione) == 1){
			CancellaConsiglioMossa(PosizioneAttuale);
			DrawDelete(PosizioneAttuale.x,PosizioneAttuale.y, Black);
			DrawPawn(NuovaPosizione.x, NuovaPosizione.y, NuovaPosizione.player);
			//ConsiglioMossa(PosizioneDaConsigliare); 
			joystick_MOVE = 1;
		}
	}
	if(press_INT0 != 0 && flag1 == 1){
		MuroDaConfermare.y = MuroDaConfermare.y-1;
		if(ControlloMuro(MuroDaConfermare) == 1){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y+1, Black); //Cancello il muro vecchio
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y+1, Black); //Cancello il muro vecchio
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
		}else if (ControlloMuro(MuroDaConfermare) == 0){
			if(MuroDaConfermare.ov == 1){
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y+1, Black);
				MuroDaConfermare = SkipMuroUp(MuroDaConfermare);
				DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
			if(MuroDaConfermare.ov == 2){
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y+1, Black);
				MuroDaConfermare = SkipMuroUp(MuroDaConfermare);
				DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red); 
			}
		}
	}
}
CoordinateMuro SkipMuroUp(CoordinateMuro Muro){
	CoordinateMuro MuroI;
	int i ;
	MuroI = Muro;
		for(i=1; Muro.y-i>0; i++){
			Muro.y = Muro.y-i;
			if(ControlloMuro(Muro) == 1){
				return Muro;
			}
		}
	return MuroI;
}

void joystick_Select(){
	if(press_INT0 != 0){
		if(flag1 == 0){	//se key1 != 0 vuol dire che devi mettere un muro e non salvare la mossa
			if(joystick_MOVE == 1){
				SalvaMossa(NuovaPosizione.player, 0, 0, NuovaPosizione.x, NuovaPosizione.y);
				ConsiglioMossa(PosizioneDaConsigliare);
				if(NuovaPosizione.player == 0 && NuovaPosizione.y == 6){
					WinMessage(1);
					
					joystick_SELECT = 0;
					disable_timer(0);
					return;
				}
				if(NuovaPosizione.player == 1 && NuovaPosizione.y == 0){
					WinMessage(2);
					
					joystick_SELECT = 0;
					disable_timer(0);
					return;
				}
				joystick_MOVE = 0;
				disable_timer(0);
			}
		}
		if(flag1 != 0){ // if che permette il salvataggio del muro
      MuroConfermato = MuroDaConfermare;
			SalvaMuro(MuroConfermato.player, 1, MuroConfermato.ov, MuroConfermato.x, MuroConfermato.y);
			trap1=0;
			trap2=0;
			//if(check_Trap() == 1){ //controlla che il muro non intrappoli
				playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);
				PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
				ConsiglioMossa(PosizioneDaConsigliare);
				if(playerDaConsigliare == 1){
					Muri_PL0--;
					WriteWallsNumber(0,Muri_PL0);
				}else if (playerDaConsigliare == 0){
					Muri_PL1--;
					WriteWallsNumber(1,Muri_PL1);
				}
				flag1 = 0;
				flag2 = 0;
		/*}else {
				CancellaMuro_CancellaMossa();
				MessaggioMuroBlocca ();
				enable_timer(1);
				conteggio_secondi_1 = 5; //allo scadere del tempo ritorni alla modalità giocatore
				flag1 = 0;
				flag2 = 0;
			}*/
		}
	}
	enable_timer(0);
	conteggio_secondi_0 = 20; 
}

	/*
	player: definisce il giocatore con un numero 
	moveWall: 0 se è una move, 1 se è un muro
	OrizVert: 1 orizzontale, 2 verticale
	y: coordinate di y 
	x: coordinate di x
	*/
void SalvaMossa (uint16_t player, uint16_t  moveWall, uint16_t OrizVert, uint16_t x, uint16_t  y){
	volatile uint32_t move = 0;
	
	move = move + player;
	move <<= 4;
	move = move + moveWall;
	move <<= 4;
	move = move + OrizVert;
	move <<= 8;
	move = move + y;
	move <<= 8;
	move = move + x;
	
	VettoreMosse[contatore_mosse] = move;
	contatore_mosse++;
}
void SalvaMuro (uint16_t player, uint16_t  moveWall, uint16_t OrizVert, uint16_t x, uint16_t  y){
 	volatile uint32_t wall = 0;
	volatile uint32_t poggio;
	
	wall = wall + player;
	wall <<=4;
	wall = wall + moveWall;
	wall <<=4;
	wall = wall + OrizVert;
	wall <<=8;
	wall = wall + y;
	wall <<=8;
	wall = wall + x;
	
	poggio = VettoreMosse[contatore_mosse - 2]; 
	SalvaMossa(RecuperaPlayer(poggio), 0, 0, RecuperaCoordinatax(poggio), RecuperaCoordinatay(poggio));
	VettoreMuri[contatore_muri] = wall;
	contatore_muri++;
}
void CancellaMuro_CancellaMossa(){
	contatore_muri--;
	VettoreMuri[contatore_muri] = 0;
	contatore_mosse--;
	VettoreMosse[contatore_mosse] = 0;
}
CoordinatePlayer RecuperaPosizione(uint32_t player) {
    int i = contatore_mosse - 1;
    volatile CoordinatePlayer errore = {8, 8, 8};

    for (; i >= 0; i--) {
        volatile uint32_t check = VettoreMosse[i];
				check >>= 24;
        if (check == player) {
            
            VettorePosizione.x = RecuperaCoordinatax(VettoreMosse[i]);
            VettorePosizione.y = RecuperaCoordinatay(VettoreMosse[i]);
            VettorePosizione.player = RecuperaPlayer(VettoreMosse[i]);
            return VettorePosizione;
        }
    }

    return errore; //ritorna 8,8,8
}
//recupero la coordinata y da una stringa a 32 bit eliminando il resto
uint32_t RecuperaCoordinatay (uint32_t mossa){
	volatile uint32_t y;
	mossa >>= 8;
	mossa <<= 8;
	mossa <<= 16;
	mossa >>= 24;
	y = mossa;
	return y;
}
//recupero la coordinata x da una stringa a 32 bit eliminando il resto
uint32_t RecuperaCoordinatax (uint32_t mossa){
	volatile uint32_t x;
	mossa <<= 24;
	mossa >>= 24;
	x = mossa;
	return x;
}
uint32_t RecuperaPlayer(uint32_t mossa){
		volatile uint32_t player;
		mossa >>=24;
		player = mossa;
		return player;
}
uint32_t RecuperaVerticaleOrizzontale(uint32_t mossa){
	volatile uint32_t vertOriz;
	mossa <<= 12;
	mossa >>= 28;
	vertOriz = mossa;
	return vertOriz;
}

int ControlloMossa(CoordinatePlayer coord) {
    if(coord.x<=6 && coord.x>=0 && coord.y<=6 && coord.y>=0){
			return 1; // ritorno valido per il controllo che concerne il quadrato
		} else{
			return 0; // ritorno non valido per il controllo delle coordinate
		}
}
int ControlloMossaMuroDown(CoordinatePlayer coord){
	int i;
	
	for(i=0; i<contatore_muri; i++){
		Muro.x = RecuperaCoordinatax(VettoreMuri[i]);
    Muro.y = RecuperaCoordinatay(VettoreMuri[i]);
		Muro.ov = RecuperaVerticaleOrizzontale(VettoreMuri[i]);
    Muro.player = RecuperaPlayer(VettoreMuri[i]);
		if(Muro.ov == 1){
			if(coord.y == (Muro.y+1)){
				if(coord.x == Muro.x || coord.x == (Muro.x+1)){
					return 0;
				}
			}
		}
	}
	return 1;
}
int ControlloMossaMuroUp(CoordinatePlayer coord){
	int i;
	
	for(i=0; i<contatore_muri; i++){
		Muro.x = RecuperaCoordinatax(VettoreMuri[i]);
    Muro.y = RecuperaCoordinatay(VettoreMuri[i]);
		Muro.ov = RecuperaVerticaleOrizzontale(VettoreMuri[i]);
    Muro.player = RecuperaPlayer(VettoreMuri[i]);
		if(Muro.ov == 1){
			if(coord.y == Muro.y){
				if(coord.x == Muro.x || coord.x == (Muro.x+1)){
					return 0;
				}
			}
		}
	}
	return 1;
}
int ControlloMossaMuroLeft(CoordinatePlayer coord){
	int i;
	
	for(i=0; i<contatore_muri; i++){
	Muro.x = RecuperaCoordinatax(VettoreMuri[i]);
    Muro.y = RecuperaCoordinatay(VettoreMuri[i]);           		Muro.ov = RecuperaVerticaleOrizzontale(VettoreMuri[i]);
    Muro.player = RecuperaPlayer(VettoreMuri[i]);
 		if(Muro.ov == 2){
			if(coord.x == (Muro.x+1)){
				if(coord.y == Muro.y || coord.y == (Muro.y+1)){
					return 0;
				}
			}
		}
	}
	return 1;
}
int ControlloMossaMuroRight(CoordinatePlayer coord){
	int i;
	
	for(i=0; i<contatore_muri; i++){
	Muro.x = RecuperaCoordinatax(VettoreMuri[i]);
    Muro.y = RecuperaCoordinatay(VettoreMuri[i]);
		Muro.ov = RecuperaVerticaleOrizzontale(VettoreMuri[i]);
    Muro.player = RecuperaPlayer(VettoreMuri[i]);
		if(Muro.ov == 2){
			if(coord.x == Muro.x){
				if(coord.y == Muro.y || coord.y == (Muro.y+1)){
					return 0;
				}
			}
		}
	}
	return 1;
}

int ControlloMuro(CoordinateMuro coordM) {
	int i ;
	volatile int x = 0;
	volatile int y = 0;
	volatile int vertOriz = 0;

	if(coordM.x>5 || coordM.x<0 || coordM.y>5 || coordM.y<0){//controllo sul perimetro
			return 0; // ritorno non valido 
	}
	 // muri orizzontali 1 verticali 2 perchè se nei 4 bit si ha 0 è una mossa e non un muro
	 // i muri incrociati hanno stesse x e y quindi incluso nel controllo sotto
 	for(i = 0; i<contatore_muri; i++){
		x = RecuperaCoordinatax(VettoreMuri[i]);
		y = RecuperaCoordinatay(VettoreMuri[i]);
		vertOriz = RecuperaVerticaleOrizzontale(VettoreMuri[i]);
		if(coordM.ov == 1 && coordM.ov == vertOriz){ //orizzontali sovrapposti
			if(coordM.y == y ){
				if(coordM.x == x || (coordM.x+1) == x || (coordM.x-1) == x ){
					return 0;
				}	
			}
		}
		if(coordM.ov == 2 && coordM.ov == vertOriz){ //verticali sovrapposti
			if(coordM.x == x){
				if(coordM.y == y || (coordM.y-1) == y || (coordM.y-1) == y){
					return 0;
				}	
			}
		}
		if(coordM.ov != vertOriz){ //incrociati
			if(coordM.x == x && coordM.y == y){
			return 0;
			}
		}
	}
	return 1;
}

void ConsiglioMossa(CoordinatePlayer coord){
		int x;
		int y;
		x = RecuperaCoordinatax(VettoreMosse[contatore_mosse - 1]);
		y = RecuperaCoordinatay(VettoreMosse[contatore_mosse - 1]);
    Consiglio = coord;
		Consiglio.x = Consiglio.x + 1;
		if(Consiglio.x == x && Consiglio.y == y && ControlloMossaMuroLeft(Consiglio) == 1){
			Consiglio.x = Consiglio.x +1;
		}
		if(ControlloMossa(Consiglio) == 1 && ControlloMossaMuroLeft(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Yellow);
		}
		Consiglio = coord;
		Consiglio.x = Consiglio.x - 1;
		if(Consiglio.x == x && Consiglio.y == y && ControlloMossaMuroRight(Consiglio) == 1){
			Consiglio.x = Consiglio.x -1;
		}
		if(ControlloMossa(Consiglio) == 1 && ControlloMossaMuroRight(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Yellow);
		}
		Consiglio = coord;
		Consiglio.y = Consiglio.y + 1;
		if(Consiglio.y == y && Consiglio.x == x && ControlloMossaMuroDown(Consiglio) == 1){
			Consiglio.y = Consiglio.y +1;
		}
		if(ControlloMossa(Consiglio) == 1 && ControlloMossaMuroDown(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Yellow);
		}
		Consiglio = coord;
		Consiglio.y = Consiglio.y - 1;
		if(Consiglio.y == y && Consiglio.x == x && ControlloMossaMuroUp(Consiglio) == 1){
			Consiglio.y = Consiglio.y -1;
		}
		if(ControlloMossa(Consiglio) == 1 && ControlloMossaMuroUp(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Yellow);
		}
}
void CancellaConsiglioMossa(CoordinatePlayer coord){
		int x;
		int y;
		x = RecuperaCoordinatax(VettoreMosse[contatore_mosse - 1]);
		y = RecuperaCoordinatay(VettoreMosse[contatore_mosse - 1]);
		Consiglio = coord;
		Consiglio.x = Consiglio.x + 1;
		if(Consiglio.x == x && Consiglio.y == y){
			Consiglio.x = Consiglio.x +1;
		}
		if(ControlloMossa(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Black);
		}
		Consiglio = coord;
		Consiglio.x = Consiglio.x - 1;
		if(Consiglio.x == x && Consiglio.y == y){
			Consiglio.x = Consiglio.x -1;
		}
		if(ControlloMossa(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Black);
		}
		Consiglio = coord;
		Consiglio.y = Consiglio.y + 1;
		if(Consiglio.y == y && Consiglio.x == x){
			Consiglio.y = Consiglio.y +1;
		}
		if(ControlloMossa(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Black);
		}
		Consiglio = coord;
		Consiglio.y = Consiglio.y - 1;
		if(Consiglio.y == y && Consiglio.x == x){
			Consiglio.y = Consiglio.y -1;
		}
		if(ControlloMossa(Consiglio) == 1){
			DrawDelete(Consiglio.x , Consiglio.y, Black);
		}
}
void ModalitaMuro(){

	if(Muri_PL0 == 0){
		MessaggioMuriFiniti(0);
		enable_timer(1);
		conteggio_secondi_1 = 5;
		
	}
	if(Muri_PL1 == 0){
		MessaggioMuriFiniti(1);
		enable_timer(1);
		conteggio_secondi_1 = 5; // lascio 5 secondi per leggere il messaggio che comunica che sono finiti i muri
		
	}
	//Nella prima fase della modalità muro sistemo i problemi dovuti al vettore mosse e all'aggiunta del muro
	playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);
	PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
	CancellaConsiglioMossa(PosizioneDaConsigliare); //cancello i consigli della mossa facendo capire che siamo nella modalità di posiziona muro
	MuroDaConfermare.x = 3;
	MuroDaConfermare.y = 3;
	MuroDaConfermare.ov = 1;
 	MuroDaConfermare.player = playerDaConsigliare;
	if(ControlloMuro(MuroDaConfermare) == 1){
		DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
	}else{
		MuroDaConfermare = TrovaMuroLibero(MuroDaConfermare);
		DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
	}
}
CoordinateMuro TrovaMuroLibero(CoordinateMuro Muro){
	int i ;
	int j ;
 	Muro.y = 0;
	Muro.x = 0;
	for(i=0; i<6; i++){
		for(j=0; j<6; j+=2){
			Muro.y = i;
			Muro.x = j;
			if(ControlloMuro(Muro) == 1){
				return Muro;
			}
		}
	}
	return Muro;
}
void RitornoModalitaGiocatore(){
	if(Muri_PL0 == 0 || Muri_PL1 == 0 || check_Trap() == 0){ 
		ClearLowerPart();
		LowerPart();
		WriteWallsNumber(0,Muri_PL0);
		WriteWallsNumber(1,Muri_PL1);
		WriteTimeLeft(20);
		enable_timer(0);
		conteggio_secondi_0 = 20;
	}
	playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);
	PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
	DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Black);
	ConsiglioMossa(PosizioneDaConsigliare);
}

void giraMuro(){
	DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Black);
	MuroDaConfermare.ov = 2;
	if(ControlloMuro(MuroDaConfermare) == 1){
		DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
	} else{
		MuroDaConfermare = TrovaMuroLibero(MuroDaConfermare);
		DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
	}
}
void rigiraMuro() {
	DrawVWall(MuroDaConfermare.x, MuroDaConfermare.y, Black);
	if(ControlloMuro(MuroDaConfermare) == 1){
		DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
	} else{
		MuroDaConfermare = TrovaMuroLibero(MuroDaConfermare);
		DrawOWall(MuroDaConfermare.x, MuroDaConfermare.y, Red);
	}
}
void Timer0_expired(){
	WriteTimeLeft(0);
	playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]); 
	//due mosse indietro contando che noi siamo in una vuota quella appena prima è dell'avversario quella ancora prima è l'ultima dell'interessato
	NuovaPosizione = RecuperaPosizione(playerDaConsigliare);
	SalvaMossa(NuovaPosizione.player, 0, 0, NuovaPosizione.x, NuovaPosizione.y);
	CancellaConsiglioMossa(NuovaPosizione);
	playerDaConsigliare = RecuperaPlayer(VettoreMosse[contatore_mosse - 2]);
	PosizioneDaConsigliare = RecuperaPosizione(playerDaConsigliare);
	ConsiglioMossa(PosizioneDaConsigliare);
	press_KEY1 = 0;
	press_KEY2 = 0;
	enable_timer(0);
	conteggio_secondi_0 = 20;
}

int check_Trap(){
	int i, j, k;
	if(trap1 == 0){
		if(flag == 1){ 											//se uguale a 0 non c'è stato sullo scorrimento una modifica della matrice
			scorrimento_matrice(0); 					// ricorsione sullo scorrimento per il giocatore 1(0)
		} else { 
																				//a questo punto controllo se la matrice ha senso per il giocaore 1 
																				//se ha senso faccio il controllo per il giocatore due altrimenti ritorno già 0
				for(i=0; i<7; i++){
																				//controllo per giocatore1
					if(possibili[i][6] == 1){
						trap1=1;
						flag = 1;
						for (j = 0; j < 7; j++) {
							for (k = 0; k < 7; k++) {
								possibili[i][j] = 0;		//azzero la matrice
							}
						}

					}
				}
				if(trap1 == 0){
					return 0; 										//giocatore 1(0) bloccato
				}
		}
	} else if(trap2 == 0){
		if(flag == 1){
			scorrimento_matrice(1); 					// ricorsione sullo scorrimento per il giocatore 2(1)
		} else {
			for(i=0; i<7; i++){
																				//controllo per giocatore2
					if(possibili[i][0] == 1){
						trap2=1;
						flag = 1;
						for (j = 0; j < 7; j++) {
							for (k = 0; k < 7; k++) {
								possibili[i][j] = 0;		//azzero la matrice
							}
						}
					}
				}
				if(trap2 == 0){
					return 0; 										//giocatore 2(1) bloccato
				}
		}
	}
	return 1; 														//mossa valida
}

void scorrimento_matrice(int player){
	int i;
	int j;
	flag = 0; 														//azzero lo storico dei cambiamenti
  PL = RecuperaPosizione(player);
	possibili[PL.x][PL.y] = 1;
	
	for(i = 0; i<7; i++){
		for(j = 0; j<7; j++){
																				//fai tutti gli if per vedere se modifica qualche numero nella matrice
			if(possibili[i][j] == 1){					//Se un numero nella matrice ==1
				PosizioneTrap.x = i;
				PosizioneTrap.y = j-1;
				if(ControlloMossa(PosizioneTrap) == 1 && ControlloMossaMuroUp(PosizioneTrap) == 1){ //Controllo mossa Up
					flag = 1;
					possibili[PosizioneTrap.x][PosizioneTrap.y] = 1;
				}
				PosizioneTrap.y = j+1;  				
				if(ControlloMossa(PosizioneTrap) == 1 && ControlloMossaMuroDown(PosizioneTrap) == 1){ //Controllo mossa Down
					flag = 1;
					possibili[PosizioneTrap.x][PosizioneTrap.y] = 1;
				}
				PosizioneTrap.x = i-1;	
				if(ControlloMossa(PosizioneTrap) == 1 && ControlloMossaMuroLeft(PosizioneTrap) == 1){ //Controllo mossa Left
					flag = 1;
					possibili[PosizioneTrap.x][PosizioneTrap.y] = 1;
				}
				PosizioneTrap.x = i+1;
				if(ControlloMossa(PosizioneTrap) == 1 && ControlloMossaMuroRight(PosizioneTrap) == 1){ //Controllo mossa Right
					flag = 1;
					possibili[PosizioneTrap.x][PosizioneTrap.y] = 1;
				}
			}
		}
	}
		check_Trap();									//richiamo la funzione che deciderà se finirla o meno a seconda del valore di flag
}
