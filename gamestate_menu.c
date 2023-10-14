#include "cprocessing.h"
#include "DMLib.h"
#include "gamestate_menu.h"
#include "LinkedList.h"	
#include "MathLib.h"
#include "gamestate_end.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

#define BTN_W 300
#define BTN_H 100
#define TILE_SIZE 1
#define MAX_ENEMY 200

typedef struct enemy {
	GameObject* object;
	float health;
	float damage;
	float speed;
	float collider_radius;
	int color_type;
	float range;
} Enemy;

typedef enum {
	BLUE = 0,
	RED = 1,
	GREEN = 2,
	YELLOW = 3,
	COLOR_COUNT = 4
} ColorType;

#pragma region Variables

float startBtnX;
float startBtnY;
CP_Color btnC;
CP_Color textC;

Display display;

CustomButton btns[50];
CustomText texts[50];

// UI
int kills = 0;


// Player
GameObject* player;
int gun_index;
GameObject* current_gun;
//Vector2 animation_frames[ANIMATION_STATES];
CP_Image player_sprites[5];
float move_speed = 500;
float player_health = 100;
float curr_health;

// Map Objects
GameObject* pebbles[500];

// Guns
GameObject* guns[4];
float reload_time[4] = { 0.2f, 0.025f, 0.5f, 0.7f };
float bullet_damage[4] = { 15, 5, 20, 100 };

// Blue Gun - pistol
CP_Image blue_gun_sprites[3];
Vector2 shoot_point;
Vector2 actual_shoot_point;
float shoot_angle;
GameObject* bullets[500];
int bullets_color_map[500] = { 0 };
int b_index;
float shoot_timer, shoot_time = .2f;
char* bullets_type[4] = { "./Assets/Sprites/Others/blue_bullet.png", "./Assets/Sprites/Others/red_bullet.png",
						 "./Assets/Sprites/Others/green_bullet.png", "./Assets/Sprites/Others/yellow_bullet.png" };

// Red Guns - minigun
CP_Image red_gun_sprites[2];


// Yellow Gun - sniper rifle 
CP_Image yellow_gun_sprites[2];


// Green Gun - shotgun
CP_Image green_gun_sprites[2];


// Enemies
Enemy enemies[4][MAX_ENEMY];
int test = 0;
CP_Image red_sprites[2];
CP_Image green_sprites[2];
CP_Image blue_sprites[2];
CP_Image yellow_sprites[2];

CP_Color background_color[4];
int bgc_index = 0;

CP_Sound success;
CP_Sound hit_sound;

Map grid;

// Gameplay
int curr_wave;
const float wave_time = 10;
float wave_timer;

#pragma endregion Variables

#pragma region Prototypes

//Prototypes
void layer_with_y(struct object* go);

// Combat
void draw_gun(ColorType type);
void shoot_gun(GameObject* gun);
void shoot(GameObject* go, int randomize);
void bullets_move();
void update_gun(int index);
void bullet_detect_collision();
void shoot_multiple(GameObject* gun, int num);

// Enemies
void initiate_enemies(Enemy enemy_list[][MAX_ENEMY], int size_x, int size_y);
void move_enemies(Enemy* enemy_list, int size);
void move_all_enemies();
void spawn_randomized(ColorType type);
void re_stats_e(Enemy* e, float health);
void enemy_collision();

// Players
void player_controls();

// Waves
int enemy_spawn_count(int wave);
void wave_spawn(int wave);
void next_color();
void value_reset();

#pragma endregion Prototypes

void game_setup() {
	// Enemy Initialization
	initiate_enemies(enemies, 4, 500);

	success = CP_Sound_Load("./Assets/Cowbell.wav");
	hit_sound = CP_Sound_Load("./Assets/BassDrum.wav");

	CP_Settings_RectMode(CP_POSITION_CENTER);
	background_color[0] = CP_Color_Create(86, 171, 220, 255);
	background_color[1] = CP_Color_Create(238, 109, 145, 255);
	background_color[2] = CP_Color_Create(73, 193, 149, 255);
	background_color[3] = CP_Color_Create(255, 209, 71, 255);

	initialize_display(&display); // Initializes Display
	int i;
	float map_width = 20000;
	// Pebbles
	Vector2 random_spot;
	for (i = 0; i < A_SIZE(bullets); i++) {
		random_spot = CP_Vector_Set(CP_Random_RangeFloat(-map_width / 2, map_width / 2), CP_Random_RangeFloat(-map_width / 2, map_width / 2));
		pebbles[i] = create_object(CP_Image_Load("./Assets/Sprites/Others/pebbles.png"), random_spot, 0);
		pebbles[i]->scale = CP_Vector_Set(3, 3);
	}

	// Player
	player = create_object(CP_Image_Load("./Assets/Sprites/Player/player01.png"), CP_Vector_Set(0, 0), 0);
	sprites_from_folder(player_sprites, "./Assets/Sprites/Player/player0%i.png", A_SIZE(player_sprites));
	object_set_anim(player, player_sprites, A_SIZE(player_sprites));

	object_anim_setframes(player, IDLE, CP_Vector_Set(0, 1), 0.2f);
	object_anim_setframes(player, MOVING, CP_Vector_Set(2, 3), 0.15f);


#pragma region UI

	// Score Text (0)
	texts[0] = create_text(display.mid_x, 55, 50, "Kills: %i", kills);
	texts[1] = create_text(display.mid_x - 200, 55, 25, "Wave: %i", curr_wave);
	texts[2] = create_text(display.mid_x + 150, 55, 25, "Time until next wave: %i", wave_timer);
	texts[3] = create_text(display.mid_x - 100, display.mid_y + 200, 50, "Health: %i", (int)curr_health);
	text_color(&texts[3], CP_Color_Create(0, 50, 0, 255));
	text_alignment(&texts[3], CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
	text_alignment(&texts[2], CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);

#pragma endregion


#pragma region Gun init

	gun_index = bgc_index;
	/* Guns will have to appear according to button trigger; currently loaded for testing purposes */
	// Blue Gun
	guns[BLUE] = create_object(CP_Image_Load("./Assets/Sprites/Guns/blue_gun01.png"), CP_Vector_Set(display.mid_x + 100, display.mid_y), 0);
	sprites_from_folder(blue_gun_sprites, "./Assets/Sprites/Guns/blue_gun0%i.png", A_SIZE(blue_gun_sprites));
	object_set_anim(guns[BLUE], blue_gun_sprites, A_SIZE(blue_gun_sprites));

	object_anim_setframes(guns[BLUE], IDLE, CP_Vector_Set(0, 0), 5.0f);
	object_anim_setframes(guns[BLUE], MOVING, CP_Vector_Set(1, 2), 0.1f);

	// Red Gun
	guns[RED] = create_object(CP_Image_Load("./Assets/Sprites/Guns/red_gun01.png"), CP_Vector_Set(display.mid_x + 100, display.mid_y), 0);
	sprites_from_folder(red_gun_sprites, "./Assets/Sprites/Guns/red_gun0%i.png", A_SIZE(red_gun_sprites));
	object_set_anim(guns[RED], red_gun_sprites, A_SIZE(red_gun_sprites));

	object_anim_setframes(guns[RED], IDLE, CP_Vector_Set(0, 0), 5.0f);
	object_anim_setframes(guns[RED], MOVING, CP_Vector_Set(0, 1), 0.1f);

	// Yellow Gun
	guns[YELLOW] = create_object(CP_Image_Load("./Assets/Sprites/Guns/yellow_gun01.png"), CP_Vector_Set(display.mid_x + 100, display.mid_y), 0);
	sprites_from_folder(yellow_gun_sprites, "./Assets/Sprites/Guns/yellow_gun0%i.png", A_SIZE(yellow_gun_sprites));
	object_set_anim(guns[YELLOW], yellow_gun_sprites, A_SIZE(yellow_gun_sprites));

	object_anim_setframes(guns[YELLOW], IDLE, CP_Vector_Set(0, 0), 5.0f);
	object_anim_setframes(guns[YELLOW], MOVING, CP_Vector_Set(0, 1), 0.1f);

	// Green Gun
	guns[GREEN] = create_object(CP_Image_Load("./Assets/Sprites/Guns/green_gun01.png"), CP_Vector_Set(display.mid_x + 100, display.mid_y), 0);
	sprites_from_folder(green_gun_sprites, "./Assets/Sprites/Guns/green_gun0%i.png", A_SIZE(green_gun_sprites));
	object_set_anim(guns[GREEN], green_gun_sprites, A_SIZE(green_gun_sprites));

	object_anim_setframes(guns[GREEN], IDLE, CP_Vector_Set(0, 0), 5.0f);
	object_anim_setframes(guns[GREEN], MOVING, CP_Vector_Set(0, 1), 0.1f);

#pragma endregion
	// Bullets
	for (i = 0; i < A_SIZE(bullets); i++) {
		bullets[i] = create_object(CP_Image_Load("./Assets/Sprites/Others/blue_bullet.png"), CP_Vector_Set(-500, 0), 0);
		disable_object(bullets[i]);
		object_scale(bullets[i], CP_Vector_Set(0.2f, 0.2f));
	}




	// Grid
	grid = create_map(TILE_SIZE);

	// Test
	//testBox = create_object(CP_Image_Load("./Assets/Sprites/Enemies/red_enemy.png"), CP_Vector_Set(0, 0), 0);
	//object_scale(testBox, CP_Vector_Set(0.3f, 0.3f));
	//print("%i\n", testBox->show)
	value_reset();
}

void value_reset() {
	// Gameplay
	curr_wave = 1;
	wave_timer = wave_time;
	curr_health = player_health;
	
	bgc_index = 0;
	kills = 0;

	int i, j;
	for (i = 0; i < A_SIZE(enemies); i++) {
		for (j = 0; j < A_SIZE(enemies[i]); j++) {
		
			disable_object(enemies[i][j].object);
		}
	}
	wave_spawn(curr_wave);
}

/* INIT */
void gamestate_menu_init(void) {
	
	if (get_init_state() == FALSE) {
		game_setup();
		set_init();
	}
	else {
		value_reset();
	}

	change_fov(30);
}

/* UPDATE */
void gamestate_menu_update(void) {
	//if (CP_Input_KeyTriggered(KEY_SPACE)) {
	//	next_color();
	//	//print("%f, %f\n", enemies[0][0].object->position.x, enemies[0][0].object->position.y);
	//}
	gun_index = bgc_index;
	update_gun(bgc_index);
	CP_Graphics_ClearBackground(background_color[bgc_index]);

	// Move all enemies
	move_all_enemies();

	// Game Play
	player_controls(); // Move, Shoot and Change.
	if (curr_health <= 0) {
		// Game over
		CP_Engine_SetNextGameState(gamestate_end_init, gamestate_end_update, gamestate_end_exit);
	}

	// Wave control
	wave_timer -= CP_System_GetDt();
	if (wave_timer <= 0) {
		wave_timer = wave_time;
		next_color();
		wave_spawn(++curr_wave);
	}

	// Display
	display_objects();
	text_update(&texts[0], "Kills: %i", kills);
	text_update(&texts[1], "Wave: %i", curr_wave);
	text_update(&texts[2], "Time until next wave: %i", (int)wave_timer);
	text_update(&texts[3], "Health: %i", (int)curr_health);
	display_texts(4, texts[0], texts[1], texts[2], texts[3]);

	// Collision
	bullet_detect_collision();
	enemy_collision();

}

void gamestate_menu_exit(void) {
	free_objects();
	print("cleared");
	free_texts();
}

void exitGame(void) {
	CP_Engine_Terminate();
}


void layer_with_y(struct object* go) {
	go->layer = -(int)go->position.y;
}

#pragma region Player

void player_controls() {
	int move_x = get_horizontal_axis();
	int move_y = get_vertical_axis();
	move_object(player, (float)move_x, (float)move_y, move_speed);
	camera_follow(player, &display);

	shoot_gun(current_gun);
	if (move_x != 0 || move_y != 0) {
		object_anim(player, MOVING);
	}
	else {
		object_anim(player, IDLE);
	}
	object_set_rotation(player, move_x);

	bullets_move();
}

#pragma endregion Player

#pragma region Enemies

void initiate_enemies(Enemy enemy_list[][MAX_ENEMY], int size_x, int size_y)
{
	char* sprite_path = "";
	float damage = 50;
	float speed = 50;
	float range = 50;
	int i, j;
	for (i = 0; i < size_x; i++) {
		switch (i) {
		case BLUE:
			sprite_path = "./Assets/Sprites/Enemies/blue_enemy01.png";
			//sprite_formatted = "./Assets/Sprites/Enemies/blue_enemy0%i.png";
			speed = 100;
			damage = 30;
			break;
		case RED:
			sprite_path = "./Assets/Sprites/Enemies/red_enemy01.png";
			//sprite_formatted = "./Assets/Sprites/Enemies/red_enemy0%i.png";
			speed = 200;
			damage = 10;
			break;
		case GREEN:
			sprite_path = "./Assets/Sprites/Enemies/green_enemy01.png";
			//sprite_formatted = "./Assets/Sprites/Enemies/green_enemy0%i.png";
			speed = 75;
			damage = 50;
			break;
		case YELLOW:
			sprite_path = "./Assets/Sprites/Enemies/yellow_enemy01.png";
			//sprite_formatted = "./Assets/Sprites/Enemies/yellow_enemy0%i.png";
			speed = 50;
			damage = 100;
			break;
		}
		for (j = 0; j < size_y; j++) {
			Enemy* curr_e = &enemy_list[i][j];
			curr_e->object = create_object(CP_Image_Load(sprite_path), CP_Vector_Set(50, 0), 0);
			disable_object(curr_e->object);
			curr_e->color_type = i;
			curr_e->speed = speed;
			curr_e->collider_radius = 50;
			curr_e->range = range;
			curr_e->damage = damage;
			re_stats_e(curr_e, 100);
		}
	}
}

void re_stats_e(Enemy* e, float health) {
	e->health = health;
}

void move_enemies(Enemy* enemy_list, int size) {
	Vector2 dir;
	int i;
	for (i = 0; i < size; i++) {
		if (enemy_list[i].object != NULL && enemy_list[i].object->show == TRUE) {
			dir = GetDirectionN(get_pos(enemy_list[i].object), player->position);
			move_object(enemy_list[i].object, dir.x, dir.y, enemy_list[i].speed);
			object_set_rotation(enemy_list[i].object, enemy_list[i].object->dir);
		}
	}
}

void move_all_enemies() {
	move_enemies(enemies[BLUE], A_SIZE(enemies[BLUE]));
	move_enemies(enemies[RED], A_SIZE(enemies[RED]));
	move_enemies(enemies[GREEN], A_SIZE(enemies[GREEN]));
	move_enemies(enemies[YELLOW], A_SIZE(enemies[YELLOW]));
}

void spawn_randomized(ColorType type) {
	int i;
	Vector2 x_range = CP_Vector_Set(player->position.x + GetTargetOffset().x - 500, player->position.x + GetTargetOffset().x + 500);

	Vector2 y_range = CP_Vector_Set(player->position.y + GetTargetOffset().y - 500, player->position.y + GetTargetOffset().y + 500);
	Vector2 rand_pos = CP_Vector_Set(CP_Random_RangeFloat(x_range.x, x_range.y),
		CP_Random_RangeFloat(y_range.x, y_range.y));
	if (GetDistance(rand_pos, player->position) <= 500) {
		rand_pos = CP_Vector_Subtract(CP_Vector_Scale(GetDirectionN(rand_pos, player->position), 50), rand_pos);
	}
	for (i = 0; i < A_SIZE(enemies[type]); i++) {
		if (enemies[type][i].object->show == FALSE) {
			set_object_position(enemies[type][i].object, CP_Vector_Set(rand_pos.x + 1, rand_pos.y));
			re_stats_e(&enemies[type][i], 100);
			enable_object(enemies[type][i].object);
			return;
		}
	}
}

// hurt enemy and checks for death.
void hurt_enemy(ColorType type, int index, float damage, float kb_force) {
	enemies[type][index].health -= damage;
	// Knock back with a force
	set_object_position(enemies[type][index].object, CP_Vector_Subtract(enemies[type][index].object->position, CP_Vector_Scale(enemies[type][index].object->velocity, kb_force)));
	if (enemies[type][index].health <= 0) {
		disable_object(enemies[type][index].object);
		kills++;
		CP_Sound_Play(success);
	}
}

#pragma endregion Enemies

#pragma region Guns

void update_gun(int index) {
	draw_gun(index);
	shoot_time = reload_time[index];
	current_gun = guns[index];
	int i;
	for (i = 0; i < A_SIZE(guns); i++) {
		if (i == index) enable_object(guns[i]);
		else disable_object(guns[i]);
	}
}

void bullets_move() {
	int i;
	float disable_range = 1000.0f;
	for (i = 0; i < A_SIZE(bullets); i++) {
		if (bullets[i]->show == TRUE) {
			GameObject* b = bullets[i];
			move_object(b, -b->constant_velocity.x, -b->constant_velocity.y, 1000.0f);
			if (GetDistance(get_pos(bullets[i]), player->position) > disable_range) disable_object(b);
		}
	}
}

void draw_gun(ColorType type) {
	shoot_timer -= CP_System_GetDt();
	Vector2 mouse_pos = CP_Vector_Set(CP_Input_GetMouseWorldX(), CP_Input_GetMouseWorldY());
	Vector2 gun_pos = GetDirectionN(CP_Vector_Set(display.mid_x, display.mid_y), mouse_pos);
	float angle = angle_between(gun_pos) - 90;
	update_rotation_z(guns[type], angle);
	float gun_offset = 100;
	float shoot_point_offset = 50;
	shoot_point = CP_Vector_Set(gun_pos.x * shoot_point_offset, gun_pos.y * shoot_point_offset);
	//print("X: %f, Y: %f\n", shoot_point.x, shoot_point.y);
	shoot_angle = angle;
	gun_pos = CP_Vector_Set(gun_pos.x * gun_offset, gun_pos.y * gun_offset);


	add_to_camera(guns[type], player, gun_pos);
	if (gun_pos.x < 0) {
		object_scale(guns[type], CP_Vector_Set(1, -1));
	}
	else {
		object_scale(guns[type], CP_Vector_Set(1, 1));
	}
}

void shoot_gun(GameObject* gun) {
	if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
		object_anim(gun, MOVING);
		if (shoot_timer <= 0) {
			if (bgc_index == 2) shoot_multiple(gun, 5);
			else shoot_multiple(gun, 1);

		}
	}
	else {
		object_anim(gun, IDLE);
	}
}

void shoot_multiple(GameObject* gun, int num) {
	int i;
	for (i = 0; i < num; i++) {
		if (bgc_index == 2) shoot(gun, TRUE);
		else shoot(gun, FALSE);
	}

}

void shoot(GameObject* gun, int randomize) {
	shoot_timer = shoot_time;
	if (b_index == A_SIZE(bullets)) b_index = 0;
	Vector2 mouse_pos = CP_Vector_Set(CP_Input_GetMouseWorldX(), CP_Input_GetMouseWorldY());
	GameObject* b = bullets[b_index];
	Vector2 shoot_offset = GetDirectionN(player->position, mouse_pos);
	change_sprite(b, CP_Image_Load(bullets_type[gun_index]));
	bullets_color_map[b_index] = bgc_index;
	enable_object(b);
	Vector2 final_point = GetDirectionN(shoot_point, CP_Vector_Set(0, 0));
	if (randomize == TRUE) {
		final_point = CP_Vector_Add(final_point, CP_Vector_Set(CP_Random_RangeFloat(-0.5f, 0.5f), CP_Random_RangeFloat(-0.5f, 0.5f)));
	}

	set_constant_velocity(b, Normalized(final_point));
	actual_shoot_point = CP_Vector_Set(player->position.x + GetTargetOffset().x + gun->camera_offset.x + shoot_offset.x * 20,
		player->position.y + GetTargetOffset().y + gun->camera_offset.y + shoot_offset.y * 20);
	//actual_shoot_point = CP_Vector_Add(actual_shoot_point, CP_Vector_Scale(GetNormal(Normalized(actual_shoot_point)), -200));
	set_object_position(b, actual_shoot_point);
	b_index++;
}

void bullet_detect_collision()
{
	// Loop through all active bullets, and see if hit current active enemies
	int i, j, k;
	for (i = 0; i < A_SIZE(bullets); i++) {
		if (bullets[i]->show == TRUE) {
			for (j = 0; j < A_SIZE(enemies); j++) {
				if (bullets_color_map[i] != j)
					continue;
				for (k = 0; k < A_SIZE(enemies[j]); k++)
				{
					if (enemies[j][k].object->show == TRUE)
					{
						// Test current bullet (i) for collision with enemies (j, k)
						// Uses Enemy.collider_radius
						if (GetDistance(get_pos(bullets[i]), get_pos(enemies[j][k].object)) <= enemies[j][k].collider_radius) {
							hurt_enemy(j, k, bullet_damage[bullets_color_map[i]], 20);
							CP_Sound_Play(hit_sound);
							disable_object(bullets[i]);
						}
						//print("Bullet: %f, %f ---- Enemy: %f, %f\n", get_pos(bullets[i]).x, get_pos(bullets[i]).y, get_pos(enemies[j][k].object).x, get_pos(enemies[j][k].object).y);
					}
				}
			}
		}
	}
}

void enemy_collision() {
	int i, j;
	for (i = 0; i < A_SIZE(enemies); i++) {
		for (j = 0; j < A_SIZE(enemies[i]); j++) {
			// Loop through all enemies
			if (enemies[i][j].object->show == TRUE) {
				if (GetDistance(get_pos(enemies[i][j].object), player->position) <= enemies[i][j].range) {
					curr_health -= enemies[i][j].damage;
					disable_object(enemies[i][j].object);
				}
			}
		}
	}
}

#pragma endregion Guns

#pragma region Gameplay

int enemy_spawn_count(int wave) {
	return (int)(((float)wave * 0.5f) + 5);
}

void wave_spawn(int wave) {
	int i;
	for (i = 0; i < enemy_spawn_count(wave); i++) {
		spawn_randomized((ColorType)bgc_index);
	}
}

void next_color() {
	bgc_index++;
	if (bgc_index >= COLOR_COUNT)
		bgc_index = 0;
}

#pragma endregion Gameplay