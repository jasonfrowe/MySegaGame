// fighters.c
#include <genesis.h>
#include "globals.h" // For Fighter struct, fighters array, player_x/y, map constants, screen_width_pixels etc.
#include "fighters.h"
#include "resources.h" // For fighter_sprite_res

void initFighters(){
    for (s16 i = 0; i < active_fighter_count; i++) {
        fighters[i].vxi = (random() % 256) + 16; // Base speed component
        fighters[i].vyi = (random() % 256) + 16;
        // fighters[i].vx = fighters[i].vxi; // Initial velocity (this was x in original)
        // fighters[i].vy = fighters[i].vyi; // Initial velocity (this was y in original)
        fighters[i].vx = 0; // Will be set in updateFighters logic
        fighters[i].vy = 0;
        fighters[i].status = 1; // Active

        // Adjust to be relative to player's starting world view if map is large
        // For now, assume player starts near 0,0 in world coords
        // This needs to map world coords to initial screen coords correctly.
        // The original code used fighters[i].x = fighters[i].vxi, which seems like a bug.
        // Let's assume world coordinates for fighters.x/y.
        // Conversion to screen coordinates will happen in updateFighters before SPR_addSprite.

        // Wrap initial position to be within one map iteration from origin if needed
        // This logic might need refinement based on how world coords map to screen initially
        // fighters[i].x = (random() % (MAPSIZEM1 - screen_width_pixels)) + screen_width_pixels + 1; // World coordinates
        // fighters[i].y = (random() % (MAPSIZEM1 - screen_height_pixels)) + screen_height_pixels + 1;
        // if (fighters[i].x >  MAPSIZED2) fighters[i].x -= MAPSIZE;
        // if (fighters[i].x < MMAPSIZED2) fighters[i].x += MAPSIZE; // Ensure positive if wrapped
        // if (fighters[i].y >  MAPSIZED2) fighters[i].y -= MAPSIZE;
        // if (fighters[i].y < MMAPSIZED2) fighters[i].y += MAPSIZE;

        // fighters[i].x = (random() % (MAPSIZED2 - screen_width_pixels)) + screen_width_pixels + 144;
        // fighters[i].y = (random() % (MAPSIZED2 - screen_height_pixels)) + screen_height_pixels + 104;
        fighters[i].x = (random() % (screen_width_pixels)) + screen_width_pixels + 144;
        fighters[i].y = (random() % (screen_height_pixels)) + screen_height_pixels + 104;
        if (random()%2){
            fighters[i].x = -fighters[i].x;
        }
        if (random()%2){
            fighters[i].y = -fighters[i].y;
        }

        fighters[i].new_fighter = 1; // Needs sprite
        fighters[i].nframe_fighter = (random() % 2); // Assuming 2 frames for fighter animation
        fighters[i].sprite_ptr = NULL;
        fighters[i].xrem = 0;
        fighters[i].yrem = 0;
        fighters[i].dx = 0; // Own movement delta
        fighters[i].dy = 0;

        fexplode[i].status = 0; // for explosions
    }
}

void updateFighters()
{
    // s16 f_screen_x, f_screen_y; // Fighter's screen position
    s16 fvx_applied, fvy_applied;

    for (s16 i = 0; i < active_fighter_count; i++) {
        if (fighters[i].status >= 0) { // If fighter is active

            // Calculate fighter's screen position based on its world position and player's scroll delta
            // This assumes fighter X/Y are world coordinates.
            // And player_x/y are screen coordinates of player.
            // scroll_a_x/y are total background scroll.
            // fighters[i].x is world_x, player_x is screen_x
            // fighter_screen_x = fighters[i].x_world - scroll_offset_x + player_center_x_on_screen
            // This needs a clear concept of world vs screen coordinates.
            // Simpler: fighters store screen-relative coords and adjust by -player_scroll_delta_x/y
            // The original seemed to do this:
            fighters[i].x += -player_scroll_delta_x; // Adjust for map scroll
            fighters[i].y += -player_scroll_delta_y; // Adjust for map scroll


            if (fighters[i].new_fighter > 0){
                 // Only add sprite if on screen
                if (fighters[i].x > -8 && fighters[i].x < screen_width_pixels && // Allow slightly offscreen for smooth entry
                    fighters[i].y > -8 && fighters[i].y < screen_height_pixels) {
                    fighters[i].sprite_ptr = SPR_addSprite(&fighter_sprite_res,
                                                    fighters[i].x,
                                                    fighters[i].y,
                                                    TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                    fighters[i].new_fighter = 0;
                }
            }

            // AI / Movement decision (every 30 game frames)
            if (game_nframe == 30){ // Use game_nframe
            // game_ai_rand = random() % game_ai_decision_time;
            // if (game_ai_rand == game_ai_decision){

                // Turn off fighter animation.  Not noticable anyways.  
                // fighters[i].nframe_fighter = (fighters[i].nframe_fighter + 1) % 4; // Animation
                // if(fighters[i].sprite_ptr) SPR_setFrame(fighters[i].sprite_ptr, fighters[i].nframe_fighter);

                // Basic AI: randomly change direction towards player
                s16 fdx_to_player = player_x - fighters[i].x; // Delta from fighter to player
                s16 fdy_to_player = player_y - fighters[i].y;

                if (random() > game_ai_decision){ // Chance to adjust X velocity
                    if (fdx_to_player > 0) fighters[i].vx = fighters[i].vxi; // Move right
                    else if (fdx_to_player < 0) fighters[i].vx = -fighters[i].vxi; // Move left
                    else fighters[i].vx = 0;
                }
                if (random() > game_ai_decision){ // Chance to adjust Y velocity
                    if (fdy_to_player > 0) fighters[i].vy = fighters[i].vyi; // Move down
                    else if (fdy_to_player < 0) fighters[i].vy = -fighters[i].vyi; // Move up
                    else fighters[i].vy = 0;
                }
            }

            // Apply fighter's own velocity (sub-pixel)
            fvx_applied = (fighters[i].vx + fighters[i].xrem) >> fighter_speed_1; // >>8 for 256 scale
            fvy_applied = (fighters[i].vy + fighters[i].yrem) >> fighter_speed_1;
            fighters[i].xrem = fighters[i].vx + fighters[i].xrem - fvx_applied * fighter_speed_2;
            fighters[i].yrem = fighters[i].vy + fighters[i].yrem - fvy_applied * fighter_speed_2;
            fighters[i].dx = fvx_applied; // Fighter's own movement this frame
            fighters[i].dy = fvy_applied;

            fighters[i].x += fighters[i].dx; // Apply fighter's own movement
            fighters[i].y += fighters[i].dy;

            // --- World Wrapping (relative to screen center or 0,0 world) ---
            // This logic needs to be consistent with how scrolling and player position are handled.
            // If fighters[i].x/y are screen coordinates that get adjusted by player_scroll_delta,
            // then wrapping should occur when they go too far off *screen*.
            // The original MAPSIZE logic implies world coordinates.
            // For now, let's assume fighters[i].x/y are *screen-relative world coordinates*
            // that get "scrolled" by player_scroll_delta_x/y.
            // They wrap around a virtual world.
            // if (fighters[i].x < MMAPSIZED2 - screen_width_pixels/2) fighters[i].x += MAPSIZE;
            // if (fighters[i].x > MAPSIZED2 + screen_width_pixels/2) fighters[i].x -= MAPSIZE;
            // if (fighters[i].y < MMAPSIZED2 - screen_height_pixels/2) fighters[i].y += MAPSIZE;
            // if (fighters[i].y > MAPSIZED2 + screen_height_pixels/2) fighters[i].y -= MAPSIZE;
            if (fighters[i].x >  MAPSIZED2) fighters[i].x -= MAPSIZE;
            if (fighters[i].x < MMAPSIZED2) fighters[i].x += MAPSIZE; // Ensure positive if wrapped
            if (fighters[i].y >  MAPSIZED2) fighters[i].y -= MAPSIZE;
            if (fighters[i].y < MMAPSIZED2) fighters[i].y += MAPSIZE;

            if (fighters[i].sprite_ptr) {
                // Check if fighter is on screen before drawing
                if (fighters[i].x > -8 && fighters[i].x < screen_width_pixels &&
                    fighters[i].y > -8 && fighters[i].y < screen_height_pixels) {
                    SPR_setPosition(fighters[i].sprite_ptr, fighters[i].x, fighters[i].y);
                    SPR_setVisibility(fighters[i].sprite_ptr, VISIBLE);
                } else {
                    SPR_setVisibility(fighters[i].sprite_ptr, HIDDEN); // Hide if off-screen
                }
            }
        } else if (fighters[i].status < -1){ // Fighter is exploding
            
            fighters[i].x += -player_scroll_delta_x; // Adjust for map scroll
            fighters[i].y += -player_scroll_delta_y; // Adjust for map scroll

            if ((fighters[i].status == -9) & (fexplode[i].status == 0)){
                // XGM2_playPCMEx(sfx_explode, sizeof(sfx_explode), SOUND_PCM_CH3, 4, FALSE, FALSE);
                fexplode[i].sprite_ptr = SPR_addSprite(&fighter_explode_res,
                                                    fighters[i].x,
                                                    fighters[i].y,
                                                    TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                
                fexplode[i].status = 5;
            } else if ((fighters[i].status == -2) & (fexplode[i].status == 0)){
                SPR_releaseSprite(fexplode[i].sprite_ptr);
                fexplode[i].sprite_ptr = NULL;

                // fighters[i].x = (random() % (MAPSIZED2 - screen_width_pixels)) + screen_width_pixels + 144;
                // fighters[i].y = (random() % (MAPSIZED2 - screen_height_pixels)) + screen_height_pixels + 104;
                fighters[i].x = (random() % (screen_width_pixels_d2)) + screen_width_pixels + 144;
                fighters[i].y = (random() % (screen_height_pixels_d2)) + screen_height_pixels + 104;
                if (random()%2){
                    fighters[i].x = -fighters[i].x;
                }
                if (random()%2){
                    fighters[i].y = -fighters[i].y;
                }
                
                fighters[i].status = 1; // Active
                fighters[i].new_fighter = 1;
            } else if (fexplode[i].status == 0) {
                SPR_setFrame(fexplode[i].sprite_ptr, fighters[i].status + 9);
                fexplode[i].status = 5; // delay for next frame of animation
            }

            if (fexplode[i].status > 0){

                // Check if fighter is on screen before drawing
                if (fighters[i].x > -8 && fighters[i].x < screen_width_pixels &&
                    fighters[i].y > -8 && fighters[i].y < screen_height_pixels) {
                    SPR_setPosition(fexplode[i].sprite_ptr, fighters[i].x, fighters[i].y);
                    SPR_setVisibility(fexplode[i].sprite_ptr, VISIBLE);
                } else {
                    SPR_setVisibility(fexplode[i].sprite_ptr, HIDDEN); // Hide if off-screen
                }

                // SPR_setPosition(fexplode[i].sprite_ptr, fighters[i].x, fighters[i].y);
                fexplode[i].status -= 1;
            }

            if (fexplode[i].status == 0){
                fighters[i].status += 1;
            }
            

            // if(fighters[i].sprite_ptr) {
            //      //SPR_releaseSprite(fighters[i].sprite_ptr); // Done in bullet collision
            //      //fighters[i].sprite_ptr = NULL;
            // }
            // Potentially respawn logic here if FIGHTER_RATE was used
        }
    }
}

void collideFighters(){
    for (s16 i = 0; i < active_fighter_count; i++) {
        if (fighters[i].status >= 0) {

            if (fighters[i].x     < player_x + 16 && // fighter left < bullet right
                fighters[i].x + 8 > player_x      && // fighter right > bullet left
                fighters[i].y     < player_y + 16 && // fighter top < bullet bottom
                fighters[i].y + 8 > player_y)       // fighter bottom > bullet top
            {
                fighters[i].status = -9; // Deactivate fighter (-9 means we do an explosion)
                if(fighters[i].sprite_ptr) SPR_releaseSprite(fighters[i].sprite_ptr);
                fighters[i].sprite_ptr = NULL;

                if(shield_status <= 0){
                    fighters_score += 1; // If shield down, enemy gets a point
                    XGM2_playPCMEx(sfx_explode, sizeof(sfx_explode), SOUND_PCM_CH2, 4, TRUE, FALSE);
                } else {
                    player_score += 1;   // If shield is up, player gets a point
                    game_score += 25;
                    XGM2_playPCMEx(sfx_explode, sizeof(sfx_explode), SOUND_PCM_CH3, 4, FALSE, FALSE);
                }

            }


        }
    }

}