// bullets.c
#include <genesis.h>
#include "globals.h" // For Bullet struct, bullets array, player_x/y, sin_fix, cos_fix, screen_width_pixels, etc.
#include "shield.h"
#include "resources.h" 

void enableShield(){
	if (new_shield_delay_timer >= shield_delay_max){ // This timer is incremented in handleInput
        
        new_shield_delay_timer = 0;

        shield_timer  	 = 0; // Start timer for Shield duration
        shield_pal    	 = 0; // Palette for Shield display
        shield_status 	 = 1; // Turn on shield 
        shield_pal_timer = 0; // Timer for palette switches

        XGM2_playPCMEx(sfx_ding, sizeof(sfx_ding), SOUND_PCM_CH2, 14, FALSE, FALSE); // 14 = high-priority

    }
}

void shield_animate(){

	if (shield_timer > shield_duration){

		shield_status = -1;  // Turn off shield
		PAL_setPalette(PAL1, player_palette.data, DMA_QUEUE);
		new_shield_delay_timer = 0; // Reset time for when shield is ready.
		shield_timer = 0;			// Reset shield_timer

		// XGM2_stopPCM(SOUND_PCM_CH2);

	} else if (shield_status == 1){

		if (shield_pal_timer > shield_pal_timer_switch){
			shield_pal = (shield_pal + 1) % 3;
			if (shield_pal == 0){
				PAL_setPalette(PAL1, player_pal2.data, DMA_QUEUE);
			} else if (shield_pal == 1){
				PAL_setPalette(PAL1, player_pal3.data, DMA_QUEUE);
			} else if (shield_pal == 2){
				PAL_setPalette(PAL1, player_pal4.data, DMA_QUEUE);
			}
			shield_pal_timer = 0; // Reset timer.
		}

		

		shield_pal_timer += 1;
		shield_timer += 1;

	}

}