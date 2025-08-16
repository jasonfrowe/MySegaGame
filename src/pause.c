#include "pause.h"
#include <genesis.h>
#include "globals.h" // For screen dimensions, game_paused, etc.

void pause_screen(void) {
    // Draw PAUSE centered
    VDP_clearText(0, (screen_height_pixels_d2 / 8) - 3, 40); // Clear center line
    VDP_drawText("PAUSE", (screen_width_pixels_d2 / 8) - 3, screen_height_pixels_d2 / 8);
    SPR_setVisibility(player_sprite, HIDDEN);

    for (s16 i = 0; i < NEBULLET; i++) {
        if(ebullets[i].sprite_ptr != NULL) SPR_setVisibility(ebullets[i].sprite_ptr, HIDDEN);
	}

	for (s16 i = 0; i < NBULLET; i++) {
        if(bullets[i].sprite_ptr != NULL) SPR_setVisibility(bullets[i].sprite_ptr, HIDDEN);
    }

    for (s16 i = 0; i < active_fighter_count; i++) {
        if(fighters[i].sprite_ptr != NULL) SPR_setVisibility(fighters[i].sprite_ptr, HIDDEN);
        if(fexplode[i].sprite_ptr != NULL) SPR_setVisibility(fexplode[i].sprite_ptr, HIDDEN);
    }

    for (s16 i = 0; i < NSBULLET; i++) {
        if(sbullets[i].sprite_ptr != NULL) SPR_setVisibility(sbullets[i].sprite_ptr, HIDDEN);
    }

    if(mine_sprite_ptr) SPR_setVisibility(mine_sprite_ptr, HIDDEN);
    if (mexplode_sprite_ptr) SPR_setVisibility(mexplode_sprite_ptr, HIDDEN);

    while (1) {
        u16 value = JOY_readJoypad(JOY_1);
        if (value & BUTTON_START) {
            break;
        }
        SPR_update();
        SYS_doVBlankProcess();
    }
    VDP_clearText(0, screen_height_pixels_d2 / 8, 40); // Clear PAUSE when resuming
    SPR_setVisibility(player_sprite, VISIBLE); // Restore player sprite visibility

    for (s16 i = 0; i < NEBULLET; i++) {
        if(ebullets[i].sprite_ptr != NULL) SPR_setVisibility(ebullets[i].sprite_ptr, VISIBLE);
    }
    for (s16 i = 0; i < NBULLET; i++) {
        if(bullets[i].sprite_ptr != NULL) SPR_setVisibility(bullets[i].sprite_ptr, VISIBLE);
    }
    for (s16 i = 0; i < active_fighter_count; i++) {
        if(fighters[i].sprite_ptr != NULL) SPR_setVisibility(fighters[i].sprite_ptr, VISIBLE);
        if(fexplode[i].sprite_ptr != NULL) SPR_setVisibility(fexplode[i].sprite_ptr, VISIBLE);
    }
    for (s16 i = 0; i < NSBULLET; i++) {
        if(sbullets[i].sprite_ptr != NULL) SPR_setVisibility(sbullets[i].sprite_ptr, VISIBLE);
    }   
    if(mine_sprite_ptr) SPR_setVisibility(mine_sprite_ptr, VISIBLE);
    if (mexplode_sprite_ptr) SPR_setVisibility(mexplode_sprite_ptr, VISIBLE);
}
