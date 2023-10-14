#pragma once
#include <cprocessing.h>

#define TRUE 1
#define FALSE 0
#define ANIMATION_STATES 4
#define DEFAULT_ANIM_TIME 0.1f
#define MAP_SIZE_X 50
#define MAP_SIZE_Y 50
#define MAX_OBJECT_ON_MAP 200

//MACRO
#define A_SIZE(a) (sizeof(a)/sizeof(a[0]))

//---------------------------------------------------------

// Type definitions
typedef CP_Vector Vector2;

typedef struct v2int {
	int x, y;
} Vector2Int;
// Structs

// Buttons
/*		Implementation:
				global:
					CustomButton ButtonName;
				init:
					ButtonName = create_button(...);
				update:
					display_button(ButtonName);
*/
typedef struct button
{
	float width, height, x_coord, y_coord;
	CP_Color c, h, cl, st; //c = idle color, h = hover color, c = clicked color, st = stroke color

	/* Texts */
	int has_text, has_basic_text; // Determines which type of text format this button uses.
	struct text* t;
	char* basic_text;
	float basic_text_size;
	CP_Color basic_text_color;
	void (*func)(void);

} CustomButton;

// Texts
/*		Implementation:
				global:
					CustomText TextName;
				init:
					TextName = create_text(...);
				update:
					display_text(TextName);

		* Usable by button.
*/
typedef struct text
{
	float x_coord, y_coord, x_offset, y_offset;
	float text_size;
	CP_TEXT_ALIGN_HORIZONTAL h;
	CP_TEXT_ALIGN_VERTICAL v;
	CP_Color text_color;
	char* text;
	int stored_text_index;

} CustomText;

// Display Object
/*
		Implementation:
				global: Figure it out


*/
typedef struct display
{
	float width, height, mid_x, mid_y;

} Display;

// GameObjects
/*
		Implementation:
				global: Figure it out


*/
typedef enum {
	NONE = -1, // NONE state means no animation. Will take default sprite.
	IDLE = 0,
	MOVING = 1,
	ATTACKING = 2,
	DYING = 3
} AnimationStates;

typedef enum {
	RIGHT = 1,
	LEFT = -1
} Direction;

typedef struct object
{
	CP_Image original_sprite;
	CP_Image current_sprite;
	CP_Image animation_sprites[50];
	AnimationStates anim;
	Vector2 anim_frames[ANIMATION_STATES];
	float anim_duration[ANIMATION_STATES];
	float anim_timer;
	int a_index, layer; // Animation index
	float rotation_z;

	Direction dir;
	Vector2 velocity;

	Vector2 position, scale, col_pos, col_offset, camera_offset;
	Vector2 constant_velocity;
	int has_collider, is_target, has_anim, camera_child; // FALSE by defualt.
	int show;

} GameObject;


typedef struct gridmap {
	int grid[MAP_SIZE_X][MAP_SIZE_Y];
	Vector2 origin;
	float tile_size;

} Map;


// Functions

#pragma region Button

// ---------------- A function to create a button ----
//					Only takes in width height x pos, y pos and a function pointer.
//					* use in init *
//					To Customize color/text, use the other functions below.
CustomButton create_button(float w, float h, float x, float y, void (*func)(void));

// ---------------- This function changes the color of the button, including hover/clicked colors ----
//					First parameter takes the address of a struct instance.
//					button_color(&btn, CP_Color, CP_Color, CP_Color)

void button_color(struct button * btn, CP_Color default_color, CP_Color hover_color, CP_Color clicked_color, CP_Color stroke_color);

// ---------------- This function changes the text values on the button ----
//					Call it in init.
void button_text(struct button* btn, struct text* text);


// ---------------- Another way to add text to the button. ----
//					Only a button reference, a text, size and color is needed.
//					The purpose is to prevent million texts objects, can be handled with array.
//					Alignments are defaulted.
void button_text_v2(struct button* btn, char* text, float size, CP_Color c);

// ---------------- This function displays the button struct ----
//					Needs to be put in update function to keep displaying the button.
void display_button(CustomButton btn);

#pragma endregion Button

#pragma region Text

// ---------------- Creates a text struct taking in text, x, y, and size ----
//					Basics of making a button, in otherwords, these fields are enough to draw a text.
//					Alignment: Defaulted to CP_TEXT_ALIGN_H_CENTER and CP_TEXT_ALIGN_V_MIDDLE
CustomText create_text(float x, float y, float size, const char* format, ...);


void text_update(struct text* txt, const char* format, ...);

// ---------------- Updates the text object's alignment ----
//					Pass in a horizontal and vertical alignment.
//					Pass in the address of the instantiated text object.
void text_alignment(struct text* txt, CP_TEXT_ALIGN_HORIZONTAL h, CP_TEXT_ALIGN_VERTICAL v);


// ---------------- Changes the color of the text ----
//					Pretty self explanatory.
void text_color(struct text* txt, CP_Color c);


// ---------------- Optional function to change the offset of the button ----
//					Can't think of a use for this, but maybe for updating the position of text on update?
void text_offset(struct text* txt, float x_offset, float y_offset);

// ---------------- Free the memory used by the texts ----
//					create_text uses malloc to allocate memories from the heap to reserve a block of memory
//					for CustomTexts.
//					This function is required to be called when switching gamestates.
void free_texts(void);

// ---------------- Displays the text object onto the screen ----
//					Settings can be changed during run time.
void display_text(CustomText text);

#pragma endregion Text

#pragma region GameObject

int get_init_state();
void set_init();

// ---------------- Create a gameobject object ----
//					Gives an object a sprite image, default position, and whether to
//					instantiate it with a collider or not (Not implemented yet)
GameObject* create_object(CP_Image sprite, Vector2 pos, int has_col);

void disable_object(struct object* go);
void enable_object(struct object* go);

void change_sprite(struct object* go, CP_Image new_sprite);
void free_objects(void);

void set_object_position(struct object* go, Vector2 pos);

// ---------------- Move Object ----
//					Moves an object based on the vector x, and vector y, incorporate it with speed.
//					Note: Speed has to be a big number (like 100) because move_object uses
//						  CP_System_GetDt(), to prevent movement depending on hardware.
void move_object(struct object* go, float x_mov, float y_mov, float speed);


// ---------------- Set Rotation of an Object ----
//					Takes a direction (left/right | -1/1), and flips the object accordingly.
//					It will only change the direction for X <-- For our game project only.
void object_set_rotation(struct object* go, int dir);


// ---------------- Scale the Object ----
//					Takes in a Vector2 scale, and scales the object according to the scale.
//					Default scale is Vector2(1, 1).
void object_scale(struct object* go, Vector2 scale);


// ---------------- Get sprites from a filepath ----
//					Pass in a filepath with formatted string.
//					PNG files should be put into one file, and the files should have the same name with
//					a number from 1 to n. Places it to img[]
void sprites_from_folder(struct CP_Image_Struct* img[], char* filepath, int size);


// ---------------- Give the GameObject their animation sprites ----
//					will contain every single sprites.
void object_set_anim(struct object* go, CP_Image sprites[], int size);


// ---------------- Tells the gameobject which frame to which frame is the animation ----
//					Will set the start and end index of the animation_sprites.
//					Also should duration=speed at which the animation will go at.
void object_anim_setframes(struct object* go, AnimationStates anim_state, Vector2 start_end, float duration);


// ---------------- Change the object's current anim (Used in update) ----
//					Called when doing an action.
void object_anim(struct object* go, AnimationStates state);


void update_layer(struct object* go, int layer);

Vector2 get_pos(struct object* go);

// ---------------- Display the object onto the screen ----
//					Call in update function to draw the sprite on screen. Require object to be
//					instantiated with create_object(...) prior to calling this.
void display_object(struct object* go);

void update_rotation_z(struct object* go, float angle);

void set_constant_velocity(struct object* go, Vector2 velocity);

#pragma endregion GameObject

#pragma region GamePlay

// ---------------- Horizontal Axis ----
//					Returns an int, checks for 'd' and 'a' key as right and left with respect.
int get_horizontal_axis();


// ---------------- Vertical Axis ----
//					Returns an int, checks for 'w' and 's' key as up and down with respect.
int get_vertical_axis();


// ---------------- Camera Follow ----
//						Implement: Update
//						Function: Takes an object and follow that object.
void camera_follow(struct object* target, struct display* screen);

void add_to_camera(struct object* target, struct object* master, Vector2 offset);

void change_fov(float fieldov);

void DMLib_init();

#pragma endregion GamePlay

#pragma region Grid

Map create_map(float tile_size);

Vector2 grid_to_world_coordinate(struct gridmap* grid, int x_coord, int y_coord);


#pragma endregion Grid


#pragma region Display

// ---------------- Display buttons all at once ----
//					Uses <stdarg.h> to have a dynamic parameters.
void display_buttons(int amount, ...);


// ---------------- Display texts all at once ----
//					Uses <stdarg.h>
void display_texts(int amount, ...);


void display_objects();

// ---------------- Initializes the display variables ----
//					So any screen width, height, mid poitns etc, can be referenced from one place.
void initialize_display(struct display* d);

#pragma endregion Display

#pragma region Helper Functions

void print(char* s, ...);

#pragma endregion Helper Functions

#pragma region Extensions

Vector2Int GetVector2Int(int x, int y);

Vector2 GetCameraOffset();
Vector2 GetTargetOffset();

#pragma endregion Extensions