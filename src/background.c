// background.c
#include <genesis.h>
#include "globals.h"    // For scroll offsets, player_scroll_delta_x/y, map constants
#include "background.h"
#include "resources.h" // For bg_far_tiles, bg_near_tiles for numTile

void initBackground(void) {

    // Initial scroll positions
    scroll_a_x = 0; scroll_a_y = 0;
    scroll_b_x = 0; scroll_b_y = 0;

    // VDP_loadTileSet(&star_bg_tiles, ind, DMA_QUEUE);
    // star_map = MAP_create(&star_bg_map, BG_B, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind));
    // PAL_setPalette(PAL0, star_bg_pal.data, DMA_QUEUE);
    // MAP_scrollTo(star_map, scroll_b_x, scroll_b_y);

    // New background
    PAL_setPalette(PAL0, star_bg_pal.data, DMA_QUEUE);
    VDP_drawImageEx(BG_B, &starbg, TILE_ATTR_FULL(PAL0, FALSE, FALSE, FALSE, ind), 0, 0, FALSE, TRUE);
    ind_sc = ind + starbg.tileset -> numTile;
}

// --- Update Scrolling Function ---
void updateScrolling() {
    // player_scroll_delta_x/y is how much the player *would* move if not for screen boundaries
    // This delta is applied to the background scroll in the opposite direction

    // scroll_a_x -= player_scroll_delta_x; // Near layer (Plane A)
    // scroll_a_y += player_scroll_delta_y; // SGDK Y scroll is often inverted depending on perspective

    // Parallax for far layer (Plane B)
    scroll_b_x -= (player_scroll_delta_x / PARALLAX_FACTOR_BG_B) % 512; // Or use PARALLAX_FACTOR_BG_B as per defines
    scroll_b_y += (player_scroll_delta_y / PARALLAX_FACTOR_BG_B) % 256; // Assuming A is near, B is far from defines


    // VDP_setHorizontalScroll(BG_A, scroll_a_x);
    // VDP_setVerticalScroll(BG_A, scroll_a_y);
    VDP_setHorizontalScroll(BG_B, scroll_b_x);
    VDP_setVerticalScroll(BG_B, scroll_b_y);
}