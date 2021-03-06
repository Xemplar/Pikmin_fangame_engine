/*
 * Copyright (c) Andr� 'Espyo' Silva 2013-2015.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Header for the info spot class and info spot-related functions.
 */

#ifndef INFO_POINT_INCLUDED
#define INFO_POINT_INCLUDED

#include <string>

#include "../mob.h"

using namespace std;

/* ----------------------------------------------------------------------------
 * An info spot is basically like a sign post:
 * it's scattered around the map and contains info.
 * It's not a thing in any Pikmin game, but I figured
 * it could be useful.
 * When the player approaches it, its text appears.
 * If the text is too big, the player can "attack" near it,
 * in order to open a message box, where more info fits.
 */
class info_spot : public mob {
public:
    string text;
    bool opens_box; // If true, clicking while near this info spot opens a message box with the full text.
    unsigned int text_w; // Used instead of calculating the width every time.
    
    info_spot(const float x, const float y, const float angle, const string &vars);
};

#endif // ifndef INFO_POINT_INCLUDED
