
#include "../engine.h"
#include "particle.h"

Particle particle[PARTICLE_MAX_PARTICLES];

/** OBJECT :: Load object
 */
/** ACTOR :: Initialize bullet
 */
void PARTICLE_InitParticle(int graphics_id, int entity_id, int source_x, int source_y, int target_x, int target_y, int speed, int damage) {
	int i;
	int number;
	int dx, dy;
	int distance;

	// Search free particle slot
	number = 0;
	for (i = 0; i < PARTICLE_MAX_PARTICLES; i++) {
		if (!particle[i].loaded) {
			number = i;
			break;
		}
	}

	// Check if max bullets is reached
	if (number == PARTICLE_MAX_PARTICLES - 1) {
		Error("PARTICLE_InitParticle function error", "Max number of bullets", "", ERROR_SYSTEM);
	}

	// Calculate target
	particle[number].target_x = target_x;
	particle[number].target_y = target_y;

	particle[number].pos_x = source_x;
	particle[number].pos_y = source_y;
	particle[number].speed = speed;

	particle[number].damage = damage;

	// calculate distance from source
	dx = particle[number].target_x - particle[number].pos_x;
	dy = particle[number].target_y - particle[number].pos_y;

	// calculate steps
	if (abs(dx) > abs(dy))
		//particle[number].steps = abs(dx);
		distance = abs(dx);
	else
		//particle[number].steps = abs(dy);
		distance = abs(dy);


	// calculate fixed point velocity and position
	particle[number].vx_FP = (dx << FP) / distance;
	particle[number].vy_FP = (dy << FP) / distance;

	particle[number].x_FP = particle[number].pos_x << FP;
	particle[number].y_FP = particle[number].pos_y << FP;

	particle[number].steps = distance;

	particle[number].graphics_id = graphics_id;

	// Initialize bullet sprite
	if (!GFX_InitSprite(particle[number].graphics_id, entity_id, number, &particle[number].sprite_num)) {
		sprintf(engine.system_error_message1, "PARTICLE_InitParticle function error");
		sprintf(engine.system_error_message2, "Cannot initialize particle %u sprite", number);
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
	particle[number].colission_area.up_points[0][0] = gfx_sprite_stack[particle[number].sprite_num].width_px >> 4;
	particle[number].colission_area.up_points[0][1] = gfx_sprite_stack[particle[number].sprite_num].height_px >> 4;
	// Point 2
	particle[number].colission_area.up_points[1][0] = gfx_sprite_stack[particle[number].sprite_num].width_px - (gfx_sprite_stack[particle[number].sprite_num].width_px >> 4);
	particle[number].colission_area.up_points[1][1] = gfx_sprite_stack[particle[number].sprite_num].height_px >> 4;

	// Point 3
	particle[number].colission_area.down_points[0][0] = gfx_sprite_stack[particle[number].sprite_num].width_px >> 4;
	particle[number].colission_area.down_points[0][1] = gfx_sprite_stack[particle[number].sprite_num].height_px - (gfx_sprite_stack[particle[number].sprite_num].height_px >> 4);

	// Point 4
	particle[number].colission_area.down_points[1][0] = gfx_sprite_stack[particle[number].sprite_num].width_px - (gfx_sprite_stack[particle[number].sprite_num].width_px >> 4);
	particle[number].colission_area.down_points[1][1] = gfx_sprite_stack[particle[number].sprite_num].height_px - (gfx_sprite_stack[particle[number].sprite_num].height_px >> 4);

	gfx_sprite_stack[particle[number].sprite_num].animation_base_frame = 0;
	gfx_sprite_stack[particle[number].sprite_num].animation_frames = 1;
	gfx_sprite_stack[particle[number].sprite_num].animation_speed = 0;
	gfx_sprite_stack[particle[number].sprite_num].current_frame = 0;

	gfx_sprite_stack[particle[number].sprite_num].unmasked = true;

	gfx_sprite_stack[particle[number].sprite_num].screen_pos_x = particle[number].pos_x - camera.pos_x;
	gfx_sprite_stack[particle[number].sprite_num].screen_pos_y = particle[number].pos_y - camera.pos_y;

	particle[number].loaded = true;
	particle[number].shown = true;
}

int PARTICLE_CheckParticleColission(Particle obj) {
	int i;
	int point1_x, point1_y;
	int point2_x, point2_y;
	int point3_x, point3_y;
	int point4_x, point4_y;

	point1_x = obj.pos_x + obj.colission_area.up_points[0][0];
	point1_y = obj.pos_y + obj.colission_area.up_points[0][1];
	point2_x = obj.pos_x + obj.colission_area.up_points[1][0];
	point2_y = obj.pos_y + obj.colission_area.up_points[1][1];
	point3_x = obj.pos_x + obj.colission_area.down_points[0][0];
	point3_y = obj.pos_y + obj.colission_area.down_points[0][1];
	point4_x = obj.pos_x + obj.colission_area.down_points[1][0];
	point4_y = obj.pos_y + obj.colission_area.down_points[1][1];

	/////// BACKGROUND COLISSIONS ///////////
	if (MAP_CheckColissionTile(point1_x, point1_y) == 1) { return ENTITY_ID_BACKGROUND; }// Check point 1
	if (MAP_CheckColissionTile(point2_x, point2_y) == 1) { return ENTITY_ID_BACKGROUND; }// Check point 2
	if (MAP_CheckColissionTile(point3_x, point3_y) == 1) { return ENTITY_ID_BACKGROUND; }// Check point 3
	if (MAP_CheckColissionTile(point4_x, point4_y) == 1) { return ENTITY_ID_BACKGROUND; }// Check point 4

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
			}
		}
	}

	/////// ACTOR COLISSIONS ///////////
	if (gfx_sprite_stack[actor.sprite_num].shown) {
		//Point 1
		if (point1_x < actor.pos_x + actor.hit_area.points[1][0]

			&& point1_x > actor.pos_x + actor.hit_area.points[0][0]

			&& point1_y > actor.pos_y + actor.hit_area.points[0][1]

			&& point1_y < actor.pos_y + actor.hit_area.points[2][1]) {

			return gfx_sprite_stack[actor.sprite_num].id;
		}
	}

	return false;
}

/** ACTOR :: Update bullets	
 */
void PARTICLE_UpdateParticles(void) {
	int i, j;
	int obj_number, enemy_number;

	for (i = 0; i < PARTICLE_MAX_PARTICLES; i++) {
		if (particle[i].loaded) {

			// Not on target
			if (!particle[i].on_target) {
				// Calculate new pos

				//for (j = 0; j < particle[i].speed; j++) {
				particle[i].x_FP += particle[i].vx_FP;
				particle[i].y_FP += particle[i].vy_FP;

				particle[i].pos_x = (particle[i].x_FP >> FP);
				particle[i].pos_y = (particle[i].y_FP >> FP);

				// Update sprite screen pos
				gfx_sprite_stack[particle[i].sprite_num].screen_pos_x = particle[i].pos_x - camera.pos_x;
				gfx_sprite_stack[particle[i].sprite_num].screen_pos_y = particle[i].pos_y - camera.pos_y;

				// Check if hits something
				particle[i].hit_on = PARTICLE_CheckParticleColission(particle[i]);

				switch (gfx_sprite_stack[particle[i].sprite_num].entity_id) {
					case ENTITY_ID_BLOOD:
						// Check hit on background, object or enemy
						switch (particle[i].hit_on & 0x00FF) {
							case ENTITY_ID_BACKGROUND:
								// can be background
								particle[i].on_target = true;
								break;
						}
						break;
					case ENTITY_ID_EXPLOSION:
						// Check hit on background, object or enemy
						switch (particle[i].hit_on & 0x00FF) {
							case ENTITY_ID_BACKGROUND:
								// can be background
								particle[i].on_target = true;
								break;
							case ENTITY_ID_BARREL:
								particle[i].on_target = true;
								// Get object number
								obj_number = (particle[i].hit_on & 0xFF00) >> 8;
								object[obj_number].is_hit = true;
								object[obj_number].damage = particle[i].damage;
								break;
							case ENTITY_ID_ENEMY:
								particle[i].on_target = true;
								// Get enemy number
								enemy_number = (particle[i].hit_on & 0xFF00) >> 8;
								enemy[enemy_number].is_hit = true;
								enemy[enemy_number].damage = particle[i].damage;
								break;
							case ENTITY_ID_ACTOR:
								particle[i].on_target = true;
								actor.is_hit = true;
								actor.damage = particle[i].damage;
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}


				particle[i].current_step++;
				if (particle[i].current_step >= particle[i].steps) {
					particle[i].on_target = true;
					// update bullet graphic
					gfx_sprite_stack[particle[i].sprite_num].animation_frames = 4;
					gfx_sprite_stack[particle[i].sprite_num].current_frame = 1;
					gfx_sprite_stack[particle[i].sprite_num].animation_counter = 0;
					gfx_sprite_stack[particle[i].sprite_num].animation_speed = 5;
					gfx_sprite_stack[particle[i].sprite_num].animation_end = false;
				}
				//}
			}

			// On target
			if (particle[i].on_target) {

				// Update sprite screen pos
				gfx_sprite_stack[particle[i].sprite_num].screen_pos_x = particle[i].pos_x - camera.pos_x;
				gfx_sprite_stack[particle[i].sprite_num].screen_pos_y = particle[i].pos_y - camera.pos_y;

				if (gfx_sprite_stack[particle[i].sprite_num].animation_end) {
					particle[i].on_target = false;
					particle[i].shown = false;
					particle[i].current_step = 0;
					particle[i].loaded = false;
					GFX_UnloadSprite(particle[i].sprite_num);
				}
			}
		}
	}
}
