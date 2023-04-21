/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "week1.h"

#include "../archive.h"
#include "../mem.h"
#include "../stage.h"

//Week 1 background structure
typedef struct
{
    //Stage background base structure
    StageBack back;
    
    //Textures
    Gfx_Tex tex_back0; //back

    IO_Data arc_jake, arc_jake_ptr[4];
    Gfx_Tex tex_jake;
    u8 jake_frame, jake_tex_id;   
    Animatable jake_animatable;

    IO_Data arc_finn, arc_finn_ptr[1];
    Gfx_Tex tex_finn;
    u8 finn_frame, finn_tex_id;   
    Animatable finn_animatable;

    IO_Data arc_bubble, arc_bubble_ptr[2];
    Gfx_Tex tex_bubble;
    u8 bubble_frame, bubble_tex_id;   
    Animatable bubble_animatable;

    IO_Data arc_guys, arc_guys_ptr[4];
    Gfx_Tex tex_guys;
    u8 guys_frame, guys_tex_id;   
    Animatable guys_animatable;
} Back_Week1;

void draw_rect(const RECT_FIXED *dst, u8 cr, u8 cg, u8 cb, fixed_t zoom)
{
    fixed_t xz = dst->x;
    fixed_t yz = dst->y;
    fixed_t wz = dst->w;
    fixed_t hz = dst->h;

    xz += stage.cam_shake.shake;
    yz += stage.cam_shake.shake;
    
    fixed_t l = (screen.SCREEN_WIDTH2  << FIXED_SHIFT) + FIXED_MUL(xz, zoom);// + FIXED_DEC(1,2);
    fixed_t t = (screen.SCREEN_HEIGHT2 << FIXED_SHIFT) + FIXED_MUL(yz, zoom);// + FIXED_DEC(1,2);
    fixed_t r = l + FIXED_MUL(wz, zoom);
    fixed_t b = t + FIXED_MUL(hz, zoom);
    
    l >>= FIXED_SHIFT;
    t >>= FIXED_SHIFT;
    r >>= FIXED_SHIFT;
    b >>= FIXED_SHIFT;
    
    RECT sdst = {
        l,
        t,
        r - l,
        b - t,
    };
    Gfx_DrawRect(&sdst, cr, cg, cb);
}

static const CharFrame jake_frame[] = {
    {0, {  0,   0,  242, 137}, {151,  86}}, //0
    {1, {  0,   0,  244, 136}, {150,  87}}, //1 
    {2, {  0,   0,  242, 133}, {152,  87}}, //2 
    {3, {  0,   0,  248, 135}, {151,  88}}, //3
};

static const Animation jake_anim[] = {
    {1, (const u8[]){0, 1, 2, 3, ASCR_CHGANI, 0}},
};

static const CharFrame finn_frame[] = {
    {0, {  0,   0,  125, 102}, {119,   98}}, //0
    {0, {125,   0,  127,  97}, {122,   93}}, //1 
    {0, {  0, 102,  126,  92}, {121,   90}}, //2 
    {0, {126,  97,  125,  91}, {121,   90}}, //3
};

static const Animation finn_anim[] = {
    {2, (const u8[]){0, 1, 2, 3, ASCR_BACK, 0}},
};

static const CharFrame bubble_frame[] = {
    {0, {  0,   0,   85,  98}, { 15,   73}}, //0
    {0, { 85,   0,   86,  98}, { 17,   73}}, //1 
    {0, {171,   0,   85, 100}, { 15,   74}}, //2 
    {0, {  0,  98,   86, 100}, { 15,   74}}, //3
    {0, { 86,  98,   85, 100}, { 14,   74}}, //4
    {0, {171, 100,   85,  98}, { 15,   75}}, //5
    {1, {  0,   0,   85, 100}, { 14,   75}}, //6
    {1, { 85,   0,   86, 100}, { 15,   75}}, //7
};

static const Animation bubble_anim[] = {
    {2, (const u8[]){0, 1, 2, 3, 4, 5, 6, 7, ASCR_BACK, 0}},
};

static const CharFrame guys_frame[] = {
    {0, {  0,   0,  250, 130}, {212,  130}}, //0
    {1, {  0,   0,  250, 130}, {212,  130}}, //1 
    {2, {  0,   0,  250, 131}, {211,  131}}, //2 
    {3, {  0,   0,  248, 131}, {210,  131}}, //3
};

static const Animation guys_anim[] = {
    {2, (const u8[]){0, 1, 2, 3, ASCR_BACK, 0}},
};

//jake functions
void Week1_jake_SetFrame(void *user, u8 frame)
{
    Back_Week1 *this = (Back_Week1*)user;
    
    //Check if this is a new frame
    if (frame != this->jake_frame)
    {
        //Check if new art shall be loaded
        const CharFrame *cframe = &jake_frame[this->jake_frame = frame];
        if (cframe->tex != this->jake_tex_id)
            Gfx_LoadTex(&this->tex_jake, this->arc_jake_ptr[this->jake_tex_id = cframe->tex], 0);
    }
}

void Week1_jake_Draw(Back_Week1 *this, fixed_t x, fixed_t y, fixed_t scale)
{
    //Draw character
    const CharFrame *cframe = &jake_frame[this->jake_frame];
    
    fixed_t ox = x - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[0],1), scale));
    fixed_t oy = y - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[1],1), scale));

    RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
    RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
    Debug_StageMoveDebug(&dst, 5, stage.camera.x, stage.camera.y);
    Stage_DrawTex(&this->tex_jake, &src, &dst, stage.camera.bzoom);
}

//finn functions
void Week1_finn_SetFrame(void *user, u8 frame)
{
    Back_Week1 *this = (Back_Week1*)user;
    
    //Check if this is a new frame
    if (frame != this->finn_frame)
    {
        //Check if new art shall be loaded
        const CharFrame *cframe = &finn_frame[this->finn_frame = frame];
        if (cframe->tex != this->finn_tex_id)
            Gfx_LoadTex(&this->tex_finn, this->arc_finn_ptr[this->finn_tex_id = cframe->tex], 0);
    }
}

void Week1_finn_Draw(Back_Week1 *this, fixed_t x, fixed_t y, fixed_t scale)
{
    //Draw character
    const CharFrame *cframe = &finn_frame[this->finn_frame];
    
    fixed_t ox = x - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[0],1), scale));
    fixed_t oy = y - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[1],1), scale));

    RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
    RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
    Debug_StageMoveDebug(&dst, 6, stage.camera.x, stage.camera.y);
    Stage_DrawTex(&this->tex_finn, &src, &dst, stage.camera.bzoom);
}

//bubble functions
void Week1_bubble_SetFrame(void *user, u8 frame)
{
    Back_Week1 *this = (Back_Week1*)user;
    
    //Check if this is a new frame
    if (frame != this->bubble_frame)
    {
        //Check if new art shall be loaded
        const CharFrame *cframe = &bubble_frame[this->bubble_frame = frame];
        if (cframe->tex != this->bubble_tex_id)
            Gfx_LoadTex(&this->tex_bubble, this->arc_bubble_ptr[this->bubble_tex_id = cframe->tex], 0);
    }
}

void Week1_bubble_Draw(Back_Week1 *this, fixed_t x, fixed_t y, fixed_t scale)
{
    //Draw character
    const CharFrame *cframe = &bubble_frame[this->bubble_frame];
    
    fixed_t ox = x - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[0],1), scale));
    fixed_t oy = y - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[1],1), scale));

    RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
    RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
    Debug_StageMoveDebug(&dst, 7, stage.camera.x, stage.camera.y);
    Stage_DrawTex(&this->tex_bubble, &src, &dst, stage.camera.bzoom);
}

//guys functions
void Week1_guys_SetFrame(void *user, u8 frame)
{
    Back_Week1 *this = (Back_Week1*)user;
    
    //Check if this is a new frame
    if (frame != this->guys_frame)
    {
        //Check if new art shall be loaded
        const CharFrame *cframe = &guys_frame[this->guys_frame = frame];
        if (cframe->tex != this->guys_tex_id)
            Gfx_LoadTex(&this->tex_guys, this->arc_guys_ptr[this->guys_tex_id = cframe->tex], 0);
    }
}

void Week1_guys_Draw(Back_Week1 *this, fixed_t x, fixed_t y, fixed_t scale)
{
    //Draw character
    const CharFrame *cframe = &guys_frame[this->guys_frame];
    
    fixed_t ox = x - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[0],1), scale));
    fixed_t oy = y - ((fixed_t)FIXED_MUL(FIXED_DEC(cframe->off[1],1), scale));

    RECT src = {cframe->src[0], cframe->src[1], cframe->src[2], cframe->src[3]};
    RECT_FIXED dst = {ox, oy, src.w << FIXED_SHIFT, src.h << FIXED_SHIFT};
    Debug_StageMoveDebug(&dst, 8, stage.camera.x, stage.camera.y);
    Stage_DrawTex(&this->tex_guys, &src, &dst, stage.camera.bzoom);
}

//Week 1 background functions
void Back_Week1_DrawBG(StageBack *back)
{
    Back_Week1 *this = (Back_Week1*)back;
    
    fixed_t fx, fy;
    //Draw bg
    fx = stage.camera.x;
    fy = stage.camera.y;

    if (stage.stage_id == StageId_1_4 || stage.stage_id == StageId_1_1)    
    {
        Animatable_Animate(&this->jake_animatable, (void*)this, Week1_jake_SetFrame);
        Week1_jake_Draw(this, FIXED_DEC(25,1) - fx, FIXED_DEC(-68,1) - fy, FIXED_DEC(12,10));
    }
    if (stage.stage_id == StageId_1_3)
    {
        if (stage.flag & STAGE_FLAG_JUST_STEP && !(stage.song_step % 8))
            Animatable_SetAnim(&this->bubble_animatable, 0);
        
        Animatable_Animate(&this->bubble_animatable, (void*)this, Week1_bubble_SetFrame);
        Week1_bubble_Draw(this, FIXED_DEC(-35,1) - fx, FIXED_DEC(-105,1) - fy, FIXED_DEC(1,1));
    }
    if (stage.stage_id == StageId_1_2)
    {
        if (stage.flag & STAGE_FLAG_JUST_STEP && !(stage.song_step % 8))
            Animatable_SetAnim(&this->finn_animatable, 0);
        
        Animatable_Animate(&this->finn_animatable, (void*)this, Week1_finn_SetFrame);
        Week1_finn_Draw(this, FIXED_DEC(50,1) - fx, FIXED_DEC(-80,1) - fy, FIXED_DEC(1,1));
    }
    
    if (stage.flag & STAGE_FLAG_JUST_STEP && !(stage.song_step % 8))
        Animatable_SetAnim(&this->guys_animatable, 0);
        
    Animatable_Animate(&this->guys_animatable, (void*)this, Week1_guys_SetFrame);
    Week1_guys_Draw(this, FIXED_DEC(0,1) - fx, FIXED_DEC(80,1) - fy, FIXED_DEC(1,1));

    RECT bg_src = {0, 0, 255, 255};
    RECT_FIXED bg_dst = {
        FIXED_DEC(-450,1) - fx,
        FIXED_DEC(-240,1) - fy,
        FIXED_DEC(751,1),
        FIXED_DEC(399,1)
    };
    Debug_StageMoveDebug(&bg_dst, 4, fx, fy);
    Stage_DrawTex(&this->tex_back0, &bg_src, &bg_dst, stage.camera.bzoom);
    draw_rect(&bg_dst, 68, 130, 176, stage.camera.bzoom);
}

void Back_Week1_Free(StageBack *back)
{
    Back_Week1 *this = (Back_Week1*)back;
    
    Mem_Free(this->arc_jake);
    Mem_Free(this->arc_finn);
    Mem_Free(this->arc_bubble);
    Mem_Free(this->arc_guys);

    //Free structure
    Mem_Free(this);
}

StageBack *Back_Week1_New(void)
{
    //Allocate background structure
    Back_Week1 *this = (Back_Week1*)Mem_Alloc(sizeof(Back_Week1));
    if (this == NULL)
        return NULL;
    
    //Set background functions
    this->back.draw_fg = NULL;
    this->back.draw_md = NULL;
    this->back.draw_bg = Back_Week1_DrawBG;
    this->back.free = Back_Week1_Free;
    
    //Load background textures
    IO_Data arc_back = IO_Read("\\WEEK1\\BACK.ARC;1");
    Gfx_LoadTex(&this->tex_back0, Archive_Find(arc_back, "back0.tim"), 0);
    Mem_Free(arc_back);

    //Load bg textures
    if (stage.stage_id == StageId_1_4 || stage.stage_id == StageId_1_1)    
    {
        this->arc_jake = IO_Read("\\WEEK1\\JAKE.ARC;1");
        this->arc_jake_ptr[0] = Archive_Find(this->arc_jake, "jake0.tim");
        this->arc_jake_ptr[1] = Archive_Find(this->arc_jake, "jake1.tim");
        this->arc_jake_ptr[2] = Archive_Find(this->arc_jake, "jake2.tim");
        this->arc_jake_ptr[3] = Archive_Find(this->arc_jake, "jake3.tim");
        //Initialize jake state
        Animatable_Init(&this->jake_animatable, jake_anim);
        Animatable_SetAnim(&this->jake_animatable, 0);
        this->jake_frame = this->jake_tex_id = 0xFF; //Force art load
    }
    if (stage.stage_id == StageId_1_2)    
    {
        this->arc_finn = IO_Read("\\WEEK1\\FINN.ARC;1");
        this->arc_finn_ptr[0] = Archive_Find(this->arc_finn, "finn.tim");
        //Initialize finn state
        Animatable_Init(&this->finn_animatable, finn_anim);
        Animatable_SetAnim(&this->finn_animatable, 0);
        this->finn_frame = this->finn_tex_id = 0xFF; //Force art load
    }
    if (stage.stage_id == StageId_1_3)    
    {
        this->arc_bubble = IO_Read("\\WEEK1\\BUBBLE.ARC;1");
        this->arc_bubble_ptr[0] = Archive_Find(this->arc_bubble, "bubble0.tim");
        this->arc_bubble_ptr[1] = Archive_Find(this->arc_bubble, "bubble1.tim");
        //Initialize bubble state
        Animatable_Init(&this->bubble_animatable, bubble_anim);
        Animatable_SetAnim(&this->bubble_animatable, 0);
        this->bubble_frame = this->bubble_tex_id = 0xFF; //Force art load
    }
        
    this->arc_guys = IO_Read("\\WEEK1\\GUYS.ARC;1");
    this->arc_guys_ptr[0] = Archive_Find(this->arc_guys, "guys0.tim");
    this->arc_guys_ptr[1] = Archive_Find(this->arc_guys, "guys1.tim");
    this->arc_guys_ptr[2] = Archive_Find(this->arc_guys, "guys2.tim");
    this->arc_guys_ptr[3] = Archive_Find(this->arc_guys, "guys3.tim");
    //Initialize guys state
    Animatable_Init(&this->guys_animatable, guys_anim);
    Animatable_SetAnim(&this->guys_animatable, 0);
    this->guys_frame = this->guys_tex_id = 0xFF; //Force art load
    Gfx_SetClear(0, 0, 0);
    return (StageBack*)this;
}
