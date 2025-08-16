// bullets.c
#include <genesis.h>
#include "globals.h" // For Bullet struct, bullets array, player_x/y, sin_fix, cos_fix, screen_width_pixels, etc.
#include "game_level_screen.h"
#include "clear_sprites.h"
#include "res/resources.h" // For sprites

#include "bullets.h"
#include "ebullets.h"
#include "sbullets.h"
#include "fighters.h"

#include "title_screen.h"
#include "background.h"
#include "hud.h"

void level_up(){


	// Game over loop

    u8 game_over = 0;
    u16 value;
    u16 value_old;
    
    XGM2_stop();
    if (player_score > fighters_score){
        VDP_drawText(" Level Up!", 15, 13);

        game_level += 1;
        efire_cooldown_timer -= 4;
        if (efire_cooldown_timer < efire_cooldown_timer_min){
            efire_cooldown_timer = efire_cooldown_timer_min;
        }

        if (game_level < 5){
            // fighter_speed_1 = 8;
            // fighter_speed_2 = 256;
            game_ai_decision = 20000;      // when ships will change direction (when == game_nframe)
            // game_ai_decision_time = 60;
        } else if (game_level < 7){
            // fighter_speed_1 = 7;
            // fighter_speed_2 = 128;
            game_ai_decision = 10000;      // when ships will change direction (when == game_nframe)
            // game_ai_decision_time = 30;
        } else {
            // fighter_speed_1 = 6;
            // fighter_speed_2 = 64;
            game_ai_decision = 5000;      // when ships will change direction (when == game_nframe)
            // game_ai_decision_time = 15;
        }

        player_score = 0;
        player_score_old = 1;
        fighters_score = 0;
        fighters_score_old = 1;
        score_to_win = 100; 

    }
    else {
        // VDP_drawText(" You Lose ", 15, 13);  // This is the game-over part...
        VDP_drawText(" Game Over", 15, 13);  // This is the game-over part...

        // Once we have a proper game loop the next 4 lines can probably be removed.
        player_score = 0;
        player_score_old = 1;
        fighters_score = 0;
        fighters_score_old = 1;

        game_over = 1;
    }

    VDP_drawText("Push  Start", 15, 14);
    while(1){
        
        value = JOY_readJoypad(JOY_1);
        if (value & BUTTON_START) {
            break;
        }

        SPR_update();
        SYS_doVBlankProcess();
    }

    VDP_clearText(15, 13, DEBUG_TEXT_LEN + 6); // Remove text
    VDP_clearText(15, 14, DEBUG_TEXT_LEN + 6);

    // Reset game.
    if (game_over){

        
        value_old = JOY_readJoypad(JOY_1);
        u8 x = 1;
        while(x){
            value = JOY_readJoypad(JOY_1);
            if (value != value_old){
                x = 0;
            } 
            // else {
            //     VDP_clearText(1, 5, DEBUG_TEXT_LEN + 6);
            //     intToStr(value, text_vel_x, 5);
            //     VDP_drawText("Button ", 1, 5);
            //     VDP_drawText(text_vel_x , 9, 5);
            //     value_old = value;
            // }

            SPR_update();
            SYS_doVBlankProcess();
        }


        clear_sprites();

        init_game_vars(); // Set up player/enemy scores

        // Reset scroll positions
        scroll_a_x = 0; scroll_a_y = 0;
        scroll_b_x = 0; scroll_b_y = 0;
        VDP_setHorizontalScroll(BG_B, scroll_b_x);
        VDP_setVerticalScroll(BG_B, scroll_b_y);


        VDP_clearText(8,  1, 8);
        VDP_clearText(23, 1, 8);

        clearHud();

        title_screen();   // Show title screen.

        PAL_setPalette(PAL3, title_pal_1.data, DMA_QUEUE); // Reset PAL3 after title 

        initBackground(); // Initializes tiles, maps, and initial scroll
        initHud();

        // Initialize game entities
        initBullets();
        init_SBullets();
        initFighters();
        init_eBullets();

        // Create player sprite (player_x, player_y are from game_data.c)
        // player_sprite = SPR_addSprite(&player_sprite_res,
        //                             player_x, player_y,
        //                             TILE_ATTR(PAL1, TRUE, FALSE, FALSE));


        // Reset player
        player_x = 144; player_y = 104;
        player_vx = 0; player_vy = 0;
        player_vx_applied = 0; player_vy_applied = 0;
        player_x_remainder = 0; player_y_remainder = 0;
        player_rotation_index = 0;
        player_thrust_momentum_x = 0;
        player_thrust_momentum_y = 0;

        SPR_setVisibility(player_sprite, VISIBLE);

        game_score = 0;


    }


    XGM2_play(track1);

}