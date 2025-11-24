// sbullets.c
#include <genesis.h>
#include "globals.h" // For Bullet struct, bullets array, player_x/y, sin_fix, cos_fix, screen_width_pixels, etc.
#include "sbullets.h"
#include "resources.h" // For bullet_sprite_res

// --- Initialize S_Bullet Pool ---
void init_SBullets() {
	for (u16 i = 0; i < NSBULLET; i++) {
        sbullets[i].status       = -1; // Inactive
        sbullets[i].new_bullet   = 0;
        sbullets[i].sprite_ptr   = NULL;
        sbullets[i].bvxrem       = 0;
        sbullets[i].bvyrem       = 0;
    }
    current_sbullet_index = 0;
    new_sbullet_delay_timer = 0; // Already in player.c, maybe move shared timer to globals
}


void fire_SBullet(){

	if (new_sbullet_delay_timer > NSBULLET_TIMER_MAX){

		new_sbullet_delay_timer = 0; // reset timer

		if ( (sbullets[0].status<0) & (sbullets[1].status<0) & (sbullets[2].status<0)){

			sbullets[0].status = player_rotation_index - 1;
			if (sbullets[0].status > player_rotation_index_max){
                sbullets[0].status = 0;
            } else if (sbullets[0].status < 0){
                sbullets[0].status = player_rotation_index_max;
            }

			sbullets[1].status = player_rotation_index    ;
			if (sbullets[1].status > player_rotation_index_max){
                sbullets[1].status = 0;
            } else if (sbullets[1].status < 0){
                sbullets[1].status = player_rotation_index_max;
            }

			sbullets[2].status = player_rotation_index + 1;
			if (sbullets[2].status > player_rotation_index_max){
                sbullets[2].status = 0;
            } else if (sbullets[2].status < 0){
                sbullets[2].status = player_rotation_index_max;
            }

            for (u16 i = 0; i < NSBULLET; i++) {
		        sbullets[i].new_bullet = 1;
		        sbullets[i].x = player_x + 4; // Offset from player center
		        sbullets[i].y = player_y + 4;
		        sbullets[i].bvxrem = 0;
		        sbullets[i].bvyrem = 0;
		    }

		    XGM2_playPCMEx(sfx_sbullet, sizeof(sfx_sbullet), SOUND_PCM_CH2, 2, FALSE, FALSE);

		}

	}

}

void update_SBullets(){

	s16 bvx_req, bvy_req;       // Requested velocity per bullet
    s16 bvx_applied, bvy_applied; // Applied velocity (after remainder)

    for (s16 i = 0; i < NSBULLET; i++) {
        if (sbullets[i].status >= 0) { // If bullet is active
            if (sbullets[i].new_bullet > 0){
                sbullets[i].sprite_ptr = SPR_addSprite(&sbullet_sprite_res,
                                                sbullets[i].x,
                                                sbullets[i].y,
                                                TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                sbullets[i].new_bullet = 0;
            }

            // Collision with fighters
            for (u16 f = 0; f < active_fighter_count; f++) {
                if (fighters[f].status >= 0){ // If fighter is active
                    // Simple AABB collision check (8x8 sprite assumed for fighter, 4x4 for bullet, adjust as needed)
                    if (fighters[f].x     < sbullets[i].x + 4 && // fighter left < bullet right
                        fighters[f].x + 8 > sbullets[i].x     && // fighter right > bullet left
                        fighters[f].y     < sbullets[i].y + 4 && // fighter top < bullet bottom
                        fighters[f].y + 8 > sbullets[i].y)       // fighter bottom > bullet top
                    {
                        sbullets[i].status = -1; // Deactivate bullet
                        if(sbullets[i].sprite_ptr) SPR_releaseSprite(sbullets[i].sprite_ptr);
                        sbullets[i].sprite_ptr = NULL;

                        fighters[f].status = -9; // Deactivate fighter (-8 means we do an explosion)
                        if(fighters[f].sprite_ptr) SPR_releaseSprite(fighters[f].sprite_ptr);
                        fighters[f].sprite_ptr = NULL;
                        XGM2_playPCMEx(sfx_explode, sizeof(sfx_explode), SOUND_PCM_CH3, 4, FALSE, FALSE);

                        player_score +=1;
                        game_score +=7;
                        
                        // Potentially add explosion, score, sound effect here
                        break; // Bullet can only hit one fighter per frame
                    }
                }
            }

            // If bullet still active after collision checks
            if (sbullets[i].status >= 0) {
                bvx_req = -sin_fix[sbullets[i].status]; // Using stored direction
                bvy_req = -cos_fix[sbullets[i].status];

                // Speed adjustment (>>6 means divide by 64, so sin/cos are scaled up)
                // Original code implies sin_fix/cos_fix are scaled by 255.
                // For a speed of 4 pixels/frame, 255 / 64 is approx 4.
                bvx_applied = ( (bvx_req + sbullets[i].bvxrem) >> 6);
                bvy_applied = ( (bvy_req + sbullets[i].bvyrem) >> 6);
                sbullets[i].bvxrem  = bvx_req + sbullets[i].bvxrem - bvx_applied * 64;
                sbullets[i].bvyrem  = bvy_req + sbullets[i].bvyrem - bvy_applied * 64;
                sbullets[i].x += bvx_applied;
                sbullets[i].y += bvy_applied;

                // Check screen boundaries
                if (sbullets[i].x > 0 && sbullets[i].x < screen_width_pixels &&
                    sbullets[i].y > 0 && sbullets[i].y < screen_height_pixels) {
                    if(sbullets[i].sprite_ptr) SPR_setPosition(sbullets[i].sprite_ptr, sbullets[i].x, sbullets[i].y);
                } else {
                    sbullets[i].status = -1; // Deactivate
                    if(sbullets[i].sprite_ptr) SPR_releaseSprite(sbullets[i].sprite_ptr);
                    sbullets[i].sprite_ptr = NULL;
                }
            }

        }

    }

}
