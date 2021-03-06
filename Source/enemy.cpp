/*
 * Copyright (c) Andr� 'Espyo' Silva 2013-2015.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Enemy class and enemy-related functions.
 */

#include "enemy.h"
#include "functions.h"

/* ----------------------------------------------------------------------------
 * Creates an enemy.
 */
enemy::enemy(const float x, const float y, enemy_type* type, const float angle, const string &vars)
    : mob(x, y, type, angle, vars) {
    
    ene_type = type;
    team = MOB_TEAM_ENEMIES_1; // TODO removeish.
    
    spawn_delay = s2f(get_var_value(vars, "spawn_delay", "0"));
    // TODO day apperance interval
}
