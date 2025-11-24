// ebullets.c
#include <genesis.h>
#include <maths.h>   // For ABS
#include "globals.h" // For Bullet struct, bullets array, player_x/y, sin_fix, cos_fix, screen_width_pixels, etc.
#include "ebullets.h"
#include "resources.h" // For bullet_sprite_res
// #include "fighters.h" // Not directly, globals.h has fighters array for collision


#define ENEMY_BULLET_SPEED  4 

void init_eBullets(){
	for (s16 i = 0; i < NEBULLET; i++) {
        ebullets[i].status       = -1; // Inactive
        ebullets[i].new_bullet   = 0;
        ebullets[i].sprite_ptr   = NULL;
        ebullets[i].bvxrem       = 0;
        ebullets[i].bvyrem       = 0;
    }
    current_ebullet_index = 0;
    new_ebullet_delay_timer = 2; // Already in player.c, maybe move shared timer to globals
}

void fire_eBullet(){

	if (new_ebullet_delay_timer > NEBULLET_TIMER_MAX){ // This timer is incremented in handleInput
        new_ebullet_delay_timer = 0;
        if (ebullets[current_ebullet_index].status < 0){

        	// Now we loop through fighters to see if one is in position to fire.
        	for (s16 i = 0; i < active_fighter_count; i++) {
        		if (fighters[i].status == 1) {

        			if (fighters[i].x > -8 && fighters[i].x < screen_width_pixels && // Allow slightly offscreen for smooth entry
                    fighters[i].y > -8 && fighters[i].y < screen_height_pixels) {

                    	s16 fdx =  player_x - fighters[i].x;
            			s16 fdy = -player_y + fighters[i].y; // May need to flip this one
            			s16 distance = abs(fdx) + abs(fdy); // Rough distance

            			if (distance > 0){

            				s16 tti_frames = distance / ENEMY_BULLET_SPEED;
        					if (tti_frames == 0 && distance > 0) tti_frames = 1; // Min 1 frame prediction if close

        					s16 pre_player_x = player_x + 4 + (player_vx_applied * tti_frames);
        					s16 pre_player_y = player_y + 4 + (player_vy_applied * tti_frames); // Screen Y

        					// Update target deltas to this predicted position
					        fdx =  pre_player_x - fighters[i].x;
					        fdy = -pre_player_y + fighters[i].y;

					        s16 best_index = 0;
					        s32 max_dot = -32768 * 256; // Smallest possible s32 (approx)
					        for (s16 j = 0; j < SINCOS_TABLE_STEPS; ++j) {
					        	s32 current_dot = (s32)fdx * cos_fix[j] + (s32)fdy * sin_fix[j];
					        	if (current_dot > max_dot) {
						            max_dot = current_dot;
						            best_index = j;
						        }

					        } 

					        ebullets[current_ebullet_index].status       = best_index;
					        ebullets[current_ebullet_index].new_bullet   = 1;
					        ebullets[current_ebullet_index].x 			 = fighters[i].x;
					        ebullets[current_ebullet_index].y 			 = fighters[i].y;

            				XGM2_playPCMEx(sfx_elaser, sizeof(sfx_elaser), SOUND_PCM_CH3, 1, TRUE, FALSE);
        					fighters[i].status += 1;

        					break;

            			}

        			}

        		} else if (fighters[i].status > 1){
        			fighters[i].status += 1;
        			if (fighters[i].status > efire_cooldown_timer){  // here is the delay for the next bullet to be fired.  
        				fighters[i].status = 1; // Ready to fire again.
        			}

        		}
        	}

        	current_ebullet_index++;
            if (current_ebullet_index >= NEBULLET){
                current_ebullet_index = 0;  // Reset enemy bullet timer
            }
        }
    }

}

void update_eBullets(){

	s16 bvx_req, bvy_req;       // Requested velocity per bullet
    s16 bvx_applied, bvy_applied; // Applied velocity (after remainder)

    for (s16 i = 0; i < NEBULLET; i++) {
        if (ebullets[i].status >= 0) { // If bullet is active
            if (ebullets[i].new_bullet > 0){
                ebullets[i].sprite_ptr = SPR_addSprite(&ebullet_sprite_res,
                                                ebullets[i].x,
                                                ebullets[i].y,
                                                TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                ebullets[i].new_bullet = 0;
            }

            // Collision with Player
            if (player_x      < ebullets[i].x + 2 && // fighter left < bullet right
                player_x + 16 > ebullets[i].x     && // fighter right > bullet left
                player_y      < ebullets[i].y + 2 && // fighter top < bullet bottom
                player_y + 16 > ebullets[i].y)       // fighter bottom > bullet top
            {
                ebullets[i].status = -1; // Deactivate bullet
                if(ebullets[i].sprite_ptr) SPR_releaseSprite(ebullets[i].sprite_ptr);
                ebullets[i].sprite_ptr = NULL;

                XGM2_playPCMEx(sfx_explode, sizeof(sfx_explode), SOUND_PCM_CH2, 4, TRUE, FALSE);

                if (shield_status < 0){  // Check shield status
                	fighters_score += 1; // Score one for the bad guys
                }
                               
                // Potentially add explosion, score, sound effect here
            }


            if (ebullets[i].status >= 0) {

            	// This might degrade enemy aim? 
            	ebullets[i].x += -player_scroll_delta_x; // Adjust for map scroll
                ebullets[i].y += -player_scroll_delta_y; // Adjust for map scroll

	            // bvx_req = -sin_fix[ebullets[i].status]; // Using stored direction
	            // bvy_req = -cos_fix[ebullets[i].status];
	            bvx_req =  cos_fix[ebullets[i].status]; // Using stored direction
	            bvy_req = -sin_fix[ebullets[i].status];

	            // Speed adjustment (>>6 means divide by 64, so sin/cos are scaled up)
	            // Original code implies sin_fix/cos_fix are scaled by 255.
	            // For a speed of 4 pixels/frame, 255 / 64 is approx 4.
	            bvx_applied = ( (bvx_req + ebullets[i].bvxrem) >> 6);
	            bvy_applied = ( (bvy_req + ebullets[i].bvyrem) >> 6);
	            ebullets[i].bvxrem  = bvx_req + ebullets[i].bvxrem - bvx_applied * 64;
	            ebullets[i].bvyrem  = bvy_req + ebullets[i].bvyrem - bvy_applied * 64;
	            ebullets[i].x += bvx_applied;
	            ebullets[i].y += bvy_applied;

	            // Check screen boundaries
	            if (ebullets[i].x > 0 && ebullets[i].x < screen_width_pixels &&
	                ebullets[i].y > 0 && ebullets[i].y < screen_height_pixels) {
	                if(ebullets[i].sprite_ptr) SPR_setPosition(ebullets[i].sprite_ptr, ebullets[i].x, ebullets[i].y);
	            } else {
	                ebullets[i].status = -1; // Deactivate
	                if(ebullets[i].sprite_ptr) SPR_releaseSprite(ebullets[i].sprite_ptr);
	                ebullets[i].sprite_ptr = NULL;
	            }
	        }
        }


}



	new_ebullet_delay_timer += 1; // Update bullet timer

}