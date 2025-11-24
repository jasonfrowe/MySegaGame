// bullets.c
#include <genesis.h>
#include "globals.h" // For Bullet struct, bullets array, player_x/y, sin_fix, cos_fix, screen_width_pixels, etc.
#include "bullets.h"
#include "resources.h" // For bullet_sprite_res
// #include "fighters.h" // Not directly, globals.h has fighters array for collision

// --- Initialize Bullet Pool ---
void initBullets() {
    for (s16 i = 0; i < NBULLET; i++) {
        bullets[i].status       = -1; // Inactive
        bullets[i].new_bullet   = 0;
        bullets[i].sprite_ptr   = NULL;
        bullets[i].bvxrem       = 0;
        bullets[i].bvyrem       = 0;
    }
    current_bullet_index = 0;
    new_bullet_delay_timer = 0; // Already in player.c, maybe move shared timer to globals
}

void fireBullet(){
    if (new_bullet_delay_timer > NBULLET_TIMER_MAX){ // This timer is incremented in handleInput
        new_bullet_delay_timer = 0;
        if (bullets[current_bullet_index].status < 0){ // Check if current slot is available
            bullets[current_bullet_index].status = player_rotation_index; // Store direction
            bullets[current_bullet_index].new_bullet = 1;
            bullets[current_bullet_index].x = player_x + 4; // Offset from player center
            bullets[current_bullet_index].y = player_y + 4;
            bullets[current_bullet_index].bvxrem = 0;
            bullets[current_bullet_index].bvyrem = 0;

            current_bullet_index++;
            if (current_bullet_index >= NBULLET){
                current_bullet_index = 0;
            }
            // priority 0 is the lowest 
            // XGM_startPlayPCM(SFX_LASER, 1, SOUND_PCM_CH2); //name, priority (0-15), channel (2-4) 
            XGM2_playPCMEx(sfx_laser, sizeof(sfx_laser), SOUND_PCM_CH2, 2, FALSE, FALSE);
        }
    }
}

void updateBullets()
{
    s16 bvx_req, bvy_req;       // Requested velocity per bullet
    s16 bvx_applied, bvy_applied; // Applied velocity (after remainder)

    for (s16 i = 0; i < NBULLET; i++) {
        if (bullets[i].status >= 0) { // If bullet is active
            if (bullets[i].new_bullet > 0){
                bullets[i].sprite_ptr = SPR_addSprite(&bullet_sprite_res,
                                                bullets[i].x,
                                                bullets[i].y,
                                                TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                bullets[i].new_bullet = 0;
            }

            // Collision with fighters
            for (s16 f = 0; f < active_fighter_count; f++) {
                if (fighters[f].status >= 0){ // If fighter is active
                    // Simple AABB collision check (8x8 sprite assumed for fighter, 4x4 for bullet, adjust as needed)
                    if (fighters[f].x     < bullets[i].x + 2 && // fighter left < bullet right
                        fighters[f].x + 8 > bullets[i].x     && // fighter right > bullet left
                        fighters[f].y     < bullets[i].y + 2 && // fighter top < bullet bottom
                        fighters[f].y + 8 > bullets[i].y)       // fighter bottom > bullet top
                    {
                        bullets[i].status = -1; // Deactivate bullet
                        if(bullets[i].sprite_ptr) SPR_releaseSprite(bullets[i].sprite_ptr);
                        bullets[i].sprite_ptr = NULL;

                        fighters[f].status = -9; // Deactivate fighter (-9 means we do an explosion)
                        if(fighters[f].sprite_ptr) SPR_releaseSprite(fighters[f].sprite_ptr);
                        fighters[f].sprite_ptr = NULL;
                        XGM2_playPCMEx(sfx_explode, sizeof(sfx_explode), SOUND_PCM_CH3, 4, FALSE, FALSE);

                        player_score += 1;
                        game_score +=3;
                        
                        // Potentially add explosion, score, sound effect here
                        break; // Bullet can only hit one fighter per frame
                    }
                }
            }
             // If bullet still active after collision checks
            if (bullets[i].status >= 0) {

                // bullets[i].x += -player_scroll_delta_x; // Adjust for map scroll
                // bullets[i].y += -player_scroll_delta_y; // Adjust for map scroll

                bvx_req = -sin_fix[bullets[i].status]; // Using stored direction
                bvy_req = -cos_fix[bullets[i].status];

                // Speed adjustment (>>6 means divide by 64, so sin/cos are scaled up)
                // Original code implies sin_fix/cos_fix are scaled by 255.
                // For a speed of 4 pixels/frame, 255 / 64 is approx 4.
                bvx_applied = ( (bvx_req + bullets[i].bvxrem) >> 6);
                bvy_applied = ( (bvy_req + bullets[i].bvyrem) >> 6);
                bullets[i].bvxrem  = bvx_req + bullets[i].bvxrem - bvx_applied * 64;
                bullets[i].bvyrem  = bvy_req + bullets[i].bvyrem - bvy_applied * 64;
                bullets[i].x += bvx_applied;
                bullets[i].y += bvy_applied;

                // Check screen boundaries
                if (bullets[i].x > 0 && bullets[i].x < screen_width_pixels &&
                    bullets[i].y > 0 && bullets[i].y < screen_height_pixels) {
                    if(bullets[i].sprite_ptr) SPR_setPosition(bullets[i].sprite_ptr, bullets[i].x, bullets[i].y);
                } else {
                    bullets[i].status = -1; // Deactivate
                    if(bullets[i].sprite_ptr) SPR_releaseSprite(bullets[i].sprite_ptr);
                    bullets[i].sprite_ptr = NULL;
                }
            }
        }
    }
}