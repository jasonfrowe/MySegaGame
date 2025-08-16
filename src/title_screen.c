// title_screen.c
#include <genesis.h>
#include "globals.h" // For Fighter struct, fighters array, player_x/y, map constants, screen_width_pixels etc.
#include "res/resources.h" 
#include "title_screen.h"


u16 button_delay = 30;
u16 fcounter_l = 0;
u16 fcounter_r = 0;

u16 level_pos = 23;

u16 pal_count  = 0;
u16 pal_change = 35; // how often to change palette 
u16 pal_to_use = 0;

void title_screen(){

    PAL_setPalette(PAL3, title_pal_1.data, DMA_QUEUE);

    VDP_drawImageEx(BG_B, &title, TILE_ATTR_FULL(PAL3, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);

	// VDP_drawText("  Ready?  ", 15, 13);
    // VDP_drawText("Push Start", 15, 14);

    // intToStr(game_level, text_vel_x, 2);
    // VDP_drawText(" Level:   ", 15, level_pos);
    // VDP_drawText(text_vel_x  , 22, level_pos);
    if (control_style == 0){
        VDP_drawTextBGFill(BG_A, "Control R", 15, level_pos, 8); //Rotational style
    } else {
        VDP_drawTextBGFill(BG_A, "Control D", 15, level_pos, 8); //Directional style
    }

    // Start music
    XGM2_setLoopNumber(-1);
    XGM2_play(title_music);
    XGM2_setFMVolume(75);

    fcounter_l = 0;
    fcounter_r = 0;

    u16 value_old = JOY_readJoypad(JOY_1);

    while(1){

        pal_count += 1;
        if (pal_count > pal_change){
            pal_to_use = (pal_to_use + 1)%4;
            pal_count = 0;
            if (pal_change == 35){
                pal_change = 59;
            } else {
                pal_change = 35;
            }
        }
        
        if (pal_to_use == 0){
            PAL_setPalette(PAL3, title_pal_1.data, DMA_QUEUE);
        } else if (pal_to_use == 1){
            PAL_setPalette(PAL3, title_pal_2.data, DMA_QUEUE);
        } else if (pal_to_use == 2){
            PAL_setPalette(PAL3, title_pal_3.data, DMA_QUEUE);
        } else if (pal_to_use == 3){
            PAL_setPalette(PAL3, title_pal_4.data, DMA_QUEUE);
        }
        
        u16 value = JOY_readJoypad(JOY_1);

        if (value != value_old){
        	value_old = value;
        	fcounter_l = 0;
        	fcounter_r = 0;
        }

        if (value & BUTTON_START) {
            // Prevents reading pause while start is held while entering game loop
            start_was_pressed = 1;
            break;  // Let's start the game.
        }

        if (value & BUTTON_LEFT){
        	if (fcounter_l == 0){
	        	// game_level -= 1;
	        	// if (game_level < 1){
	        	// 	game_level = 1;
	        	// }
                control_style -= 1;
                if (control_style < 0){
                    control_style = 1;
                }
	        	fcounter_l = 1;
	        }
        }

        if (value & BUTTON_RIGHT){
        	if (fcounter_r == 0){
	        	// game_level += 1;
	        	// if (game_level > 10){
	        	// 	game_level = 10;
	        	// }
                control_style += 1;
                if (control_style > 1){
                    control_style = 0;
                }
	        	fcounter_r = 1;
	        }
        }

        // if (game_level != game_level_old){
        // 	game_level_old = game_level;
        // 	VDP_clearText(15, level_pos, DEBUG_TEXT_LEN + 6);
        // 	intToStr(game_level, text_vel_x, 2);
		//     VDP_drawText(" Level:   ", 15, level_pos);
		//     VDP_drawText(text_vel_x  , 22, level_pos);
        // }
        if (control_style != control_style_old){
            control_style_old = control_style;
            if (control_style == 0){
                VDP_drawTextBGFill(BG_A, "Control R", 15, level_pos, 8); //Rotational style
                boost_factor = 5;
            } else {
                VDP_drawTextBGFill(BG_A, "Control D", 15, level_pos, 8); //Directional style
                boost_factor = 2; 
            }
        }

        
        if (fcounter_l > button_delay){
        	fcounter_l = 0;
        }
        if (fcounter_l > 0){
        	fcounter_l += 1;
        }

        if (fcounter_r > button_delay){
        	fcounter_r = 0;
        }
        if (fcounter_r > 0){
        	fcounter_r += 1;
        }

        SPR_update();
        SYS_doVBlankProcess();
    }

    // VDP_clearText(15, 13, DEBUG_TEXT_LEN + 6);
    // VDP_clearText(15, 14, DEBUG_TEXT_LEN + 6);
    VDP_clearText(15, level_pos, DEBUG_TEXT_LEN + 6);
    game_level_old = 0; // Reset to allow screen updates
    efire_cooldown_timer = 4 * (5 - game_level);
    if (efire_cooldown_timer > 16){
        efire_cooldown_timer = 16;
    }
    else if (efire_cooldown_timer < efire_cooldown_timer_min){
        efire_cooldown_timer = efire_cooldown_timer_min;
    }

    fighter_speed_1 = 8;
    fighter_speed_2 = 256;

    game_ai_decision = 20000;      // when ships will change direction (when == game_nframe)
    // game_ai_decision_time = 60;

    XGM2_stop(); // Stop title music

}

// Set up scoring and level details
void init_game_vars(){
	player_score = 0;
    player_score_old = 1;

    fighters_score = 0;
    fighters_score_old = 1;

    game_level = 1;
    game_level_old = 0;
    score_to_win = 100;
}
