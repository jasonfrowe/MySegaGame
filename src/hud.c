// spaceMines.c
#include <genesis.h>
#include "globals.h" 
#include "constants.h" // BAR_WIDTH_TILES ; STRIPS_PER_TILE
#include "hud.h"
#include "res/resources.h"

void drawHud(){
	// --- Draw Player Score ---
    if (player_score != player_score_old ){

        // --- Add filling bar on Player side

	    // if player_score goes above 100, this may overflow!
	    // total_strip_states = ( (s32)current_score * (BAR_WIDTH_TILES * STRIPS_PER_TILE) ) / MAX_SCORE;
	    u16 total_strip_states = (  player_score * (BAR_WIDTH_TILES * STRIPS_PER_TILE) ) / score_to_win;

	    // Determine how many tiles are completely full (8 strips)
	    u16 num_full_tiles = total_strip_states / STRIPS_PER_TILE;

	    // Determine how many strips are in the tile immediately following the full ones
    	u16 strips_in_next_tile = total_strip_states % STRIPS_PER_TILE;

	    u16 STRIP_TILE_8_IDX   = player_tiles + 8;
	    u16 EMPTY_BAR_TILE_IDX = player_tiles;
	    u16 tile_index_to_draw;

	    for (u16 i = 0; i < BAR_WIDTH_TILES; ++i) {
	        if (i < num_full_tiles) {
	            // This tile is completely full
	            tile_index_to_draw = STRIP_TILE_8_IDX;
	        } else if (i == num_full_tiles) {
	            // This is the tile that might be partially filled, or empty if strips_in_next_tile is 0
	            // EMPTY_BAR_TILE_IDX + 0 is the empty tile
	            // EMPTY_BAR_TILE_IDX + 1 is the 1-strip tile, ..., EMPTY_BAR_TILE_IDX + 8 is the 8-strip tile
	            tile_index_to_draw = EMPTY_BAR_TILE_IDX + strips_in_next_tile;
	        } else {
	            // This tile is beyond the filled portion, so it's empty
	            tile_index_to_draw = EMPTY_BAR_TILE_IDX;
	        }
	        // VDP_setTileMapData(WINDOW, TILE_ATTR_FULL(pal, FALSE, FALSE, FALSE, tile_index_to_draw),
	        //                    start_x + i, start_y, DMA_QUEUE);

	        VDP_setTileMapXY(BG_A,TILE_ATTR_FULL(PAL3,0,FALSE,FALSE,tile_index_to_draw),8+i,1);
	    }


	    intToStr(player_score, text_vel_x, 3); // Using player_x from globals
	    VDP_clearText(1, 1, 7);
        VDP_drawText("You ", 1, 1); VDP_drawText(text_vel_x, 5, 1);
        player_score_old = player_score;


    }

    // --- Draw Enemy Score ---
    if (fighters_score != fighters_score_old ){
        VDP_clearText(31, 1, 8);
        intToStr(fighters_score, text_vel_x, 3); // Using player_x from globals
        VDP_drawText(" THEM", 34, 1); VDP_drawText(text_vel_x, 31, 1);
        fighters_score_old = fighters_score;
    }

    // --- Draw Level Indicator ---
    if (game_level != game_level_old ){
        VDP_clearText(16, 1, 7);
        intToStr(game_level, text_vel_x, 2); // Using player_x from globals
        VDP_drawText("Level", 16, 1); VDP_drawText(text_vel_x, 21, 1);
        game_level_old = game_level;
    }

}

void initHud(){

	player_tiles = VDP_loadTileSet(&player_score_tiles, 1, DMA_QUEUE);



}