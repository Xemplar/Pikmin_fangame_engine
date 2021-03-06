/*
 * Copyright (c) Andr� 'Espyo' Silva 2014.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Program initializer functions.
 */

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

#include "init.h"

#include "controls.h"
#include "functions.h"
#include "vars.h"

void init_allegro() {
    al_init();
    al_install_mouse();
    al_install_keyboard();
    al_install_audio();
    al_install_joystick();
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_acodec_addon();
}

void init_controls() {
    // TODO create a manager for this, like the mob category manager and whatnot.
    controls.push_back(control_info(BUTTON_THROW, 0, "mb_1"));
    controls.push_back(control_info(BUTTON_WHISTLE, 0, "mb_2"));
    controls.push_back(control_info(BUTTON_MOVE_RIGHT, 0, "k_4"));
    controls.push_back(control_info(BUTTON_MOVE_UP, 0, "k_23"));
    controls.push_back(control_info(BUTTON_MOVE_LEFT, 0, "k_1"));
    controls.push_back(control_info(BUTTON_MOVE_DOWN, 0, "k_19"));
    controls.push_back(control_info(BUTTON_GROUP_MOVE_GO_TO_CURSOR, 0, "k_75"));
    controls.push_back(control_info(BUTTON_SWITCH_CAPTAIN_RIGHT, 0, "k_64"));
    controls.push_back(control_info(BUTTON_DISMISS, 0, "k_217"));
    controls.push_back(control_info(BUTTON_USE_SPRAY_1, 0, "k_18"));
    controls.push_back(control_info(BUTTON_USE_SPRAY_2, 0, "k_6"));
    controls.push_back(control_info(BUTTON_USE_SPRAY, 0, "k_18"));
    controls.push_back(control_info(BUTTON_SWITCH_SPRAY_RIGHT, 0, "k_5"));
    controls.push_back(control_info(BUTTON_SWITCH_SPRAY_LEFT, 0, "k_17"));
    controls.push_back(control_info(BUTTON_SWITCH_TYPE_RIGHT, 0, "mb_2"));
    controls.push_back(control_info(BUTTON_SWITCH_ZOOM, 0, "k_3"));
    controls.push_back(control_info(BUTTON_ZOOM_IN, 0, "mwu"));
    controls.push_back(control_info(BUTTON_ZOOM_OUT, 0, "mwd"));
    controls.push_back(control_info(BUTTON_LIE_DOWN, 0, "k_26"));
    controls.push_back(control_info(BUTTON_PAUSE, 0, "k_59"));
}

void init_error_bitmap() {
    // Error bitmap.
    int bmp_error_w, bmp_error_h;
    al_get_text_dimensions(al_create_builtin_font(), "ERROR", NULL, NULL, &bmp_error_w, &bmp_error_h);
    bmp_error = al_create_bitmap(bmp_error_w, bmp_error_h);
    al_set_target_bitmap(bmp_error); {
        al_clear_to_color(al_map_rgba(64, 0, 0, 128));
        al_draw_text(al_create_builtin_font(), al_map_rgb(255, 0, 0), 0, 0, 0, "ERROR");
    } al_set_target_backbuffer(display);
}

void init_event_things(ALLEGRO_TIMER* &timer, ALLEGRO_EVENT_QUEUE* &queue) {
    al_set_new_window_position(window_x, window_y);
    display = al_create_display(scr_w, scr_h);
    timer = al_create_timer(1.0 / game_fps);
    
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_joystick_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
}

void init_fonts() {
    int font_ranges[] = {
        0x0020, 0x007E, // ASCII
        0x00A0, 0x00A1, // Non-breaking space and inverted !
        0x00BF, 0x00FF, // Inverted ? and European vowels and such
    };
    int counter_font_ranges[] = {
        0x002D, 0x002D, // Dash
        0x002F, 0x0039, // Slash and numbers
        0x0078, 0x0078, // x
    };
    int value_font_ranges[] = {
        0x0024, 0x0024, // Dollar sign
        0x002D, 0x002D, // Dash
        0x0030, 0x0039, // Numbers
    };
    
    // We can't load the font directly because we want to set the ranges.
    // So we load into a bitmap first.
    ALLEGRO_BITMAP* temp_font_bitmap = load_bmp("Font.png");
    if(temp_font_bitmap) font = al_grab_font_from_bitmap(temp_font_bitmap, 3, font_ranges);
    al_destroy_bitmap(temp_font_bitmap);
    
    temp_font_bitmap = load_bmp("Area_name_font.png");
    if(temp_font_bitmap) font_area_name = al_grab_font_from_bitmap(temp_font_bitmap, 3, font_ranges);
    al_destroy_bitmap(temp_font_bitmap);
    
    temp_font_bitmap = load_bmp("Counter_font.png");
    if(temp_font_bitmap) font_counter = al_grab_font_from_bitmap(temp_font_bitmap, 3, counter_font_ranges);
    al_destroy_bitmap(temp_font_bitmap);
    
    temp_font_bitmap = load_bmp("Value_font.png");
    if(temp_font_bitmap) font_value = al_grab_font_from_bitmap(temp_font_bitmap, 3, value_font_ranges);
    al_destroy_bitmap(temp_font_bitmap);
    
    if(font) font_h = al_get_font_line_height(font);
    if(font_counter) font_counter_h = al_get_font_line_height(font_counter);
}

void init_misc() {
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);
    al_set_window_title(display, "Pikmin fangame engine");
    if(smooth_scaling) al_set_new_bitmap_flags(ALLEGRO_MAG_LINEAR | ALLEGRO_MIN_LINEAR | ALLEGRO_MIPMAP);
    al_reserve_samples(16);
    
    srand(time(NULL));
    
    // TODO the function is always returning 0.
    area_image_size = /*al_get_new_display_option(ALLEGRO_MAX_BITMAP_SIZE, NULL)*/ 800;
}

void init_misc_graphics() {
    // Graphics.
    bmp_red_onion = load_bmp("Red_onion.png");
    bmp_yellow_onion = load_bmp("Yellow_onion.png");
    bmp_blue_onion = load_bmp("Blue_onion.png");
    bmp_ship = load_bmp("Ship.png");
    
    bmp_bubble = load_bmp(          "Bubble.png");
    bmp_cursor = load_bmp(          "Cursor.png");
    bmp_day_bubble = load_bmp(      "Day_bubble.png");
    bmp_enemy_spirit = load_bmp(    "Enemy_spirit.png");
    bmp_hard_bubble = load_bmp(     "Hard_bubble.png");
    bmp_icon = load_bmp(            "Icon.png");
    bmp_idle_glow = load_bmp(       "Idle_glow.png");
    bmp_info_spot = load_bmp(       "Info_spot.png");
    bmp_message_box = load_bmp(     "Message_box.png");
    bmp_mouse_cursor = load_bmp(    "Mouse_cursor.png");
    bmp_group_move_arrow = load_bmp("Group_move_arrow.png");
    bmp_nectar = load_bmp(          "Nectar.png");
    bmp_no_pikmin = load_bmp(       "No_Pikmin.png");
    bmp_number_bubble = load_bmp(   "Number_bubble.png");
    bmp_pikmin_spirit = load_bmp(   "Pikmin_spirit.png");
    bmp_shadow = load_bmp(          "Shadow.png");
    bmp_smack = load_bmp(           "Smack.png");
    bmp_smoke = load_bmp(           "Smoke.png");
    bmp_sparkle = load_bmp(         "Sparkle.png");
    bmp_sun = load_bmp(             "Sun.png");
    bmp_sun_bubble = load_bmp(      "Sun_bubble.png");
    bmp_tp = load_bmp(              "TP.png");
    bmp_ub_spray = load_bmp(        "Ultra-bitter_spray.png");
    bmp_us_spray = load_bmp(        "Ultra-spicy_spray.png");
    
    bmp_test = load_bmp("Test.png");
    
    al_set_display_icon(display, bmp_icon);
}

void init_misc_sounds() {
    // Sound effects.
    voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16,   ALLEGRO_CHANNEL_CONF_2);
    mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
    al_attach_mixer_to_voice(mixer, voice);
    sfx_attack = load_sample(              "Attack.ogg",               mixer);
    sfx_pikmin_attack = load_sample(       "Pikmin_attack.ogg",        mixer);
    sfx_pikmin_carrying = load_sample(     "Pikmin_carrying.ogg",      mixer);
    sfx_pikmin_carrying_grab = load_sample("Pikmin_carrying_grab.ogg", mixer);
    sfx_pikmin_caught = load_sample(       "Pikmin_caught.ogg",        mixer);
    sfx_pikmin_dying = load_sample(        "Pikmin_dying.ogg",         mixer);
    sfx_pikmin_held = load_sample(         "Pikmin_held.ogg",          mixer);
    sfx_pikmin_idle = load_sample(         "Pikmin_idle.ogg",          mixer);
    sfx_pikmin_thrown = load_sample(       "Pikmin_thrown.ogg",        mixer);
    sfx_pikmin_pluck = load_sample(        "Pikmin_pluck.ogg",         mixer);
    sfx_pikmin_plucked = load_sample(      "Pikmin_plucked.ogg",       mixer);
    sfx_pikmin_called = load_sample(       "Pikmin_called.ogg",        mixer);
    sfx_dismiss = load_sample(             "Dismiss.ogg",              mixer);
    sfx_olimar_whistle = load_sample(      "Olimar_whistle.ogg",       mixer);
    sfx_louie_whistle = load_sample(       "Louie_whistle.ogg",        mixer);
    sfx_president_whistle = load_sample(   "President_whistle.ogg",    mixer);
    sfx_olimar_name_call = load_sample(    "Olimar_name_call.ogg",     mixer);
    sfx_louie_name_call = load_sample(     "Louie_name_call.ogg",      mixer);
    sfx_president_name_call = load_sample( "President_name_call.ogg",  mixer);
    sfx_throw = load_sample(               "Throw.ogg",                mixer);
    sfx_switch_pikmin = load_sample(       "Switch_Pikmin.ogg",        mixer);
    sfx_camera = load_sample(              "Camera.ogg",               mixer);
}

void init_mob_categories() {

    mob_categories.register_category(MOB_CATEGORY_NONE, "None", "None", [] (vector<string> &) { },
    [] (const string &) -> mob_type* { return NULL; });
    
    mob_categories.register_category(MOB_CATEGORY_ENEMIES, "Enemies", "Enemy", [] (vector<string> &li) {
        for(auto e = enemy_types.begin(); e != enemy_types.end(); e++) li.push_back(e->first);
    }, [] (const string & n) -> mob_type* {
        auto it = enemy_types.find(n); if(it == enemy_types.end()) return NULL; return it->second;
    });
    
    mob_categories.register_category(MOB_CATEGORY_LEADERS, "Leaders", "Leader", [] (vector<string> &li) {
        for(auto l = leader_types.begin(); l != leader_types.end(); l++) li.push_back(l->first);
    }, [] (const string & n) -> mob_type* {
        auto it = leader_types.find(n); if(it == leader_types.end()) return NULL; return it->second;
    });
    
    mob_categories.register_category(MOB_CATEGORY_ONIONS, "Onions", "Onion", [] (vector<string> &li) {
        for(auto o = onion_types.begin(); o != onion_types.end(); o++) li.push_back(o->first);
    }, [] (const string & n) -> mob_type* {
        auto it = onion_types.find(n); if(it == onion_types.end()) return NULL; return it->second;
    });
    
    mob_categories.register_category(MOB_CATEGORY_PELLETS, "Pellets", "Pellet", [] (vector<string> &li) {
        for(auto p = pellet_types.begin(); p != pellet_types.end(); p++) li.push_back(p->first);
    }, [] (const string & n) -> mob_type* {
        auto it = pellet_types.find(n); if(it == pellet_types.end()) return NULL; return it->second;
    });
    
    mob_categories.register_category(MOB_CATEGORY_PIKMIN, "Pikmin", "Pikmin", [] (vector<string> &li) {
        for(auto p = pikmin_types.begin(); p != pikmin_types.end(); p++) li.push_back(p->first);
    }, [] (const string & n) -> mob_type* {
        auto it = pikmin_types.find(n); if(it == pikmin_types.end()) return NULL; return it->second;
    });
    
    mob_categories.register_category(MOB_CATEGORY_SHIPS, "Ships", "Ship", [] (vector<string> &li) {
        for(auto s = ship_types.begin(); s != ship_types.end(); s++) li.push_back(s->first);
    }, [] (const string & n) -> mob_type* {
        auto it = ship_types.find(n); if(it == ship_types.end()) return NULL; return it->second;
    });
    
    mob_categories.register_category(MOB_CATEGORY_SPECIAL, "Special", "Special", [] (vector<string> &li) {
        for(auto s = spec_mob_types.begin(); s != spec_mob_types.end(); s++) li.push_back(s->first);
    }, [] (const string & n) -> mob_type* {
        auto it = spec_mob_types.find(n); if(it == spec_mob_types.end()) return NULL; return it->second;
    });
    
    mob_categories.register_category(MOB_CATEGORY_TREASURES, "Treasures", "Treasure", [] (vector<string> &li) {
        for(auto t = treasure_types.begin(); t != treasure_types.end(); t++) li.push_back(t->first);
    }, [] (const string & n) -> mob_type* {
        auto it = treasure_types.find(n); if(it == treasure_types.end()) return NULL; return it->second;
    });
}

void init_sector_types() {
    sector_types.register_type(SECTOR_TYPE_NORMAL, "Normal");
    sector_types.register_type(SECTOR_TYPE_BOTTOMLESS_PIT, "Bottomless pit");
    sector_types.register_type(SECTOR_TYPE_LANDING_SITE, "Landing site");
    sector_types.register_type(SECTOR_TYPE_WALL, "Wall");
}

void init_special_mob_types() {
    mob_type* info_spot_mt = new mob_type();
    info_spot_mt->name = "Info spot";
    info_spot_mt->radius = 16;
    info_spot_mt->create_mob = [] (float x, float y, float angle, const string & vars) {
        create_mob(new info_spot(x, y, angle, vars));
    };
    spec_mob_types["Info spot"] = info_spot_mt;
    
    mob_type* nectar_mt = new mob_type();
    nectar_mt->name = "Nectar";
    nectar_mt->always_active = true;
    nectar_mt->radius = 8;
    nectar_mt->create_mob = [] (float x, float y, float angle, const string & vars) {
        create_mob(new nectar(x, y, vars));
    };
    spec_mob_types["Nectar"] = nectar_mt;
}