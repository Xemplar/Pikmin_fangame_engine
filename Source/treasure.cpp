#include "treasure.h"

treasure::treasure(const float x, const float y, sector* sec, treasure_type* type)
    : mob(x, y, sec->floors[0].z, type, sec) {
    
    carrier_info = new carrier_info_struct(this, type->max_carriers, true);
}