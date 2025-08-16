// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include <genesis.h> // For Sprite, u16, s16 etc.
#include "res/resources.h" // For bg_far_tiles.numTile etc.
#include "constants.h"     // Include our constants

// --- Struct Definitions ---
typedef struct {
    s16 status;
    s16 x;
    s16 y;
    s16 bvxrem;
    s16 bvyrem;
    s16 new_bullet; // Renamed from 'new' to avoid keyword clash if C++ compiler/linter
    Sprite* sprite_ptr;
} Bullet;

typedef struct {
    s16 status;
    s16 x;
    s16 y;
    s16 lx1old; // Consider if these 'old' vars are still needed
    s16 lx2old;
    s16 ly1old;
    s16 ly2old;
    s16 dx;
    s16 dy;
    s16 vx;
    s16 vy;
    s16 vxi;
    s16 vyi;
    s16 xrem;
    s16 yrem;
    s16 new_fighter; // Renamed
    s16 nframe_fighter; // Renamed to avoid clash with global nframe
    Sprite* sprite_ptr;
} Fighter;


// --- Global Variables (declared as extern) ---

// Screen Dimensions (Consolidated)
extern s16 screen_width_pixels;
extern s16 screen_height_pixels;
extern s16 screen_width_pixels_d2; //dimensions / 2
extern s16 screen_height_pixels_d2;

// Scroll Boundaries
extern s16 scroll_boundary_x1; // Renamed BX1
extern s16 scroll_boundary_x2; // Renamed BX2
extern s16 scroll_boundary_y1; // Renamed BY1
extern s16 scroll_boundary_y2; // Renamed BY2

// Background Scroll Offsets
extern s16 scroll_a_x; extern s16 scroll_a_y; // Plane A (Near)
extern s16 scroll_b_x; extern s16 scroll_b_y; // Plane B (Far)
// extern s16 scroll_x; extern s16 scroll_y; // Overall map scroll if different (seems covered by dx/dy from player)

// Sine/Cosine Tables
extern const s16 sin_fix[];
extern const s16 cos_fix[];
extern const s16 sin_fix_d2[];
extern const s16 cos_fix_d2[];
#define SINCOS_TABLE_STEPS (24) // Hardcode for now based on data, or calculate in game_data.c

// Player Sprite
extern Sprite* player_sprite;

// Background Map Data Arrays
// These are large, could be static in background.c if only used there.
// If generateRandomMapLayer is in background.c and uses them, they can be static there.
// Let's make them static in background.c for now to keep globals.h cleaner.
// extern u16 farMapData[MAP_HW_WIDTH * MAP_HW_HEIGHT];
// extern u16 nearMapData[MAP_HW_WIDTH * MAP_HW_HEIGHT];

// Player/Spacecraft properties
extern s16 player_x; extern s16 player_y;
extern s16 player_vx; extern s16 player_vy;
extern s16 player_vx_applied; extern s16 player_vy_applied; // Renamed vxapp, vyapp
extern s16 player_x_remainder; extern s16 player_y_remainder; // Renamed xrem, yrem
extern s16 player_rotation_index; // Renamed ri
extern const s16 player_rotation_index_max; // Renamed ri_max
extern s16 player_rotation_iframe; // Renamed iframe
extern const s16 player_rotation_iframe_threshold; // Renamed iframe_old
extern s16 player_thrust_delay_timer; // Renamed tdelay
extern const s16 player_thrust_delay_max; // Renamed tdelay_max
extern s16 player_thrust_counter; // Renamed tcount
extern s16 player_thrust_momentum_x; // Renamed thrust_x
extern s16 player_thrust_momentum_y; // Renamed thrust_y
extern s16 player_scroll_delta_x; // Renamed dx (player's contribution to scroll)
extern s16 player_scroll_delta_y; // Renamed dy
extern s16 boost_factor;

// Player D-control
extern u8 dpadUp;
extern u8 dpadDown;
extern u8 dpadLeft;
extern u8 dpadRight;

// Bullet Pool and related
extern Bullet bullets[NBULLET];
extern u16 fire_cooldown_timer; // Renamed fire_cooldown
extern u16 current_bullet_index; // Renamed bullet_c
extern u16 new_bullet_delay_timer; // Renamed bullet_timer
// extern s16 bullet_vx_req; // Renamed bvx - seems calculated on the fly
// extern s16 bullet_vy_req; // Renamed bvy
// extern s16 bullet_vx_applied; // Renamed bvxapp
// extern s16 bullet_vy_applied; // Renamed bvyapp

// E-Bullet Pool
extern Bullet ebullets[NEBULLET];
extern u16 efire_cooldown_timer; 
extern u16 efire_cooldown_timer_min;
extern u16 current_ebullet_index; 
extern u16 new_ebullet_delay_timer; 

// S-Bullet Pool
extern Bullet sbullets[NSBULLET];
extern u16 sfire_cooldown_timer; 
extern u16 current_sbullet_index; 
extern u16 new_sbullet_delay_timer; 

// Fighter Pool and related
extern Fighter fighters[NFIGHTER_MAX];
extern Fighter fexplode[NFIGHTER_MAX];
extern s16 active_fighter_count; // Renamed nfighter (maybe better to count active ones)
extern s16 fighter_speed_1;
extern s16 fighter_speed_2;
extern u16 game_ai_decision;      // when ships will change direction (when == game_nframe)
// extern s16 game_ai_decision_time;
// extern s16 game_ai_rand;

// Debug Text Buffers
extern char text_vel_x[DEBUG_TEXT_LEN];
extern char text_vel_y[DEBUG_TEXT_LEN];

// General frame counter
extern s16 game_nframe; // Renamed nframe to be more specific

// Tile indexing based on resource sizes
#define BG_NEAR_TILE_INDEX (BG_FAR_TILE_INDEX + bg_far_tiles.numTile)

// Loading background map
extern u16 ind;
extern u16 ind_sc; // for score tiles

// Scores and Levels
extern s16 player_score;
extern s16 player_score_old;
extern u16 fighters_score;
extern u16 fighters_score_old;
extern u16 score_to_win;
extern u16 game_level;
extern u16 game_level_old;
extern u16 game_score;
extern u16 game_score_old;

// HUD
extern u16 player_tiles;
// extern u16 player_tiles_red;
extern u16 coffset;
extern u16 STRIP_TILE_8_IDX;
extern u16 EMPTY_BAR_TILE_IDX;

// Pause state
extern u16 game_paused;
// Detect start press instead of start down
extern u16 start_was_pressed;

// Title screen
extern s16 control_style;
extern s16 control_style_old;


// Ship Shield
extern u16 new_shield_delay_timer;
extern u16 shield_delay_max;
extern u16 shield_timer;
extern u16 shield_duration;
extern u16 shield_pal;
extern s16 shield_status;
extern u16 shield_pal_timer;
extern u16 shield_pal_timer_switch;

//Boost
extern u16 player_boost_timer;
extern u16 player_boost_timer_max; // How long the boost lasts
extern u16 player_boost_delay_timer;
extern u16 player_boost_delay_timer_max; // How often the player can boost
extern s16 player_boost_status;

// Space Mines
extern s16 mine_status; // Shield is off.
extern s16 mine_x;
extern s16 mine_y;
extern u16 mine_timer;   // Time for mine to arm itself
extern u16 mine_timer_max;
extern s16 mexplode_status;
extern Sprite* mine_sprite_ptr;
extern Sprite* mexplode_sprite_ptr;


#endif // GLOBALS_H