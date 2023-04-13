/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "jake.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//jake character structure
enum
{  
  jake_main_Idle0,
  jake_main_Idle1,
  jake_main_Idle2,
  jake_main_Idle3,
  jake_main_Idle4,
  jake_main_Left0,
  jake_main_Leftalt0,
  jake_main_Down0,
  jake_main_Downalt0,
  jake_main_Up0,
  jake_main_Up1,
  jake_main_Upalt0,
  jake_main_Upalt1,
  jake_main_Right0,
  jake_main_Rightalt0,
  jake_main_Rightalt1,

    jake_Arc_Max,
};

typedef struct
{
    //Character base structure
    Character character;
    
    //Render data and state
    IO_Data arc_main;
    IO_Data arc_ptr[jake_Arc_Max];
    
    Gfx_Tex tex;
    u8 frame, tex_id;
} Char_jake;

//jake character definitions
static const CharFrame char_jake_frame[] = {    
  {jake_main_Idle0, {  0,  0,175,109}, {155,152}}, //0 idle 1
  {jake_main_Idle0, {  0,109,175,110}, {155,153}}, //1 idle 2
  {jake_main_Idle1, {  0,  0,180,113}, {157,156}}, //2 idle 3
  {jake_main_Idle1, {  0,113,182,113}, {160,156}}, //3 idle 4
  {jake_main_Idle2, {  0,  0,180,116}, {160,159}}, //4 idle 5
  {jake_main_Idle2, {  0,116,176,114}, {156,157}}, //5 idle 6
  {jake_main_Idle3, {  0,  0,173,114}, {155,157}}, //6 idle 7
  {jake_main_Idle3, {  0,114,174,113}, {154,156}}, //7 idle 8
  {jake_main_Idle4, {  0,  0,174,113}, {154,156}}, //8 idle 9
  {jake_main_Idle4, {  0,113,173,113}, {154,156}}, //9 idle 10

  {jake_main_Left0, {  0,  0,185,122}, {177,164}}, //10 left 1
  {jake_main_Left0, {  0,122,197,123}, {202,165}}, //11 left 2

  {jake_main_Leftalt0, {  0,  0,216,125}, {186,166}}, //12 leftb 1
  {jake_main_Leftalt0, {  0,125,198,122}, {207,164}}, //13 leftb 2

  {jake_main_Down0, {  0,  0,181,103}, {157,146}}, //14 down 1
  {jake_main_Down0, {  0,103,168,108}, {169,147}}, //15 down 2

  {jake_main_Downalt0, {  0,  0,191,123}, {161,166}}, //16 downb 1
  {jake_main_Downalt0, {  0,123,171,108}, {171,147}}, //17 downb 2

  {jake_main_Up0, {  0,  0,181,186}, {159,226}}, //18 up 1
  {jake_main_Up1, {  0,  0,182,184}, {160,223}}, //19 up 2

  {jake_main_Upalt0, {  0,  0,179,191}, {156,231}}, //20 upb 1
  {jake_main_Upalt1, {  0,  0,182,187}, {156,227}}, //21 upb 2

  {jake_main_Right0, {  0,  0,207,123}, {141,166}}, //22 right 1
  {jake_main_Right0, {  0,123,204,122}, {142,164}}, //23 right 2

  {jake_main_Rightalt0, {  0,  0,223,129}, {148,171}}, //24 rightb 1
  {jake_main_Rightalt1, {  0,  0,208,122}, {141,165}}, //25 rightb 2
};

static const Animation char_jake_anim[CharAnim_Max] = {
    {2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
    {2, (const u8[]){10, 11, ASCR_BACK, 0}},         //CharAnim_Left
    {2, (const u8[]){12, 13, ASCR_BACK, 0}},   //CharAnim_LeftAlt
    {2, (const u8[]){14, 15, ASCR_BACK, 0}},         //CharAnim_Down
    {2, (const u8[]){16, 17, ASCR_BACK, 0}},   //CharAnim_DownAlt
    {2, (const u8[]){18, 19, ASCR_BACK, 0}},         //CharAnim_Up
    {2, (const u8[]){20, 21, ASCR_BACK, 0}},   //CharAnim_UpAlt
    {2, (const u8[]){22, 23, ASCR_BACK, 0}},         //CharAnim_Right
    {2, (const u8[]){24, 25, ASCR_BACK, 0}},   //CharAnim_RightAlt
};

//jake character functions
void Char_jake_SetFrame(void *user, u8 frame)
{
    Char_jake *this = (Char_jake*)user;
    
    //Check if this is a new frame
    if (frame != this->frame)
    {
        //Check if new art shall be loaded
        const CharFrame *cframe = &char_jake_frame[this->frame = frame];
        if (cframe->tex != this->tex_id)
            Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
    }
}

void Char_jake_Tick(Character *character)
{
    Char_jake *this = (Char_jake*)character;
    
    //Perform idle dance
    if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
        Character_PerformIdle(character);
    
    //Animate and draw
    Animatable_Animate(&character->animatable, (void*)this, Char_jake_SetFrame);
    Character_Draw(character, &this->tex, &char_jake_frame[this->frame]);
}

void Char_jake_SetAnim(Character *character, u8 anim)
{
    //Set animation
    Animatable_SetAnim(&character->animatable, anim);
    Character_CheckStartSing(character);
}

void Char_jake_Free(Character *character)
{
    Char_jake *this = (Char_jake*)character;
    
    //Free art
    Mem_Free(this->arc_main);
}

Character *Char_jake_New(fixed_t x, fixed_t y)
{
    //Allocate jake object
    Char_jake *this = Mem_Alloc(sizeof(Char_jake));
    if (this == NULL)
    {
        sprintf(error_msg, "[Char_jake_New] Failed to allocate jake object");
        ErrorLock();
        return NULL;
    }
    
    //Initialize character
    this->character.tick = Char_jake_Tick;
    this->character.set_anim = Char_jake_SetAnim;
    this->character.free = Char_jake_Free;
    
    Animatable_Init(&this->character.animatable, char_jake_anim);
    Character_Init((Character*)this, x, y);
    
    //Set character information
    this->character.spec = 0;
    
    this->character.health_i = 1;

    //health bar color
    this->character.health_bar = 0xFFAD63D6;
    
    this->character.focus_x = FIXED_DEC(-193 - -168,1);
    this->character.focus_y = FIXED_DEC(-6 - 150,1);
    this->character.focus_zoom = FIXED_DEC(1,1);
    
    //Load art
    this->arc_main = IO_Read("\\CHAR\\JAKE.ARC;1");
    
    const char **pathp = (const char *[]){   
      "idle0.tim",
      "idle1.tim",
      "idle2.tim",
      "idle3.tim",
      "idle4.tim",
      "left0.tim",
      "leftb0.tim",
      "down0.tim",
      "downb0.tim",
      "up0.tim",
      "up1.tim",
      "upb0.tim",
      "upb1.tim",
      "right0.tim",
      "rightb0.tim",
      "rightb1.tim",
        NULL
    };
    IO_Data *arc_ptr = this->arc_ptr;
    for (; *pathp != NULL; pathp++)
        *arc_ptr++ = Archive_Find(this->arc_main, *pathp);
    
    //Initialize render state
    this->tex_id = this->frame = 0xFF;
    return (Character*)this;
}
