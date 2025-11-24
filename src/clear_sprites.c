// ebullets.c
#include <genesis.h>
#include "globals.h" // For Bullet struct, bullets array, player_x/y, sin_fix, cos_fix, screen_width_pixels, etc.
#include "clear_sprites.h"
#include "resources.h" // For bullet_sprite_res

#include "bullets.h"
#include "ebullets.h"
#include "sbullets.h"
#include "fighters.h"


void clear_sprites(){

	for (s16 i = 0; i < NEBULLET; i++) {
		ebullets[i].status = -1; // Deactivate bullet
        if(ebullets[i].sprite_ptr != NULL) SPR_releaseSprite(ebullets[i].sprite_ptr);
        ebullets[i].sprite_ptr = NULL;
	}

	for (s16 i = 0; i < NBULLET; i++) {
		bullets[i].status = -1; // Deactivate bullet
        if(bullets[i].sprite_ptr != NULL) SPR_releaseSprite(bullets[i].sprite_ptr);
        bullets[i].sprite_ptr = NULL;
    }

    for (s16 i = 0; i < active_fighter_count; i++) {
    	fighters[i].status = -1; // Deactivate fighter (-9 means we do an explosion)
        if(fighters[i].sprite_ptr != NULL) SPR_releaseSprite(fighters[i].sprite_ptr);
        fighters[i].sprite_ptr = NULL;

        fexplode[i].status = 0;
        if(fexplode[i].sprite_ptr != NULL) SPR_releaseSprite(fexplode[i].sprite_ptr);
        fexplode[i].sprite_ptr = NULL;

    }

    for (s16 i = 0; i < NSBULLET; i++) {
    	sbullets[i].status = -1; // Deactivate bullet
        if(sbullets[i].sprite_ptr != NULL) SPR_releaseSprite(sbullets[i].sprite_ptr);
        sbullets[i].sprite_ptr = NULL;
    }

    // Clear mines
    if(mine_sprite_ptr) SPR_releaseSprite(mine_sprite_ptr);
    if (mexplode_sprite_ptr) SPR_releaseSprite(mexplode_sprite_ptr);
    mine_status = 0;

    // if(player_sprite != NULL) SPR_releaseSprite(player_sprite);
    // player_sprite = NULL;
    SPR_setVisibility(player_sprite, HIDDEN);

}