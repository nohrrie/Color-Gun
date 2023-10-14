#include "cprocessing.h"
#include "gamestate_screen.h"


float timer;
int frame_count;

CP_Image logo;
CP_Image FmodLogo;

void logo_init(void) {
	timer = 0;

	CP_Graphics_ClearBackground(CP_Color_Create(240, 128, 128, 255));

	logo = CP_Image_Load("./Assets/DigiPen_BLACK.png");
	FmodLogo = CP_Image_Load("./Assets/FmodLogo.png");
	CP_Color text_color = CP_Color_Create(68, 68, 68, 255);
	CP_Settings_Fill(text_color);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	CP_Settings_TextSize(40);
	CP_Font_DrawText("All content (C) 2022 DigiPen (USA) Corporation, all rights reserved.", CP_System_GetWindowWidth() / 2.0f, CP_System_GetWindowHeight() - 50.0f);
}

void logo_update(void) {
	frame_count = CP_System_GetFrameCount();

	timer += CP_System_GetDt();

	if (timer > 6.0f)
	{
		CP_Engine_SetNextGameState(gamestate_screen_init, gamestate_screen_update, gamestate_screen_exit);
	}

	if (timer > 0 && timer <= 3.0f) {
		CP_Image_Draw(logo, (float)CP_System_GetWindowWidth() / 2.0f, (float)CP_System_GetWindowHeight() / 2.0f,
			1026, 249, frame_count / 5);
		
	}
		
	else if (timer > 3.0f) 
		CP_Image_Draw(FmodLogo, (float)CP_System_GetWindowWidth() / 2.0f, (float)CP_System_GetWindowHeight() / 2.0f,
			1024, 574, frame_count / 5);
	
}

void logo_exit(void) {
	
}