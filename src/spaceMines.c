// spaceMines.c
#include <genesis.h>
#include "globals.h" 
#include "spaceMines.h"
#include "resources.h" 


void placeMine(){
	if (mine_status == 0){
		mine_status = 1;
		mine_x = player_x;
		mine_y = player_y;
		mine_timer = 0; // Arm the space mine timer.
		mine_sprite_ptr = SPR_addSprite(&space_mine_res,
                                            mine_x, mine_y, TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
		SPR_setFrame(mine_sprite_ptr, 0);
	}

}

void updateMine(){

	if (mine_status > 0){
		mine_x += -player_scroll_delta_x; // Adjust for map scroll
	    mine_y += -player_scroll_delta_y; // Adjust for map scroll

	    if (mine_x >  MAPSIZED2) mine_x -= MAPSIZE;
        if (mine_x < MMAPSIZED2) mine_x += MAPSIZE; // Ensure positive if wrapped
        if (mine_y >  MAPSIZED2) mine_y -= MAPSIZE;
        if (mine_y < MMAPSIZED2) mine_y += MAPSIZE;


	    if (mine_x > -8 && mine_x < screen_width_pixels &&
            mine_y > -8 && mine_y < screen_height_pixels) {
            SPR_setPosition(mine_sprite_ptr, mine_x, mine_y);
            SPR_setVisibility(mine_sprite_ptr, VISIBLE);
        } else {
            SPR_setVisibility(mine_sprite_ptr, HIDDEN); // Hide if off-screen
        }
	    
	    if (mine_timer < mine_timer_max){
	    	mine_timer += 1;
	    } else if (mine_timer == mine_timer_max) {
	    	mine_status = 2; // Arm the space mine
	    	SPR_setFrame(mine_sprite_ptr, 1);
	    	mine_timer = mine_timer_max + 1;
	    }

	} else if (mine_status < -1){

		mine_x += -player_scroll_delta_x; // Adjust for map scroll
	    mine_y += -player_scroll_delta_y; // Adjust for map scroll

	    if (mine_x >  MAPSIZED2) mine_x -= MAPSIZE;
        if (mine_x < MMAPSIZED2) mine_x += MAPSIZE; // Ensure positive if wrapped
        if (mine_y >  MAPSIZED2) mine_y -= MAPSIZE;
        if (mine_y < MMAPSIZED2) mine_y += MAPSIZE;

	    if ((mine_status == -9) & (mexplode_status == 0)){
	    	
	    	mexplode_sprite_ptr = SPR_addSprite(&mine_explode_res,
                                    mine_x,	mine_y,	TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
	 		SPR_setFrame(mexplode_sprite_ptr, 0);
            mexplode_status = 5;

	    } else if ((mine_status == -2) & (mexplode_status == 0)){ // Remove sprite
            
            SPR_releaseSprite(mexplode_sprite_ptr);
            mexplode_sprite_ptr = NULL;
            mine_status = -1;
        
        } else if (mexplode_status == 0) {  // Advance frame
            
            SPR_setFrame(mexplode_sprite_ptr, mine_status + 9);
            mexplode_status = 5;
        
        }

        if (mexplode_status > 0){ 
        	if (mine_x > -8 && mine_x < screen_width_pixels &&
	            mine_y > -8 && mine_y < screen_height_pixels) {
	            SPR_setPosition(mexplode_sprite_ptr, mine_x, mine_y);
	            SPR_setVisibility(mexplode_sprite_ptr, VISIBLE);
	        } else {
	            SPR_setVisibility(mexplode_sprite_ptr, HIDDEN); // Hide if off-screen
	        }

            mexplode_status -= 1;
        }
        
        if (mexplode_status == 0){
            mine_status += 1;
        }

	}


	if (mine_status > 1){ // If mine is armed, check for collisions

		for (s16 i = 0; i < active_fighter_count; i++) {

			if (fighters[i].status >= 0) { // only explode alive fighters

				if (fighters[i].x     < mine_x + 16 && // fighter left < bullet right
	                fighters[i].x + 8 > mine_x     && // fighter right > bullet left
	                fighters[i].y     < mine_y + 16 && // fighter top < bullet bottom
	                fighters[i].y + 8 > mine_y)       // fighter bottom > bullet top
	            {
	            	fighters[i].status = -9; // Deactivate fighter (-9 means we do an explosion)
	                if(fighters[i].sprite_ptr) SPR_releaseSprite(fighters[i].sprite_ptr);
	                fighters[i].sprite_ptr = NULL;

	            	mine_status = -9; // Deactivate mine (-9 means we do an explosion)
	            	mexplode_status = 0;
		            if(mine_sprite_ptr) SPR_releaseSprite(mine_sprite_ptr);
		            mine_sprite_ptr = NULL;
		            player_score += 5;
		            if (player_score > 100){
		            	player_score = 100;
		            }
		            game_score += 20;
		            XGM2_playPCMEx(sfx_mexplode, sizeof(sfx_mexplode), SOUND_PCM_CH3, 4, FALSE, FALSE);

		            break; // Once an explosion happens, stop loop.
	            }
	        }

		}

		if (mine_x     < player_x + 16 && // fighter left < bullet right
            mine_x + 8 > player_x      && // fighter right > bullet left
            mine_y     < player_y + 16 && // fighter top < bullet bottom
            mine_y + 8 > player_y)       // fighter bottom > bullet top
        {
        	mine_status = -9; // Deactivate mine (-9 means we do an explosion)
        	mexplode_status = 0;
            if(mine_sprite_ptr) SPR_releaseSprite(mine_sprite_ptr);
            mine_sprite_ptr = NULL;
            player_score -= 10;
            if (player_score < 0){
            	player_score = 0;
            }
            XGM2_playPCMEx(sfx_mexplode, sizeof(sfx_mexplode), SOUND_PCM_CH2, 4, FALSE, FALSE);
        }
	}

}