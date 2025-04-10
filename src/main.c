#include <genesis.h>
#include <maths.h>
#include "res/resources.h"


// --- Background & Scrolling Constants ---
// Map dimensions in Hardware Tiles (Use VDP Plane size directly for simplicity)
#define MAP_HW_WIDTH            64
#define MAP_HW_HEIGHT           32 // Use 64x32 plane size (common, saves VRAM/RAM)
// Tile indexing - Need base indices for both layers
#define BG_FAR_TILE_INDEX       TILE_USER_INDEX                 // Start far tiles after font
#define BG_NEAR_TILE_INDEX      (BG_FAR_TILE_INDEX + bg_far_tiles.numTile) // Start near tiles after far tiles
// Scrolling Parameters
#define BBX                     100 // Scroll when player X is < this or > screen_w - this
#define BBY                     80  // Scroll when player Y is < this or > screen_h - this
// Parallax Factors (Higher value = slower scroll) - Use powers of 2 for shift efficiency
#define PARALLAX_FACTOR_BG_B    4  // Far layer scrolls at 1/4 player speed delta
#define PARALLAX_FACTOR_BG_A    2  // Near layer scrolls at 1/2 player speed delta

// For storing size of screen
s16 SWIDTH;
s16 SHEIGHT;

// Boundary for scrolling -- how far we can move the spaceshift until the background scrolls 
s16 BX1; s16 BX2; s16 BY1; s16 BY2;

// How much the map is scrolled
s16 scroll_x = 0; s16 scroll_y = 0;

// --- Pre-calculated Sine/Cosine Tables ---
// Scaled by 255, 24 steps (+1 for wraparound)
static const s16 sin_fix[] = {
      0,  65, 127, 180, 220, 246, 255, 246, 220, 180, 127,  65,
      0, -65,-127,-180,-220,-246,-255,-246,-220,-180,-127, -65, 0 // Index 24 = Index 0
};
static const s16 cos_fix[] = {
    255, 246, 220, 180, 127,  65,   0, -65,-127,-180,-220,-246,
   -255,-246,-220,-180,-127, -65,   0,  65, 127, 180, 220, 246, 255 // Index 24 = Index 0
};
// Calculate the number of unique steps (excluding the wrap-around entry)
#define SINCOS_TABLE_STEPS (sizeof(sin_fix) / sizeof(sin_fix[0]) - 1) // Should be 24

// --- Sound effects ---
#define SFX_LASER 64

// --- Game Variables ---
Sprite* player_sprite;
// Background Scroll Offsets (in pixels)
s16 scroll_a_x = 0; s16 scroll_a_y = 0; // Plane A (Near) scroll
s16 scroll_b_x = 0; s16 scroll_b_y = 0; // Plane B (Far) scroll
// Screen Dimensions
s16 screen_width_pixels; s16 screen_height_pixels;

// --- Background Map Data Arrays (Size based on Plane size: 64x32 = 4KB RAM total) ---
static u16 farMapData[MAP_HW_WIDTH * MAP_HW_HEIGHT];   // Plane B
static u16 nearMapData[MAP_HW_WIDTH * MAP_HW_HEIGHT];  // Plane A

// --- Spacecraft properties --- //
#define SHIP_ROT_SPEED 3 // How fast the spaceship can rotate, must be >= 1.
// Makes rotation of ship slower
s16 iframe      = 0;                // Timer for rotationation of spacecraft when dpad is held
s16 iframe_old  = SHIP_ROT_SPEED;  
s16 ri          = 0;                // current rotation info for spaceship
s16 ri_max      = 23;               // max rotations 
// Initial position and velocity of spacecraft
s16 x           = 144;              //Screen-coordinates of spacecraft
s16 y           = 104;
s16 vx          = 0;                //Requested velocity of spacecraft.
s16 vy          = 0;
s16 vxapp       = 0;                //Applied velocity of spacecraft.
s16 vyapp       = 0;

s16 xtry        = 144;              //For displaying Space ship
s16 ytry        = 104;
s16 xrem        = 0;                //Tracking remainder for smooth motion of space ship
s16 yrem        = 0;

s16 tdelay      = 0;                // Counter for thrust/momentum/friction 
s16 tdelay_max  = 8;                // momentum
s16 tcount      = 0;

s16 thrust_x    = 0;                // Initialize amount of thrust applied (acts like momentum)
s16 thrust_y    = 0;

s16 thx         = 0;                //Checking thrust for max allowed values.
s16 thy         = 0;

s16 dx          = 0;
s16 dy          = 0;
// -----------------------------------


// --- Properties for bullets --- //
#define NBULLET 8  // maximum good-guy bullets
#define NBULLET_TIMER_MAX 8 // Sets interval for new bullets when fire button is held

// --- Bullet Structure ---
typedef struct {
    s16 status;         // 0 = inactive, 1 = active
    s16 x;              // X-position
    s16 y;              // Y-position
    s16 bvxrem;         // Tracking remainders for smooth motion 
    s16 bvyrem;         // 
    s16 new   ;         // If this is a new build 0 = no, 1 = yes
    Sprite* sprite_ptr; // Pointer to the hardware sprite object
} Bullet;

// Bullet Pool
Bullet bullets[NBULLET];
s16 fire_cooldown           = 0;    // Timer for limiting fire rate
s16 bullet_c                = 0;    //Counter for bullets
s16 bullet_timer            = 0;    //delay timer for new bullets

// static const s16 bullet_v   = 4;    //Bullet Speed (not implemented -- fixed at the moment)
s16 bvx                     = 0;    //Requested velocity
s16 bvy                     = 0;
s16 bvxapp                  = 0;    //Applied velocity (round-off)
s16 bvyapp                  = 0;


// --- ADD BUFFERS FOR DEBUG TEXT ---
#define DEBUG_TEXT_LEN 16 // Max length for the velocity strings
char text_vel_x[DEBUG_TEXT_LEN];
char text_vel_y[DEBUG_TEXT_LEN];
// -----------------------------------

// Function prototypes
void handleInput();
void updatePhysics();       // Motion of spaceship
void updateBullets();       // Update bullets
void fireBullet();          // New function
void initBullets();         // Initialize bullet class
void generateRandomMapLayer(u16* mapData, u16 mapWidth, u16 mapHeight, u16 baseTileIndex, u16 numTilesInSet, u16 pal); // Modified generator
void updateScrolling(); // <-- New function

// --- Main Function ---
int main()
{
    SYS_disableInts();

    // VDP text system is implicitly initialized by VDP_init
    VDP_init();
    SPR_init();
    JOY_init();
    // === Add this line to enable 6-button support detection ===
    JOY_setSupport(PORT_1, JOY_SUPPORT_6BTN);

    //  --- Sound effects ---
    XGM_setPCM(SFX_LASER, sfx_laser, sizeof(sfx_laser));
    
    VDP_setScreenWidth320();

    // Get screen dimensions
    SWIDTH = VDP_getScreenWidth(); // Store for frequent use
    SHEIGHT = VDP_getScreenHeight();

    BX1 = BBX;
    BX2 = SWIDTH - BBX;
    BY1 = BBY;
    BY2 = SHEIGHT - BBY;

    // --- Setup Background Planes ---
    VDP_setPlaneSize(BG_A, MAP_HW_WIDTH, MAP_HW_HEIGHT); // Near Stars (64x32 tiles)
    VDP_setPlaneSize(BG_B, MAP_HW_WIDTH, MAP_HW_HEIGHT); // Far Stars (64x32 tiles)
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    // Set Priorities: Plane A (Near) is HIGHER (0) than Plane B (Far) (1)
    // VDP_setPlanePriority(BG_A, TRUE);  // High priority for Plane A
    // VDP_setPlanePriority(BG_B, FALSE); // Low priority for Plane B
    // Enable per-plane scrolling
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);
    // -----------------------------

    VDP_setTextPlane(BG_B); VDP_setTextPalette(3); // Text on top of BG_B temporarily

    // --- Load Palettes ---
    PAL_setPalette(PAL0, bg_far_palette.data, DMA_QUEUE);  // Far stars palette to PAL0
    PAL_setPalette(PAL1, player_palette.data, DMA_QUEUE); // Player/Bullet palette to PAL1
    PAL_setPalette(PAL2, bg_near_palette.data, DMA_QUEUE); // Near stars palette to PAL2
    // ---------------------

    // --- Load Background Tiles ---
    // Load far tiles first, starting at BG_FAR_TILE_INDEX
    VDP_loadTileData(bg_far_tiles.tiles, BG_FAR_TILE_INDEX, bg_far_tiles.numTile, DMA);
    // Load near tiles immediately after far tiles
    VDP_loadTileData(bg_near_tiles.tiles, BG_NEAR_TILE_INDEX, bg_near_tiles.numTile, DMA);
    // ---------------------------

    // --- Generate and Upload Maps ---
    generateRandomMapLayer(farMapData, MAP_HW_WIDTH, MAP_HW_HEIGHT, BG_FAR_TILE_INDEX, bg_far_tiles.numTile, PAL0);
    generateRandomMapLayer(nearMapData, MAP_HW_WIDTH, MAP_HW_HEIGHT, BG_NEAR_TILE_INDEX, bg_near_tiles.numTile, PAL2);
    // Upload maps to respective VDP planes
    VDP_setTileMapDataRect(BG_B, farMapData,  0, 0, MAP_HW_WIDTH, MAP_HW_HEIGHT, MAP_HW_WIDTH, DMA); // Far stars to B
    VDP_setTileMapDataRect(BG_A, nearMapData, 0, 0, MAP_HW_WIDTH, MAP_HW_HEIGHT, MAP_HW_WIDTH, DMA); // Near stars to A
    // ------------------------------

    // Initialize bullet structures
    initBullets();

    // Create sprite
    player_sprite = SPR_addSprite(
                        &player_sprite_res,
                        x,
                        y,
                        TILE_ATTR(PAL1, TRUE, FALSE, FALSE));

    XGM_setLoopNumber(-1);
    XGM_startPlay(track1);

    VDP_setBackgroundColor(0);
    SYS_enableInts();


    // Main Game Loop
    while (1)
    {

        // Calculate player's current SCREEN position (needed for scrolling logic)
        // s16 player_screen_x = x;
        // s16 player_screen_y = y;

        handleInput();
        updatePhysics(); // Update velocities BEFORE drawing them
        updateBullets();  // Update bullets fired by player

        // --- Set Sprite Frame based on Rotation ---
        SPR_setFrame(player_sprite, ri); // Use 'ri' to select the frame

        // --- Draw Debug Text ---
        // Clear previous text (optional, avoids ghosting if text length changes)
        VDP_clearText(1, 1, DEBUG_TEXT_LEN + 6); // Clear area for X velocity (X=1, Y=1, Length="VelX: "+value)
        VDP_clearText(1, 2, DEBUG_TEXT_LEN + 6); // Clear area for Y velocity (X=1, Y=2, Length="VelY: "+value)

        // Convert fix16 velocities to strings (e.g., 3 decimal places)
        intToStr(scroll_a_x, text_vel_x, 0);
        intToStr(scroll_a_y, text_vel_y, 0);

        // Draw labels and values
        VDP_drawText("PosX:", 1, 1);
        VDP_drawText(text_vel_x, 7, 1); // Draw value starting at column 7
        VDP_drawText("PosY:", 1, 2);
        VDP_drawText(text_vel_y, 7, 2); // Draw value starting at column 7
        // -----------------------

        // Update Graphics
        SPR_setPosition(player_sprite,
                        x,
                        y);
        updateScrolling();

        SPR_update();
        SYS_doVBlankProcess(); // VDP text updates happen during VBlank
    }
    return (0);
}

// --- Update Scrolling Function ---
void updateScrolling() {
    // Calculate parallax scroll for each plane
    // Use >> for division by power of 2 parallax factor

    // scroll_a_x -= (dx >> 2);  
    // scroll_a_y += (dy >> 2); 
    // scroll_b_x -= (dx >> 2);
    // scroll_b_y += (dy >> 2);

    scroll_a_x -= dx;  
    scroll_a_y += dy; 
    // scroll_b_x -= dx;
    // scroll_b_y += dy;  <- make this move slowly

    // Apply scroll values to VDP registers
    VDP_setHorizontalScroll(BG_A, scroll_a_x);
    VDP_setVerticalScroll(BG_A, scroll_a_y);
    VDP_setHorizontalScroll(BG_B, scroll_b_x);
    VDP_setVerticalScroll(BG_B, scroll_b_y);
}

// --- Generate Random Background Map Layer ---
void generateRandomMapLayer(u16* mapData, u16 mapWidth, u16 mapHeight, u16 baseTileIndex, u16 numTilesInSet, u16 pal) {
    u32 mapIdx = 0; // Use u32 for larger map index if needed (though 64x32 fits u16)
    for (u16 ty = 0; ty < mapHeight; ty++) {
        for (u16 tx = 0; tx < mapWidth; tx++) {
            // Choose a random tile index from the provided set (0 to numTilesInSet - 1)
            u16 randomTileOffset = random() % numTilesInSet;
            // Add base VRAM index
            u16 tileVRAMIndex = baseTileIndex + randomTileOffset;

            // Set tile attributes (Use provided palette, low prio, no flip)
            mapData[mapIdx] = TILE_ATTR_FULL(pal, FALSE, FALSE, FALSE, tileVRAMIndex);
            mapIdx++;
        }
    }
}

// --- Initialize Bullet Pool ---
void initBullets() {
    // Set all bullets to inactive initially
    // memset(bullets, 0, sizeof(bullets));
    // // Alternative loop:
    for (s16 i = 0; i < NBULLET; i++) {
        bullets[i].status       = -1;
        bullets[i].new         = 0;
        bullets[i].sprite_ptr   = NULL;
    }
}

void fireBullet(){
    if (bullet_timer > NBULLET_TIMER_MAX){
        bullet_timer = 0;
        if (bullets[bullet_c].status < 0){
            bullets[bullet_c].status = ri;
            bullets[bullet_c].new          = 1;
            bullets[bullet_c].x      = x+4;
            bullets[bullet_c].y      = y+4;
            bullet_c += 1;
            if (bullet_c >= NBULLET){
                bullet_c = 0;
            }
            XGM_startPlayPCM(SFX_LASER,1,SOUND_PCM_CH2);
        }
    }
}

// --- Update bullets fired by player --- ///
void updateBullets()
{
    const s16 screen_w = VDP_getScreenWidth();
    const s16 screen_h = VDP_getScreenHeight();
    
    for (s16 ii = 0; ii < NBULLET; ii++) {
            if ((bullets[ii].status >= 0) & (bullets[ii].new > 0)){
                // set(bullet_x[ii], bullet_y[ii], 0x00); <- this was for plotting (turn off pixel from old-position)
                bullets[ii].sprite_ptr = SPR_addSprite(&bullet_sprite_res,
                                                bullets[ii].x,
                                                bullets[ii].y,
                                                TILE_ATTR(PAL1, TRUE, FALSE, FALSE));
                bullets[ii].new = 0;
                // //Check for collision
                // bullet_fighter(ii);
            }

            if (bullets[ii].status >= 0){
                bvx = -sin_fix[bullets[ii].status];
                bvy = -cos_fix[bullets[ii].status];
                bvxapp = ( (bvx + bullets[ii].bvxrem) >> 6);
                bvyapp = ( (bvy + bullets[ii].bvyrem) >> 6);
                bullets[ii].bvxrem  = bvx + bullets[ii].bvxrem - bvxapp * 64; 
                bullets[ii].bvyrem  = bvy + bullets[ii].bvyrem - bvyapp * 64;
                bullets[ii].x += bvxapp;
                bullets[ii].y += bvyapp;

                if (bullets[ii].x > 0 && bullets[ii].x < screen_w && bullets[ii].y > 0 && bullets[ii].y < screen_h){
                    // set(bullet_x[ii], bullet_y[ii], 0xFF); <- this was for plotting (turn on)
                    SPR_setPosition(bullets[ii].sprite_ptr, bullets[ii].x, bullets[ii].y);
                } else {
                    bullets[ii].status = -1;
                    // Release the associated hardware sprite
                    SPR_releaseSprite(bullets[ii].sprite_ptr);
                    bullets[ii].sprite_ptr = NULL; // Good practice
                }
            }
        }
}

// --- Input Handling Function ---
void handleInput()
{
    u16 value = JOY_readJoypad(JOY_1);

    // --- Rotation ---
    if (iframe >= iframe_old){
        iframe = 0;

        if (value & BUTTON_LEFT) {

            if (ri == ri_max){
                ri = 0;
            } else {
                ri += 1;
            }

        } else if (value & BUTTON_RIGHT) {

            if (ri == 0){
                ri = ri_max;
            } else {
                ri -= 1;
            }
        }
    }
    iframe += 1;

    // --- Thrust ---
    vx = 0; // Default is no thrust applied.
    vy = 0; 
    if (value & BUTTON_UP) {

        vx = -sin_fix[ri];
        vy = -cos_fix[ri];
        tdelay = 0;
        
    }

    // Fire main weapon
    if (value & BUTTON_B) {
        fireBullet();
    }

    bullet_timer += 1;
}

// --- Physics Update Function ---
void updatePhysics()
{

    //Update position
    vxapp = ( (vx + xrem + thrust_x ) >> 9); //Apply velocity, remainder and momentum 
    vyapp = ( (vy + yrem + thrust_y ) >> 9); // 9 and 512 must balance (e.g., 2^9 = 512)
    xrem = vx + xrem + thrust_x - vxapp * 512; //Update remainder
    yrem = vy + yrem + thrust_y - vyapp * 512;
    xtry = x + vxapp; //Update ship position
    ytry = y + vyapp;

    //Update thrust if joystick is held.
    thx = thrust_x + (vx >> 4);
    if (thx < 1024 && thx > -1024){
        thrust_x = thx;
    }
    thy = thrust_y + (vy >> 4);
    if (thy < 1024 && thy > -1024){
        thrust_y = thy;
    }

    //Update momentum by applying friction 
    if (tdelay < tdelay_max && tcount > 50){
        tdelay += 1;
        tcount = 0;
        if (vx == 0){
            thrust_x = thrust_x >> 1;
        }
        if (vy == 0){
            thrust_y = thrust_y >> 1;
        }
    }
    if (tdelay >= tdelay_max){
        thrust_x = 0;
        thrust_y = 0;
    }
    tcount += 1;


    // Keep spacecraft in bounds
    if (xtry > BX1 && xtry < BX2){
        x = xtry;
        dx = 0;
    } else {
        dx = (xtry - x);
    }
        
    if (ytry > BY1 && ytry < BY2){
        y = ytry;
        dy = 0;
    } else {
        dy = (ytry - y);
    }

    
}