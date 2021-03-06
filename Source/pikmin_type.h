/*
 * Copyright (c) Andr� 'Espyo' Silva 2013-2015.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the Pikmin type class and Pikmin type-related functions.
 */

#ifndef PIKMIN_TYPE_INCLUDED
#define PIKMIN_TYPE_INCLUDED

#include <string>
#include <vector>

#include <allegro5/allegro.h>

#include "hazard.h"
#include "mob_type.h"

using namespace std;

/* ----------------------------------------------------------------------------
 * Pikmin types, almost the basic meat of the fan-games.
 * The canon ones are Red, Yellow, Blue, White,
 * Purple, Bulbmin, Winged and Rock, but with the engine,
 * loads of fan-made ones can be made.
 */
class pikmin_type : public mob_type {
public:
    vector<hazard*> resistences;
    unsigned char attack_attribute; // TODO What.
    float carry_strength;
    float attack_power;
    float attack_interval;
    float weight;
    float carry_speed;
    float size;
    float throw_height_mult;
    bool has_onion;
    bool can_dig;
    bool can_fly;
    bool can_swim;
    bool can_latch;
    bool can_carry_bomb_rocks;
    ALLEGRO_BITMAP* bmp_top[3]; // Top (leaf/bud/flower) bitmap for each maturity.
    ALLEGRO_BITMAP* bmp_icon[3]; // Standby icons for each maturity.
    
    pikmin_type();
};



enum PIKMIN_ANIMATIONS {
    PIKMIN_ANIM_IDLE,
    PIKMIN_ANIM_WALK,
    PIKMIN_ANIM_THROWN,
    PIKMIN_ANIM_ATTACK,
    PIKMIN_ANIM_GRAB,
    PIKMIN_ANIM_BURROWED,
    PIKMIN_ANIM_LYING,
    PIKMIN_ANIM_GET_UP,
};

#endif // ifndef PIKMIN_TYPE_INCLUDED
