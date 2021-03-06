/*
 * Copyright (c) Andr� 'Espyo' Silva 2013-2015.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the enemy type class and enemy type-related functions.
 */

#ifndef ENEMY_TYPE_INCLUDED
#define ENEMY_TYPE_INCLUDED

#include <string>
#include <vector>

#include "hazard.h"
#include "mob_type.h"

using namespace std;

/* ----------------------------------------------------------------------------
 * A type of enemy. A species, if you will.
 */
class enemy_type : public mob_type {
public:
    unsigned char pikmin_seeds;
    float value;
    float revive_speed;
    float regenerate_speed;
    bool is_boss;
    bool drops_corpse;
};

#endif // ifndef ENEMY_TYPE_INCLUDED
