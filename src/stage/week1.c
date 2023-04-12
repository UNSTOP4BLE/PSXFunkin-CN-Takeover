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
    Gfx_Tex tex_back0; //Stage and back
    Gfx_Tex tex_back1; //Curtains

    IO_Data arc_jake, arc_jake_ptr[4];
    Gfx_Tex tex_jake;
    u8 jake_frame, jake_tex_id;   
    Animatable jake_animatable;
} Back_Week1;

static const CharFrame jake_frame[] = {
    {0, {  0,   0,  242, 137}, {151,  86}}, //0
    {1, {  0,   0,  244, 136}, {150,  87}}, //1 
    {2, {  0,   0,  242, 133}, {152,  87}}, //2 
    {3, {  0,   0,  248, 135}, {151,  88}}, //3
};

static const Animation jake_anim[] = {
    {1, (const u8[]){0, 1, 2, 3, ASCR_CHGANI, 0}},
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

//Week 1 background functions
void Back_Week1_DrawBG(StageBack *back)
{
    Back_Week1 *this = (Back_Week1*)back;
    
    fixed_t fx, fy;
    
    Gfx_SetClear(68, 130, 176);

    //Draw bg
    fx = stage.camera.x;
    fy = stage.camera.y;
    
    Animatable_Animate(&this->jake_animatable, (void*)this, Week1_jake_SetFrame);
    Week1_jake_Draw(this, FIXED_DEC(25,1) - fx, FIXED_DEC(-68,1) - fy, FIXED_DEC(12,10));

    RECT bg_src = {0, 0, 256, 256};
    RECT_FIXED bg_dst = {
        FIXED_DEC(-450,1) - fx,
        FIXED_DEC(-240,1) - fy,
        FIXED_DEC(751,1),
        FIXED_DEC(399,1)
    };
    Debug_StageMoveDebug(&bg_dst, 4, fx, fy);
    Stage_DrawTex(&this->tex_back0, &bg_src, &bg_dst, stage.camera.bzoom);
}

void Back_Week1_Free(StageBack *back)
{
    Back_Week1 *this = (Back_Week1*)back;
    
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
    Gfx_LoadTex(&this->tex_back1, Archive_Find(arc_back, "back1.tim"), 0);
    Mem_Free(arc_back);

    //Load jake textures
    this->arc_jake = IO_Read("\\WEEK1\\JAKE.ARC;1");
    this->arc_jake_ptr[0] = Archive_Find(this->arc_jake, "jake0.tim");
    this->arc_jake_ptr[1] = Archive_Find(this->arc_jake, "jake1.tim");
    this->arc_jake_ptr[2] = Archive_Find(this->arc_jake, "jake2.tim");
    this->arc_jake_ptr[3] = Archive_Find(this->arc_jake, "jake3.tim");
    Gfx_SetClear(68, 130, 176);
    
    //Initialize jake state
    Animatable_Init(&this->jake_animatable, jake_anim);
    Animatable_SetAnim(&this->jake_animatable, 0);
    this->jake_frame = this->jake_tex_id = 0xFF; //Force art load

    return (StageBack*)this;
}
