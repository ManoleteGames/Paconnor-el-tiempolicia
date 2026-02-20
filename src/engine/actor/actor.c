#include "../engine.h"
#include "pc.h"
#include "actor.h"

Actor actor;

void ACTOR_SetGun(int type, int graphics_id, int bullet_graphics_id) {
	int i;

	// Set graphics id
	actor.gun.graphics_id = graphics_id;
	for (i = 0; i < ACTOR_MAX_BULLETS; i++) {
		actor.gun.bullet[i].graphics_id = bullet_graphics_id;
	}

	// Set gun type
	actor.gun.type = type;
	switch (type) {
		case ACTOR_GUN_BARE_HANDS:
			actor.gun.max_accuracy = 1;
			actor.gun.damage = 1;
			actor.gun.recoil_time = 5;
			actor.gun.max_distance = 1;
			actor.gun.accurate = false;
			actor.gun.shoots = 1;
			actor.gun.bullet_speed = 1;
			break;
		case ACTOR_GUN_PISTOL:
			actor.gun.max_accuracy = 4;
			actor.gun.damage = 2;
			actor.gun.recoil_time = 4;
			actor.gun.max_distance = 200;
			actor.gun.accurate = true;
			actor.gun.shoots = 1;
			actor.gun.bullet_speed = 15;
			break;
		case ACTOR_GUN_RIFLE:
			actor.gun.max_accuracy = 2;
			actor.gun.damage = 5;
			actor.gun.recoil_time = 10;
			actor.gun.max_distance = 300;
			actor.gun.accurate = true;
			actor.gun.shoots = 1;
			actor.gun.bullet_speed = 15;
			break;
		case ACTOR_GUN_AK:
			actor.gun.max_accuracy = 1;
			actor.gun.damage = 5;
			actor.gun.recoil_time = 1;
			actor.gun.max_distance = 250;
			actor.gun.accurate = false;
			actor.gun.shoots = 1;
			actor.gun.bullet_speed = 20;
			break;
		case ACTOR_GUN_SHOTGUN:
			actor.gun.max_accuracy = 1;
			actor.gun.damage = 6;
			actor.gun.recoil_time = 15;
			actor.gun.max_distance = 100;
			actor.gun.accurate = false;
			actor.gun.shoots = 3;
			actor.gun.bullet_speed = 15;
			break;
		case ACTOR_GUN_MISILE:
			actor.gun.max_accuracy = 1;
			actor.gun.damage = 10;
			actor.gun.recoil_time = 20;
			actor.gun.max_distance = 300;
			actor.gun.accurate = true;
			actor.gun.shoots = 1;
			actor.gun.bullet_speed = 10;
			break;
		default:
			sprintf(engine.system_error_message1, "ACTOR_SetGun function error");
			sprintf(engine.system_error_message2, "Undefined gun type %u ", type);
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_SYSTEM);
			break;
	}
}

/** ACTOR :: Initializes actor data
 * - Sets default values for the actor structure 
 */
void ACTOR_Init(int x, int y, int graphics_id, int gun_graphics_id, int bullet_graphics_id, int colission_frame, int hit_frame) {
	actor.pos_x = x;
	actor.pos_y = y;
	actor.x_FP = x << FP;
	actor.y_FP = y << FP;
	actor.vx_FP = 0;
	actor.vy_FP = 0;

	actor.speed_horizontal_FP = ACTOR_SPEED << FP;
	actor.speed_vertical_FP = (ACTOR_SPEED << FP) - ((ACTOR_SPEED << FP) >> 3);
	actor.speed_diagonalv_FP = (actor.speed_vertical_FP * 10) / 14;
	actor.speed_diagonalh_FP = (actor.speed_horizontal_FP * 10) / 14;

	actor.speed_horizontal_loop_FP = actor.speed_horizontal_FP + (actor.speed_horizontal_FP >> 1);
	actor.speed_vertical_loop_FP = actor.speed_vertical_FP + (actor.speed_vertical_FP >> 1);
	actor.speed_diagonalv_loop_FP = actor.speed_diagonalv_FP + (actor.speed_diagonalv_FP >> 1);
	actor.speed_diagonalh_loop_FP = actor.speed_diagonalh_FP + (actor.speed_diagonalh_FP >> 1);
	actor.speed = ACTOR_SPEED;
	actor.max_life = ACTOR_LIFE;
	actor.life = ACTOR_LIFE;
	actor.damage = 0;
	actor.is_hit = false;

	actor.shoot_accuracy_speed = ACTOR_ACCURACY_SPEED;
	actor.shoot_accuracy_counter = 0;

	actor.status_loop = false;
	actor.status_shoot = false;
	actor.status_punch = false;
	actor.status_stand = false;
	actor.status_walk = false;
	actor.status_facing = 0;

	actor.mode_combat = false;

	// Initialize actor sprite
	if (!GFX_InitSprite(graphics_id, ENTITY_ID_ACTOR, 0, &actor.sprite_num)) {
		sprintf(engine.system_error_message1, "ACTOR_Init function error");
		sprintf(engine.system_error_message2, "Cannot initialize actor sprite");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Set colission points
	//  ----------------
	// |                |
	// |                |
	// |                |
	// |   1    2    3  |
	// |  4           5 |
	// |  6           7 |
	// |  8           9 |
	// |   10   11  12  |
	//  ----------------
	// Point 1
	actor.colission_area.up_points[0][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 3;
	actor.colission_area.up_points[0][1] = (gfx_sprite_stack[actor.sprite_num].height_px >> 1) - (gfx_sprite_stack[actor.sprite_num].height_px >> 4);
	// Point 2
	actor.colission_area.up_points[1][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 1;
	actor.colission_area.up_points[1][1] = (gfx_sprite_stack[actor.sprite_num].height_px >> 1) - (gfx_sprite_stack[actor.sprite_num].height_px >> 4);
	// Point 3
	actor.colission_area.up_points[2][0] = gfx_sprite_stack[actor.sprite_num].width_px - (gfx_sprite_stack[actor.sprite_num].width_px >> 3);
	actor.colission_area.up_points[2][1] = (gfx_sprite_stack[actor.sprite_num].height_px >> 1) - (gfx_sprite_stack[actor.sprite_num].height_px >> 4);

	// Point 4
	actor.colission_area.left_points[0][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 4;
	actor.colission_area.left_points[0][1] = gfx_sprite_stack[actor.sprite_num].height_px >> 1;
	// Point 6
	actor.colission_area.left_points[1][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 4;
	actor.colission_area.left_points[1][1] = (gfx_sprite_stack[actor.sprite_num].height_px >> 1) + (gfx_sprite_stack[actor.sprite_num].height_px >> 2);
	// Point 8
	actor.colission_area.left_points[2][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 4;
	actor.colission_area.left_points[2][1] = gfx_sprite_stack[actor.sprite_num].height_px - (gfx_sprite_stack[actor.sprite_num].height_px >> 4);

	// Point 5
	actor.colission_area.right_points[0][0] = gfx_sprite_stack[actor.sprite_num].width_px - (gfx_sprite_stack[actor.sprite_num].width_px >> 4);
	actor.colission_area.right_points[0][1] = gfx_sprite_stack[actor.sprite_num].height_px >> 1;
	// Point 7
	actor.colission_area.right_points[1][0] = gfx_sprite_stack[actor.sprite_num].width_px - (gfx_sprite_stack[actor.sprite_num].width_px >> 4);
	actor.colission_area.right_points[1][1] = (gfx_sprite_stack[actor.sprite_num].height_px >> 1) + (gfx_sprite_stack[actor.sprite_num].height_px >> 2);
	// Point 9
	actor.colission_area.right_points[2][0] = gfx_sprite_stack[actor.sprite_num].width_px - (gfx_sprite_stack[actor.sprite_num].width_px >> 4);
	actor.colission_area.right_points[2][1] = gfx_sprite_stack[actor.sprite_num].height_px - (gfx_sprite_stack[actor.sprite_num].height_px >> 4);

	// Point 10
	actor.colission_area.down_points[0][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 3;
	actor.colission_area.down_points[0][1] = gfx_sprite_stack[actor.sprite_num].height_px;
	// Point 11
	actor.colission_area.down_points[1][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 1;
	actor.colission_area.down_points[1][1] = gfx_sprite_stack[actor.sprite_num].height_px;
	// Point 12
	actor.colission_area.down_points[2][0] = gfx_sprite_stack[actor.sprite_num].width_px - (gfx_sprite_stack[actor.sprite_num].width_px >> 3);
	actor.colission_area.down_points[2][1] = gfx_sprite_stack[actor.sprite_num].height_px;

	// Set hit box
	//  ----------------
	// |                |
	// |                |
	// |  1          2  |
	// |                |
	// |                |
	// |                |
	// |                |
	// |  3          4  |
	//  ----------------
	// Point 1
	actor.hit_area.points[0][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 3;
	actor.hit_area.points[0][1] = (gfx_sprite_stack[actor.sprite_num].height_px >> 1) - (gfx_sprite_stack[actor.sprite_num].height_px >> 3);
	// Point 2
	actor.hit_area.points[1][0] = gfx_sprite_stack[actor.sprite_num].width_px - (gfx_sprite_stack[actor.sprite_num].width_px >> 3);
	actor.hit_area.points[1][1] = (gfx_sprite_stack[actor.sprite_num].height_px >> 1) - (gfx_sprite_stack[actor.sprite_num].height_px >> 3);
	// Point 3
	actor.hit_area.points[2][0] = gfx_sprite_stack[actor.sprite_num].width_px >> 3;
	actor.hit_area.points[2][1] = gfx_sprite_stack[actor.sprite_num].height_px - (gfx_sprite_stack[actor.sprite_num].height_px >> 4);
	// Point 4
	actor.hit_area.points[3][0] = gfx_sprite_stack[actor.sprite_num].width_px - (gfx_sprite_stack[actor.sprite_num].width_px >> 3);
	actor.hit_area.points[3][1] = gfx_sprite_stack[actor.sprite_num].height_px - (gfx_sprite_stack[actor.sprite_num].height_px >> 4);

	gfx_sprite_stack[actor.sprite_num].animation_base_frame = 20;
	gfx_sprite_stack[actor.sprite_num].animation_frames = 1;
	gfx_sprite_stack[actor.sprite_num].animation_speed = ACTOR_ANIMATION_SPEED;
	gfx_sprite_stack[actor.sprite_num].current_frame = 1;

	gfx_sprite_stack[actor.sprite_num].screen_pos_x = actor.pos_x - camera.pos_x;
	gfx_sprite_stack[actor.sprite_num].screen_pos_y = actor.pos_y - camera.pos_y;

	// Initialize gun
	ACTOR_SetGun(ACTOR_GUN_BARE_HANDS, gun_graphics_id, bullet_graphics_id);
}

void ACTOR_DrawColissionPixels(void) {
	//  Draw colission pixels
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.up_points[0][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.up_points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.up_points[1][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.up_points[1][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.up_points[2][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.up_points[2][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.down_points[0][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.down_points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.down_points[1][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.down_points[1][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.down_points[2][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.down_points[2][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.left_points[0][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.left_points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.left_points[1][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.left_points[1][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.left_points[2][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.left_points[2][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.right_points[0][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.right_points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.right_points[1][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.right_points[1][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.colission_area.right_points[2][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.colission_area.right_points[2][1], 229);
}

void ACTOR_DrawHitPixels(void) {
	//  Draw colission pixels
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.hit_area.points[0][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.hit_area.points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.hit_area.points[1][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.hit_area.points[1][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.hit_area.points[2][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.hit_area.points[2][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x + actor.hit_area.points[3][0], gfx_sprite_stack[actor.sprite_num].screen_pos_y + actor.hit_area.points[3][1], 229);
}

bool ACTOR_CheckUpColission(void) {
	int i;
	int point1_x, point1_y;
	int point2_x, point2_y;
	int point3_x, point3_y;

	point1_x = actor.pos_x + actor.colission_area.up_points[0][0];
	point1_y = actor.pos_y + actor.colission_area.up_points[0][1];
	point2_x = actor.pos_x + actor.colission_area.up_points[1][0];
	point2_y = actor.pos_y + actor.colission_area.up_points[1][1];
	point3_x = actor.pos_x + actor.colission_area.up_points[2][0];
	point3_y = actor.pos_y + actor.colission_area.up_points[2][1];

	/////// BACKGROUND COLISSIONS ///////////
	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2
	if (MAP_CheckColissionTile(point3_x, point3_y)) { return true; }// Check point 3

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x < object[i].pos_x + object[i].colission_area.down_points[2][0]

					&& point1_x > object[i].pos_x + object[i].colission_area.down_points[0][0]

					&& point1_y > object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point1_y < object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x < object[i].pos_x + object[i].colission_area.down_points[2][0]

					&& point2_x > object[i].pos_x + object[i].colission_area.down_points[0][0]

					&& point2_y > object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point2_y < object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 3
				if (point3_x < object[i].pos_x + object[i].colission_area.down_points[2][0]

					&& point3_x > object[i].pos_x + object[i].colission_area.down_points[0][0]

					&& point3_y > object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point3_y < object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	/////// ENEMIES COLISSIONS ///////////
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown) {
				//Point 1
				if (point1_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point1_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point1_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point1_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point2_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point2_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point2_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 3
				if (point3_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point3_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point3_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point3_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

bool ACTOR_CheckDownColission(void) {
	int i;

	int point1_x, point1_y;
	int point2_x, point2_y;
	int point3_x, point3_y;

	point1_x = actor.pos_x + actor.colission_area.down_points[0][0];
	point1_y = actor.pos_y + actor.colission_area.down_points[0][1];
	point2_x = actor.pos_x + actor.colission_area.down_points[1][0];
	point2_y = actor.pos_y + actor.colission_area.down_points[1][1];
	point3_x = actor.pos_x + actor.colission_area.down_points[2][0];
	point3_y = actor.pos_y + actor.colission_area.down_points[2][1];

	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2
	if (MAP_CheckColissionTile(point3_x, point3_y)) { return true; }// Check point 3

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x < object[i].pos_x + object[i].colission_area.up_points[2][0]

					&& point1_x > object[i].pos_x + object[i].colission_area.up_points[0][0]

					&& point1_y > object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point1_y < object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x < object[i].pos_x + object[i].colission_area.up_points[2][0]

					&& point2_x > object[i].pos_x + object[i].colission_area.up_points[0][0]

					&& point2_y > object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point2_y < object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 3
				if (point3_x < object[i].pos_x + object[i].colission_area.up_points[2][0]

					&& point3_x > object[i].pos_x + object[i].colission_area.up_points[0][0]

					&& point3_y > object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point3_y < object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	/////// ENEMIES COLISSIONS ///////////
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown) {
				//Point 1
				if (point1_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point1_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point1_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point1_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point2_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point2_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point2_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 3
				if (point3_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point3_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point3_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point3_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

bool ACTOR_CheckLeftColission(void) {
	int i;

	int point1_x, point1_y;
	int point2_x, point2_y;
	int point3_x, point3_y;

	point1_x = actor.pos_x + actor.colission_area.left_points[0][0];
	point1_y = actor.pos_y + actor.colission_area.left_points[0][1];
	point2_x = actor.pos_x + actor.colission_area.left_points[1][0];
	point2_y = actor.pos_y + actor.colission_area.left_points[1][1];
	point3_x = actor.pos_x + actor.colission_area.left_points[2][0];
	point3_y = actor.pos_y + actor.colission_area.left_points[2][1];

	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2
	if (MAP_CheckColissionTile(point3_x, point3_y)) { return true; }// Check point 3

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x < object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point1_x > object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point1_y > object[i].pos_y + object[i].colission_area.right_points[0][1]

					&& point1_y < object[i].pos_y + object[i].colission_area.right_points[2][1]) {
					return true;
				}

				//Point 1
				if (point2_x < object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point2_x > object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point2_y > object[i].pos_y + object[i].colission_area.right_points[0][1]

					&& point2_y < object[i].pos_y + object[i].colission_area.right_points[2][1]) {
					return true;
				}

				//Point 1
				if (point3_x < object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point3_x > object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point3_y > object[i].pos_y + object[i].colission_area.right_points[0][1]

					&& point3_y < object[i].pos_y + object[i].colission_area.right_points[2][1]) {
					return true;
				}
			}
		}
	}

	/////// ENEMIES COLISSIONS ///////////
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown) {

				//Point 1
				if (point1_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point1_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point1_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point1_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point2_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point2_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point2_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 3
				if (point3_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point3_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point3_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point3_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

bool ACTOR_CheckRightColission(void) {
	int i;

	int point1_x, point1_y;
	int point2_x, point2_y;
	int point3_x, point3_y;

	point1_x = actor.pos_x + actor.colission_area.right_points[0][0];
	point1_y = actor.pos_y + actor.colission_area.right_points[0][1];
	point2_x = actor.pos_x + actor.colission_area.right_points[1][0];
	point2_y = actor.pos_y + actor.colission_area.right_points[1][1];
	point3_x = actor.pos_x + actor.colission_area.right_points[2][0];
	point3_y = actor.pos_y + actor.colission_area.right_points[2][1];

	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2
	if (MAP_CheckColissionTile(point3_x, point3_y)) { return true; }// Check point 3

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x < object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point1_x > object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point1_y > object[i].pos_y + object[i].colission_area.left_points[0][1]

					&& point1_y < object[i].pos_y + object[i].colission_area.left_points[2][1]) {
					return true;
				}

				//Point 1
				if (point2_x < object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point2_x > object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point2_y > object[i].pos_y + object[i].colission_area.left_points[0][1]

					&& point2_y < object[i].pos_y + object[i].colission_area.left_points[2][1]) {
					return true;
				}

				//Point 1
				if (point3_x < object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point3_x > object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point3_y > object[i].pos_y + object[i].colission_area.left_points[0][1]

					&& point3_y < object[i].pos_y + object[i].colission_area.left_points[2][1]) {
					return true;
				}
			}
		}
	}

	/////// ENEMIES COLISSIONS ///////////
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown) {
				//Point 1
				if (point1_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point1_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point1_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point1_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point2_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point2_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point2_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 3
				if (point3_x < enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point3_x > enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point3_y > enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point3_y < enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

/** ACTOR :: In game update
 */
void ACTOR_Update(void) {
	bool status_idle;
	byte movement;
	int axis_x, axis_y, axis_relation;

	// Switch mode combat
	if (kbKeyState[SCANCODE_C]) {
		if (actor.mode_combat) {
			actor.mode_combat = false;
		} else {
			actor.mode_combat = true;
		}
		kbKeyState[SCANCODE_C] = false;
	}

	movement = 0;
	movement = movement | (kbKeyState[kbBindingUp] << 3);
	movement = movement | (kbKeyState[kbBindingDown] << 2);
	movement = movement | (kbKeyState[kbBindingLeft] << 1);
	movement = movement | (kbKeyState[kbBindingRight]);

	// Calculate facing direction with actor and mouse positions
	actor.status_facing = 0;
	if (cursor.pos_x <= gfx_sprite_stack[actor.sprite_num].screen_pos_x) {// facing left
		if (cursor.pos_y <= gfx_sprite_stack[actor.sprite_num].screen_pos_y) {
			axis_x = gfx_sprite_stack[actor.sprite_num].screen_pos_x - cursor.pos_x + 100;
			axis_y = gfx_sprite_stack[actor.sprite_num].screen_pos_y - cursor.pos_y + 100;
			axis_relation = axis_x / axis_y;

			if (axis_relation > 1) {// facing left
				actor.status_facing = actor.status_facing | (1 << 1);
			}// facing left
			if (axis_relation == 1) {
				actor.status_facing = actor.status_facing | (1 << 1);// facing left
				actor.status_facing = actor.status_facing | (1 << 3);// facing up
			}
			if (axis_relation < 1) {// facing up
				actor.status_facing = actor.status_facing | (1 << 3);
			}// up
		} else {
			axis_x = gfx_sprite_stack[actor.sprite_num].screen_pos_x - cursor.pos_x + 100;
			axis_y = cursor.pos_y - gfx_sprite_stack[actor.sprite_num].screen_pos_y + 100;
			axis_relation = axis_x / axis_y;

			if (axis_relation > 1) {// facing left
				actor.status_facing = actor.status_facing | (1 << 1);
			}
			if (axis_relation == 1) {
				actor.status_facing = actor.status_facing | (1 << 1);// facing left
				actor.status_facing = actor.status_facing | (1 << 2);// facing down
			}
			if (axis_relation < 1) {
				actor.status_facing = actor.status_facing | (1 << 2);// facing down
			}
		}
	} else {//facing right
		if (cursor.pos_y <= gfx_sprite_stack[actor.sprite_num].screen_pos_y) {
			axis_x = cursor.pos_x - gfx_sprite_stack[actor.sprite_num].screen_pos_x + 1;
			axis_y = gfx_sprite_stack[actor.sprite_num].screen_pos_y - cursor.pos_y + 1;
			axis_relation = axis_x / axis_y;

			if (axis_relation > 1) { actor.status_facing = actor.status_facing | 1; }// facing right
			if (axis_relation == 1) {
				actor.status_facing = actor.status_facing | 1;       // facing right
				actor.status_facing = actor.status_facing | (1 << 3);// facing up
			}
			if (axis_relation < 1) { actor.status_facing = actor.status_facing | (1 << 3); }// up
		} else {
			axis_x = cursor.pos_x - gfx_sprite_stack[actor.sprite_num].screen_pos_x + 1;
			axis_y = cursor.pos_y - gfx_sprite_stack[actor.sprite_num].screen_pos_y + 1;
			axis_relation = axis_x / axis_y;

			if (axis_relation > 1) { actor.status_facing = actor.status_facing | 1; }// facing right
			if (axis_relation == 1) {
				actor.status_facing = actor.status_facing | 1;       // facing right
				actor.status_facing = actor.status_facing | (1 << 2);// facing down
			}
			if (axis_relation < 1) { actor.status_facing = actor.status_facing | (1 << 2); }// down
		}
	}
	if (actor.status_dead) { actor.status_facing = 0; }

	// Check idle status
	status_idle = !actor.status_dead & !actor.status_loop & !actor.status_shoot & !actor.status_punch;

	//// ---------------- INITIALIZE ACTION ----------------

	// loop
	if (status_idle & actor.mode_combat & (kbKeyState[kbBindingJump] & (movement != 0))) {
		status_idle = false;
		actor.status_loop = true;
		actor.status_walk = false;
		actor.status_shoot = false;
		actor.status_punch = false;
		actor.status_stand = false;

		actor.movement = movement;
		actor.action_step = 0;
		actor.last_action = ACTOR_ACTION_LOOP;

		gfx_sprite_stack[actor.sprite_num].animation_speed = ACTOR_ANIMATION_SPEED >> 1;
		gfx_sprite_stack[actor.sprite_num].animation_frames = 6;
		gfx_sprite_stack[actor.sprite_num].current_frame = 1;
		gfx_sprite_stack[actor.sprite_num].animation_end = false;
		gfx_sprite_stack[actor.sprite_num].animation_loop = false;

		switch (actor.movement) {
			case 1:// loop right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 50;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 2:// loop left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 50;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 4:// loop down
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 70;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 5:// loop down-right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 50;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 6:// loop down-left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 50;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 8:// loop up
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 60;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 9:// loop up-right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 50;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 10:// loop up-left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 50;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
		}
	}

	// shoot
	if (status_idle & actor.mode_combat & (actor.gun.type != ACTOR_GUN_BARE_HANDS) & (kbKeyState[kbBindingFire] | cursor.left_click)) {
		status_idle = false;
		actor.status_shoot = true;
		actor.status_loop = false;
		actor.status_walk = false;
		actor.status_stand = false;
		actor.status_punch = false;

		actor.movement = 0;
		actor.shoot_accuracy = 0;
		actor.action_step = 0;
		actor.last_action = ACTOR_ACTION_SHOOT;

		gfx_sprite_stack[actor.sprite_num].animation_speed = ACTOR_ANIMATION_SPEED;
		gfx_sprite_stack[actor.sprite_num].animation_frames = 1;
		gfx_sprite_stack[actor.sprite_num].current_frame = 1;
		gfx_sprite_stack[actor.sprite_num].animation_end = false;
		gfx_sprite_stack[actor.sprite_num].animation_loop = false;

		switch (actor.status_facing) {
			case 1:// facing right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 28;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 2:// facing left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 28;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 4:// facing down
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 48;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 5:// facing down-right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 38;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 6:// facing down-left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 38;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 8:// facing up
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 8;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 9:// facing up-right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 18;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 10:// facing up-left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 18;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			default:
				break;
		}
	}

	// punch
	if (status_idle & actor.mode_combat & (actor.gun.type == ACTOR_GUN_BARE_HANDS) & (kbKeyState[kbBindingFire] | cursor.left_click)) {
		status_idle = false;
		actor.status_punch = true;
		actor.status_shoot = false;
		actor.status_loop = false;
		actor.status_walk = false;
		actor.status_stand = false;

		actor.movement = 0;
		actor.shoot_accuracy = 0;
		actor.action_step = 0;
		actor.last_action = ACTOR_ACTION_SHOOT;

		gfx_sprite_stack[actor.sprite_num].animation_speed = 8;
		gfx_sprite_stack[actor.sprite_num].animation_frames = 3;
		gfx_sprite_stack[actor.sprite_num].current_frame = 1;
		gfx_sprite_stack[actor.sprite_num].animation_end = false;
		gfx_sprite_stack[actor.sprite_num].animation_loop = false;


		switch (actor.status_facing) {
			case 1:// facing right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 15;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 2:// facing left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 15;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 4:// facing down
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 25;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 5:// facing down-right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 25;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 6:// facing down-left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 25;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 8:// facing up
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 5;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 9:// facing up-right
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 5;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 10:// facing up-left
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 5;
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			default:
				break;
		}
	}

	// walk
	if (status_idle & (movement != 0)) {
		actor.status_walk = true;
		actor.status_shoot = false;
		actor.status_loop = false;
		actor.status_stand = false;
		actor.status_punch = false;

		actor.movement = movement;
		actor.action_step = 0;
		actor.last_action = ACTOR_ACTION_WALK;

		gfx_sprite_stack[actor.sprite_num].animation_speed = ACTOR_ANIMATION_SPEED;
		gfx_sprite_stack[actor.sprite_num].animation_frames = 4;
		gfx_sprite_stack[actor.sprite_num].animation_loop = true;
		gfx_sprite_stack[actor.sprite_num].animation_end = false;

		switch (actor.status_facing) {
			case 1:// facing right
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 21;
				break;
			case 2:// facing left
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 21;
				break;
			case 4:// moving down
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 41;
				break;
			case 5:// moving down-right
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 31;
				break;
			case 6:// moving down-left
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 31;
				break;
			case 8:// moving up
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 1;
				break;
			case 9:// moving up-right
				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 11;
				break;
			case 10:// moving up-left
				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				gfx_sprite_stack[actor.sprite_num].animation_base_frame = 11;
				break;
			default:
				break;
		}
	}

	// no action
	if (status_idle & (movement == 0)) {
		actor.movement = 0;
		actor.action_step = 0;

		if (!actor.status_stand) {
			actor.idle_counter++;
			if (actor.idle_counter > ACTOR_IDLE_TICKS) {
				actor.status_stand = true;
				actor.last_action = 0;
				actor.idle_counter = 0;
			}
		}

		switch (actor.status_facing) {
			case 1:// facing right
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 28;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 20;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 2:// facing left
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 28;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 20;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 4:// facing down
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 48;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 40;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 5:// facing down-right
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 38;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 30;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 6:// facing down-left
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 38;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 30;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			case 8:// facing up
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 8;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 0;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 9:// facing up-right
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 18;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 10;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = false;
				break;
			case 10:// facing up-left
				if (actor.last_action == ACTOR_ACTION_SHOOT) gfx_sprite_stack[actor.sprite_num].animation_base_frame = 18;
				else
					gfx_sprite_stack[actor.sprite_num].animation_base_frame = 10;

				gfx_sprite_stack[actor.sprite_num].animation_inverted = true;
				break;
			default:
				break;
		}

		gfx_sprite_stack[actor.sprite_num].animation_frames = 1;
		gfx_sprite_stack[actor.sprite_num].current_frame = 1;
		gfx_sprite_stack[actor.sprite_num].animation_speed = ACTOR_ANIMATION_SPEED;
		gfx_sprite_stack[actor.sprite_num].animation_loop = false;
	}

	if (!actor.mode_combat) {
	}

	// Check if is hit
	if (actor.is_hit) {
		actor.is_hit = false;

		gfx_sprite_stack[actor.sprite_num].blink = true;
		if (actor.damage > actor.life) actor.damage = actor.life;
		actor.life -= actor.damage;
		GFX_UpdatePanel(&gfx_actor_status_panel, actor.life, actor.life + actor.damage, actor.max_life, 1);

		if (actor.life <= 0) {
			actor.life = 0;

			actor.status_dead = true;
			actor.status_shoot = false;
			actor.status_loop = false;
			actor.status_walk = false;
			actor.status_stand = false;

			actor.movement = 0;
			actor.shoot_accuracy = 0;
			actor.action_step = 0;
			actor.last_action = 0;

			gfx_sprite_stack[actor.sprite_num].animation_speed = 20;
			gfx_sprite_stack[actor.sprite_num].animation_frames = 3;
			gfx_sprite_stack[actor.sprite_num].current_frame = 0;

			gfx_sprite_stack[actor.sprite_num].animation_base_frame = 35;
			gfx_sprite_stack[actor.sprite_num].animation_inverted = false;

			GFX_SetPanelPortait(&gfx_actor_status_panel, 2);
		}
		actor.life_shown = true;
		actor.damage = 0;

	} else {
		gfx_sprite_stack[actor.sprite_num].blink = false;
	}

	//// ---------------- POSITIONING ----------------

	// loop position calculation
	if (actor.status_loop) {
		switch (actor.movement) {
			case 1:// loop right
				actor.x_FP += actor.speed_horizontal_loop_FP;
				actor.pos_x = actor.x_FP >> FP;
				if (ACTOR_CheckRightColission()) {
					actor.x_FP -= actor.speed_horizontal_loop_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 2:// loop left
				actor.x_FP -= actor.speed_horizontal_loop_FP;
				actor.pos_x = actor.x_FP >> FP;
				if (ACTOR_CheckLeftColission()) {
					actor.x_FP += actor.speed_horizontal_loop_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 4:// loop down
				actor.y_FP += actor.speed_vertical_loop_FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckDownColission()) {
					actor.y_FP -= actor.speed_vertical_loop_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				break;
			case 5:// loop down-right
				actor.x_FP += actor.speed_diagonalh_loop_FP;
				actor.y_FP += actor.speed_diagonalv_loop_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckDownColission()) {
					actor.y_FP -= actor.speed_diagonalv_loop_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckRightColission()) {
					actor.x_FP -= actor.speed_diagonalh_loop_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 6:// loop down-left
				actor.x_FP -= actor.speed_diagonalh_loop_FP;
				actor.y_FP += actor.speed_diagonalv_loop_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckDownColission()) {
					actor.y_FP -= actor.speed_diagonalv_loop_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckLeftColission()) {
					actor.x_FP += actor.speed_diagonalh_loop_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 8:// loop up
				actor.y_FP -= actor.speed_vertical_loop_FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckUpColission()) {
					actor.y_FP += actor.speed_vertical_loop_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				break;
			case 9:// loop up-right
				actor.x_FP += actor.speed_diagonalh_loop_FP;
				actor.y_FP -= actor.speed_diagonalv_loop_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckUpColission()) {
					actor.y_FP += actor.speed_diagonalv_loop_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckRightColission()) {
					actor.x_FP -= actor.speed_diagonalh_loop_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 10:// loop up-left
				actor.x_FP -= actor.speed_diagonalh_loop_FP;
				actor.y_FP -= actor.speed_diagonalv_loop_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckUpColission()) {
					actor.y_FP += actor.speed_diagonalv_loop_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckLeftColission()) {
					actor.x_FP += actor.speed_diagonalh_loop_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
		}

		// End of loop action
		if (gfx_sprite_stack[actor.sprite_num].animation_end) {
			actor.status_loop = false;
			gfx_sprite_stack[actor.sprite_num].current_frame = 1;
		}
	}

	// Shooting and animation
	if (actor.status_shoot) {
		switch (actor.action_step) {
			case 0:// Jump to mode step
				if (actor.gun.accurate) {
					actor.action_step++;
				} else {
					// Bad accuracy
					actor.shoot_accuracy = 0;
					// next step
					actor.action_step = 2;
				}
				break;
			case 1:// Wait until trigger release
				if ((kbKeyState[kbBindingFire] | cursor.left_click)) actor.shoot_accuracy_counter++;
				if (actor.shoot_accuracy_counter >= actor.shoot_accuracy_speed) {
					actor.shoot_accuracy_counter = 0;
					if (actor.shoot_accuracy < actor.gun.max_accuracy) actor.shoot_accuracy++;

					MOUSE_SetCursorFrame(actor.shoot_accuracy + 1);
				}
				if (!kbKeyState[kbBindingFire] & !cursor.left_click) actor.action_step++;
				break;
			case 2:// Calculate target
				actor.shoot_x = camera.pos_x + cursor.pos_x;
				actor.shoot_y = camera.pos_y + cursor.pos_y;
				actor.action_step++;
				break;
			case 3:// Set shoot animation
				MOUSE_SetCursorFrame(1);
				gfx_sprite_stack[actor.sprite_num].animation_base_frame++;
				actor.action_step++;
				break;
			case 4:// Shoot
				AUDIO_PlaySound(AUDIO_GUN_EFFECT);
				switch (actor.status_facing) {
					case 1:// facing right
						ACTOR_InitBullet(actor.pos_x + 32, actor.pos_y + 8, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					case 2:// facing left
						ACTOR_InitBullet(actor.pos_x, actor.pos_y + 8, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					case 4:// facing down
						ACTOR_InitBullet(actor.pos_x + 16, actor.pos_y + 20, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					case 5:// moving down-right
						ACTOR_InitBullet(actor.pos_x + 18, actor.pos_y + 16, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					case 6:// moving down-left
						ACTOR_InitBullet(actor.pos_x + 6, actor.pos_y + 16, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					case 8:// moving up
						ACTOR_InitBullet(actor.pos_x + 16, actor.pos_y - 8, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					case 9:// moving up-right
						ACTOR_InitBullet(actor.pos_x + 25, actor.pos_y - 1, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					case 10:// moving up-left
						ACTOR_InitBullet(actor.pos_x + 6, actor.pos_y - 1, actor.shoot_x, actor.shoot_y, actor.shoot_accuracy);
						break;
					default:
						break;
				}
				actor.action_step++;
				break;
			case 5:// Set pointing animation
				actor.gun.current_recoil = 0;
				actor.action_step++;
				break;
			case 6:// Wait recoil time
				actor.gun.current_recoil++;
				if (actor.gun.current_recoil >= actor.gun.recoil_time) {
					actor.gun.current_recoil = 0;
					actor.action_step++;
				}
				break;
			case 7:// End sequence
				gfx_sprite_stack[actor.sprite_num].animation_base_frame--;
				actor.action_step = 0;
				actor.status_shoot = false;
				break;
			default:
				actor.action_step = 0;
				actor.status_shoot = false;
				break;
		}
	}

	// Punch and animation
	if (actor.status_punch) {
		if (gfx_sprite_stack[actor.sprite_num].animation_end) {
			actor.status_punch = false;
		}
	}

	// Walking and animation
	if (actor.status_walk) {
		switch (actor.movement) {
			case 1:// moving right
				actor.x_FP += actor.speed_horizontal_FP;
				actor.pos_x = actor.x_FP >> FP;
				if (ACTOR_CheckRightColission()) {
					actor.x_FP -= actor.speed_horizontal_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 2:// moving left
				actor.x_FP -= actor.speed_horizontal_FP;
				actor.pos_x = actor.x_FP >> FP;
				if (ACTOR_CheckLeftColission()) {
					actor.x_FP += actor.speed_horizontal_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 4:// moving down
				actor.y_FP += actor.speed_vertical_FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckDownColission()) {
					actor.y_FP -= actor.speed_vertical_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				break;
			case 5:// moving down-right
				actor.x_FP += actor.speed_diagonalh_FP;
				actor.y_FP += actor.speed_diagonalv_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckDownColission()) {
					actor.y_FP -= actor.speed_diagonalv_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckRightColission()) {
					actor.x_FP -= actor.speed_diagonalh_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 6:// moving down-left
				actor.x_FP -= actor.speed_diagonalh_FP;
				actor.y_FP += actor.speed_diagonalv_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckDownColission()) {
					actor.y_FP -= actor.speed_diagonalv_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckLeftColission()) {
					actor.x_FP += actor.speed_diagonalh_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 8:// moving up
				actor.y_FP -= actor.speed_vertical_FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckUpColission()) {
					actor.y_FP += actor.speed_vertical_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				break;
			case 9:// moving up-right
				actor.x_FP += actor.speed_diagonalh_FP;
				actor.y_FP -= actor.speed_diagonalv_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckUpColission()) {
					actor.y_FP += actor.speed_diagonalv_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckRightColission()) {
					actor.x_FP -= actor.speed_diagonalh_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			case 10:// moving up-left
				actor.x_FP -= actor.speed_diagonalh_FP;
				actor.y_FP -= actor.speed_diagonalv_FP;
				actor.pos_x = actor.x_FP >> FP;
				actor.pos_y = actor.y_FP >> FP;
				if (ACTOR_CheckUpColission()) {
					actor.y_FP += actor.speed_diagonalv_FP;
					actor.pos_y = actor.y_FP >> FP;
				}
				if (ACTOR_CheckLeftColission()) {
					actor.x_FP += actor.speed_diagonalh_FP;
					actor.pos_x = actor.x_FP >> FP;
				}
				break;
			default:
				break;
		}

		actor.status_walk = false;
	}

	// Check if is dead
	if (actor.status_dead) {
		if (gfx_sprite_stack[actor.sprite_num].animation_end) {
			// end game
		}
	}

	// Life shown
	if (actor.life_shown) {


		//VIDEO_DrawLifeBarOnScreenBuffer(gfx_sprite_stack[actor.sprite_num].screen_pos_x, gfx_sprite_stack[actor.sprite_num].screen_pos_y, 16, ((actor.life << 4) / actor.max_life));
		actor.life_shown_counter++;
		if (actor.life_shown_counter > 10) { actor.life_shown = false; }
	}

	// Update sprite position
	gfx_sprite_stack[actor.sprite_num].screen_pos_x = actor.pos_x - camera.pos_x;
	gfx_sprite_stack[actor.sprite_num].screen_pos_y = actor.pos_y - camera.pos_y;

	// Update bullets
	ACTOR_UpdateBullets();
}

/** ACTOR :: Initialize bullet
 */
void ACTOR_InitBullet(int source_x, int source_y, int target_x, int target_y, int accuracy) {
	int i, j;
	int number;
	int dx, dy;
	int distance;

	for (i = 0; i < actor.gun.shoots; i++) {
		// Search free bullet slot
		number = 0;
		for (j = 0; j < ACTOR_MAX_BULLETS; j++) {
			if (!actor.gun.bullet[j].loaded) {
				number = j;
				break;
			}
		}

		// Check if max bullets is reached
		if (number == ACTOR_MAX_BULLETS - 1) {
			Error("ACTOR_InitBullet function error", "Max number of bullets", "", ERROR_SYSTEM);
		}

		// Calculate target
		switch (accuracy) {
			case 0:
				actor.gun.bullet[number].target_x = target_x - 4 - 8 + (rand() % 32);
				actor.gun.bullet[number].target_y = target_y - 4 - 8 + (rand() % 32);
				break;
			case 1:
				actor.gun.bullet[number].target_x = target_x - 4 + (rand() % 24);
				actor.gun.bullet[number].target_y = target_y - 4 + (rand() % 24);
				break;
			case 2:
				actor.gun.bullet[number].target_x = target_x - 8 + (rand() % 16);
				actor.gun.bullet[number].target_y = target_y - 8 + (rand() % 16);
				break;
			case 3:
				actor.gun.bullet[number].target_x = target_x + 4 + (rand() % 8);
				actor.gun.bullet[number].target_y = target_y + 4 + (rand() % 8);
				break;
			case 4:
				actor.gun.bullet[number].target_x = target_x + 4;
				actor.gun.bullet[number].target_y = target_y + 4;
				break;
			default:
				actor.gun.bullet[number].target_x = target_x + 4;
				actor.gun.bullet[number].target_y = target_y + 4;
				break;
		}

		actor.gun.bullet[number].pos_x = source_x;
		actor.gun.bullet[number].pos_y = source_y;
		actor.gun.bullet[number].speed = actor.gun.bullet_speed;
		actor.gun.bullet[number].damage = actor.gun.damage;

		// calculate distance from source
		dx = actor.gun.bullet[number].target_x - actor.gun.bullet[number].pos_x;
		dy = actor.gun.bullet[number].target_y - actor.gun.bullet[number].pos_y;

		// calculate steps
		if (abs(dx) > abs(dy))
			//actor.gun.bullet[number].steps = abs(dx);
			distance = abs(dx);
		else
			//actor.gun.bullet[number].steps = abs(dy);
			distance = abs(dy);


		// calculate fixed point velocity and position
		actor.gun.bullet[number].vx_FP = (dx << FP) / distance;
		actor.gun.bullet[number].vy_FP = (dy << FP) / distance;

		actor.gun.bullet[number].x_FP = actor.gun.bullet[number].pos_x << FP;
		actor.gun.bullet[number].y_FP = actor.gun.bullet[number].pos_y << FP;

		actor.gun.bullet[number].steps = distance;
		if (actor.gun.bullet[number].steps > actor.gun.max_distance)
			actor.gun.bullet[number].steps = actor.gun.max_distance;

		// Initialize bullet sprite

		if (!GFX_InitSprite(actor.gun.bullet[number].graphics_id, ENTITY_ID_ACTOR_BULLET, number, &actor.gun.bullet[number].sprite_num)) {
			sprintf(engine.system_error_message1, "ACTOR_InitBullet function error");
			sprintf(engine.system_error_message2, "Cannot initialize bullet %u sprite", number);
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
		}


		// Set colission points
		//  ---------------
		// |  1         2  |
		// |               |
		// |               |
		// |               |
		// |  3         4  |
		//  ---------------
		// Point 1
		actor.gun.bullet[number].colission_area.up_points[0][0] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].width_px >> 4;
		actor.gun.bullet[number].colission_area.up_points[0][1] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].height_px >> 4;
		// Point 2
		actor.gun.bullet[number].colission_area.up_points[1][0] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].width_px - (gfx_sprite_stack[actor.gun.bullet[number].sprite_num].width_px >> 4);
		actor.gun.bullet[number].colission_area.up_points[1][1] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].height_px >> 4;

		// Point 3
		actor.gun.bullet[number].colission_area.down_points[0][0] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].width_px >> 4;
		actor.gun.bullet[number].colission_area.down_points[0][1] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].height_px - (gfx_sprite_stack[actor.gun.bullet[number].sprite_num].height_px >> 4);

		// Point 4
		actor.gun.bullet[number].colission_area.down_points[1][0] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].width_px - (gfx_sprite_stack[actor.gun.bullet[number].sprite_num].width_px >> 4);
		actor.gun.bullet[number].colission_area.down_points[1][1] = gfx_sprite_stack[actor.gun.bullet[number].sprite_num].height_px - (gfx_sprite_stack[actor.gun.bullet[number].sprite_num].height_px >> 4);

		gfx_sprite_stack[actor.gun.bullet[number].sprite_num].animation_base_frame = 0;
		gfx_sprite_stack[actor.gun.bullet[number].sprite_num].animation_frames = 1;
		gfx_sprite_stack[actor.gun.bullet[number].sprite_num].animation_speed = 0;
		gfx_sprite_stack[actor.gun.bullet[number].sprite_num].current_frame = 0;

		gfx_sprite_stack[actor.gun.bullet[number].sprite_num].unmasked = true;

		gfx_sprite_stack[actor.gun.bullet[number].sprite_num].screen_pos_x = actor.gun.bullet[number].pos_x - camera.pos_x;
		gfx_sprite_stack[actor.gun.bullet[number].sprite_num].screen_pos_y = actor.gun.bullet[number].pos_y - camera.pos_y;

		actor.gun.bullet[number].loaded = true;
		actor.gun.bullet[number].shown = true;
	}
}

int ACTOR_CheckBulletColission(Bullet bullet) {
	int i;
	int point1_x, point1_y;
	int point2_x, point2_y;
	int point3_x, point3_y;
	int point4_x, point4_y;
	int tile_type;

	point1_x = bullet.pos_x + bullet.colission_area.up_points[0][0];
	point1_y = bullet.pos_y + bullet.colission_area.up_points[0][1];
	point2_x = bullet.pos_x + bullet.colission_area.up_points[1][0];
	point2_y = bullet.pos_y + bullet.colission_area.up_points[1][1];
	point3_x = bullet.pos_x + bullet.colission_area.down_points[0][0];
	point3_y = bullet.pos_y + bullet.colission_area.down_points[0][1];
	point4_x = bullet.pos_x + bullet.colission_area.down_points[1][0];
	point4_y = bullet.pos_y + bullet.colission_area.down_points[1][1];

	/////// BACKGROUND COLISSIONS ///////////
	tile_type = MAP_CheckColissionTile(point1_x, point1_y);// Check point 1
	if (tile_type != 0) {
		return ((tile_type & 0xFF) << 8) | ENTITY_ID_BACKGROUND;
	}
	tile_type = MAP_CheckColissionTile(point2_x, point2_y);// Check point 2
	if (tile_type != 0) {
		return ((tile_type & 0xFF) << 8) | ENTITY_ID_BACKGROUND;
	}
	tile_type = MAP_CheckColissionTile(point3_x, point3_y);// Check point 3
	if (tile_type != 0) {
		return ((tile_type & 0xFF) << 8) | ENTITY_ID_BACKGROUND;
	}
	tile_type = MAP_CheckColissionTile(point4_x, point4_y);// Check point 4
	if (tile_type != 0) {
		return ((tile_type & 0xFF) << 8) | ENTITY_ID_BACKGROUND;
	}

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1

				if (point1_x < object[i].pos_x + object[i].hit_area.points[1][0]

					&& point2_x > object[i].pos_x + object[i].hit_area.points[0][0]

					&& point1_y > object[i].pos_y + object[i].hit_area.points[0][1]

					&& point3_y < object[i].pos_y + object[i].hit_area.points[2][1]) {
					return ((i << 8) | object[i].type);
				}
			}
		}
	}

	/////// ENEMIES COLISSIONS ///////////
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown) {
				//Point 1
				if (point1_x < enemy[i].pos_x + enemy[i].hit_area.points[1][0]

					&& point1_x > enemy[i].pos_x + enemy[i].hit_area.points[0][0]

					&& point1_y > enemy[i].pos_y + enemy[i].hit_area.points[0][1]

					&& point1_y < enemy[i].pos_y + enemy[i].hit_area.points[2][1]) {
					return gfx_sprite_stack[enemy[i].sprite_num].id;
				}

				//Point 2
				if (point2_x < enemy[i].pos_x + enemy[i].hit_area.points[1][0]

					&& point2_x > enemy[i].pos_x + enemy[i].hit_area.points[0][0]

					&& point2_y > enemy[i].pos_y + enemy[i].hit_area.points[0][1]

					&& point2_y < enemy[i].pos_y + enemy[i].hit_area.points[2][1]) {
					return gfx_sprite_stack[enemy[i].sprite_num].id;
				}

				//Point 3
				if (point3_x < enemy[i].pos_x + enemy[i].hit_area.points[1][0]

					&& point3_x > enemy[i].pos_x + enemy[i].hit_area.points[0][0]

					&& point3_y > enemy[i].pos_y + enemy[i].hit_area.points[0][1]

					&& point3_y < enemy[i].pos_y + enemy[i].hit_area.points[2][1]) {
					return gfx_sprite_stack[enemy[i].sprite_num].id;
				}

				//Point 4
				if (point4_x < enemy[i].pos_x + enemy[i].hit_area.points[1][0]

					&& point4_x > enemy[i].pos_x + enemy[i].hit_area.points[0][0]

					&& point4_y > enemy[i].pos_y + enemy[i].hit_area.points[0][1]

					&& point4_y < enemy[i].pos_y + enemy[i].hit_area.points[2][1]) {
					return gfx_sprite_stack[enemy[i].sprite_num].id;
				}
			}
		}
	}

	return false;
}

/** ACTOR :: Update bullets	
 */
void ACTOR_UpdateBullets(void) {
	int i, j;
	int obj_number, enemy_number;
	byte tile_type;
	int tile_number, tile_data, tileset_index;
	int tile_x, tile_y;
	int tile_screen_x, tile_screen_y;

	for (i = 0; i < ACTOR_MAX_BULLETS; i++) {
		if (actor.gun.bullet[i].loaded) {

			// Not on target
			if (!actor.gun.bullet[i].on_target) {
				// Calculate new pos

				for (j = 0; j < actor.gun.bullet[i].speed; j++) {
					actor.gun.bullet[i].x_FP += actor.gun.bullet[i].vx_FP;
					actor.gun.bullet[i].y_FP += actor.gun.bullet[i].vy_FP;

					actor.gun.bullet[i].pos_x = (actor.gun.bullet[i].x_FP >> FP);
					actor.gun.bullet[i].pos_y = (actor.gun.bullet[i].y_FP >> FP);

					// Update sprite screen pos
					gfx_sprite_stack[actor.gun.bullet[i].sprite_num].screen_pos_x = actor.gun.bullet[i].pos_x - camera.pos_x;
					gfx_sprite_stack[actor.gun.bullet[i].sprite_num].screen_pos_y = actor.gun.bullet[i].pos_y - camera.pos_y;

					// Check if hits something
					actor.gun.bullet[i].hit_on = ACTOR_CheckBulletColission(actor.gun.bullet[i]);

					// Check hit on background, object or enemy
					switch (actor.gun.bullet[i].hit_on & 0xFF) {
						case ENTITY_ID_BACKGROUND:
							tile_type = (actor.gun.bullet[i].hit_on & 0xFF00) >> 8;
							switch (tile_type) {
								case 1:// full colission
									actor.gun.bullet[i].on_target = true;
									break;
								case 2:// half colission
									break;
								case 3:// breaking tile. Set colission data to half colission
									tile_number = MAP_GetTileNumber(actor.gun.bullet[i].pos_x, actor.gun.bullet[i].pos_y);
									map.layer[MAP_COLISSION_LAYER][tile_number] = 2;
									// Get map tile position on screen
									tile_screen_x = ((actor.gun.bullet[i].pos_x >> 4) << 4) - camera.pos_x;
									tile_screen_y = ((actor.gun.bullet[i].pos_y >> 4) << 4) - camera.pos_y;
									// Get map tile position on video buffer
									tile_x = camera.pos_abs_x + tile_screen_x;
									if (tile_x >= video.map_buffer_width) {
										tile_x = tile_x - video.map_buffer_width;
									}
									tile_y = camera.pos_abs_y + tile_screen_y;
									if (tile_y >= video.map_buffer_height) {
										tile_y = tile_y - video.map_buffer_height;
									}
									// redraw tile
									tile_data = map.layer[MAP_BACKGROUND_LAYER][tile_number];
									tileset_index = tile_data << 8;
									MAP_DrawToVideoBuffer(tile_x, tile_y, MAP_TILE_WIDTH, MAP_TILE_HEIGHT, &map.layer_tiles[MAP_BACKGROUND_LAYER].buffer[tileset_index], video.map_buffer[MAP_BACKGROUND_LAYER]);

									PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_ELECTRIC1, ((actor.gun.bullet[i].pos_x >> 4) << 4) + 2, ((actor.gun.bullet[i].pos_y >> 4) << 4) + 2, ((actor.gun.bullet[i].pos_x >> 4) << 4) + 4, ((actor.gun.bullet[i].pos_y >> 4) << 4) + 4, 1, 1);
									actor.gun.bullet[i].on_target = true;
									break;
								default:
									break;
							}
							break;
						case ENTITY_ID_BARREL:
							actor.gun.bullet[i].on_target = true;
							// Get object number
							obj_number = (actor.gun.bullet[i].hit_on & 0xFF00) >> 8;
							object[obj_number].is_hit = true;
							object[obj_number].hit_by = ENTITY_ID_ACTOR_BULLET;
							object[obj_number].damage += actor.gun.damage;
							break;
						case ENTITY_ID_ENEMY:
							actor.gun.bullet[i].on_target = true;
							// Get enemy number
							enemy_number = (actor.gun.bullet[i].hit_on & 0xFF00) >> 8;
							enemy[enemy_number].is_hit = true;
							enemy[enemy_number].hit_by = ENTITY_ID_ACTOR_BULLET;
							enemy[enemy_number].damage += actor.gun.damage;
							break;
						default:
							break;
					}

					actor.gun.bullet[i].current_step++;
					if (actor.gun.bullet[i].current_step >= actor.gun.bullet[i].steps) {
						actor.gun.bullet[i].on_target = true;
					}

					if (actor.gun.bullet[i].on_target) {

						// update bullet graphic
						gfx_sprite_stack[actor.gun.bullet[i].sprite_num].animation_frames = 4;
						gfx_sprite_stack[actor.gun.bullet[i].sprite_num].current_frame = 1;
						gfx_sprite_stack[actor.gun.bullet[i].sprite_num].animation_counter = 0;
						gfx_sprite_stack[actor.gun.bullet[i].sprite_num].animation_speed = 1;
						gfx_sprite_stack[actor.gun.bullet[i].sprite_num].animation_end = false;

						break;
					}
				}
			}

			// On target
			if (actor.gun.bullet[i].on_target) {

				// Update sprite screen pos
				gfx_sprite_stack[actor.gun.bullet[i].sprite_num].screen_pos_x = actor.gun.bullet[i].pos_x - camera.pos_x;
				gfx_sprite_stack[actor.gun.bullet[i].sprite_num].screen_pos_y = actor.gun.bullet[i].pos_y - camera.pos_y;

				if (gfx_sprite_stack[actor.gun.bullet[i].sprite_num].animation_end) {
					actor.gun.bullet[i].on_target = false;
					actor.gun.bullet[i].shown = false;
					actor.gun.bullet[i].current_step = 0;
					actor.gun.bullet[i].loaded = false;
					GFX_UnloadSprite(actor.gun.bullet[i].sprite_num);
				}
			}
		}
	}
}
