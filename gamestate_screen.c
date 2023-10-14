#include "cprocessing.h"
#include "DMLib.h"
#include "gamestate_menu.h"
#include "gamestate_screen.h"
#include "credits_screen.h"

CustomButton start_btn;
CustomButton credits_btn;

CustomText start_text;
CustomText credits_text;

void gamestate_screen_init(void) {
	CP_Graphics_ClearBackground(CP_Color_Create(128, 128, 128, 255));

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_TextSize(100);
	CP_Font_DrawText("COLOR GUN", CP_System_GetWindowWidth() / 2.0f, 150);

	start_button();
	credits_button();
}

void gamestate_screen_update(void) {
	display_button(start_btn);
	display_button(credits_btn);
	
}

void gamestate_screen_exit(void) {
	
}

void start_button(void) {
	CP_Settings_StrokeWeight(20);
	start_btn = create_button(200.0f, 75.0f, CP_System_GetWindowWidth() / 2.0f, 300, start_game);

	CP_Color mainColor = CP_Color_Create(112, 253, 255, 200);
	CP_Color hover = CP_Color_Create(0, 224, 217, 200);
	CP_Color clicked = CP_Color_Create(120, 170, 227, 200);
	start_btn.c = mainColor;
	start_btn.st = mainColor;
	start_btn.h = hover;
	start_btn.cl = clicked;

	start_text.x_coord = 150.0f;
	start_text.y_coord = 150.0f;
	start_text.text = "start";

	start_btn.basic_text_size = 45.0f;
	start_btn.basic_text_color = CP_Color_Create(0, 0, 0, 255);
	start_btn.basic_text = start_text.text;
}

void credits_button(void) {
	CP_Settings_StrokeWeight(20);
	credits_btn = create_button(200.0f, 75.0f, CP_System_GetWindowWidth() / 2.0f, 500, credits_menu);

	CP_Color mainColor = CP_Color_Create(112, 253, 255, 200);
	CP_Color hover = CP_Color_Create(0, 224, 217, 200);
	CP_Color clicked = CP_Color_Create(120, 170, 227, 200);
	credits_btn.c = mainColor;
	credits_btn.st = mainColor;
	credits_btn.h = hover;
	credits_btn.cl = clicked;

	credits_text.x_coord = 150.0f;
	credits_text.y_coord = 150.0f;
	credits_text.text = "credits";

	credits_btn.basic_text_size = 45.0f;
	credits_btn.basic_text_color = CP_Color_Create(0, 0, 0, 255);
	credits_btn.basic_text = credits_text.text;
}

void start_game(void) {
	CP_Engine_SetNextGameState(gamestate_menu_init, gamestate_menu_update, gamestate_menu_exit);
}

void credits_menu(void) {
	CP_Engine_SetNextGameState(credits_init, credits_update, credits_exit);
}