#include <typeinfo>

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>

#include "const.h"
#include "controls.h"
#include "functions.h"
#include "vars.h"

void handle_controls(ALLEGRO_EVENT ev){
	//Debugging.
	if(ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_T){
		//Debug testing.
		//ToDo remove.
		//leaders[current_leader]->health--;
		day_minutes += 60;
	}

	size_t n_controls = controls.size();
	for(size_t c=0; c<n_controls; c++){

		if(controls[c].type == CONTROL_TYPE_KEYBOARD_KEY && (ev.type == ALLEGRO_EVENT_KEY_DOWN || ev.type == ALLEGRO_EVENT_KEY_UP)){
 			if(controls[c].button == ev.keyboard.keycode){
				handle_button(controls[c].action, (ev.type == ALLEGRO_EVENT_KEY_DOWN) ? 1 : 0);
			}
		}else if(controls[c].type == CONTROL_TYPE_MOUSE_BUTTON && (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN || ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP)){
			if(controls[c].button == (signed) ev.mouse.button){
				handle_button(controls[c].action, (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) ? 1 : 0);
			}
		}else if(controls[c].type == CONTROL_TYPE_MOUSE_WHEEL_UP && ev.type == ALLEGRO_EVENT_MOUSE_AXES){
			if(ev.mouse.dz > 0){
				handle_button(controls[c].action, ev.mouse.dz);
			}
		}else if(controls[c].type == CONTROL_TYPE_MOUSE_WHEEL_DOWN && ev.type == ALLEGRO_EVENT_MOUSE_AXES){
			if(ev.mouse.dz < 0){
				handle_button(controls[c].action, -ev.mouse.dz);
			}
		}else if(controls[c].type == CONTROL_TYPE_MOUSE_WHEEL_LEFT && ev.type == ALLEGRO_EVENT_MOUSE_AXES){
			if(ev.mouse.dw < 0){
				handle_button(controls[c].action, -ev.mouse.dw);
			}
		}else if(controls[c].type == CONTROL_TYPE_MOUSE_WHEEL_RIGHT && ev.type == ALLEGRO_EVENT_MOUSE_AXES){
			if(ev.mouse.dw > 0){
				handle_button(controls[c].action, ev.mouse.dz);
			}
		}else if(controls[c].type == CONTROL_TYPE_JOYSTICK_BUTTON && (ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN || ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_UP)){
			if(controls[c].device_nr == joystick_numbers[ev.joystick.id] && (signed) controls[c].button == ev.joystick.button){
				handle_button(controls[c].action, (ev.type == ALLEGRO_EVENT_JOYSTICK_BUTTON_DOWN) ? 1 : 0);
			}
		}else if(controls[c].type == CONTROL_TYPE_JOYSTICK_AXIS_POS && ev.type == ALLEGRO_EVENT_JOYSTICK_AXIS){
			if(
				controls[c].device_nr == joystick_numbers[ev.joystick.id] && controls[c].stick == ev.joystick.stick &&
				controls[c].axis == ev.joystick.axis && ev.joystick.pos >= 0){
					handle_button(controls[c].action, ev.joystick.pos);
			}
		}else if(controls[c].type == CONTROL_TYPE_JOYSTICK_AXIS_NEG && ev.type == ALLEGRO_EVENT_JOYSTICK_AXIS){
			if(
				controls[c].device_nr == joystick_numbers[ev.joystick.id] && controls[c].stick == ev.joystick.stick &&
				controls[c].axis == ev.joystick.axis && ev.joystick.pos <= 0){
					handle_button(controls[c].action, -ev.joystick.pos);
			}
		}
	}

	for(unsigned char p=0; p<4; p++){
		if(ev.type == ALLEGRO_EVENT_MOUSE_AXES && mouse_moves_cursor[p]){
			mouse_cursor_x = ev.mouse.x;
			mouse_cursor_y = ev.mouse.y;
		}
	}

}

void handle_button(unsigned int button, float pos){

	if(
		button == BUTTON_MOVE_RIGHT ||
		button == BUTTON_MOVE_UP ||
		button == BUTTON_MOVE_LEFT ||
		button == BUTTON_MOVE_DOWN
		){

			/*******************
			*              \O/ *
			*   Move   ---> |  *
			*              / \ *
			*******************/

			if(fabs(pos) < 0.75) pos = 0;
			active_control();

			if(button == BUTTON_MOVE_RIGHT)     leaders[current_leader]->speed_x = LEADER_MOVE_SPEED * pos;
			else if(button == BUTTON_MOVE_UP)   leaders[current_leader]->speed_y = -LEADER_MOVE_SPEED * pos;
			else if(button == BUTTON_MOVE_LEFT) leaders[current_leader]->speed_x = -LEADER_MOVE_SPEED * pos;
			else if(button == BUTTON_MOVE_DOWN) leaders[current_leader]->speed_y = LEADER_MOVE_SPEED * pos;

	}else if(
		button == BUTTON_MOVE_CURSOR_RIGHT ||
		button == BUTTON_MOVE_CURSOR_UP ||
		button == BUTTON_MOVE_CURSOR_LEFT ||
		button == BUTTON_MOVE_CURSOR_DOWN
		){
			/********************
			*             .-.   *
			*   Cursor   ( = )> *
			*             `-�   *
			********************/

			if(button == BUTTON_MOVE_CURSOR_RIGHT)     mouse_cursor_speed_x = (1.0/game_fps) * MOUSE_CURSOR_MOVE_SPEED * pos;
			else if(button == BUTTON_MOVE_CURSOR_UP)   mouse_cursor_speed_y = -(1.0/game_fps) * MOUSE_CURSOR_MOVE_SPEED * pos;
			else if(button == BUTTON_MOVE_CURSOR_LEFT) mouse_cursor_speed_x = -(1.0/game_fps) * MOUSE_CURSOR_MOVE_SPEED * pos;
			else if(button == BUTTON_MOVE_CURSOR_DOWN) mouse_cursor_speed_y = (1.0/game_fps) * MOUSE_CURSOR_MOVE_SPEED * pos;

	}else if(
		button == BUTTON_MOVE_GROUP_RIGHT ||
		button == BUTTON_MOVE_GROUP_UP ||
		button == BUTTON_MOVE_GROUP_LEFT ||
		button == BUTTON_MOVE_GROUP_DOWN
		){
			/******************
			*            ***  *
			*   Group   ****O *
			*            ***  *
			******************/
			
			active_control();

			if(button == BUTTON_MOVE_GROUP_RIGHT)     moving_group_pos_x = pos;
			else if(button == BUTTON_MOVE_GROUP_UP)   moving_group_pos_y = -pos;
			else if(button == BUTTON_MOVE_GROUP_LEFT) moving_group_pos_x = -pos;
			else if(button == BUTTON_MOVE_GROUP_DOWN) moving_group_pos_y = pos;

	}else if(button == BUTTON_MOVE_GROUP_TO_CURSOR){

		active_control();

		if(pos > 0){
			moving_group_to_cursor = true;
			moving_group_intensity = 1;
		}else{
			moving_group_to_cursor = false;
			moving_group_intensity = 0;
		}

	}else if(button == BUTTON_PUNCH){

		/*******************
		*            .--._ *
		*   Punch   ( U  _ *
		*            `--�  *
		*******************/

		if(pos > 0){ //Button press.

			if(auto_pluck_input_time > 0){
				leaders[current_leader]->auto_pluck_mode = true;

				size_t n_leaders = leaders.size();
				for(size_t l=0; l<n_leaders; l++){
					if(leaders[l]->following_party == leaders[current_leader]){
						leaders[l]->auto_pluck_mode = true;
					}
				}
			}else{
				active_control();
			}
			
			bool done = false;
				
			//First check if the leader should pluck a Pikmin.
			float d;
			pikmin* p = get_closest_burrowed_pikmin(leaders[current_leader]->x, leaders[current_leader]->y, &d, false);
			if(p && d <= MIN_PLUCK_RANGE){
				pluck_pikmin(leaders[current_leader], p);
				auto_pluck_input_time = AUTO_PLUCK_INPUT_INTERVAL;
				done = true;
			}
	

			//Now check if the leader should open an onion's menu.
			if(!done){
				//ToDo
				size_t n_onions = onions.size();
				for(size_t o=0; o<n_onions; o++){
					if(dist(leaders[current_leader]->x, leaders[current_leader]->y, onions[o]->x, onions[o]->y) < MIN_ONION_CHECK_RANGE){
						if(pikmin_list.size() < max_pikmin_in_field){
							//ToDo this is not how it works, there can be less onions on the field than the total number of Pikmin types.
							pikmin_in_onions[onions[o]->type]--;
							create_mob(new pikmin(onions[o]->type, onions[o]->x, onions[o]->y, onions[o]->sec));
							add_to_party(leaders[current_leader], pikmin_list[pikmin_list.size()-1]);
						}
						done = true;
					}
				}
			}
		
			//Now check if the leader should heal themselves on the ship.
			if(!done){
				size_t n_ships = ships.size();
				for(size_t s=0; s<n_ships; s++){
					if(dist(leaders[current_leader]->x, leaders[current_leader]->y, ships[s]->x + ships[s]->size / 2 + SHIP_BEAM_RANGE, ships[s]->y) < SHIP_BEAM_RANGE){
						//ToDo make it prettier.
						leaders[current_leader]->health = leaders[current_leader]->max_health;
						done = true;
					}
				}
			}

			//Now check if the leader should grab a Pikmin.

			if(!done){
				if(closest_party_member){
					leaders[current_leader]->holding_pikmin = closest_party_member;
					al_play_sample(sfx_pikmin_held.sample, 1, 0.5, 1, ALLEGRO_PLAYMODE_ONCE, &sfx_pikmin_held.id);
					done = true;
				}
			}

			//Now check if the leader should punch.

			if(!done){
				//ToDo
			}

		}else{ //Button release.
			mob* holding_ptr = leaders[current_leader]->holding_pikmin;
			if(holding_ptr){

				holding_ptr->x = leaders[current_leader]->x;
				holding_ptr->y = leaders[current_leader]->y;
				holding_ptr->z = leaders[current_leader]->z;

				float d = dist(leaders[current_leader]->x, leaders[current_leader]->y, cursor_x, cursor_y);
				holding_ptr->speed_x = cos(leaders[current_leader]->angle) * d * THROW_DISTANCE_MULTIPLIER;
				holding_ptr->speed_y = sin(leaders[current_leader]->angle) * d * THROW_DISTANCE_MULTIPLIER;
				holding_ptr->speed_z = 2;

				holding_ptr->was_thrown = true;
				
				remove_from_party(holding_ptr);
				leaders[current_leader]->holding_pikmin = NULL;

				al_stop_sample(&sfx_pikmin_held.id);
				al_stop_sample(&sfx_pikmin_thrown.id);
				al_stop_sample(&sfx_throw.id);
				al_play_sample(sfx_pikmin_thrown.sample, 1, 0.5, 1, ALLEGRO_PLAYMODE_ONCE, &sfx_pikmin_thrown.id);
				al_play_sample(sfx_throw.sample, 1, 0.5, 1, ALLEGRO_PLAYMODE_ONCE, &sfx_throw.id);
			}
		}

	}else if(button == BUTTON_WHISTLE){

		/********************
		*              .--= *
		*   Whistle   ( @ ) *
		*              `-�  *
		********************/

		active_control();

		if(pos > 0){ //Button pressed.
			whistling = true;
			al_play_sample(leaders[current_leader]->sfx_whistle.sample, 1, 0.5, 1, ALLEGRO_PLAYMODE_ONCE, &leaders[current_leader]->sfx_whistle.id);

			for(unsigned char d=0; d<6; d++) whistle_dot_radius[d] = -1;
			whistle_fade_time = 0;
			whistle_fade_radius = 0;

		}else{ //Button released.
			stop_whistling();
		}

	}else if(
		button == BUTTON_SWITCH_CAPTAIN_RIGHT ||
		button == BUTTON_SWITCH_CAPTAIN_LEFT
		){

			/******************************
			*                    \O/  \O/ *
			*   Switch captain    | -> |  *
			*                    / \  / \ *
			******************************/

			if(pos == 0) return;

			size_t new_leader = current_leader;
			if(button == BUTTON_SWITCH_CAPTAIN_RIGHT)
				new_leader = (current_leader + 1) % leaders.size();
			else if(button == BUTTON_SWITCH_CAPTAIN_LEFT){
				if(current_leader == 0) new_leader = leaders.size() - 1;
				else new_leader = current_leader - 1;
			}

			mob* swap_leader = NULL;

			if(!leaders[current_leader]->speed_z){
				leaders[current_leader]->speed_x = 0;
				leaders[current_leader]->speed_y = 0;
			}
			if(!leaders[new_leader]->speed_z){
				leaders[new_leader]->speed_x = 0;
				leaders[new_leader]->speed_y = 0;
			}
			leaders[new_leader]->remove_target(true);

			//If the new leader is in another one's party, swap them.
			size_t n_leaders = leaders.size();
			for(size_t l=0; l<n_leaders; l++){
				if(l==new_leader) continue;
				size_t n_party_members = leaders[l]->party.size();
				for(size_t m=0; m<n_party_members; m++){
					if(leaders[l]->party[m] == leaders[new_leader]){
						swap_leader=leaders[l];
						break;
					}
				}
		}

		if(swap_leader){
			size_t n_party_members = swap_leader->party.size();
			for(size_t m=0; m<n_party_members; m++){
				mob* member = swap_leader->party[0];
				remove_from_party(member);
				if(member != leaders[new_leader]){
					add_to_party(leaders[new_leader], member);
				}
			}
					
			add_to_party(leaders[new_leader], swap_leader);
		}

		current_leader = new_leader;
		start_camera_pan(leaders[new_leader]->x, leaders[new_leader]->y);
		al_play_sample(leaders[new_leader]->sfx_name_call.sample, 1, 0.5, 1, ALLEGRO_PLAYMODE_ONCE, &leaders[new_leader]->sfx_name_call.id);

	}else if(button == BUTTON_DISMISS){

		/***********************
		*             \O/ / *  *
		*   Dismiss    |   - * *
		*             / \ \ *  *
		***********************/

		if(pos == 0) return;

		active_control();

		dismiss();
		al_play_sample(leaders[current_leader]->sfx_dismiss.sample, 1, 0.5, 1, ALLEGRO_PLAYMODE_ONCE, &leaders[current_leader]->sfx_dismiss.id);

	}else if(button == BUTTON_PAUSE){

		/********************
		*           +-+ +-+ *
		*   Pause   | | | | *
		*           +-+ +-+ *
		********************/

		if(pos == 0) return;

		running = false; //ToDo menu, not quit.
		//paused = true;

	}else if(button == BUTTON_USE_SPRAY_1){

		/*******************
		*             +=== *
		*   Sprays   (   ) *
		*             `-�  *
		*******************/
		if(pos == 0) return;

		active_control();
		
		if(spray_types.size() == 1 || spray_types.size() == 2){
			use_spray(0);
		}

	}else if(button == BUTTON_USE_SPRAY_2){

		if(pos == 0) return;

		active_control();

		if(spray_types.size() == 2){
			use_spray(1);
		}

	}else if(button == BUTTON_SWITCH_SPRAY_RIGHT || button == BUTTON_SWITCH_SPRAY_LEFT){

		if(pos == 0) return;

		if(spray_types.size() > 2){
			if(button == BUTTON_SWITCH_SPRAY_RIGHT){
				selected_spray = (selected_spray + 1) % spray_types.size();
			}else{
				if(selected_spray == 0) selected_spray = spray_types.size() - 1;
				else selected_spray--;
			}
		}

	}else if(button == BUTTON_USE_SPRAY){

		if(pos == 0) return;

		active_control();

		if(spray_types.size() > 2){
			use_spray(selected_spray);
		}

	}else if(button == BUTTON_SWITCH_ZOOM){

		/***************
		*           _  *
		*   Zoom   (_) *
		*          /   *
		***************/

		if(pos == 0) return;

		float new_zoom;
		float zoom_to_compare;
		if(cam_trans_zoom_time_left > 0) zoom_to_compare = cam_trans_zoom_final_level; else zoom_to_compare = cam_zoom;

		if(zoom_to_compare < 1){
			new_zoom = MAX_ZOOM_LEVEL;
		}else if(zoom_to_compare > 1){
			new_zoom = 1;
		}else{
			new_zoom = MIN_ZOOM_LEVEL;
		}

		start_camera_zoom(new_zoom);

	}else if(button == BUTTON_ZOOM_IN || button == BUTTON_ZOOM_OUT){

		if((cam_zoom == MAX_ZOOM_LEVEL && button == BUTTON_ZOOM_IN) || (cam_zoom == MIN_ZOOM_LEVEL && button == BUTTON_ZOOM_OUT)) return;

		float new_zoom;
		float current_zoom;
		if(cam_trans_zoom_time_left) current_zoom = cam_trans_zoom_final_level; else current_zoom = cam_zoom;

		pos = floor(pos);

		if(button == BUTTON_ZOOM_IN) new_zoom = current_zoom + 0.1 * pos; else new_zoom = current_zoom - 0.1 * pos;
		
		if(new_zoom > MAX_ZOOM_LEVEL) new_zoom = MAX_ZOOM_LEVEL;
		if(new_zoom < MIN_ZOOM_LEVEL) new_zoom = MIN_ZOOM_LEVEL;
		
		if(cam_trans_zoom_time_left){
			cam_trans_zoom_final_level = new_zoom;
		}else{
			start_camera_zoom(new_zoom);
		}

	}else if(button == BUTTON_LIE_DOWN){

		/**********************
		*                     *
		*   Lie down  -()/__/ *
		*                     *
		***********************/

		if(pos == 0) return;

		leader* leader_ptr = leaders[current_leader];

		if(leader_ptr->carrier_info){
			active_control();
		}else{
			
			dismiss();

			leader_ptr->carrier_info = new carrier_info_struct(
				leader_ptr,
				3, //ToDo
				false);
		}
	}
}

//void handle_button_up(unsigned int button){
//	//ToDo change to angles.
//	if(button == BUTTON_MOVE_UP || button == BUTTON_MOVE_DOWN){
//
//		/*******************
//		*              \O/ *
//		*   Move   ---> |  *
//		*              / \ *
//		*******************/
//
//		leaders[current_leader]->speed_y = 0;
//
//	}else if(button == BUTTON_MOVE_LEFT || button == BUTTON_MOVE_RIGHT){
//		leaders[current_leader]->speed_x = 0;
//
//	}else if(button == BUTTON_MOVE_CURSOR_UP || button == BUTTON_MOVE_CURSOR_DOWN){
//
//		/********************
//		*             .-.   *
//		*   Cursor   ( = )> *
//		*             `-�   *
//		********************/
//
//		mouse_cursor_speed_y = 0;
//
//	}else if(button == BUTTON_MOVE_CURSOR_LEFT || button == BUTTON_MOVE_CURSOR_RIGHT){
//		mouse_cursor_speed_x = 0;
//
//	}else if(button == BUTTON_MOVE_GROUP_TO_CURSOR){
//
//		/******************
//		*            ***  *
//		*   Group   ****O *
//		*            ***  *
//		******************/
//
//		moving_group_to_cursor = false;
//		moving_group_intensity = 0;
//
//	}else if(button == BUTTON_PUNCH){
//
//		/*******************
//		*            .--._ *
//		*   Punch   ( U  _ *
//		*            `--�  *
//		*******************/
//
//		
//
//	}else if(button == BUTTON_WHISTLE){
//
//		/********************
//		*              .--= *
//		*   Whistle   ( @ ) *
//		*              `-�  *
//		********************/
//
//		stop_whistling();
//
//	}
//}
//
//void handle_analog(unsigned int action, float pos){
//	if(action == AXIS_ACTION_MOVE_X || action == AXIS_ACTION_MOVE_Y){
//		/*******************
//		*              \O/ *
//		*   Move   ---> |  *
//		*              / \ *
//		*******************/
//
//		if(fabs(pos) < 0.75) pos = 0;
//
//		if(action == AXIS_ACTION_MOVE_X){
//			leaders[current_leader]->speed_x = pos * LEADER_MOVE_SPEED;
//		}else{
//			leaders[current_leader]->speed_y = pos * LEADER_MOVE_SPEED;
//		}
//
//	}else if(action == AXIS_ACTION_MOVE_CURSOR_X || action == AXIS_ACTION_MOVE_CURSOR_Y){
//		/********************
//		*             .-.   *
//		*   Cursor   ( = )> *
//		*             `-�   *
//		********************/
//
//		if(action == AXIS_ACTION_MOVE_CURSOR_X){
//			mouse_cursor_speed_x = pos * (1.0/game_fps) * MOUSE_CURSOR_MOVE_SPEED;
//		}else{
//			mouse_cursor_speed_y = pos * (1.0/game_fps) * MOUSE_CURSOR_MOVE_SPEED;
//		}
//
//	}else if(action == AXIS_ACTION_MOVE_GROUP_X || action == AXIS_ACTION_MOVE_GROUP_Y){
//		/******************
//		*            ***  *
//		*   Group   ****O *
//		*            ***  *
//		******************/
//
//		if(action == AXIS_ACTION_MOVE_GROUP_X){
//			moving_group_pos_x = pos;
//		}else{
//			moving_group_pos_y = pos;
//		}
//
//	}
//}

//void handle_mouse(unsigned int action, float mx, float my){
//	if(action == AXIS_ACTION_MOVE_CURSOR){
//
//		/********************
//		*             .-.   *
//		*   Cursor   ( = )> *
//		*             `-�   *
//		********************/
//
//		mouse_cursor_x = mx;
//		mouse_cursor_y = my;
//
//	}
//
//}


control_info::control_info(unsigned char action, unsigned char player, string s){
	this->action = action;
	this->player = player;
	type = CONTROL_TYPE_NONE;
	
	vector<string> parts = split(s, "_");
	size_t n_parts = parts.size();

	if(n_parts == 0) return;
	if(parts[0] == "k"){	//Keyboard.
		if(n_parts > 1){
			type = CONTROL_TYPE_KEYBOARD_KEY;
			button = atoi(parts[1]);
		}

	}else if(parts[0] == "mb"){	//Mouse button.
		if(n_parts > 1){
			type = CONTROL_TYPE_MOUSE_BUTTON;
			button = atoi(parts[1]);
		}

	}else if(parts[0] == "mwu"){ //Mouse wheel up.
		type = CONTROL_TYPE_MOUSE_WHEEL_UP;

	}else if(parts[0] == "mwd"){ //Mouse wheel down.
		type = CONTROL_TYPE_MOUSE_WHEEL_DOWN;

	}else if(parts[0] == "mwl"){ //Mouse wheel left.
		type = CONTROL_TYPE_MOUSE_WHEEL_LEFT;

	}else if(parts[0] == "mwr"){ //Mouse wheel right.
		type = CONTROL_TYPE_MOUSE_WHEEL_RIGHT;

	}else if(parts[0] == "jb"){ //Joystick button.
		if(n_parts > 2){
			type = CONTROL_TYPE_JOYSTICK_BUTTON;
			device_nr = atoi(parts[1]);
			button = atoi(parts[2]);
		}

	}else if(parts[0] == "jap"){ //Joystick axis, positive.
		if(n_parts > 3){
			type = CONTROL_TYPE_JOYSTICK_AXIS_POS;
			device_nr = atoi(parts[1]);
			stick = atoi(parts[2]);
			axis = atoi(parts[3]);
		}
	}else if(parts[0] == "jan"){ //Joystick axis, negative.
		if(n_parts > 3){
			type = CONTROL_TYPE_JOYSTICK_AXIS_NEG;
			device_nr = atoi(parts[1]);
			stick = atoi(parts[2]);
			axis = atoi(parts[3]);
		}
	}else{
		error_log(
			"Unrecognized control type \"" + parts[0] + "\" for player " +
			to_string((long long) (player + 1)) + " (value=\"" + s + "\").");
	}
}

string control_info::stringify(){
	if(type == CONTROL_TYPE_KEYBOARD_KEY){
		return "k_" + to_string((long long) button);
	}else if(type == CONTROL_TYPE_MOUSE_BUTTON){
		return "mb_" + to_string((long long) button);
	}else if(type == CONTROL_TYPE_MOUSE_WHEEL_UP){
		return "mwu";
	}else if(type == CONTROL_TYPE_MOUSE_WHEEL_DOWN){
		return "mwd";
	}else if(type == CONTROL_TYPE_MOUSE_WHEEL_LEFT){
		return "mwl";
	}else if(type == CONTROL_TYPE_MOUSE_WHEEL_RIGHT){
		return "mwr";
	}else if(type == CONTROL_TYPE_JOYSTICK_BUTTON){
		return "jb_" + to_string((long long) device_nr) + "_" + to_string((long long) button);
	}else if(type == CONTROL_TYPE_JOYSTICK_AXIS_POS){
		return "jap_" + to_string((long long) device_nr) + "_" + to_string((long long) stick) + "_" + to_string((long long) axis);
	}else if(type == CONTROL_TYPE_JOYSTICK_AXIS_NEG){
		return "jan_" + to_string((long long) device_nr) + "_" + to_string((long long) stick) + "_" + to_string((long long) axis);
	}

	return "";
}