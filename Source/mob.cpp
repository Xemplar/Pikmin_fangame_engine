/*
 * Copyright (c) Andr� 'Espyo' Silva 2013-2015.
 * The following source file belongs to the open-source project
 * Pikmin fangame engine. Please read the included README file
 * for more information.
 * Pikmin is copyright (c) Nintendo.
 *
 * === FILE DESCRIPTION ===
 * Mob class and mob-related functions.
 */

#include <algorithm>

#include "const.h"
#include "functions.h"
#include "mob.h"
#include "pikmin.h"
#include "ship.h"
#include "vars.h"

/* ----------------------------------------------------------------------------
 * Creates a mob.
 */
mob::mob(const float x, const float y, mob_type* type, const float angle, const string &vars) {
    this->x = x;
    this->y = y;
    this->type = type;
    this->angle = angle;
    
    sector* sec = get_sector(x, y, NULL, true);
    z = sec->z;
    ground_z = sec->z;
    lighting = sec->brightness;
    intended_angle = angle;
    
    anim = animation_instance(&type->anims);
    
    to_delete = false;
    reached_destination = false;
    
    speed_x = speed_y = speed_z = 0;
    home_x = x; home_y = y;
    affected_by_gravity = true;
    
    //TODO TEMP, REMOVE.
    if(type->max_health == 750) {
        float a = 0;
    }
    health = type->max_health;
    invuln_period = 0;
    knockdown_period = 0;
    team = MOB_TEAM_DECORATION;
    
    go_to_target = false;
    gtt_instant = false;
    target_x = x;
    target_y = y;
    target_z = NULL;
    target_rel_x = NULL;
    target_rel_y = NULL;
    can_move = true;
    
    focused_opponent = NULL;
    focused_opponent_near = false;
    
    fsm = mob_fsm(this);
    set_state(type->first_state_nr);
    spawned = false;
    timer = timer_interval = 0;
    script_wait_event = NULL;
    dead = false;
    state = MOB_STATE_IDLE;
    time_in_state = 0;
    
    following_party = NULL;
    was_thrown = false;
    unwhistlable_period = 0;
    untouchable_period = 0;
    party = NULL;
    
    carrier_info = NULL;
}


/* ----------------------------------------------------------------------------
 * Makes the mob follow a game tick.
 * This basically calls sub-tickers.
 * Think of it this way: when you want to go somewhere,
 * you first think about rotating your body to face that
 * point, and then think about moving your legs.
 * Then, the actual physics go into place, your nerves
 * send signals to the muscles, and gravity, intertia, etc.
 * take over the rest, to make you move.
 */
void mob::tick() {
    tick_brain();
    tick_physics();
    tick_misc_logic();
    tick_script();
    tick_animation();
}


/* ----------------------------------------------------------------------------
 * Ticks one game frame into the mob's animations.
 */
void mob::tick_animation() {
    bool finished_anim = anim.tick(delta_t);
    
    // TODO
    if(finished_anim) {
        fsm.run_event(MOB_EVENT_ANIMATION_END, this);
    }
}


/* ----------------------------------------------------------------------------
 * Ticks the mob's brain for the next frame.
 * This has nothing to do with the mob's individual script.
 * This is related to mob-global things, like
 * thinking about where to move next and such.
 */
void mob::tick_brain() {
    // Chasing a target.
    if(go_to_target && !gtt_instant && speed_z == 0 && knockdown_period == 0) {
    
        // Calculate where the target is.
        float final_target_x, final_target_y;
        get_final_target(&final_target_x, &final_target_y);
        
        if(!gtt_instant) {
        
            if(
                !(fabs(final_target_x - x) < target_distance &&
                  fabs(final_target_y - y) < target_distance)
            ) {
                // If it still hasn't reached its target (or close enough to the target),
                // time to make it think about how to get there.
                
                // Let the mob think about facing the actual target.
                face(atan2(final_target_y - y, final_target_x - x));
                // Let the mob think about moving forward.
                speed = type->move_speed;
                
            } else {
                // Reached the location. The mob should now think
                // about stopping.
                
                speed = 0;
                reached_destination = true;
            }
            
        }
    }
}


/* ----------------------------------------------------------------------------
 * Performs some logic code for this game frame.
 */
void mob::tick_misc_logic() {
    // Other things.
    if(unwhistlable_period > 0) {
        unwhistlable_period -= delta_t;
        unwhistlable_period = max(unwhistlable_period, 0.0f);
    }
    if(untouchable_period > 0) {
        untouchable_period -= delta_t;
        untouchable_period = max(untouchable_period, 0.0f);
    }
    
    if(party) {
        float party_center_mx = 0, party_center_my = 0;
        move_point(
            party->party_center_x, party->party_center_y,
            x, y,
            type->move_speed,
            get_leader_to_group_center_dist(this),
            &party_center_mx, &party_center_my, NULL, NULL
        );
        party->party_center_x += party_center_mx * delta_t;
        party->party_center_y += party_center_my * delta_t;
        
        size_t n_members = party->members.size();
        for(size_t m = 0; m < n_members; m++) {
            party->members[m]->face(atan2(y - party->members[m]->y, x - party->members[m]->x));
        }
    }
    
    time_in_state += delta_t;
    
    if(invuln_period > 0) {
        invuln_period -= delta_t;
        invuln_period = max(invuln_period, 0.0f);
    }
    
    if(speed_z == 0) {
        if(knockdown_period > 0) {
            knockdown_period -= delta_t;
            knockdown_period = max(knockdown_period, 0.0f);
        }
    }
}


/* ----------------------------------------------------------------------------
 * Ticks the mob's actual physics procedures:
 * falling because of gravity, moving forward, etc.
 */
void mob::tick_physics() {
    // Movement.
    bool finished_moving = false;
    bool doing_slide = false;
    
    float new_x = x, new_y = y, new_z = z;
    float new_ground_z = ground_z;
    unsigned char new_lighting = lighting;
    float pre_move_ground_z = ground_z;
    
    float move_speed_x = speed_x;
    float move_speed_y = speed_y;
    
    // Change the facing angle to the angle the mob wants to face.
    if(angle > M_PI)  angle -= M_PI * 2;
    if(angle < -M_PI) angle += M_PI * 2;
    if(intended_angle > M_PI)  intended_angle -= M_PI * 2;
    if(intended_angle < -M_PI) intended_angle += M_PI * 2;
    
    float angle_dif = intended_angle - angle;
    if(angle_dif > M_PI)  angle_dif -= M_PI * 2;
    if(angle_dif < -M_PI) angle_dif += M_PI * 2;
    
    angle += sign(angle_dif) * min((double) (type->rotation_speed * delta_t), (double) fabs(angle_dif));
    
    if(go_to_target) {
        // If the mob is meant to teleport somewhere,
        // let's just do so.
        float final_target_x, final_target_y;
        get_final_target(&final_target_x, &final_target_y);
        
        if(gtt_instant) {
            sector* sec = get_sector(final_target_x, final_target_y, NULL, true);
            if(!sec) {
                // TODO Out of bounds! Kill it!
                
            } else {
                if(target_z) {
                    ground_z = sec->z;
                    z = *target_z;
                }
                speed_x = speed_y = speed_z = 0;
            }
            
        } else if(reached_destination) {
            go_to_target = false;
            speed = 0;
            
        } else {
            if(can_move) {
            
                // Make it go to the direction it wants.
                float d = dist(x, y, final_target_x, final_target_y);
                float move_amount = min((double) (d / delta_t), (double) speed);
                
                float movement_angle = gtt_free_move ?
                                       atan2(final_target_y - y, final_target_x - x) :
                                       angle;
                                       
                move_speed_x = cos(movement_angle) * move_amount;
                move_speed_y = sin(movement_angle) * move_amount;
            }
        }
    }
    
    if(!can_move) {
        // If it can't consciously move, let the physics do its thing.
        
        move_speed_x = speed_x;
        move_speed_y = speed_y;
        
    }
    
    
    // Try placing it in the place it should be at, judging
    // from the movement speed.
    while(!finished_moving) {
    
        if(move_speed_x != 0 || move_speed_y != 0) {
        
            // Trying to move to a new spot. Check sector collisions.
            bool successful_move = true;
            
            new_x = x + delta_t* move_speed_x;
            new_y = y + delta_t* move_speed_y;
            new_z = z;
            new_ground_z = ground_z;
            new_lighting = lighting;
            
            float move_angle;
            float move_speed;
            coordinates_to_angle(speed_x, speed_y, &move_angle, &move_speed);
            
            sector* base_sector = get_sector(new_x, new_y, NULL, true);
            if(!base_sector) {
                // TODO out of bounds! Kill it!
                break;
            } else {
                new_ground_z = base_sector->z;
                new_lighting = base_sector->brightness;
            }
            
            // Use the bounding box to know which blockmap blocks the mob is on.
            size_t bx1 = cur_area_map.bmap.get_col(new_x - type->radius);
            size_t bx2 = cur_area_map.bmap.get_col(new_x + type->radius);
            size_t by1 = cur_area_map.bmap.get_row(new_y - type->radius);
            size_t by2 = cur_area_map.bmap.get_row(new_y + type->radius);
            
            if(
                bx1 == string::npos || bx2 == string::npos ||
                by1 == string::npos || by2 == string::npos
            ) {
                // TODO out of bounds! Kill it!
                break;
            }
            
            float slide_angle = move_angle;
            float slide_angle_dif = -(M_PI * 4);
            float highest_z = 0;
            bool have_highest_z = false;
            
            linedef* l_ptr = NULL;
            // Check the linedefs inside the blockmaps for collisions.
            for(size_t bx = bx1; bx <= bx2; bx++) {
                for(size_t by = by1; by <= by2; by++) {
                
                    vector<linedef*>* linedefs = &cur_area_map.bmap.linedefs[bx][by];
                    
                    for(size_t l = 0; l < linedefs->size(); l++) {
                        l_ptr = (*linedefs)[l];
                        if(
                            circle_intersects_line(
                                new_x, new_y, type->radius,
                                l_ptr->vertices[0]->x, l_ptr->vertices[0]->y,
                                l_ptr->vertices[1]->x, l_ptr->vertices[1]->y,
                                NULL, NULL
                            )
                        ) {
                        
                            // If the mob intersects with the linedef, it means it's on both sectors.
                            // Check if any of the sectors say the mob shouldn't be there.
                            // Also, adjust height for steps and the like.
                            for(size_t s = 0; s < 2; s++) {
                                sector* s_ptr = l_ptr->sectors[s];
                                
                                if(!have_highest_z) {
                                    have_highest_z = true;
                                    highest_z = s_ptr->z;
                                }
                                
                                if(!s_ptr) {
                                    // TODO out of bounds! Kill it!
                                    break;
                                    
                                } else {
                                
                                    if(s_ptr->type == SECTOR_TYPE_WALL) {
                                        // The mob cannot be inside a wall-type sector whatsoever.
                                        successful_move = false;
                                        
                                    } else {
                                        if(s_ptr->z > highest_z) highest_z = s_ptr->z;
                                        if(s_ptr->z <= new_z && s_ptr->z > new_ground_z) new_ground_z = s_ptr->z;
                                    }
                                }
                                
                                if(new_z >= s_ptr->z - SECTOR_STEP && s_ptr->z > new_z && new_z == new_ground_z) {
                                    // Only step up a step if it's on the ground.
                                    new_z = s_ptr->z;
                                    new_ground_z = s_ptr->z;
                                }
                                if(highest_z > new_z) successful_move = false;
                            }
                            
                            // Save the angle for the sliding, in case this linedef blocked something.
                            if(!doing_slide && !successful_move) {
                                float wall_angle1 = normalize_angle(atan2(l_ptr->vertices[1]->y - l_ptr->vertices[0]->y, l_ptr->vertices[1]->x - l_ptr->vertices[0]->x));
                                float wall_angle2 = normalize_angle(wall_angle1 + M_PI);
                                // Because we don't know which side of the wall the mob is on, just try both angles.
                                float dif1 = get_angle_smallest_dif(wall_angle1, move_angle);
                                float dif2 = get_angle_smallest_dif(wall_angle2, move_angle);
                                
                                if(abs(dif1 - dif2) < 0.01) {
                                    // Just forget the slide, you're slamming against the wall head-on, perpendicularly.
                                    successful_move = false;
                                    doing_slide = true;
                                    
                                } else {
                                
                                    float next_angle_dif, next_angle;
                                    if(dif1 < dif2) {
                                        next_angle_dif = dif1;
                                        next_angle = wall_angle1;
                                    } else {
                                        next_angle_dif = dif2;
                                        next_angle = wall_angle2;
                                    }
                                    
                                    if(next_angle_dif > slide_angle_dif) {
                                        slide_angle_dif = next_angle_dif;
                                        slide_angle = next_angle;
                                    }
                                }
                                
                            }
                        }
                    }
                }
            }
            
            if(successful_move) {
                x = new_x;
                y = new_y;
                z = new_z;
                ground_z = new_ground_z;
                lighting = new_lighting;
                finished_moving = true;
                
            } else {
            
                // Try sliding.
                if(doing_slide) {
                    // We already tried... Let's just stop completely.
                    speed_x = 0;
                    speed_y = 0;
                    finished_moving = true;
                    
                } else {
                
                    doing_slide = true;
                    move_speed *= 1 - (slide_angle_dif / M_PI);
                    angle_to_coordinates(slide_angle, move_speed, &move_speed_x, &move_speed_y);
                    
                }
                
            }
            
        } else {
        
            finished_moving = true;
            
        }
    }
    
    
    // Vertical movement.
    // If the current ground is one step (or less) below
    // the previous ground, just instantly go down the step.
    if(pre_move_ground_z - ground_z <= SECTOR_STEP && z == pre_move_ground_z) {
        z = ground_z;
    }
    
    bool was_airborne = z > ground_z;
    z += delta_t* speed_z;
    if(z <= ground_z) {
        z = ground_z;
        if(was_airborne) {
            speed_x = 0;
            speed_y = 0;
            speed_z = 0;
            was_thrown = false;
        }
    }
    
    // Gravity.
    if(z > ground_z && affected_by_gravity) {
        speed_z += delta_t* (GRAVITY_ADDER);
    }
}


/* ----------------------------------------------------------------------------
 * Checks general events in the mob's script for this frame.
 */
void mob::tick_script() {
    if(!spawned) {
        fsm.set_state(type->first_state_nr);
        spawned = true;
    }
    
    //TODO move these to the logic code, on the code where all mobs interact with all mobs.
    mob_event* see_opponent_ev  = fsm.get_event(MOB_EVENT_SEE_OPPONENT);
    mob_event* lose_opponent_ev = fsm.get_event(MOB_EVENT_LOSE_OPPONENT);
    mob_event* near_opponent_ev = fsm.get_event(MOB_EVENT_NEAR_OPPONENT);
    if(see_opponent_ev || lose_opponent_ev || near_opponent_ev) {
    
        mob* real_opponent = NULL;
        if(focused_opponent) if(!focused_opponent->dead) real_opponent = focused_opponent;
        
        if(real_opponent) {
            float d = dist(x, y, real_opponent->x, real_opponent->y);
            
            // Opponent is near.
            if(d <= type->near_radius) {
                focused_opponent_near = true;
                if(near_opponent_ev) near_opponent_ev->run(this);
            }
            
            if(d > type->sight_radius) {
                // Opponent is suddenly out of sight.
                unfocus_mob(this, real_opponent, true);
                
            } else {
            
                // Opponent was near, but is now far.
                if(focused_opponent_near) {
                    if(d > type->near_radius) {
                        focused_opponent_near = false;
                    }
                }
            }
            
        } else {
        
            //Find an opponent.
            for(auto m = mobs.begin(); m != mobs.end(); m++) {
                if(should_attack(this, *m)) {
                    float d = dist(x, y, (*m)->x, (*m)->y);
                    if(d <= type->sight_radius) {
                        focus_mob(this, *m, d <= type->near_radius, true);
                        break;
                    }
                }
            }
            
        }
        
    }
    
    //Timer events.
    mob_event* timer_ev = fsm.get_event(MOB_EVENT_TIMER);
    if(timer_ev) {
        if(timer > 0 && timer_interval > 0) {
            timer -= delta_t;
            if(timer <= 0) {
                timer = timer_interval;
                timer_ev->run(this);
            }
        }
    }
    
    //Has it reached its home?
    mob_event* reach_home_ev = fsm.get_event(MOB_EVENT_REACH_HOME);
    if(reach_home_ev) {
        if(reached_destination && target_code == MOB_TARGET_HOME) {
            target_code = MOB_TARGET_NONE;
            reach_home_ev->run(this);
        }
    }
    
    //Is it dead?
    if(!dead && health <= 0) {
        dead = true;
        fsm.run_event(MOB_EVENT_DEATH, this);
    }
}



/* ----------------------------------------------------------------------------
 * Returns the actual location of the movement target.
 */
void mob::get_final_target(float* x, float* y) {
    *x = target_x;
    *y = target_y;
    if(target_rel_x) *x += *target_rel_x;
    if(target_rel_y) *y += *target_rel_y;
}


/* ----------------------------------------------------------------------------
 * Sets a target for the mob to follow.
 * target_*:     Coordinates of the target, relative to either the world origin,
   * or another point, specified in the next parameters.
 * target_rel_*: Pointers to moving coordinates. If NULL, it's the world origin.
   * Use this to make the mob follow another mob wherever they go, for instance.
 * instant:      If true, the mob teleports to that spot, instead of walking to it.
 * target_z:     Teleports to this Z coordinate, too.
 * free_move:    If true, the mob can go to a direction they're not facing.
 * target_distance: Distance from the target in which the mob is considered as being there.
 */
void mob::set_target(float target_x, float target_y, float* target_rel_x, float* target_rel_y, bool instant, float* target_z, bool free_move, float target_distance) {
    if(this->knockdown_period > 0 && !instant) return;
    
    this->target_x = target_x; this->target_y = target_y;
    this->target_rel_x = target_rel_x; this->target_rel_y = target_rel_y;
    this->gtt_instant = instant;
    this->target_z = target_z;
    this->gtt_free_move =
        this->target_distance = target_distance;
        
    go_to_target = true;
    reached_destination = false;
    target_code = MOB_TARGET_NONE;
}


/* ----------------------------------------------------------------------------
 * Makes a mob not follow any target.
 * stop: If true, the mob stops dead on its tracks.
 */
void mob::remove_target(bool stop) {
    go_to_target = false;
    reached_destination = false;
    target_z = NULL;
    
    if(stop) {
        speed_x = 0;
        speed_y = 0;
        speed_z = 0;
    }
}


/* ----------------------------------------------------------------------------
 * Makes a mob gradually face a new angle.
 */
void mob::face(float new_angle) {
    intended_angle = new_angle;
}


/* ----------------------------------------------------------------------------
 * Changes a mob's state.
 * new_state: The new state ID. Use MOB_STATE_*.
 */
void mob::set_state(unsigned char new_state) {
    state = new_state;
    time_in_state = 0;
}


/* ----------------------------------------------------------------------------
 * Returns the base speed for this mob.
 * This is overwritten by some child classes.
 */
float mob::get_base_speed() {
    return this->type->move_speed;
}


mob::~mob() {}


/* ----------------------------------------------------------------------------
 * Creates a structure with info about carrying.
 * m:             The mob this info belongs to.
 * max_carriers:  The maximum number of carrier Pikmin.
 * carry_to_ship: If true, this mob is delivered to a ship. Otherwise, an Onion.
 */
carrier_info_struct::carrier_info_struct(mob* m, unsigned int max_carriers, bool carry_to_ship) {
    this->max_carriers = max_carriers;
    this->carry_to_ship = carry_to_ship;
    
    current_carrying_strength = 0;
    current_n_carriers = 0;
    decided_type = NULL;
    
    for(size_t c = 0; c < max_carriers; c++) {
        carrier_spots.push_back(NULL);
        float angle = (M_PI * 2) / max_carriers * c;
        carrier_spots_x.push_back(cos(angle) * m->type->radius);
        carrier_spots_y.push_back(sin(angle) * m->type->radius);
    }
}


/* ----------------------------------------------------------------------------
 * Deletes a carrier info structure.
 * Makes all carrying Pikmin drop it in the process.
 */
carrier_info_struct::~carrier_info_struct() {
    for(size_t s = 0; s < max_carriers; s++) {
        if(carrier_spots[s]) {
            if(typeid(*carrier_spots[s]) == typeid(pikmin)) {
                drop_mob((pikmin*) carrier_spots[s]);
            }
        }
    }
}


/* ----------------------------------------------------------------------------
 * Adds a mob to another mob's party.
 */
void add_to_party(mob* party_leader, mob* new_member) {
    if(new_member->following_party == party_leader) return; // Already following, never mind.
    
    new_member->following_party = party_leader;
    party_leader->party->members.push_back(new_member);
    
    // Find a spot.
    if(party_leader->party) {
        if(party_leader->party->party_spots) {
            float spot_x = 0, spot_y = 0;
            
            party_leader->party->party_spots->add(new_member, &spot_x, &spot_y);
            
            new_member->set_target(
                spot_x, spot_y,
                &party_leader->party->party_center_x, &party_leader->party->party_center_y,
                false
            );
        }
    }
    
    make_uncarriable(new_member);
}


/* ----------------------------------------------------------------------------
 * Makes m1 attack m2.
 * Stuff like status effects and maturity (Pikmin only) are taken into account.
 */
void attack(mob* m1, mob* m2, const bool m1_is_pikmin, const float damage, const float angle, const float knockback, const float new_invuln_period, const float new_knockdown_period) {
    if(m2->invuln_period > 0) return;
    
    pikmin* p_ptr = NULL;
    float total_damage = damage;
    if(m1_is_pikmin) {
        p_ptr = (pikmin*) m1;
        total_damage += p_ptr->maturity * damage * MATURITY_POWER_MULT;
    }
    
    m2->invuln_period = new_invuln_period;
    m2->knockdown_period = new_knockdown_period;
    m2->health -= damage;
    
    if(knockback != 0) {
        m2->remove_target(true);
        m2->speed_x = cos(angle) * knockback;
        m2->speed_y = sin(angle) * knockback;
        m2->speed_z = 500;
    }
    
    m2->fsm.run_event(MOB_EVENT_DAMAGE, m2);
    
    // If before taking damage, the interval was dividable X times, and after it's only dividable by Y (X>Y), an interval was crossed.
    if(m2->type->big_damage_interval > 0 && m2->health != m2->type->max_health) {
        if(floor((m2->health + damage) / m2->type->big_damage_interval) > floor(m2->health / m2->type->big_damage_interval)) {
            m2->fsm.run_event(MOB_EVENT_BIG_DAMAGE, m2);
        }
    }
}


/* ----------------------------------------------------------------------------
 * Creates a mob, adding it to the corresponding vectors.
 */
void create_mob(mob* m) {
    mobs.push_back(m);
    
    if(typeid(*m) == typeid(pikmin)) {
        pikmin_list.push_back((pikmin*) m);
        
    } else if(typeid(*m) == typeid(leader)) {
        leaders.push_back((leader*) m);
        
    } else if(typeid(*m) == typeid(onion)) {
        onions.push_back((onion*) m);
        
    } else if(typeid(*m) == typeid(nectar)) {
        nectars.push_back((nectar*) m);
        
    } else if(typeid(*m) == typeid(pellet)) {
        pellets.push_back((pellet*) m);
        
    } else if(typeid(*m) == typeid(ship)) {
        ships.push_back((ship*) m);
        
    } else if(typeid(*m) == typeid(treasure)) {
        treasures.push_back((treasure*) m);
        
    } else if(typeid(*m) == typeid(info_spot)) {
        info_spots.push_back((info_spot*) m);
        
    } else if(typeid(*m) == typeid(enemy)) {
        enemies.push_back((enemy*) m);
        
    }
}


/* ----------------------------------------------------------------------------
 * Deletes a mob from the relevant vectors.
 * It's always removed from the vector of mobs, but it's
 * also removed from the vector of Pikmin if it's a Pikmin,
 * leaders if it's a leader, etc.
 */
void delete_mob(mob* m) {
    remove_from_party(m);
    vector<mob*> focusers = m->focused_by;
    for(size_t m_nr = 0; m_nr < focusers.size(); m_nr++) {
        unfocus_mob(focusers[m_nr], m, true);
    }
    
    mobs.erase(find(mobs.begin(), mobs.end(), m));
    
    if(typeid(*m) == typeid(pikmin)) {
        pikmin* p_ptr = (pikmin*) m;
        drop_mob(p_ptr);
        pikmin_list.erase(find(pikmin_list.begin(), pikmin_list.end(), p_ptr));
        
    } else if(typeid(*m) == typeid(leader)) {
        leaders.erase(find(leaders.begin(), leaders.end(), (leader*) m));
        
    } else if(typeid(*m) == typeid(onion)) {
        onions.erase(find(onions.begin(), onions.end(), (onion*) m));
        
    } else if(typeid(*m) == typeid(nectar)) {
        nectars.erase(find(nectars.begin(), nectars.end(), (nectar*) m));
        
    } else if(typeid(*m) == typeid(pellet)) {
        pellets.erase(find(pellets.begin(), pellets.end(), (pellet*) m));
        
    } else if(typeid(*m) == typeid(ship)) {
        ships.erase(find(ships.begin(), ships.end(), (ship*) m));
        
    } else if(typeid(*m) == typeid(treasure)) {
        treasures.erase(find(treasures.begin(), treasures.end(), (treasure*) m));
        
    } else if(typeid(*m) == typeid(info_spot)) {
        info_spots.erase(find(info_spots.begin(), info_spots.end(), (info_spot*) m));
        
    } else if(typeid(*m) == typeid(enemy)) {
        enemies.erase(find(enemies.begin(), enemies.end(), (enemy*) m));
        
    } else {
        // TODO warn somehow.
        
    }
    
    delete m;
}


/* ----------------------------------------------------------------------------
 * Makes m1 focus on m2.
 */
void focus_mob(mob* m1, mob* m2, const bool is_near, const bool call_event) {
    unfocus_mob(m1, m1->focused_opponent, false);
    
    m1->focused_opponent = m2;
    m1->focused_opponent_near = true;
    m2->focused_by.push_back(m1);
    
    if(call_event) {
        m1->focused_opponent_near = is_near;
        if(is_near) m1->fsm.run_event(MOB_EVENT_NEAR_OPPONENT, m1);
        else m1->fsm.run_event(MOB_EVENT_SEE_OPPONENT, m1);
    }
}


/* ----------------------------------------------------------------------------
 * Returns the closest hitbox to a point, belonging to a mob's current frame of animation and position.
 * x, y: Point.
 * m:    The mob.
 */
hitbox_instance* get_closest_hitbox(const float x, const float y, mob* m) {
    frame* f = m->anim.get_frame();
    if(!f) return NULL;
    hitbox_instance* closest_hitbox = NULL;
    float closest_hitbox_dist = 0;
    
    for(size_t h = 0; h < f->hitbox_instances.size(); h++) {
        hitbox_instance* h_ptr = &f->hitbox_instances[h];
        float hx, hy;
        rotate_point(h_ptr->x, h_ptr->y, m->angle, &hx, &hy);
        float d = dist(x - m->x, y - m->y, hx, hy) - h_ptr->radius;
        if(h == 0 || d < closest_hitbox_dist) {
            closest_hitbox_dist = d;
            closest_hitbox = h_ptr;
        }
    }
    
    return closest_hitbox;
}


/* ----------------------------------------------------------------------------
 * Returns the hitbox instance in the current animation with the specified name.
 */
hitbox_instance* get_hitbox_instance(mob* m, const size_t nr) {
    frame* f = m->anim.get_frame();
    for(size_t h = 0; h < f->hitbox_instances.size(); h++) {
        hitbox_instance* h_ptr = &f->hitbox_instances[h];
        if(h_ptr->hitbox_nr == nr) return h_ptr;
    }
    return NULL;
}


/* ----------------------------------------------------------------------------
 * Makes a mob impossible to be carried, and makes the Pikmin carrying it drop it.
 */
void make_uncarriable(mob* m) {
    if(!m->carrier_info) return;
    
    delete m->carrier_info;
    m->carrier_info = NULL;
}


/* ----------------------------------------------------------------------------
 * Removes a mob from its leader's party.
 */
void remove_from_party(mob* member) {
    if(!member->following_party) return;
    
    member->following_party->party->members.erase(find(
                member->following_party->party->members.begin(),
                member->following_party->party->members.end(),
                member));
                
    if(member->following_party->party->party_spots) {
        member->following_party->party->party_spots->remove(member);
    }
    
    member->following_party = NULL;
    member->remove_target(false);
    member->unwhistlable_period = UNWHISTLABLE_PERIOD;
    member->untouchable_period = UNTOUCHABLE_PERIOD;
}


/* ----------------------------------------------------------------------------
 * Should m1 attack m2? Teams are used to decide this.
 */
bool should_attack(mob* m1, mob* m2) {
    if(m2->team == MOB_TEAM_DECORATION) return false;
    if(m1->team == MOB_TEAM_NONE) return true;
    if(m1->team == m2->team) return false;
    return true;
}


/* ----------------------------------------------------------------------------
 * Makes m1 lose focus on m2.
 */
void unfocus_mob(mob* m1, mob* m2, const bool call_event) {
    if(m2) {
        if(m1->focused_opponent != m2) return;
        
        for(size_t m = 0; m < m2->focused_by.size();) {
            if(m2->focused_by[m] == m1) m2->focused_by.erase(m2->focused_by.begin() + m);
            else m++;
        }
    }
    
    m1->focused_opponent = NULL;
    m1->focused_opponent_near = false;
    if(call_event) {
        m1->fsm.run_event(MOB_EVENT_LOSE_OPPONENT, m1);
    }
}
