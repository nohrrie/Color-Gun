//---------------------------------------------------------
// file:	main.c
//
// brief:	Main entry point for the sample project
//			of the CProcessing library
//
// documentation link:
// https://github.com/DigiPen-Faculty/CProcessing/wiki
//
// Copyright © 2020 DigiPen, All rights reserved.
//---------------------------------------------------------

#include "cprocessing.h"
#include "logo_screen.h"
#include "gamestate_screen.h"
#include "gamestate_end.h"


// main() the starting point for the program
// CP_Engine_SetNextGameState() tells CProcessing which functions to use for init, update and exit
// CP_Engine_Run() is the core function that starts the simulation
int main(void)
{
	CP_System_SetWindowSize(1920, 1080);
	CP_System_SetFrameRate(144);
	// CP_Engine_SetNextGameState(gamestate_screen_init, gamestate_screen_update, gamestate_screen_exit);
	CP_Engine_SetNextGameState(logo_init, logo_update, logo_exit);
	CP_Engine_Run();
	return 0;
}
