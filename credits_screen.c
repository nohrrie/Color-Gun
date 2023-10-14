#include "cprocessing.h"
#include "DMLib.h"
#include "gamestate_screen.h"
#include "credits_screen.h"

CustomButton main_btn;
CustomText main_text;

void credits_init(void) {
	CP_Graphics_ClearBackground(CP_Color_Create(240, 128, 128, 255));

	CP_Color text_color = CP_Color_Create(68, 68, 68, 255);
	CP_Settings_Fill(text_color);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(100);
	CP_Font_DrawText("CREDITS", 250, 100);

	CP_Settings_TextSize(40);
	CP_Font_DrawText("All content (C) 2022 DigiPen (USA) Corporation, all rights reserved.", CP_System_GetWindowWidth() / 2.0f, CP_System_GetWindowHeight() - 50.0f);
	CP_Font_DrawText("Made with FMOD Studio by Firelight Technologies Pty Ltd.", CP_System_GetWindowWidth() / 2.0f, CP_System_GetWindowHeight() - 150.0f);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_DrawText("lead programmer: wei-no wu", 140, 200);
	CP_Font_DrawText("sound designer / programmer: harrison butler", 140, 300);
	CP_Font_DrawText("artist / programmer: naylah nhem", 140, 400);
	CP_Font_DrawText("class instructors: Andy Ellinger", 140, 500);
	CP_Font_DrawText("President: Claude Comair", 140, 600);
	CP_Font_DrawText("All DigiPen students, faculty, and staff who supported this project", 140, 700);


	





	main_menu();
	
}

void credits_update(void) {
	display_button(main_btn);
	
}

void credits_exit(void) {
	
}

void main_menu(void) {
	CP_Settings_StrokeWeight(10);
	main_btn = create_button(150.0f, 50.0f, CP_System_GetWindowWidth() - 200.0f, 600, back_to_start);

	CP_Color mainColor = CP_Color_Create(112, 253, 255, 200);
	CP_Color hover = CP_Color_Create(0, 224, 217, 200);
	CP_Color clicked = CP_Color_Create(120, 170, 227, 200);
	main_btn.c = mainColor;
	main_btn.st = mainColor;
	main_btn.h = hover;
	main_btn.cl = clicked;

	main_text.x_coord = 150.0f;
	main_text.y_coord = 150.0f;
	main_text.text = "back";

	main_btn.basic_text_size = 45.0f;
	main_btn.basic_text_color = CP_Color_Create(0, 0, 0, 255);
	main_btn.basic_text = main_text.text;
}

void back_to_start(void) {
	CP_Engine_SetNextGameState(gamestate_screen_init, gamestate_screen_update, gamestate_screen_exit);
}