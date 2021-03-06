/*
 * Copyright (c) Andr� 'Espyo' Silva 2013-2015.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the globally accessible variables.
 */

#ifndef VARS_INCLUDED
#define VARS_INCLUDED

#include <map>
#include <unordered_set>
#include <vector>

#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>

#include "animation.h"
#include "const.h"
#include "controls.h"
#include "hitbox.h"
#include "LAFI/gui.h"
#include "LAFI/label.h"
#include "leader.h"
#include "misc_structs.h"
#include "onion.h"
#include "particle.h"
#include "pikmin.h"
#include "pikmin_type.h"
#include "sector.h"
#include "ship.h"
#include "ship_type.h"
#include "spray_type.h"
#include "spec_objs/info_spot.h"
#include "spec_objs/nectar.h"
#include "pellet.h"
#include "status.h"
#include "treasure.h"
#include "weather.h"

using namespace std;

// Bitmaps.
extern ALLEGRO_BITMAP* bmp_blue_onion;
extern ALLEGRO_BITMAP* bmp_bubble;
extern ALLEGRO_BITMAP* bmp_cloaking_burrow_nit;
extern ALLEGRO_BITMAP* bmp_cursor;
extern ALLEGRO_BITMAP* bmp_day_bubble;
extern ALLEGRO_BITMAP* bmp_enemy_spirit;
extern ALLEGRO_BITMAP* bmp_hard_bubble;
extern ALLEGRO_BITMAP* bmp_icon;
extern ALLEGRO_BITMAP* bmp_idle_glow;
extern ALLEGRO_BITMAP* bmp_info_spot;
extern ALLEGRO_BITMAP* bmp_message_box;
extern ALLEGRO_BITMAP* bmp_mouse_cursor;
extern ALLEGRO_BITMAP* bmp_group_move_arrow;
extern ALLEGRO_BITMAP* bmp_nectar;
extern ALLEGRO_BITMAP* bmp_no_pikmin;
extern ALLEGRO_BITMAP* bmp_number_bubble;
extern ALLEGRO_BITMAP* bmp_pikmin_spirit;
extern ALLEGRO_BITMAP* bmp_red[3];
extern ALLEGRO_BITMAP* bmp_red_onion;
extern ALLEGRO_BITMAP* bmp_shadow;
extern ALLEGRO_BITMAP* bmp_ship;
extern ALLEGRO_BITMAP* bmp_smack;
extern ALLEGRO_BITMAP* bmp_smoke;
extern ALLEGRO_BITMAP* bmp_sparkle;
extern ALLEGRO_BITMAP* bmp_sun;
extern ALLEGRO_BITMAP* bmp_sun_bubble;
extern ALLEGRO_BITMAP* bmp_tp;
extern ALLEGRO_BITMAP* bmp_ub_spray;
extern ALLEGRO_BITMAP* bmp_us_spray;
extern ALLEGRO_BITMAP* bmp_yellow_onion;

// Test things, temporary.
extern ALLEGRO_BITMAP* bmp_test;
extern sector test_sector;
extern vector<linedef> test_linedefs;

// Sound effects.
extern sample_struct sfx_attack;
extern sample_struct sfx_camera;
extern sample_struct sfx_dismiss;
extern sample_struct sfx_louie_whistle;
extern sample_struct sfx_louie_name_call;
extern sample_struct sfx_president_name_call;
extern sample_struct sfx_olimar_whistle;
extern sample_struct sfx_olimar_name_call;
extern sample_struct sfx_president_whistle;
extern sample_struct sfx_president_name_call;
extern sample_struct sfx_pikmin_attack;
extern sample_struct sfx_pikmin_called;
extern sample_struct sfx_pikmin_carrying;
extern sample_struct sfx_pikmin_carrying_grab;
extern sample_struct sfx_pikmin_caught;
extern sample_struct sfx_pikmin_dying;
extern sample_struct sfx_pikmin_held;
extern sample_struct sfx_pikmin_idle;
extern sample_struct sfx_pikmin_pluck;
extern sample_struct sfx_pikmin_plucked;
extern sample_struct sfx_pikmin_thrown;
extern sample_struct sfx_switch_pikmin;
extern sample_struct sfx_throw;

// General globals.
extern vector<vector<ALLEGRO_BITMAP*> > area_images;
extern float                            area_images_scale;
extern int                              area_image_size;
extern float                            area_images_x1; // Top-left corner of the area, in world coordinates.
extern float                            area_images_y1;
extern float                            auto_pluck_input_time; // Time left until the player can press the pluck button again, to make the captain auto-pluck.
extern vector<unsigned int>             berries;
extern bmp_manager                      bitmaps;
extern ALLEGRO_BITMAP*                  bmp_error;
extern float                            cam_trans_pan_final_x;
extern float                            cam_trans_pan_final_y;
extern float                            cam_trans_pan_initial_x;
extern float                            cam_trans_pan_initial_y;
extern float                            cam_trans_pan_time_left;
extern float                            cam_trans_zoom_final_level;
extern float                            cam_trans_zoom_initial_level;
extern float                            cam_trans_zoom_time_left;
extern float                            cam_x;
extern float                            cam_y;
extern float                            cam_zoom;
extern vector<control_info>             controls;
extern mob*                             closest_party_member;
extern area_map                         cur_area_map;
extern size_t                           cur_leader_nr;
extern string                           cur_message;
extern size_t                           cur_message_char;
extern float                            cur_message_char_time;
extern size_t                           cur_message_section;
extern ALLEGRO_BITMAP*                  cur_message_speaker;
extern vector<size_t>                   cur_message_stopping_chars; // The message stops scrolling when it reaches one of these characters.
extern movement_struct                  cursor_movement;            //Movement of the cursor via non-mouse.
extern unsigned char                    cur_screen;
extern float                            cur_sun_strength;
extern float                            cursor_angle;
extern float                            cursor_invalid_effect;    // Effect for the invalid cursor fading in or out. The opacity is calculated using this number's sign.
extern float                            cursor_save_time;         // Time left until the position of the cursor is saved on the vector.
extern float                            cursor_spin_angle;
extern vector<point>                    cursor_spots;             // Spots the cursor has been through. Used for the faint trail left behind it.
extern float                            cursor_x;                 // Leader's cursor.
extern float                            cursor_y;
extern unsigned int                     day;
extern float                            day_minutes;
extern float                            day_minutes_end;          // The day ends when the in-game minutes reach this value.
extern float                            day_minutes_per_irl_sec;  // Every real-life second, these many in-game minutes pass.
extern float                            day_minutes_start;        // The in-game minutes start with this value every day.
extern bool                             daylight_effect;
extern double                           delta_t;                  // Time between the previous frame and the current.
extern ALLEGRO_DISPLAY*                 display;
extern bool                             draw_cursor_trail;
extern animation_set                    ed_anims;
extern bool                             ed_anim_playing;
extern bool                             ed_bg_aspect_ratio;
extern ALLEGRO_BITMAP*                  ed_bg_bitmap;
extern string                           ed_bg_file_name;
extern float                            ed_bg_x;
extern float                            ed_bg_y;
extern float                            ed_bg_w;
extern float                            ed_bg_h;
extern unsigned char                    ed_bg_a;
extern animation*                       ed_cur_anim;
extern frame*                           ed_cur_frame;
extern size_t                           ed_cur_frame_instance_nr;
extern float                            ed_cur_frame_time;
extern hitbox*                          ed_cur_hitbox;
extern float                            ed_cur_hitbox_alpha;  // The alpha is calculated using the sine of this value.
extern size_t                           ed_cur_hitbox_instance_nr;
extern mob_gen*                         ed_cur_mob;
extern sector*                          ed_cur_sector;
extern tree_shadow*                     ed_cur_shadow;
extern float                            ed_double_click_time;
extern mob_gen*                         ed_error_mob_ptr;
extern sector*                          ed_error_sector_ptr;
extern tree_shadow*                     ed_error_shadow_ptr;
extern string                           ed_error_string;
extern unsigned char                    ed_error_type;
extern vertex*                          ed_error_vertex_ptr;
extern string                           ed_file_name;
extern size_t                           ed_grabbing_hitbox;   // Hitbox being grabbed by the mouse cursor. string::npos = none.
extern bool                             ed_grabbing_hitbox_edge;
extern float                            ed_grabbing_hitbox_x; // X world coordinate of the point we're grabbing, or the anchor, when in resize mode.
extern float                            ed_grabbing_hitbox_y;
extern lafi::gui*                       ed_gui;
extern bool                             ed_hitboxes_visible;
extern bool                             ed_holding_m1;
extern bool                             ed_holding_m2;
extern vector<linedef_intersection>     ed_intersecting_lines;
extern unordered_set<linedef*>          ed_lone_lines;
extern unsigned char                    ed_maturity; // Current maturity of the Pikmin, used to check the visuals of different Pikmin tops.
extern unsigned char                    ed_mob_type_list; // Use MOB_TYPE_*.
extern unsigned char                    ed_mode;
extern size_t                           ed_moving_thing; // Current vertex, object or shadow being moved.
extern float                            ed_moving_thing_x; // Relative X coordinate of the point where the vertex, object or shadow was grabbed.
extern float                            ed_moving_thing_y;
extern float                            ed_new_hitbox_corner_x; // FLT_MAX = none.
extern float                            ed_new_hitbox_corner_y;
extern unordered_set<sector*>           ed_non_simples;
extern string                           ed_object_name;
extern sector*                          ed_on_sector;
extern unsigned char                    ed_sec_mode; // Secondary/sub mode.
extern bool                             ed_shift_pressed;
extern bool                             ed_show_bg;
extern bool                             ed_show_shadows;
extern ALLEGRO_BITMAP*                  ed_top_bmp[3]; // Top bitmaps for the current Pikmin type.
extern lafi::widget*                    ed_wum; // Widget under mouse.
extern map<string, enemy_type*>         enemy_types;
extern vector<enemy*>                   enemies;
extern ALLEGRO_FONT*                    font;
extern ALLEGRO_FONT*                    font_area_name;
extern ALLEGRO_FONT*                    font_counter;
extern unsigned int                     font_counter_h;
extern unsigned int                     font_h;
extern ALLEGRO_FONT*                    font_value;             // Font for the carrying / money values.
extern unsigned short                   game_fps;
extern float                            group_move_angle;
extern vector<float>                    group_move_arrows;          // Distance of the arrows that appear when the "move group to cursor" button is held.
extern float                            group_move_intensity;     // General intensity of the group move in the specified angle.
extern float                            group_move_next_arrow_time; // Time remaining until the next arrow on the "move group arrows" appears.
extern bool                             group_move_go_to_cursor;     // Is the "move group to cursor" button being pressed?
extern movement_struct                  group_movement; //Joystick coordinates for the group movement.
extern vector<vector<float> >           group_spots_x;
extern vector<vector<float> >           group_spots_y;
extern float                            idle_glow_angle;
extern vector<info_spot*>               info_spots;
extern map<ALLEGRO_JOYSTICK*, int>      joystick_numbers;
extern vector<leader*>                  leaders;
extern movement_struct                  leader_movement; //How hard the joystick is pressed in each direction ([0, 1]);
extern map<string, leader_type*>        leader_types;
extern unsigned                         max_pikmin_in_field;
extern ALLEGRO_MIXER*                   mixer;
extern mob_category_manager             mob_categories;
extern vector<mob*>                     mobs;
extern float                            mouse_cursor_x;             // The physical mouse's cursor.
extern float                            mouse_cursor_y;
extern bool                             mouse_cursor_valid;
extern bool                             mouse_moves_cursor[4];
extern vector<nectar*>                  nectars;
extern bool                             no_error_logs_today;        // Have there been no errors in this play session?
extern map<string, onion_type*>         onion_types;
extern vector<onion*>                   onions;
extern unsigned char                    particle_quality;
extern vector<particle>                 particles;
extern bool                             paused;
extern map<string, pellet_type*>        pellet_types;
extern vector<pellet*>                  pellets;
extern vector<point>                    percipitation;
extern float                            percipitation_time_left;
extern unsigned char                    pikmin_ai_portion;
extern map<pikmin_type*, unsigned long> pikmin_in_onions;
extern vector<pikmin*>                  pikmin_list;
extern map<string, pikmin_type*>        pikmin_types;
extern bool                             pretty_whistle;              // If true, the whistle radius is merely drawn as a circle. Used to improve performance.
extern double                           prev_frame_time;             // Time since start, on the previous frame. Used to calculate the time difference between the current and last frames.
extern float                            prev_group_move_intensity; // The group move intensity on the previous frame. Used to figure out if, on 0-intensity, we should put the group's center near the leader or let it go there by itself.
extern bool                             running;
extern unsigned short                   scr_h;
extern unsigned short                   scr_w;
extern sector_types_manager             sector_types;
extern unsigned int                     selected_spray;
extern unsigned char                    ship_beam_ring_color[3];
extern bool                             ship_beam_ring_color_up[3];
extern map<string, ship_type*>          ship_types;
extern vector<ship*>                    ships;
extern bool                             smooth_scaling;     // If false, images that are scaled up and down will look pixelated.
extern map<string, mob_type*>           spec_mob_types;
extern vector<unsigned long>            spray_amounts;      // How many of each spray the player has.
extern vector<spray_type>               spray_types;
extern vector<status>                   statuses;
extern float                            sun_meter_sun_angle;
extern float                            throw_particle_timer;
extern map<string, treasure_type*>      treasure_types;
extern vector<treasure*>                treasures;
extern float                            tree_shadow_sway;
extern ALLEGRO_VOICE*                   voice;  // Voice from which the sound effects play.
extern map<string, weather>             weather_conditions;
extern float                            whistle_dot_offset;    // How much each dot of the whistle should spin.
extern float                            whistle_dot_radius[6]; // Radius of every 6th dot.
extern float                            whistle_fade_radius;   // Radius the whistle was at pre-fade.
extern float                            whistle_fade_time;     // Time left for the whistle's fading animations.
extern float                            whistle_max_hold;      // The whistle area is at max size. Hold the whistle for these many seconds.
extern float                            whistle_next_dot_time;
extern float                            whistle_next_ring_time;
extern float                            whistle_radius;
extern vector<unsigned char>            whistle_ring_colors;
extern unsigned char                    whistle_ring_prev_color;
extern vector<float>                    whistle_rings;
extern bool                             whistling;             // Is the whistle currently being blown?
extern int                              window_x;
extern int                              window_y;



// Debugging variables.
extern bool debug_show_framerate;
extern float debug_framerate_update_timer;
extern unsigned int debug_framerate_counter;
extern string debug_last_axis;

#endif // ifndef VARS_INCLUDED
