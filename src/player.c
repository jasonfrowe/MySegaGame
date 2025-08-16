// player.c
#include <genesis.h>
#include "globals.h"    // For player variables, sin_fix, cos_fix, screen boundaries
#include "player.h"
#include "bullets.h"    // For fireBullet()
#include "sbullets.h"   // For fire_SBullet()
#include "shield.h"     // Player Shield
#include "spaceMines.h" // Space mines


// --- Input Handling Function ---
void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    if (value & BUTTON_START) {
        if (!start_was_pressed) {
            game_paused = !game_paused;
            start_was_pressed = 1;
        }
    } else {
        start_was_pressed = 0;
    }

    if (control_style == 0){
        // --- Rotation ---
        if (player_rotation_iframe >= player_rotation_iframe_threshold){
            player_rotation_iframe = 0;

            if (value & BUTTON_LEFT) {
                if (player_rotation_index == player_rotation_index_max){
                    player_rotation_index = 0;
                } else {
                    player_rotation_index += 1;
                }
            } else if (value & BUTTON_RIGHT) {
                if (player_rotation_index == 0){
                    player_rotation_index = player_rotation_index_max;
                } else {
                    player_rotation_index -= 1;
                }
            }
        }
        player_rotation_iframe += 1;

        // --- Thrust ---
        player_vx = 0; // Default is no thrust applied.
        player_vy = 0;
        if (value & BUTTON_UP) {
            player_vx = -sin_fix[player_rotation_index];
            player_vy = -cos_fix[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if (value & BUTTON_DOWN) {
            player_vx =  sin_fix_d2[player_rotation_index]; // should replace with lookup to avoid division
            player_vy =  cos_fix_d2[player_rotation_index]; 
            player_thrust_delay_timer = 0;
        }

    } else {
        // --- Thrust ---
        player_vx = 0; // Default is no thrust applied.
        player_vy = 0;

        
        dpadUp = 0 ; dpadDown = 0; dpadLeft = 0 ; dpadRight = 0;

        if (value & BUTTON_UP){
            dpadUp = 1;
        } else if (value & BUTTON_DOWN){
            dpadDown = 1;
        }

        if (value & BUTTON_LEFT){
            dpadLeft = 1;
        } else if (value & BUTTON_RIGHT){
            dpadRight = 1;
        }


        if ((dpadUp) & (dpadRight)){
            player_rotation_index = 21;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if ((dpadRight) & (dpadDown)){
            player_rotation_index = 15;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if ((dpadDown) & (dpadLeft)){
            player_rotation_index = 9;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if ((dpadLeft) & (dpadUp)){
            player_rotation_index = 3;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if (dpadUp){
            player_rotation_index = 0;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if (dpadRight){
            player_rotation_index = 18;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if (dpadDown){
            player_rotation_index = 12;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        } else if (dpadLeft){
            player_rotation_index = 6;
            player_vx = -sin_fix_d2[player_rotation_index];
            player_vy = -cos_fix_d2[player_rotation_index];
            player_thrust_delay_timer = 0;
        }

    }
    

    // Fire main weapon
    if (value & BUTTON_B) {
        if (shield_status < 0){
            fireBullet(); // Call fireBullet from bullets module.  Can only fire when shields down
        }
        
    }
    new_bullet_delay_timer += 1; // This is related to bullet firing rate

    // Fire secondary weapon
    if (value & BUTTON_C) {
        if (shield_status < 0){
            fire_SBullet(); // Call fireBullet from bullets module. Can only fire when shields down
        }
    }
    new_sbullet_delay_timer += 1; // This is related to bullet firing rate

    if (value & BUTTON_A) { // Player boost.
        if (player_boost_delay_timer >= player_boost_delay_timer_max){
            player_boost_status = 1;
            player_boost_delay_timer = 0;
            player_boost_timer = 0;
            XGM2_playPCMEx(sfx_turbo, sizeof(sfx_turbo), SOUND_PCM_CH2, 3, FALSE, FALSE);
        } 
    }

    // Turn on Shield
    if (value & BUTTON_X) {
        enableShield();
    }
    // if (new_shield_delay_timer == shield_delay_max - 1) {
    //     XGM2_playPCMEx(sfx_ding, sizeof(sfx_ding), SOUND_PCM_CH2, 3, FALSE, FALSE); // Shield is ready.
    // }
    if ((new_shield_delay_timer < shield_delay_max) & (shield_status < 0)){
        new_shield_delay_timer += 1;
    } 

    // Drop a mine
    if (value & BUTTON_Y){
        placeMine();
    }

}

void playerBoost(){


    if (player_boost_status > 0){

        // VDP_clearText(1, 5, 15);
        // intToStr(player_boost_timer, text_vel_x, 2);
        // VDP_drawText("BST:", 1, 5); VDP_drawText(text_vel_x, 6, 5);

        if (player_boost_timer < player_boost_timer_max){
            player_vx = -sin_fix[player_rotation_index] * boost_factor;
            player_vy = -cos_fix[player_rotation_index] * boost_factor;
            player_boost_timer += 1;
        } else {
            player_boost_status = -1;
        }

    } // else {
    //     VDP_clearText(1, 5, 15);
    // }

    if (player_boost_delay_timer < player_boost_delay_timer_max) {
        player_boost_delay_timer += 1;
    }
        
}

// --- Physics Update Function ---
void updatePhysics()
{
    s16 xtry, ytry; // Local temps for new position
    s16 thx, thy;   // Local temps for thrust calculation

    //Update position
    player_vx_applied = ( (player_vx + player_x_remainder + player_thrust_momentum_x ) >> 9);
    player_vy_applied = ( (player_vy + player_y_remainder + player_thrust_momentum_y ) >> 9);
    player_x_remainder = player_vx + player_x_remainder + player_thrust_momentum_x - player_vx_applied * 512;
    player_y_remainder = player_vy + player_y_remainder + player_thrust_momentum_y - player_vy_applied * 512;
    xtry = player_x + player_vx_applied;
    ytry = player_y + player_vy_applied;

    //Update thrust if joystick is held.
    thx = player_thrust_momentum_x + (player_vx >> 4);
    if (thx < 1024 && thx > -1024){ // Consider using FIX16 for these limits if they are scaled
        player_thrust_momentum_x = thx;
    }
    thy = player_thrust_momentum_y + (player_vy >> 4);
    if (thy < 1024 && thy > -1024){
        player_thrust_momentum_y = thy;
    }

    //Update momentum by applying friction
    if (player_thrust_delay_timer < player_thrust_delay_max && player_thrust_counter > 100){ // magic number 100 <-- should be parameterized
        player_thrust_delay_timer += 1;
        player_thrust_counter = 0;
        if (player_vx == 0){
            player_thrust_momentum_x = player_thrust_momentum_x >> 1;
        }
        if (player_vy == 0){
            player_thrust_momentum_y = player_thrust_momentum_y >> 1;
        }
    }
    if (player_thrust_delay_timer >= player_thrust_delay_max){
        player_thrust_momentum_x = 0;
        player_thrust_momentum_y = 0;
    }

    if (player_thrust_counter < 101){ //catch runaway counter if player is idle for a long time.
        player_thrust_counter += 1;
    }
    

    // Keep spacecraft in bounds and determine scroll delta
    if (xtry > scroll_boundary_x1 && xtry < scroll_boundary_x2){
        player_x = xtry;
        player_scroll_delta_x = 0;
    } else {
        player_scroll_delta_x = (xtry - player_x);
        // player_x doesn't change here if scrolling, world moves instead
    }

    if (ytry > scroll_boundary_y1 && ytry < scroll_boundary_y2){
        player_y = ytry;
        player_scroll_delta_y = 0;
    } else {
        player_scroll_delta_y = (ytry - player_y);
        // player_y doesn't change here
    }
}