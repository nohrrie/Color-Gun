#include "cprocessing.h"
#include "DMLib.h"
#include "MathLib.h"
#include "LinkedList.h"

#include <stdarg.h> /* va_list, va_start */
#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

// *1 Remember to implement customizable alpha (Hovered)
// *2 Implement cutomizable alpha (Hold down left click on button)
// *3 Remember to implement customizable text on button.

void swap_nodes(ObjNode* a, ObjNode* b);

#pragma region Variables

Vector2 camera_offset, target_offset;
int check_overlapping;

//GameObject map_obj[MAX_OBJECT_ON_MAP];
ObjNode map_obj;
int initiated = FALSE;
#pragma endregion Variables

#pragma region Button
CustomButton create_button(float w, float h, float x, float y, void (*func)(void)) {

	CustomButton _button;

	_button.width = w;
	_button.height = h;
	_button.x_coord = x;
	_button.y_coord = y;
	_button.func = func;
	_button.has_text = FALSE;


	//Creating a default color buttons (Default: White)
	_button.c = CP_Color_Create(255, 255, 255, 255);
	_button.h = CP_Color_Create(200, 200, 200, 255);
	_button.cl = CP_Color_Create(50, 50, 50, 255);

	return _button;
}


void button_color(struct button * btn_ptr, CP_Color c, CP_Color h, CP_Color cl, CP_Color st) {
	(*btn_ptr).c = c;
	(*btn_ptr).h = h;
	(*btn_ptr).cl = cl;
	(*btn_ptr).st = st;
}


void button_text(struct button* btn_ptr, struct text* t) {

	(*btn_ptr).t = t;

	(*btn_ptr).has_text = TRUE;
	(*btn_ptr).has_basic_text = FALSE;
}


void button_text_v2(struct button* btn_ptr, char* s, float size, CP_Color c) {

	(*btn_ptr).basic_text = s;
	(*btn_ptr).basic_text_size = size;
	(*btn_ptr).basic_text_color = c;

	(*btn_ptr).has_basic_text = TRUE;
	(*btn_ptr).has_text = FALSE;
}


void display_button(CustomButton btn) {

#pragma region initialization
	float minX = btn.x_coord - (btn.width / 2);
	float maxX = btn.x_coord + (btn.width / 2);
	float minY = btn.y_coord - (btn.height / 2);
	float maxY = btn.y_coord + (btn.height / 2);

	float mouseX = CP_Input_GetMouseX();
	float mouseY = CP_Input_GetMouseY();
#pragma endregion initialization

	// Default button color.
	CP_Settings_Fill(CP_Color_Create(btn.c.r, btn.c.g, btn.c.b, 255));
	CP_Settings_Stroke(btn.st);

	// Button mouse interaction check.
	if (mouseX > minX && mouseX < maxX && mouseY > minY && mouseY < maxY) {
		//When mouse is hovered;
		CP_Settings_Fill(CP_Color_Create(btn.h.r, btn.h.g, btn.h.b, btn.h.a)); // *1
		if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
			CP_Settings_Fill(CP_Color_Create(btn.cl.r, btn.cl.g, btn.cl.b, btn.cl.a)); // *2
		}
		if (CP_Input_MouseReleased(MOUSE_BUTTON_LEFT)) {
			btn.func();
		}
	}

	// Draws the rectangle of the button.
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Graphics_DrawRect(btn.x_coord, btn.y_coord, btn.width, btn.height);
	

	// Draws texts on the button
	//
	// If utilizes CustomText, has_text == true && has_basic_text == false;
	// If uses a basic text, has_text == false && has_basic_text == true;
	if (btn.has_text) {
		display_text(*btn.t);
	}
	else {
		CP_Settings_TextSize(btn.basic_text_size);
		CP_Settings_Fill(btn.basic_text_color);

		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText(btn.basic_text, btn.x_coord, btn.y_coord);
	}

}
#pragma endregion Button

#pragma region Text
CustomText* stored_texts[500]; // Allows a maximum amount of 500 texts to be created.
int texts_counter = 0; // Private counter used by free_texts();


CustomText create_text(float x, float y, float size, const char* format, ...) {
	CustomText _text;

	size_t format_size = sizeof(format) + 20;
	char* buffer = malloc(format_size);
	va_list arglist;
	va_start(arglist, format);
	if (buffer != NULL) { //Check if buffer is NULL, to avoid "Warning C6387"
		vsprintf_s(buffer, format_size, format, arglist);
	}
	va_end(arglist);

	//Initializes values of text
	_text.x_coord = x;
	_text.y_coord = y;
	_text.text = buffer;
	_text.text_size = size;

	// Default text color (Default: Black)
	_text.text_color = CP_Color_Create(0, 0, 0, 255);
	_text.h = CP_TEXT_ALIGN_H_CENTER;
	_text.v = CP_TEXT_ALIGN_V_MIDDLE;
	_text.stored_text_index = texts_counter++;
	stored_texts[_text.stored_text_index] = &_text;
	return _text;
}


void text_update(struct text* txt, const char* format, ...) {

	int index = (*txt).stored_text_index;
	size_t format_size = sizeof(format) + 20;
	char* buffer = malloc(format_size);

	va_list arglist;

	va_start(arglist, format);
	if (buffer != NULL) { //Check if buffer is NULL, to avoid "Warning C6387"
		vsprintf_s(buffer, format_size, format, arglist);
	}
	va_end(arglist);

	(*txt).text = buffer;
	stored_texts[index] = txt;
}


void text_alignment(struct text* txt, CP_TEXT_ALIGN_HORIZONTAL h, CP_TEXT_ALIGN_VERTICAL v) {
	txt->h = h;
	txt->v = v;
}


void text_color(struct text* txt, CP_Color c) {
	txt->text_color = c;
}


void text_offset(struct text* txt, float x_offset, float y_offset) {
	txt->x_offset = x_offset;
	txt->y_offset = y_offset;
}


void free_texts(void) {
	/*int i;

	for (i = 0; i < texts_counter; i++) {
		free(stored_texts[i]->text);
	}*/

}

void free_objects(void)
{
	ObjNode* head = &map_obj;
	head = NULL;
}


void display_text(CustomText text) {


	CP_Settings_TextSize(text.text_size); // *
	CP_Settings_Fill(text.text_color); // * 

	//char buffer[50] = { 0 }; // * 
	//sprintf_s(buffer, _countof(buffer), text.text);


	CP_Settings_TextAlignment(text.h, text.v);
	CP_Font_DrawText(text.text, text.x_coord, text.y_coord); // *3
}
#pragma endregion Text

#pragma region GameObject

int get_init_state() {
	return initiated;
}

void set_init() {
	initiated = TRUE;
}

GameObject* create_object(CP_Image sprite, Vector2 pos, int has_col) {
	GameObject obj;
	obj.original_sprite = sprite;
	obj.current_sprite = obj.original_sprite;
	obj.position = pos;
	obj.has_collider = has_col;
	obj.dir = 1;
	obj.scale = CP_Vector_Set(1, 1);
	obj.anim_timer = 0;
	obj.a_index = 0;
	obj.is_target = FALSE;
	obj.has_anim = FALSE;
	obj.show = TRUE;
	obj.rotation_z = 0;
	obj.camera_child = FALSE;

	//Animation
	int i;
	for (i = 0; i < ANIMATION_STATES; i++) {
		obj.anim_frames[i] = CP_Vector_Set(0, 0);
		obj.anim_duration[i] = DEFAULT_ANIM_TIME;
	}
	ObjNode* node = add_node(&map_obj, obj);

	return &node->data.object;
}

Vector2 get_pos(struct object* go) {
	return CP_Vector_Subtract(go->position, target_offset);
}

void disable_object(struct object* go) {
	go->show = FALSE;
}

void enable_object(struct object* go) {
	go->show = TRUE;
}

void change_sprite(struct object* go, CP_Image new_sprite)
{
	go->current_sprite = new_sprite;
}

void set_object_position(struct object* go, Vector2 pos) {
	go->position = pos;
}


void move_object(struct object* go, float x, float y, float speed) {

	float last_x = go->position.x;
	go->position.x += x * speed * CP_System_GetDt();
	go->position.y += y * speed * CP_System_GetDt();
	go->velocity = CP_Vector_Set(x, y);
	if (last_x > go->position.x) {
		go->dir = 1;
	}
	else if (last_x < go->position.x) {
		go->dir = -1;
	}
}


void object_set_rotation(struct object* go, int dir) {

	if (dir != 0) {
		go->dir = dir;
		object_scale(go, CP_Vector_Set((float)dir, 1));
	}
}


void object_scale(struct object* go, Vector2 scale) {
	go->scale.x = scale.x;
	go->scale.y = scale.y;
}

void sprites_from_folder(CP_Image img[], char* filepath, int size) {
	int i;
	for (i = 0; i < size; i++) {
		char buffer[50];
		sprintf_s(buffer, sizeof(buffer), filepath, i + 1);
		img[i] = CP_Image_Load(buffer);
	}
}

void object_set_anim(struct object* go, CP_Image sprites[], int size) {
	/*s_size = (int)sizeof(sprites) / sizeof(sprites[0]);
	a_size = sizeof(go->animation_sprites) / sizeof(CP_Image);*/
	int i;
	for (i = 0; i < size; i++) {
		go->animation_sprites[i] = sprites[i];
	}
	go->has_anim = TRUE;
}


void object_anim_setframes(struct object* go, AnimationStates anim_s, Vector2 s_e, float d) {
	go->anim_frames[anim_s] = s_e;
	go->anim_duration[anim_s] = d;
}


void object_anim_setframes_all(struct object* go, Vector2 start_end[], float duration, int size) {
	int i;
	for (i = 0; i < ANIMATION_STATES; i++) {
		go->anim_frames[i] = start_end[i];
		go->anim_duration[i] = duration;
	}
}


void object_anim(struct object* go, AnimationStates state) {
	if (go->anim != state) {
		go->anim = state;
		go->anim_timer = 0;
	}

	Vector2 s_e = go->anim_frames[go->anim];

	if (go->anim_timer > 0) {
		go->anim_timer -= CP_System_GetDt();
	}
#pragma region Animation
	if (go->has_anim == TRUE) {

		// Animation
		// Changes current_sprite constantly based on go->anim.

		if (go->anim != NONE) { //If its not NONE state

			if (go->anim_timer <= 0) {

				if (go->a_index > (int)s_e.y) {
					go->a_index = (int)s_e.x;
				}
				go->current_sprite = go->animation_sprites[go->a_index];

				go->a_index++;
				go->anim_timer = go->anim_duration[go->anim];
			}
		}
		else {
			go->current_sprite = go->original_sprite;
		}
	}
#pragma endregion Animation

}

void update_layer(struct object* go, int layer) {
	go->layer = layer;
}

float fov = 0.2f;

void change_fov(float fieldov) {
	fov = fieldov / (float)100;
}

void display_object(struct object* go) {

	float w = (float)CP_Image_GetWidth(go->current_sprite) * go->scale.x;
	float h = (float)CP_Image_GetHeight(go->current_sprite) * go->scale.y;
	float x = go->position.x;
	float y = go->position.y;

	if (go->camera_child == FALSE) {
		x -= camera_offset.x;
		y -= camera_offset.y;
		//print("Pos: (%f,%f)\n", camera_offset.x, camera_offset.y);
	}
	else {
		x = target_offset.x + go->camera_offset.x;
		y = target_offset.y + go->camera_offset.y;
		//print("Gun Pos: (%f,%f)\n", x, y);
	}


	if (go->rotation_z == 0) CP_Image_Draw(go->current_sprite, x, y, w * fov, h * fov, 255);
	else CP_Image_DrawAdvanced(go->current_sprite, x, y, w * fov, h * fov, 255, go->rotation_z);

}

void update_rotation_z(struct object* go, float angle) {
	go->rotation_z = angle;
}

void set_constant_velocity(struct object* go, Vector2 velocity) {
	go->constant_velocity = velocity;
}

#pragma endregion GameObject


#pragma region GamePlay

int get_horizontal_axis() {
	CP_BOOL left = CP_Input_KeyDown(KEY_A);
	CP_BOOL right = CP_Input_KeyDown(KEY_D);
	int l = left == TRUE ? -1 : 0;
	int r = right == TRUE ? 1 : 0;

	return l + r;
}

int get_vertical_axis() {
	CP_BOOL up = CP_Input_KeyDown(KEY_W);
	CP_BOOL down = CP_Input_KeyDown(KEY_S);
	int u = up == TRUE ? -1 : 0;
	int d = down == TRUE ? 1 : 0;

	return u + d;
}

GameObject* cam_target;

void camera_follow(struct object* target, struct display* screen) {
	if (target->is_target == FALSE) {
		target->is_target = TRUE;
	}
	cam_target = target;
	target->camera_offset = CP_Vector_Set(0, 0);
	target->camera_child = TRUE;
	camera_offset = target->position;
	target_offset = CP_Vector_Set(screen->mid_x, screen->mid_y);
}

void add_to_camera(struct object* target, struct object* master, Vector2 offset) {
	if (target->camera_child == FALSE) target->camera_child = TRUE;
	//print("%f, %f\n", target->position);
	target->camera_offset = offset;
}

void DMLib_init() {





}

#pragma endregion GamePlay

#pragma region Grid

Map create_map(float tile_size) {
	Map new_map;
	new_map.grid[0][0] = 0;
	new_map.tile_size = tile_size;
	new_map.origin = CP_Vector_Set(0, 0);
	return new_map;
}

Vector2 grid_to_world_coordinate(struct gridmap* grid, int x, int y) {

	float xC, yC;
	xC = (*grid).origin.x + (x * (*grid).tile_size);
	yC = (*grid).origin.y + (y * (*grid).tile_size);
	Vector2 point = CP_Vector_Set(xC, yC);
	return point;

}

#pragma endregion Grid


#pragma region Display

void display_buttons(int num_of_btn, ...) {
	va_list vList; // Declare a va_list called vList.
	va_start(vList, num_of_btn); // Initialize the va_list.
	int i; // For loop counter.

	for (i = 0; i < num_of_btn; i++) {
		display_button(va_arg(vList, CustomButton)); // For num_of_btn times, va_arg will return CustomButton
													 // into display_button(CustomButton btn);
	}

	va_end(vList); // Clear the memory reserved for va_list.
}

void display_texts(int num_of_txt, ...) {
	va_list vList; // Declare a va_list called vList.
	va_start(vList, num_of_txt); // Initialize the va_list.
	int i; // For loop counter.

	for (i = 0; i < num_of_txt; i++) {
		CustomText ct = va_arg(vList, CustomText);
		display_text(ct);			// For num_of_btn times, va_arg will return CustomButton
									// into display_button(CustomButton btn);
	}

	va_end(vList); // Clear the memory reserved for va_list.
}

void display_objects() {

	/*if (map_obj.data.object.layer < map_obj.next->data.object.layer) {
		swap_nodes(&map_obj, map_obj.next);
	}*/

	ObjNode* node = &map_obj;
	while (node != NULL) {
		if (node->data.object.show == TRUE) {
			display_object(&node->data.object);

		}
		node = node->next;

	}



}

void initialize_display(struct display* d) {
	d->width = (float)CP_System_GetWindowWidth();
	d->height = (float)CP_System_GetWindowHeight();

	d->mid_x = (*d).width / 2;
	d->mid_y = (*d).height / 2;
}

#pragma endregion Display


void print(char* s, ...) {

	char buffer[100];
	va_list argList;
	va_start(argList, s);
	vsprintf_s(buffer, 100, s, argList);
	va_end(argList);
	OutputDebugStringA(buffer);
}

#pragma region Extensions

Vector2Int GetVector2Int(int x, int y) {
	Vector2Int v2int;
	v2int.x = x;
	v2int.y = y;
	return v2int;
}

Vector2 GetCameraOffset() {
	return camera_offset;
}

Vector2 GetTargetOffset() {
	return target_offset;
}

#pragma endregion Extensions


void swap_nodes(ObjNode* a, ObjNode* b) {

}