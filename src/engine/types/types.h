#ifndef TYPES_H
#define TYPES_H

enum { false = 0,
	   true = 1 };

typedef unsigned char byte;
typedef unsigned int word;
typedef unsigned long dword;
typedef int ibool;
typedef byte bool;
typedef signed char sbyte;

typedef struct {
	bool debug_mode;
	bool good_mode;

	bool dosVersionOK;
	bool memOK;
	bool videoOK;
	bool audioOK;
	bool mouseOK;

	bool loading;
	bool logo;
	bool sequence;
	bool ingame;
	bool exit_game;
	int scene;

	// Audio
	bool AUDIO_Initialized;
	int AUDIO_SelectedMode;
	bool ADLIB_present;
	bool SPEAKER_present;
	bool SB_Present;

	// Memory
	bool MM_Initialized;

	// Mouse
	bool MOUSE_initialized;
	bool MOUSE_present;

	// Video
	bool VGA_Present;
	bool EGA_Present;
	bool CGA_Present;
	int VIDEO_SelectedMode;

	char system_error_message1[80];
	char system_error_message3[80];
	char system_error_message2[80];

	int debug1_INT;
	int debug2_INT;
	int debug3_INT;
	int debug4_INT;

	int fps;
	int frame_counter;
	dword limit_old_time;
	dword fps_old_time;
	dword start_time;
	dword sample_time;

	int interrupt_time_ms;
	int delay_time_ms;

} System;

typedef struct {
	byte tremolo;
	byte vibrato;
	byte eg_type;
	byte ksr;
	byte multi;
	byte ksl;
	byte total_level;
	byte attack;
	byte decay;
	byte sustain;
	byte release;
	byte wave_shape;
} Operator;

typedef struct {
	Operator modulator;
	Operator carrier;
	byte feedback;
	byte syn_type;

} Instrument;

typedef struct {
	word base;
	word dsp_version;
	byte lo_DMA;
	byte hi_DMA;
	byte irq;
	int fm_port;
} SBlaster;

typedef struct {
	word opl;
	word port;
	int current_block;
	int current_note;
} Adlib;

typedef struct {
	bool shown;
	int timeout;
	int current_time;

	int portait_graphics_id;
	int portait_x, portait_y;
	int portait_frame;

	int lifebar_graphics_id;
	int lifebar_x, lifebar_y;
	int current_life, old_life;
	int life_speed, life_speed_counter;

	word width;
	word height;
	int pos_x;
	int pos_y;
	int transition_width;
	int transition_height;
} StatusPanel;

typedef struct {
	int timeout;
	int current_time;

	int portait_graphics_id;
	int portait_x, portait_y;
	int portait_frame;
	bool portait_inverted;

	int chat_graphics_id;
	int chat_x, chat_y;
	int chat_frame;
	bool chat_inverted;

	char line[3][40];

	int pos_x;
	int pos_y;

} ChatPanel;

typedef struct {

	int screen_width, screen_height;
	byte *screen_buffer[3];

	byte *map_buffer[3];
	word map_buffer_width, map_buffer_height;

	bool fading_in_async;
	bool faded_in;
	bool fading_out_async;
	bool faded_out;
	int fading_speed;
	int fading_step;
	bool rotate_palette_async;
	int rotate_first_index;
	int rotate_last_index;
	int rotate_speed;


} Video;

typedef struct {

	int visible_width;
	int visible_height;
	int offset_x;
	int offset_y;
	int adjustment_x;
	int adjustment_y;
	int pos_x, pos_y;
	int old_pos_x, old_pos_y;
	int pos_abs_x, pos_abs_y;
	int old_pos_abs_x, old_pos_abs_y;
	int pos_max_x, pos_max_y;
	byte speed_index;
	byte speed_array[65];
	byte speed[49];

} Camera;

typedef struct {
	int visible_width;
	int visible_height;
	int pos_x, pos_y;
	int old_pos_x, old_pos_y;
	int pos_abs_x, pos_abs_y;
	int old_pos_abs_x, old_pos_abs_y;
	int pos_max_x, pos_max_y;
	int tile_offset_x, tile_offset_y;
	int delay;
	int delay_counter;

} CameraParallax;

typedef struct {
	word width_px;
	word height_px;
	int width;
	int height;
	int num_tiles;
	byte *buffer;

} Tileset;

typedef struct {
	int *layer[5];
	Tileset layer_tiles[3];

	word width;
	word height;
	word width_px;
	word height_px;

	bool update_required;
	bool loaded;
	bool shown;
} Map;

typedef struct {
	bool loaded;
	bool shown;
	bool blink;
	bool invisible;
	bool unmasked;

	int id;
	byte graphics_id;
	byte entity_id;

	byte current_frame;

	bool animation_loop;
	bool animation_end;
	bool animation_inverted;
	byte animation_base_frame;
	byte animation_frames;
	byte animation_speed;
	byte animation_counter;

	int screen_pos_x;
	int screen_pos_y;
	int width_px;
	int height_px;

} Sprite;

typedef struct {
	bool loaded;
	int width_px;
	int height_px;
	int num_frames;
	int *frame_offset;
	int *row_offset;
	byte transparent_color;
	byte hit_color;
	byte *buffer;
} SpriteGraphic;

typedef struct {
	bool left_click; // left click flag
	bool right_click;// right click flag
	int pos_x, pos_y;// Position values
	int sprite_num;
	int point_on;
} MouseData;

typedef struct {
	bool show_description;
	bool show_speech;
	bool freeze;
	bool pause;
	bool exit_request;
	int button_pressed;

	byte name_length;
	byte speech_length;
	byte speech_timeout;
	byte speech_time;
	unsigned char object_name[40];
	unsigned char speech[40];

} UI;

typedef struct {
	int up_points[3][2];
	int down_points[3][2];
	int left_points[3][2];
	int right_points[3][2];
} ColissionArea;

typedef struct {
	int points[4][2];
} HitArea;

typedef struct {
	bool loaded;
	bool shown;
	bool on_target;

	int pos_x, pos_y;
	byte speed;
	byte damage;
	int hit_on;
	int steps;
	int current_step;
	int target_x;
	int target_y;

	dword vx_FP;
	dword vy_FP;
	dword speed_FP;
	dword x_FP, y_FP;

	int sprite_num;
	int graphics_id;

	ColissionArea colission_area;

} Bullet;

typedef struct {
	bool loaded;
	bool shown;
	bool on_target;

	int pos_x, pos_y;
	byte speed;
	int hit_on;
	int steps;
	int current_step;
	int target_x;
	int target_y;

	dword vx_FP;
	dword vy_FP;
	dword speed_FP;
	dword x_FP, y_FP;

	int damage;

	int sprite_num;
	int graphics_id;

	ColissionArea colission_area;

} Particle;

typedef struct {
	bool is_loaded;
	int life;
	byte max_life;
	byte type;
	bool life_shown;
	byte life_shown_counter;

	int pos_x, pos_y;

	bool is_hit;
	bool explode;
	byte hit_by;
	byte damage;

	int steps;
	int current_step;
	int num_sprite;

	ColissionArea colission_area;
	HitArea hit_area;
} Object;

typedef struct {
	bool is_loaded;
	byte type;

	int pos_x, pos_y;

	int steps;
	int current_step;

	int num_sprite;
} Effect;

typedef struct {
	bool is_loaded;
	byte type;

	int pos_x, pos_y;

	byte hit_by;

	int steps;
	int current_step;
	int num_sprite;

	HitArea hit_area;
} Button;

typedef struct {
	bool accurate;
	byte type;
	byte max_accuracy;
	byte shoots;
	byte current_recoil;
	byte recoil_time;
	byte damage;
	byte bullet_speed;
	int max_distance;
	int graphics_id;
	int effect_graphics_id;
	Bullet bullet[64];
} Gun;

typedef struct {
	bool action_shoot;
	bool action_punch;
	bool action_loop;
	bool action_walk;
	bool action_stand;
	bool action_dying;
	bool action_dead;
	bool action_hit;
	byte action_punch_combo;


	bool is_hit;
	bool life_shown;
	byte life_shown_counter;

	bool status_facing_up;
	bool status_facing_up_right;
	bool status_facing_right;
	bool status_facing_down_right;
	bool status_facing_down;
	bool status_facing_down_left;
	bool status_facing_left;
	bool status_facing_up_left;
	int status_facing;

	bool mode_combat;

	int pos_x, pos_y;
	int speed;
	dword vx_FP;
	dword vy_FP;
	dword speed_horizontal_FP;
	dword speed_vertical_FP;
	dword speed_diagonalv_FP;
	dword speed_diagonalh_FP;
	dword speed_horizontal_loop_FP;
	dword speed_vertical_loop_FP;
	dword speed_diagonalv_loop_FP;
	dword speed_diagonalh_loop_FP;
	dword x_FP, y_FP;

	byte max_life;
	int life;
	byte damage;

	int idle_counter;
	int last_action;
	int action_step;

	int shoot_x, shoot_y;

	byte shoot_accuracy;
	byte shoot_accuracy_speed;
	byte shoot_accuracy_counter;

	byte movement;

	int sprite_num;

	ColissionArea colission_area;
	HitArea hit_area;

	Gun gun;
} Actor;

typedef struct {
	bool is_loaded;
	bool is_hit;

	bool action_shoot;
	bool action_punch;
	bool action_walk;
	bool action_stand;
	bool action_hit;
	bool action_dead;

	int status_behavior;

	bool status_facing_up;
	bool status_facing_up_right;
	bool status_facing_right;
	bool status_facing_down_right;
	bool status_facing_down;
	bool status_facing_down_left;
	bool status_facing_left;
	bool status_facing_up_left;
	int status_facing;

	int pos_x, pos_y;
	int speed;
	dword vx_FP;
	dword vy_FP;
	dword speed_horizontal_FP;
	dword speed_vertical_FP;
	dword speed_diagonalv_FP;
	dword speed_diagonalh_FP;
	dword speed_horizontal_loop_FP;
	dword speed_vertical_loop_FP;
	dword speed_diagonalv_loop_FP;
	dword speed_diagonalh_loop_FP;
	dword x_FP, y_FP;

	bool movement_allowed;
	byte max_life;
	byte life;
	byte damage;
	byte hit_by;

	int idle_counter;
	int last_action;
	int action_step;

	bool in_shoot_range;
	bool in_punch_range;
	int shoot_range;
	int shoot_x, shoot_y;
	byte shoot_accuracy;
	byte shoot_recoil_time;
	byte shoot_recoul_counter;
	byte reaction_time;
	byte reaction_counter;

	byte movement;
	int sprite_num;

	ColissionArea colission_area;
	HitArea hit_area;

	Gun gun;
} Enemy;

typedef enum {
	CT_ENGINE,
	CT_PALETTE,
	CT_FONT,
	CT_GRAPHICS,
	CT_VIDEO_BUFFER,
	CT_MOUSE,
	CT_TILES,
	CT_MAP,
	CT_COMMON,
	CT_ACTOR,
	CT_SPRITE,
	CT_MAP_DATA,
	CT_MUSIC,
	CT_TEMPORARY,
	CT_CZONE,
	CT_MASKED_TILES,
	CT_INTRO_SOUND_FX,
} ChunkType;

typedef struct {
	unsigned char r, g, b;
} Color;

typedef struct {

	int buffer[51];
	int note;
	int note_index;
	int note_duration;
	int note_time;
	bool play, playing_note;

} SoundEffect;

typedef struct {
	void (*callback)(byte);// Function pointer for the callback
	byte param;
	dword current_time;
	dword end_time;// When the callback should be called
} Timeout;

typedef struct {
	bool loaded;
	bool play;
	bool stop;
	bool pause;
	bool songend;
	bool loop;

	bool speed_update;

	int current_pattern;
	int current_line;
	int current_order;
	bool pattern_break;
	bool pattern_delay;
	int tickXF;
	int tickD;
	int ticks;
	int next_line;
	int next_order;
	bool irq_mode;

	int ticklooper;
	int macro_ticklooper;
	int speed;
	int tempo;
	int macro_speedup;


} Song;

typedef struct {
	byte *data;
	word char_width;
	word char_height;
} Font;

typedef struct {
	byte *palette_shown;
	byte *palette_loaded;
	Font font[4];
	byte *image_buffer1;
	word image_buffer1_width;
	word image_buffer1_height;
	byte *image_buffer2;
	word image_buffer2_width;
	word image_buffer2_height;

} Graphics;


#endif
