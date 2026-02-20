#include "../engine.h"
#include "enemy.h"

Enemy enemy[ENEMY_MAX_ENEMIES];
Bullet enemy_bullets[ENEMY_MAX_BULLETS];

/** ENEMY :: Initialize bullet
 */
void ENEMY_InitBullet(int enemy_number, int source_x, int source_y, int target_x, int target_y, int accuracy) {
	int i, j;
	int number;
	int dx, dy;
	int distance;

	for (i = 0; i < enemy[enemy_number].gun.shoots; i++) {
		// Search free bullet slot
		number = 0;
		for (j = 0; j < ENEMY_MAX_BULLETS; j++) {
			if (!enemy[enemy_number].gun.bullet[j].loaded) {
				number = j;
				break;
			}
		}

		// Check if max bullets is reached
		if (number == ENEMY_MAX_BULLETS - 1) {
			Error("ENEMY_InitBullet function error", "Max number of bullets", "", ERROR_SYSTEM);
		}

		// Calculate target
		switch (accuracy) {
			case 0:
				enemy[enemy_number].gun.bullet[number].target_x = target_x - 4 - 8 + (rand() % 32);
				enemy[enemy_number].gun.bullet[number].target_y = target_y - 4 - 8 + (rand() % 32);
				break;
			case 1:
				enemy[enemy_number].gun.bullet[number].target_x = target_x - 4 + (rand() % 24);
				enemy[enemy_number].gun.bullet[number].target_y = target_y - 4 + (rand() % 24);
				break;
			case 2:
				enemy[enemy_number].gun.bullet[number].target_x = target_x - 8 + (rand() % 16);
				enemy[enemy_number].gun.bullet[number].target_y = target_y - 8 + (rand() % 16);
				break;
			case 3:
				enemy[enemy_number].gun.bullet[number].target_x = target_x + 4 + (rand() % 8);
				enemy[enemy_number].gun.bullet[number].target_y = target_y + 4 + (rand() % 8);
				break;
			case 4:
				enemy[enemy_number].gun.bullet[number].target_x = target_x + 4;
				enemy[enemy_number].gun.bullet[number].target_y = target_y + 4;
				break;
			default:
				enemy[enemy_number].gun.bullet[number].target_x = target_x + 4;
				enemy[enemy_number].gun.bullet[number].target_y = target_y + 4;
				break;
		}

		enemy[enemy_number].gun.bullet[number].pos_x = source_x;
		enemy[enemy_number].gun.bullet[number].pos_y = source_y;
		enemy[enemy_number].gun.bullet[number].speed = enemy[enemy_number].gun.bullet_speed;
		enemy[enemy_number].gun.bullet[number].damage = enemy[enemy_number].gun.damage;

		// calculate distance from source
		dx = enemy[enemy_number].gun.bullet[number].target_x - enemy[enemy_number].gun.bullet[number].pos_x;
		dy = enemy[enemy_number].gun.bullet[number].target_y - enemy[enemy_number].gun.bullet[number].pos_y;

		// calculate steps
		if (abs(dx) > abs(dy))
			//enemy[enemy_number].gun.bullet[number].steps = abs(dx);
			distance = abs(dx);
		else
			//enemy[enemy_number].gun.bullet[number].steps = abs(dy);
			distance = abs(dy);


		// calculate fixed point velocity and position
		enemy[enemy_number].gun.bullet[number].vx_FP = (dx << FP) / distance;
		enemy[enemy_number].gun.bullet[number].vy_FP = (dy << FP) / distance;

		enemy[enemy_number].gun.bullet[number].x_FP = enemy[enemy_number].gun.bullet[number].pos_x << FP;
		enemy[enemy_number].gun.bullet[number].y_FP = enemy[enemy_number].gun.bullet[number].pos_y << FP;

		enemy[enemy_number].gun.bullet[number].steps = distance;
		if (enemy[enemy_number].gun.bullet[number].steps > enemy[enemy_number].gun.max_distance)
			enemy[enemy_number].gun.bullet[number].steps = enemy[enemy_number].gun.max_distance;

		// Initialize bullet sprite

		if (!GFX_InitSprite(enemy[enemy_number].gun.bullet[number].graphics_id, ENTITY_ID_ENEMY_BULLET, number, &enemy[enemy_number].gun.bullet[number].sprite_num)) {
			sprintf(engine.system_error_message1, "ENEMY_InitBullet function error");
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
		enemy[enemy_number].gun.bullet[number].colission_area.up_points[0][0] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].width_px >> 4;
		enemy[enemy_number].gun.bullet[number].colission_area.up_points[0][1] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].height_px >> 4;
		// Point 2
		enemy[enemy_number].gun.bullet[number].colission_area.up_points[1][0] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].width_px - (gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].width_px >> 4);
		enemy[enemy_number].gun.bullet[number].colission_area.up_points[1][1] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].height_px >> 4;

		// Point 3
		enemy[enemy_number].gun.bullet[number].colission_area.down_points[0][0] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].width_px >> 4;
		enemy[enemy_number].gun.bullet[number].colission_area.down_points[0][1] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].height_px - (gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].height_px >> 4);

		// Point 4
		enemy[enemy_number].gun.bullet[number].colission_area.down_points[1][0] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].width_px - (gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].width_px >> 4);
		enemy[enemy_number].gun.bullet[number].colission_area.down_points[1][1] = gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].height_px - (gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].height_px >> 4);

		gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].animation_base_frame = 0;
		gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].animation_frames = 1;
		gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].animation_speed = 0;
		gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].current_frame = 0;

		gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].unmasked = true;

		gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].screen_pos_x = enemy[enemy_number].gun.bullet[number].pos_x - camera.pos_x;
		gfx_sprite_stack[enemy[enemy_number].gun.bullet[number].sprite_num].screen_pos_y = enemy[enemy_number].gun.bullet[number].pos_y - camera.pos_y;

		enemy[enemy_number].gun.bullet[number].loaded = true;
		enemy[enemy_number].gun.bullet[number].shown = true;
	}
}

int ENEMY_CheckBulletColission(Bullet bullet) {
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

	/////// ACTOR COLISSIONS ///////////
	//Point 1
	if (point1_x < actor.pos_x + actor.hit_area.points[1][0]

		&& point1_x > actor.pos_x + actor.hit_area.points[0][0]

		&& point1_y > actor.pos_y + actor.hit_area.points[0][1]

		&& point1_y < actor.pos_y + actor.hit_area.points[2][1]) {
		return gfx_sprite_stack[actor.sprite_num].id;
	}

	//Point 2
	if (point2_x < actor.pos_x + actor.hit_area.points[1][0]

		&& point2_x > actor.pos_x + actor.hit_area.points[0][0]

		&& point2_y > actor.pos_y + actor.hit_area.points[0][1]

		&& point2_y < actor.pos_y + actor.hit_area.points[2][1]) {
		return gfx_sprite_stack[actor.sprite_num].id;
	}

	//Point 3
	if (point3_x < actor.pos_x + actor.hit_area.points[1][0]

		&& point3_x > actor.pos_x + actor.hit_area.points[0][0]

		&& point3_y > actor.pos_y + actor.hit_area.points[0][1]

		&& point3_y < actor.pos_y + actor.hit_area.points[2][1]) {
		return gfx_sprite_stack[actor.sprite_num].id;
	}

	//Point 4
	if (point4_x < actor.pos_x + actor.hit_area.points[1][0]

		&& point4_x > actor.pos_x + actor.hit_area.points[0][0]

		&& point4_y > actor.pos_y + actor.hit_area.points[0][1]

		&& point4_y < actor.pos_y + actor.hit_area.points[2][1]) {
		return gfx_sprite_stack[actor.sprite_num].id;
	}
	return false;
}

/** ENEMY :: Update bullets	
 */
void ENEMY_UpdateBullets(int enemy_number) {
	int i, j;
	int obj_number;
	byte tile_type;
	int tile_number, tile_data, tileset_index;
	int tile_x, tile_y;
	int tile_screen_x, tile_screen_y;

	for (i = 0; i < ENEMY_MAX_BULLETS; i++) {
		if (enemy[enemy_number].gun.bullet[i].loaded) {

			// Not on target
			if (!enemy[enemy_number].gun.bullet[i].on_target) {
				// Calculate new pos

				for (j = 0; j < enemy[enemy_number].gun.bullet[i].speed; j++) {
					enemy[enemy_number].gun.bullet[i].x_FP += enemy[enemy_number].gun.bullet[i].vx_FP;
					enemy[enemy_number].gun.bullet[i].y_FP += enemy[enemy_number].gun.bullet[i].vy_FP;

					enemy[enemy_number].gun.bullet[i].pos_x = (enemy[enemy_number].gun.bullet[i].x_FP >> FP);
					enemy[enemy_number].gun.bullet[i].pos_y = (enemy[enemy_number].gun.bullet[i].y_FP >> FP);

					// Update sprite screen pos
					gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].screen_pos_x = enemy[enemy_number].gun.bullet[i].pos_x - camera.pos_x;
					gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].screen_pos_y = enemy[enemy_number].gun.bullet[i].pos_y - camera.pos_y;

					// Check if hits something
					enemy[enemy_number].gun.bullet[i].hit_on = ENEMY_CheckBulletColission(enemy[enemy_number].gun.bullet[i]);

					// Check hit on background, object or enemy
					switch (enemy[enemy_number].gun.bullet[i].hit_on & 0xFF) {
						case ENTITY_ID_BACKGROUND:
							tile_type = (enemy[enemy_number].gun.bullet[i].hit_on & 0xFF00) >> 8;
							switch (tile_type) {
								case 1:// full colission
									enemy[enemy_number].gun.bullet[i].on_target = true;
									break;
								case 2:// half colission
									break;
								case 3:// breaking tile. Set colission data to half colission
									tile_number = MAP_GetTileNumber(enemy[enemy_number].gun.bullet[i].pos_x, enemy[enemy_number].gun.bullet[i].pos_y);
									map.layer[MAP_COLISSION_LAYER][tile_number] = 2;
									// Get map tile position on screen
									tile_screen_x = ((enemy[enemy_number].gun.bullet[i].pos_x >> 4) << 4) - camera.pos_x;
									tile_screen_y = ((enemy[enemy_number].gun.bullet[i].pos_y >> 4) << 4) - camera.pos_y;
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

									PARTICLE_InitParticle(SPRITE_GRAPHICS_ID_ELECTRIC1, ((enemy[enemy_number].gun.bullet[i].pos_x >> 4) << 4) + 2, ((enemy[enemy_number].gun.bullet[i].pos_y >> 4) << 4) + 2, ((enemy[enemy_number].gun.bullet[i].pos_x >> 4) << 4) + 4, ((enemy[enemy_number].gun.bullet[i].pos_y >> 4) << 4) + 4, 1, 1);
									enemy[enemy_number].gun.bullet[i].on_target = true;
									break;
								default:
									break;
							}
							break;
						case ENTITY_ID_BARREL:
							enemy[enemy_number].gun.bullet[i].on_target = true;
							// Get object number
							obj_number = (enemy[enemy_number].gun.bullet[i].hit_on & 0xFF00) >> 8;
							object[obj_number].is_hit = true;
							object[obj_number].hit_by = ENTITY_ID_ACTOR_BULLET;
							object[obj_number].damage += enemy[enemy_number].gun.damage;
							break;
						case ENTITY_ID_ACTOR:
							enemy[enemy_number].gun.bullet[i].on_target = true;
							actor.is_hit = true;
							actor.damage += enemy[enemy_number].gun.damage;
							break;
						default:
							break;
					}

					enemy[enemy_number].gun.bullet[i].current_step++;
					if (enemy[enemy_number].gun.bullet[i].current_step >= enemy[enemy_number].gun.bullet[i].steps) {
						enemy[enemy_number].gun.bullet[i].on_target = true;
					}

					if (enemy[enemy_number].gun.bullet[i].on_target) {

						// update bullet graphic
						gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].animation_frames = 4;
						gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].current_frame = 1;
						gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].animation_counter = 0;
						gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].animation_speed = 1;
						gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].animation_end = false;

						break;
					}
				}
			}

			// On target
			if (enemy[enemy_number].gun.bullet[i].on_target) {

				// Update sprite screen pos
				gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].screen_pos_x = enemy[enemy_number].gun.bullet[i].pos_x - camera.pos_x;
				gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].screen_pos_y = enemy[enemy_number].gun.bullet[i].pos_y - camera.pos_y;

				if (gfx_sprite_stack[enemy[enemy_number].gun.bullet[i].sprite_num].animation_end) {
					enemy[enemy_number].gun.bullet[i].on_target = false;
					enemy[enemy_number].gun.bullet[i].shown = false;
					enemy[enemy_number].gun.bullet[i].current_step = 0;
					enemy[enemy_number].gun.bullet[i].loaded = false;
					GFX_UnloadSprite(enemy[enemy_number].gun.bullet[i].sprite_num);
				}
			}
		}
	}
}

void ENEMY_SetGun(int enemy_number, int type, int graphics_id, int bullet_graphics_id) {
	int i;

	// Set graphics id
	enemy[enemy_number].gun.graphics_id = graphics_id;
	for (i = 0; i < ENEMY_MAX_BULLETS; i++) {
		enemy[enemy_number].gun.bullet[i].graphics_id = bullet_graphics_id;
	}

	// Set gun type
	enemy[enemy_number].gun.type = type;
	switch (type) {
		case ENEMY_GUN_BARE_HANDS:
			enemy[enemy_number].gun.max_accuracy = 1;
			enemy[enemy_number].gun.damage = 1;
			enemy[enemy_number].gun.recoil_time = 5;
			enemy[enemy_number].gun.max_distance = 1;
			enemy[enemy_number].gun.accurate = false;
			enemy[enemy_number].gun.shoots = 1;
			enemy[enemy_number].gun.bullet_speed = 1;
			break;
		case ENEMY_GUN_PISTOL:
			enemy[enemy_number].gun.max_accuracy = 4;
			enemy[enemy_number].gun.damage = 2;
			enemy[enemy_number].gun.recoil_time = 4;
			enemy[enemy_number].gun.max_distance = 200;
			enemy[enemy_number].gun.accurate = true;
			enemy[enemy_number].gun.shoots = 1;
			enemy[enemy_number].gun.bullet_speed = 4;
			break;
		case ENEMY_GUN_RIFLE:
			enemy[enemy_number].gun.max_accuracy = 2;
			enemy[enemy_number].gun.damage = 5;
			enemy[enemy_number].gun.recoil_time = 10;
			enemy[enemy_number].gun.max_distance = 300;
			enemy[enemy_number].gun.accurate = true;
			enemy[enemy_number].gun.shoots = 1;
			enemy[enemy_number].gun.bullet_speed = 15;
			break;
		case ENEMY_GUN_AK:
			enemy[enemy_number].gun.max_accuracy = 1;
			enemy[enemy_number].gun.damage = 5;
			enemy[enemy_number].gun.recoil_time = 1;
			enemy[enemy_number].gun.max_distance = 250;
			enemy[enemy_number].gun.accurate = false;
			enemy[enemy_number].gun.shoots = 1;
			enemy[enemy_number].gun.bullet_speed = 20;
			break;
		case ENEMY_GUN_SHOTGUN:
			enemy[enemy_number].gun.max_accuracy = 1;
			enemy[enemy_number].gun.damage = 6;
			enemy[enemy_number].gun.recoil_time = 15;
			enemy[enemy_number].gun.max_distance = 100;
			enemy[enemy_number].gun.accurate = false;
			enemy[enemy_number].gun.shoots = 3;
			enemy[enemy_number].gun.bullet_speed = 15;
			break;
		case ENEMY_GUN_MISILE:
			enemy[enemy_number].gun.max_accuracy = 1;
			enemy[enemy_number].gun.damage = 10;
			enemy[enemy_number].gun.recoil_time = 20;
			enemy[enemy_number].gun.max_distance = 300;
			enemy[enemy_number].gun.accurate = true;
			enemy[enemy_number].gun.shoots = 1;
			enemy[enemy_number].gun.bullet_speed = 10;
			break;
		default:
			sprintf(engine.system_error_message1, "enemy_SetGun function error");
			sprintf(engine.system_error_message2, "Undefined gun type %u ", type);
			sprintf(engine.system_error_message3, " ");
			Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_SYSTEM);
			break;
	}
}

/** ENEMY :: Initializes enemy data
 * - Sets default values for the enemy structure 
 */
void ENEMY_Init(byte number, int x, int y, int facing, int graphics_id, int gun_type, int gun_graphics_id, int bullet_graphics_id) {

	enemy[number].is_loaded = true;

	enemy[number].pos_x = x;
	enemy[number].pos_y = y;
	enemy[number].x_FP = x << FP;
	enemy[number].y_FP = y << FP;
	enemy[number].vx_FP = 0;
	enemy[number].vy_FP = 0;

	enemy[number].status_facing = facing;

	enemy[number].speed_horizontal_FP = ACTOR_SPEED << FP;
	enemy[number].speed_vertical_FP = (ACTOR_SPEED << FP) - ((ACTOR_SPEED << FP) >> 3);
	enemy[number].speed_diagonalv_FP = (enemy[number].speed_vertical_FP * 10) / 14;
	enemy[number].speed_diagonalh_FP = (enemy[number].speed_horizontal_FP * 10) / 14;

	enemy[number].speed = ACTOR_SPEED;
	enemy[number].max_life = ACTOR_LIFE;
	enemy[number].life = ACTOR_LIFE;
	enemy[number].damage = 0;
	enemy[number].is_hit = false;

	enemy[number].movement_allowed = true;
	enemy[number].shoot_range = 100;
	enemy[number].reaction_time = 70;

	enemy[number].status_shoot = false;
	enemy[number].status_stand = false;
	enemy[number].status_walk = false;

	// Initialize enemy sprite
	if (!GFX_InitSprite(graphics_id, ENTITY_ID_ENEMY, number, &enemy[number].sprite_num)) {
		sprintf(engine.system_error_message1, "ENEMY_Init function error");
		sprintf(engine.system_error_message2, "Cannot initialize enemy sprite");
		sprintf(engine.system_error_message3, " ");
		Error(engine.system_error_message1, engine.system_error_message2, engine.system_error_message3, ERROR_GRAPHICS);
	}

	// Set colission points
	//  ----------------
	// |                |
	// |                |
	// |                |
	// |  1          2  |
	// |                |
	// |                |
	// |                |
	// |  3          4  |
	//  ----------------
	// Point 1
	enemy[number].colission_area.up_points[0][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3;
	enemy[number].colission_area.up_points[0][1] = (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 1) - (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 4);
	// Point 2
	enemy[number].colission_area.up_points[1][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px - (gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3);
	enemy[number].colission_area.up_points[1][1] = (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 1) - (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 4);

	// Point 3
	enemy[number].colission_area.down_points[0][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3;
	enemy[number].colission_area.down_points[0][1] = gfx_sprite_stack[enemy[number].sprite_num].height_px;
	// Point 4
	enemy[number].colission_area.down_points[1][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px - (gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3);
	enemy[number].colission_area.down_points[1][1] = gfx_sprite_stack[enemy[number].sprite_num].height_px;

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
	enemy[number].hit_area.points[0][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3;
	enemy[number].hit_area.points[0][1] = (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 1) - (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 3);
	// Point 2
	enemy[number].hit_area.points[1][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px - (gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3);
	enemy[number].hit_area.points[1][1] = (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 1) - (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 3);
	// Point 3
	enemy[number].hit_area.points[2][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3;
	enemy[number].hit_area.points[2][1] = gfx_sprite_stack[enemy[number].sprite_num].height_px - (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 4);
	// Point 4
	enemy[number].hit_area.points[3][0] = gfx_sprite_stack[enemy[number].sprite_num].width_px - (gfx_sprite_stack[enemy[number].sprite_num].width_px >> 3);
	enemy[number].hit_area.points[3][1] = gfx_sprite_stack[enemy[number].sprite_num].height_px - (gfx_sprite_stack[enemy[number].sprite_num].height_px >> 4);

	gfx_sprite_stack[enemy[number].sprite_num].animation_base_frame = 12;
	gfx_sprite_stack[enemy[number].sprite_num].animation_frames = 1;
	gfx_sprite_stack[enemy[number].sprite_num].animation_speed = ENEMY_ANIMATION_SPEED;
	gfx_sprite_stack[enemy[number].sprite_num].current_frame = 1;

	gfx_sprite_stack[enemy[number].sprite_num].screen_pos_x = enemy[number].pos_x - camera.pos_x;
	gfx_sprite_stack[enemy[number].sprite_num].screen_pos_y = enemy[number].pos_y - camera.pos_y;

	// Initialize gun
	ENEMY_SetGun(number, gun_type, gun_graphics_id, bullet_graphics_id);
}

void ENEMY_DrawColissionPixels(Enemy e) {
	//  Draw colission pixels
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.colission_area.up_points[0][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.colission_area.up_points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.colission_area.up_points[1][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.colission_area.up_points[1][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.colission_area.down_points[0][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.colission_area.down_points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.colission_area.down_points[1][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.colission_area.down_points[1][1], 229);
}

void ENEMY_DrawHitPixels(Enemy e) {
	//  Draw colission pixels
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.hit_area.points[0][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.hit_area.points[0][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.hit_area.points[1][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.hit_area.points[1][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.hit_area.points[2][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.hit_area.points[2][1], 229);
	VIDEO_DrawPixelOnScreenBuffer(gfx_sprite_stack[e.sprite_num].screen_pos_x + e.hit_area.points[3][0], gfx_sprite_stack[e.sprite_num].screen_pos_y + e.hit_area.points[3][1], 229);
}

bool ENEMY_CheckUpColission(Enemy e) {
	int i;
	int point1_x, point1_y;
	int point2_x, point2_y;

	point1_x = e.pos_x + e.colission_area.up_points[0][0];
	point1_y = e.pos_y + e.colission_area.up_points[0][1];
	point2_x = e.pos_x + e.colission_area.up_points[1][0];
	point2_y = e.pos_y + e.colission_area.up_points[1][1];

	/////// BACKGROUND COLISSIONS ///////////
	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown) {
				//Point 1
				if (point1_x <= object[i].pos_x + object[i].colission_area.down_points[1][0]

					&& point1_x >= object[i].pos_x + object[i].colission_area.down_points[0][0]

					&& point1_y >= object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point1_y <= object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x <= object[i].pos_x + object[i].colission_area.down_points[1][0]

					&& point2_x >= object[i].pos_x + object[i].colission_area.down_points[0][0]

					&& point2_y >= object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point2_y <= object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	/////// ENEMIES COLISSIONS ///////////
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown && (enemy[i].sprite_num != e.sprite_num)) {
				//Point 1
				if (point1_x <= enemy[i].pos_x + enemy[i].colission_area.down_points[1][0]

					&& point1_x >= enemy[i].pos_x + enemy[i].colission_area.down_points[0][0]

					&& point1_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point1_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x <= enemy[i].pos_x + enemy[i].colission_area.down_points[1][0]

					&& point2_x >= enemy[i].pos_x + enemy[i].colission_area.down_points[0][0]

					&& point2_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point2_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

bool ENEMY_CheckDownColission(Enemy e) {
	int i;

	int point1_x, point1_y;
	int point2_x, point2_y;

	point1_x = e.pos_x + e.colission_area.down_points[0][0];
	point1_y = e.pos_y + e.colission_area.down_points[0][1];
	point2_x = e.pos_x + e.colission_area.down_points[1][0];
	point2_y = e.pos_y + e.colission_area.down_points[1][1];

	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x <= object[i].pos_x + object[i].colission_area.up_points[1][0]

					&& point1_x >= object[i].pos_x + object[i].colission_area.up_points[0][0]

					&& point1_y >= object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point1_y <= object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x <= object[i].pos_x + object[i].colission_area.up_points[1][0]

					&& point2_x >= object[i].pos_x + object[i].colission_area.up_points[0][0]

					&& point2_y >= object[i].pos_y + object[i].colission_area.up_points[1][1]

					&& point2_y <= object[i].pos_y + object[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown && (enemy[i].sprite_num != e.sprite_num)) {
				//Point 1
				if (point1_x <= enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point1_x >= enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point1_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point1_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x <= enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point2_x >= enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point2_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[1][1]

					&& point2_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

bool ENEMY_CheckLeftColission(Enemy e) {
	int i;

	int point1_x, point1_y;
	int point2_x, point2_y;

	point1_x = e.pos_x + e.colission_area.up_points[0][0];
	point1_y = e.pos_y + e.colission_area.up_points[0][1];
	point2_x = e.pos_x + e.colission_area.down_points[0][0];
	point2_y = e.pos_y + e.colission_area.down_points[0][1];

	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x <= object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point1_x >= object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point1_y >= object[i].pos_y + object[i].colission_area.right_points[0][1]

					&& point1_y <= object[i].pos_y + object[i].colission_area.right_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x <= object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point2_x >= object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point2_y >= object[i].pos_y + object[i].colission_area.right_points[0][1]

					&& point2_y <= object[i].pos_y + object[i].colission_area.right_points[1][1]) {
					return true;
				}
			}
		}
	}
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown && (enemy[i].sprite_num != e.sprite_num)) {
				//Point 1
				if (point1_x <= enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point1_x >= enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point1_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[0][1]

					&& point1_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x <= enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point2_x >= enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point2_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[0][1]

					&& point2_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

bool ENEMY_CheckRightColission(Enemy e) {
	int i;

	int point1_x, point1_y;
	int point2_x, point2_y;

	point1_x = e.pos_x + e.colission_area.up_points[1][0];
	point1_y = e.pos_y + e.colission_area.up_points[1][1];
	point2_x = e.pos_x + e.colission_area.down_points[1][0];
	point2_y = e.pos_y + e.colission_area.down_points[1][1];

	if (MAP_CheckColissionTile(point1_x, point1_y)) { return true; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y)) { return true; }// Check point 2

	/////// OBJECTS COLISSIONS ///////////
	for (i = 0; i < OBJECT_MAX_OBJECTS; i++) {
		if (object[i].is_loaded) {
			if (gfx_sprite_stack[object[i].num_sprite].shown) {
				//Point 1
				if (point1_x <= object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point1_x >= object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point1_y >= object[i].pos_y + object[i].colission_area.left_points[0][1]

					&& point1_y <= object[i].pos_y + object[i].colission_area.left_points[1][1]) {
					return true;
				}

				//Point 1
				if (point2_x <= object[i].pos_x + object[i].colission_area.right_points[1][0]

					&& point2_x >= object[i].pos_x + object[i].colission_area.left_points[1][0]

					&& point2_y >= object[i].pos_y + object[i].colission_area.left_points[0][1]

					&& point2_y <= object[i].pos_y + object[i].colission_area.left_points[1][1]) {
					return true;
				}
			}
		}
	}

	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {
			if (gfx_sprite_stack[enemy[i].sprite_num].shown && (enemy[i].sprite_num != e.sprite_num)) {
				//Point 1
				if (point1_x <= enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point1_x >= enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point1_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[0][1]

					&& point1_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}

				//Point 2
				if (point2_x <= enemy[i].pos_x + enemy[i].colission_area.up_points[1][0]

					&& point2_x >= enemy[i].pos_x + enemy[i].colission_area.up_points[0][0]

					&& point2_y >= enemy[i].pos_y + enemy[i].colission_area.up_points[0][1]

					&& point2_y <= enemy[i].pos_y + enemy[i].colission_area.down_points[1][1]) {
					return true;
				}
			}
		}
	}

	return false;
}

/** ENEMY :: In game update
 */
void ENEMY_Update(void) {
	bool status_idle;
	byte movement;
	int axis_x, axis_y, axis_relation;
	int i;

	// Calculate facing direction with enemy and actor positions
	for (i = 0; i < ENEMY_MAX_ENEMIES; i++) {
		if (enemy[i].is_loaded) {

			// Set enemy warning after spots actor
			if (gfx_sprite_stack[enemy[i].sprite_num].shown & !enemy[i].status_warning) {
				switch (enemy[i].status_facing) {
					case 1:// Right
						if (actor.pos_x > enemy[i].pos_x) enemy[i].status_warning = true;
						break;
					case 2:// Left
						if (actor.pos_x < enemy[i].pos_x) enemy[i].status_warning = true;
						break;
					case 4:// Down
						if (actor.pos_y > enemy[i].pos_y) enemy[i].status_warning = true;
						break;
					case 5:// Down-right
						if ((actor.pos_x > enemy[i].pos_x) && (actor.pos_y > (enemy[i].pos_y - 100))) enemy[i].status_warning = true;
						break;
					case 6:// Down-Left
						if ((actor.pos_x < enemy[i].pos_x) && (actor.pos_y > (enemy[i].pos_y - 100))) enemy[i].status_warning = true;
						break;
					case 8:// Up
						if (actor.pos_y < enemy[i].pos_y) enemy[i].status_warning = true;
						break;
					case 9:// Up-right
						if ((actor.pos_x > enemy[i].pos_x) && (actor.pos_y < (enemy[i].pos_y + 100))) enemy[i].status_warning = true;
						break;
					case 10:// Up-left
						if ((actor.pos_x < enemy[i].pos_x) && (actor.pos_y < (enemy[i].pos_y + 100))) enemy[i].status_warning = true;
						break;
					default:
						break;
				}
			}

			// Set enemy warning if actor shoots
			if (gfx_sprite_stack[enemy[i].sprite_num].shown & !enemy[i].status_warning) {
				if (actor.status_shoot) {
					enemy[i].status_warning = true;
				}
			}

			// Enemy is warned, face actor
			if (enemy[i].status_warning) {
				enemy[i].status_facing = 0;
				if (gfx_sprite_stack[actor.sprite_num].screen_pos_x <= gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x) {// facing left
					if (gfx_sprite_stack[actor.sprite_num].screen_pos_y <= gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y) {
						axis_x = gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x - gfx_sprite_stack[actor.sprite_num].screen_pos_x + 100;
						axis_y = gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y - gfx_sprite_stack[actor.sprite_num].screen_pos_y + 100;
						axis_relation = axis_x / axis_y;

						if (axis_relation > 1) {// facing left
							enemy[i].status_facing = enemy[i].status_facing | (1 << 1);
						}// facing left
						if (axis_relation == 1) {
							enemy[i].status_facing = enemy[i].status_facing | (1 << 1);// facing left
							enemy[i].status_facing = enemy[i].status_facing | (1 << 3);// facing up
						}
						if (axis_relation < 1) {// facing up
							enemy[i].status_facing = enemy[i].status_facing | (1 << 3);
						}// up
					} else {
						axis_x = gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x - gfx_sprite_stack[actor.sprite_num].screen_pos_x + 100;
						axis_y = gfx_sprite_stack[actor.sprite_num].screen_pos_y - gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y + 100;
						axis_relation = axis_x / axis_y;

						if (axis_relation > 1) {// facing left
							enemy[i].status_facing = enemy[i].status_facing | (1 << 1);
						}
						if (axis_relation == 1) {
							enemy[i].status_facing = enemy[i].status_facing | (1 << 1);// facing left
							enemy[i].status_facing = enemy[i].status_facing | (1 << 2);// facing down
						}
						if (axis_relation < 1) {
							enemy[i].status_facing = enemy[i].status_facing | (1 << 2);// facing down
						}
					}
				} else {//facing right
					if (gfx_sprite_stack[actor.sprite_num].screen_pos_y <= gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y) {
						axis_x = gfx_sprite_stack[actor.sprite_num].screen_pos_x - gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x + 1;
						axis_y = gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y - gfx_sprite_stack[actor.sprite_num].screen_pos_y + 1;
						axis_relation = axis_x / axis_y;

						if (axis_relation > 1) { enemy[i].status_facing = enemy[i].status_facing | 1; }// facing right
						if (axis_relation == 1) {
							enemy[i].status_facing = enemy[i].status_facing | 1;       // facing right
							enemy[i].status_facing = enemy[i].status_facing | (1 << 3);// facing up
						}
						if (axis_relation < 1) { enemy[i].status_facing = enemy[i].status_facing | (1 << 3); }// up
					} else {
						axis_x = gfx_sprite_stack[actor.sprite_num].screen_pos_x - gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x + 1;
						axis_y = gfx_sprite_stack[actor.sprite_num].screen_pos_y - gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y + 1;
						axis_relation = axis_x / axis_y;

						if (axis_relation > 1) { enemy[i].status_facing = enemy[i].status_facing | 1; }// facing right
						if (axis_relation == 1) {
							enemy[i].status_facing = enemy[i].status_facing | 1;       // facing right
							enemy[i].status_facing = enemy[i].status_facing | (1 << 2);// facing down
						}
						if (axis_relation < 1) { enemy[i].status_facing = enemy[i].status_facing | (1 << 2); }// down
					}
				}
			}

			// Wait reaction time before get alarmed
			if (enemy[i].status_warning && !enemy[i].status_alarm) {
				unsigned char chr;

				enemy[i].reaction_counter++;
				if (enemy[i].reaction_counter > enemy[i].reaction_time) {
					enemy[i].status_alarm = true;
				}

				chr = '!';
				VIDEO_StringToScreenBuffer(gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x + 16, gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y - 8, 1, &chr);
			}

			// Enemy is alarmed, follow actor and shoot when on range
			if (enemy[i].status_alarm) {
				// Follow actor
				movement = 0;
				if (enemy[i].movement_allowed) {
					if (gfx_sprite_stack[actor.sprite_num].screen_pos_y < (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y - enemy[i].shoot_range)) movement = movement | (1 << 3);// up
					if (gfx_sprite_stack[actor.sprite_num].screen_pos_y > (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y + enemy[i].shoot_range)) movement = movement | (1 << 2);// down
					if (gfx_sprite_stack[actor.sprite_num].screen_pos_x < (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x - enemy[i].shoot_range)) movement = movement | (1 << 1);// left
					if (gfx_sprite_stack[actor.sprite_num].screen_pos_x > (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x + enemy[i].shoot_range)) movement = movement | (1);     // right
				}

				// In shoot range
				if ((gfx_sprite_stack[actor.sprite_num].screen_pos_y >= (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y - enemy[i].shoot_range))

					&& (gfx_sprite_stack[actor.sprite_num].screen_pos_y <= (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y + enemy[i].shoot_range))

					&& (gfx_sprite_stack[actor.sprite_num].screen_pos_x >= (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x - enemy[i].shoot_range))

					&& (gfx_sprite_stack[actor.sprite_num].screen_pos_x <= (gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x + enemy[i].shoot_range))) {
					enemy[i].in_shoot_range = true;
				} else {
					enemy[i].in_shoot_range = false;
				}
			}

			// Check idle status
			status_idle = !enemy[i].status_dead & !enemy[i].status_shoot;

			//// ---------------- INITIALIZE ACTION ----------------

			// shoot
			if (status_idle && enemy[i].in_shoot_range) {
				status_idle = false;
				enemy[i].status_shoot = true;
				enemy[i].status_walk = false;
				enemy[i].status_stand = false;

				enemy[i].movement = 0;
				enemy[i].action_step = 0;
				enemy[i].last_action = ACTOR_ACTION_SHOOT;

				gfx_sprite_stack[enemy[i].sprite_num].animation_speed = ACTOR_ANIMATION_SPEED;
				gfx_sprite_stack[enemy[i].sprite_num].animation_frames = 1;
				gfx_sprite_stack[enemy[i].sprite_num].current_frame = 1;
				gfx_sprite_stack[enemy[i].sprite_num].animation_end = false;
				gfx_sprite_stack[enemy[i].sprite_num].animation_loop = false;

				switch (enemy[i].status_facing) {
					case 1:// facing right
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 12;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 2:// facing left
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 12;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						break;
					case 4:// facing down
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 24;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 5:// facing down-right
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 18;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 6:// facing down-left
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 18;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						break;
					case 8:// facing up
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 0;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 9:// facing up-right
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 6;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 10:// facing up-left
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 6;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						break;
					default:
						break;
				}
			}

			// walk
			if (status_idle & (movement != 0)) {
				enemy[i].status_walk = true;
				enemy[i].status_shoot = false;
				enemy[i].status_stand = false;

				enemy[i].movement = movement;
				enemy[i].action_step = 0;
				enemy[i].last_action = ACTOR_ACTION_WALK;

				gfx_sprite_stack[enemy[i].sprite_num].animation_speed = ACTOR_ANIMATION_SPEED;
				gfx_sprite_stack[enemy[i].sprite_num].animation_frames = 4;
				gfx_sprite_stack[enemy[i].sprite_num].animation_loop = true;
				gfx_sprite_stack[enemy[i].sprite_num].animation_end = false;

				switch (enemy[i].status_facing) {
					case 1:// facing right
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 13;
						break;
					case 2:// facing left
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 13;
						break;
					case 4:// moving down
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 25;
						break;
					case 5:// moving down-right
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 19;
						break;
					case 6:// moving down-left
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 19;
						break;
					case 8:// moving up
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 1;
						break;
					case 9:// moving up-right
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 7;
						break;
					case 10:// moving up-left
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 7;
						break;
					default:
						break;
				}
			}

			// no action
			if (status_idle & (movement == 0)) {
				enemy[i].movement = 0;
				enemy[i].action_step = 0;

				if (!enemy[i].status_stand) {
					enemy[i].idle_counter++;
					if (enemy[i].idle_counter > ACTOR_IDLE_TICKS) {
						enemy[i].status_stand = true;
						enemy[i].last_action = 0;
						enemy[i].idle_counter = 0;
					}
				}

				switch (enemy[i].status_facing) {
					case 1:// facing right
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 12;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 2:// facing left
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 12;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						break;
					case 4:// facing down
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 25;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 5:// facing down-right
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 20;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 6:// facing down-left
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 20;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						break;
					case 8:// facing up
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 0;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 9:// facing up-right
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 6;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
						break;
					case 10:// facing up-left
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 6;
						gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = true;
						break;
					default:
						break;
				}

				gfx_sprite_stack[enemy[i].sprite_num].animation_frames = 1;
				gfx_sprite_stack[enemy[i].sprite_num].current_frame = 1;
				gfx_sprite_stack[enemy[i].sprite_num].animation_speed = ACTOR_ANIMATION_SPEED;
				gfx_sprite_stack[enemy[i].sprite_num].animation_loop = false;
			}

			// Check if is hit
			if ((!enemy[i].status_dead) && enemy[i].is_hit) {
				enemy[i].is_hit = false;

				status_idle = false;
				movement = 0;
				enemy[i].status_shoot = false;
				enemy[i].status_walk = false;

				gfx_sprite_stack[enemy[i].sprite_num].blink = true;
				if (enemy[i].damage > enemy[i].life) enemy[i].damage = enemy[i].life;
				enemy[i].life -= enemy[i].damage;
				// Check if hit by actor bullet and update enemy panel
				switch (enemy[i].hit_by) {
					case ENTITY_ID_ACTOR_BULLET:
						GFX_SetPanelGraphics(&gfx_enemy_status_panel, SPRITE_GRAPHICS_ID_ENEMY1_PORTAIT, SPRITE_GRAPHICS_ID_LIFEBAR);
						GFX_UpdatePanel(&gfx_enemy_status_panel, enemy[i].life, enemy[i].life + enemy[i].damage, enemy[i].max_life, 1);
						gfx_enemy_status_panel.shown = true;
						break;
					default:
						break;
				}

				if (enemy[i].life <= 0) {
					enemy[i].life = 0;

					enemy[i].status_dead = true;
					enemy[i].status_shoot = false;
					enemy[i].status_walk = false;
					enemy[i].status_stand = false;

					enemy[i].movement = 0;
					enemy[i].shoot_accuracy = 0;
					enemy[i].action_step = 0;
					enemy[i].last_action = 0;

					gfx_sprite_stack[enemy[i].sprite_num].animation_speed = 20;
					gfx_sprite_stack[enemy[i].sprite_num].animation_frames = 4;
					gfx_sprite_stack[enemy[i].sprite_num].current_frame = 0;

					gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame = 49;
					gfx_sprite_stack[enemy[i].sprite_num].animation_inverted = false;
					gfx_sprite_stack[enemy[i].sprite_num].animation_loop = false;
					gfx_sprite_stack[enemy[i].sprite_num].animation_end = false;

					GFX_SetPanelPortait(&gfx_enemy_status_panel, 2);
				}
				//actor.life_shown = true;
				enemy[i].damage = 0;

			} else {
				gfx_sprite_stack[enemy[i].sprite_num].blink = false;
			}
			//// ---------------- POSITIONING ----------------

			// Shooting and animation
			if (enemy[i].status_shoot) {
				switch (enemy[i].action_step) {
					case 0:// Calculate target
						enemy[i].shoot_x = actor.pos_x;
						enemy[i].shoot_y = actor.pos_y;
						enemy[i].gun.current_recoil = 0;
						enemy[i].action_step++;
						break;
					case 1:// Shoot
						gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame += 5;
						AUDIO_PlaySound(AUDIO_GUN_EFFECT);
						switch (enemy[i].status_facing) {
							case 1:// facing right
								ENEMY_InitBullet(i, enemy[i].pos_x + 32, enemy[i].pos_y + 8, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							case 2:// facing left
								ENEMY_InitBullet(i, enemy[i].pos_x, enemy[i].pos_y + 8, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							case 4:// facing down
								ENEMY_InitBullet(i, enemy[i].pos_x + 16, enemy[i].pos_y + 20, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							case 5:// moving down-right
								ENEMY_InitBullet(i, enemy[i].pos_x + 18, enemy[i].pos_y + 16, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							case 6:// moving down-left
								ENEMY_InitBullet(i, enemy[i].pos_x + 6, enemy[i].pos_y + 16, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							case 8:// moving up
								ENEMY_InitBullet(i, enemy[i].pos_x + 16, enemy[i].pos_y - 8, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							case 9:// moving up-right
								ENEMY_InitBullet(i, enemy[i].pos_x + 25, enemy[i].pos_y - 1, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							case 10:// moving up-left
								ENEMY_InitBullet(i, enemy[i].pos_x + 6, enemy[i].pos_y - 1, enemy[i].shoot_x, enemy[i].shoot_y, enemy[i].shoot_accuracy);
								break;
							default:
								break;
						}
						enemy[i].action_step++;
						break;
					case 2://
						enemy[i].gun.current_recoil++;
						if (enemy[i].gun.current_recoil >= enemy[i].gun.recoil_time) {
							enemy[i].gun.current_recoil = 0;
							gfx_sprite_stack[enemy[i].sprite_num].animation_base_frame -= 5;
							enemy[i].action_step++;
						}
						break;
					case 3:// end
						enemy[i].action_step = 0;
						enemy[i].status_shoot = false;
						enemy[i].action_step++;
						break;
					default:
						enemy[i].action_step = 0;
						enemy[i].status_shoot = false;
						break;
				}
			}

			// Walking and animation
			if (enemy[i].status_walk) {
				switch (enemy[i].movement) {
					case 1:// moving right
						enemy[i].x_FP += enemy[i].speed_horizontal_FP;
						enemy[i].pos_x = enemy[i].x_FP >> FP;
						if (ENEMY_CheckRightColission(enemy[i])) {
							enemy[i].x_FP -= enemy[i].speed_horizontal_FP;
							enemy[i].pos_x = enemy[i].x_FP >> FP;
							if (actor.pos_y < enemy[i].pos_y) {
								enemy[i].y_FP -= enemy[i].speed_vertical_FP;
								enemy[i].pos_y = enemy[i].y_FP >> FP;
								if (ENEMY_CheckUpColission(enemy[i])) {
									enemy[i].y_FP += enemy[i].speed_vertical_FP;
									enemy[i].pos_y = enemy[i].y_FP >> FP;
								}
							} else {
								enemy[i].y_FP += enemy[i].speed_vertical_FP;
								enemy[i].pos_y = enemy[i].y_FP >> FP;
								if (ENEMY_CheckDownColission(enemy[i])) {
									enemy[i].y_FP -= enemy[i].speed_vertical_FP;
									enemy[i].pos_y = enemy[i].y_FP >> FP;
								}
							}
						}
						break;
					case 2:// moving left
						enemy[i].x_FP -= enemy[i].speed_horizontal_FP;
						enemy[i].pos_x = enemy[i].x_FP >> FP;
						if (ENEMY_CheckLeftColission(enemy[i])) {
							enemy[i].x_FP += enemy[i].speed_horizontal_FP;
							enemy[i].pos_x = enemy[i].x_FP >> FP;
							if (actor.pos_y < enemy[i].pos_y) {
								enemy[i].y_FP -= enemy[i].speed_vertical_FP;
								enemy[i].pos_y = enemy[i].y_FP >> FP;
								if (ENEMY_CheckUpColission(enemy[i])) {
									enemy[i].y_FP += enemy[i].speed_vertical_FP;
									enemy[i].pos_y = enemy[i].y_FP >> FP;
								}
							} else {
								enemy[i].y_FP += enemy[i].speed_vertical_FP;
								enemy[i].pos_y = enemy[i].y_FP >> FP;
								if (ENEMY_CheckDownColission(enemy[i])) {
									enemy[i].y_FP -= enemy[i].speed_vertical_FP;
									enemy[i].pos_y = enemy[i].y_FP >> FP;
								}
							}
						}
						break;
					case 4:// moving down
						enemy[i].y_FP += enemy[i].speed_vertical_FP;
						enemy[i].pos_y = enemy[i].y_FP >> FP;
						if (ENEMY_CheckDownColission(enemy[i])) {
							enemy[i].y_FP -= enemy[i].speed_vertical_FP;
							enemy[i].pos_y = enemy[i].y_FP >> FP;
							if (actor.pos_x < enemy[i].pos_x) {
								enemy[i].x_FP -= enemy[i].speed_horizontal_FP;
								enemy[i].pos_x = enemy[i].x_FP >> FP;
								if (ENEMY_CheckLeftColission(enemy[i])) {
									enemy[i].x_FP += enemy[i].speed_horizontal_FP;
									enemy[i].pos_x = enemy[i].x_FP >> FP;
								}
							} else {
								enemy[i].x_FP += enemy[i].speed_horizontal_FP;
								enemy[i].pos_x = enemy[i].x_FP >> FP;
								if (ENEMY_CheckRightColission(enemy[i])) {
									enemy[i].x_FP -= enemy[i].speed_horizontal_FP;
									enemy[i].pos_x = enemy[i].x_FP >> FP;
								}
							}
						}
						break;
					case 5:// moving down-right
						enemy[i].x_FP += enemy[i].speed_diagonalh_FP;
						enemy[i].y_FP += enemy[i].speed_diagonalv_FP;
						enemy[i].pos_x = enemy[i].x_FP >> FP;
						enemy[i].pos_y = enemy[i].y_FP >> FP;
						if (ENEMY_CheckDownColission(enemy[i])) {
							enemy[i].y_FP -= enemy[i].speed_diagonalv_FP;
							enemy[i].pos_y = enemy[i].y_FP >> FP;
						}
						if (ENEMY_CheckRightColission(enemy[i])) {
							enemy[i].x_FP -= enemy[i].speed_diagonalh_FP;
							enemy[i].pos_x = enemy[i].x_FP >> FP;
						}
						break;
					case 6:// moving down-left
						enemy[i].x_FP -= enemy[i].speed_diagonalh_FP;
						enemy[i].y_FP += enemy[i].speed_diagonalv_FP;
						enemy[i].pos_x = enemy[i].x_FP >> FP;
						enemy[i].pos_y = enemy[i].y_FP >> FP;
						if (ENEMY_CheckDownColission(enemy[i])) {
							enemy[i].y_FP -= enemy[i].speed_diagonalv_FP;
							enemy[i].pos_y = enemy[i].y_FP >> FP;
						}
						if (ENEMY_CheckLeftColission(enemy[i])) {
							enemy[i].x_FP += enemy[i].speed_diagonalh_FP;
							enemy[i].pos_x = enemy[i].x_FP >> FP;
						}
						break;
					case 8:// moving up
						enemy[i].y_FP -= enemy[i].speed_vertical_FP;
						enemy[i].pos_y = enemy[i].y_FP >> FP;
						if (ENEMY_CheckUpColission(enemy[i])) {
							enemy[i].y_FP += enemy[i].speed_vertical_FP;
							enemy[i].pos_y = enemy[i].y_FP >> FP;
							if (actor.pos_x < enemy[i].pos_x) {
								enemy[i].x_FP -= enemy[i].speed_horizontal_FP;
								enemy[i].pos_x = enemy[i].x_FP >> FP;
								if (ENEMY_CheckLeftColission(enemy[i])) {
									enemy[i].x_FP += enemy[i].speed_horizontal_FP;
									enemy[i].pos_x = enemy[i].x_FP >> FP;
								}
							} else {
								enemy[i].x_FP += enemy[i].speed_horizontal_FP;
								enemy[i].pos_x = enemy[i].x_FP >> FP;
								if (ENEMY_CheckRightColission(enemy[i])) {
									enemy[i].x_FP -= enemy[i].speed_horizontal_FP;
									enemy[i].pos_x = enemy[i].x_FP >> FP;
								}
							}
						}
						break;
					case 9:// moving up-right
						enemy[i].x_FP += enemy[i].speed_diagonalh_FP;
						enemy[i].y_FP -= enemy[i].speed_diagonalv_FP;
						enemy[i].pos_x = enemy[i].x_FP >> FP;
						enemy[i].pos_y = enemy[i].y_FP >> FP;
						if (ENEMY_CheckUpColission(enemy[i])) {
							enemy[i].y_FP += enemy[i].speed_diagonalv_FP;
							enemy[i].pos_y = enemy[i].y_FP >> FP;
						}
						if (ENEMY_CheckRightColission(enemy[i])) {
							enemy[i].x_FP -= enemy[i].speed_diagonalh_FP;
							enemy[i].pos_x = enemy[i].x_FP >> FP;
						}
						break;
					case 10:// moving up-left
						enemy[i].x_FP -= enemy[i].speed_diagonalh_FP;
						enemy[i].y_FP -= enemy[i].speed_diagonalv_FP;
						enemy[i].pos_x = enemy[i].x_FP >> FP;
						enemy[i].pos_y = enemy[i].y_FP >> FP;
						if (ENEMY_CheckUpColission(enemy[i])) {
							enemy[i].y_FP += enemy[i].speed_diagonalv_FP;
							enemy[i].pos_y = enemy[i].y_FP >> FP;
						}
						if (ENEMY_CheckLeftColission(enemy[i])) {
							enemy[i].x_FP += enemy[i].speed_diagonalh_FP;
							enemy[i].pos_x = enemy[i].x_FP >> FP;
						}
						break;
					default:
						break;
				}

				enemy[i].status_walk = false;
			}

			// Check if is dead
			if (enemy[i].status_dead) {
				if (gfx_sprite_stack[enemy[i].sprite_num].animation_end) {

					gfx_sprite_stack[enemy[i].sprite_num].loaded = false;
					gfx_sprite_stack[enemy[i].sprite_num].shown = false;
					enemy[i].is_loaded = false;
				}
			}

			// Update sprite position
			gfx_sprite_stack[enemy[i].sprite_num].screen_pos_x = enemy[i].pos_x - camera.pos_x;
			gfx_sprite_stack[enemy[i].sprite_num].screen_pos_y = enemy[i].pos_y - camera.pos_y;

			// Update bullets
			ENEMY_UpdateBullets(i);
		}
	}
}