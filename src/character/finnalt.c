/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "finnalt.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//finnalt character structure
enum
{  
  finnalt_main_Idle0,
  finnalt_main_Idle1,
  finnalt_main_Idle2,
  finnalt_main_Idle3,
  finnalt_main_Idlealt0,
  finnalt_main_Idlealt1,
  finnalt_main_Left0,
  finnalt_main_Left1,
  finnalt_main_Left2,
  finnalt_main_Leftalt0,
  finnalt_main_Leftalt1,
  finnalt_main_Down0,
  finnalt_main_Down1,
  finnalt_main_Down2,
  finnalt_main_Downalt0,
  finnalt_main_Downalt1,
  finnalt_main_Downalt2,
  finnalt_main_Up0,
  finnalt_main_Up1,
  finnalt_main_Up2,
  finnalt_main_Upalt0,
  finnalt_main_Upalt1,
  finnalt_main_Right0,
  finnalt_main_Right1,
  finnalt_main_Right2,
  finnalt_main_Rightalt0,
  finnalt_main_Rightalt1,
  finnalt_main_Rightalt2,
    
    finnalt_Arc_Max,
};

typedef struct
{
    //Character base structure
    Character character;
    
    //Render data and state
    IO_Data arc_main;
    IO_Data arc_ptr[finnalt_Arc_Max];
    
    Gfx_Tex tex;
    u8 frame, tex_id;
} Char_finnalt;

//finnalt character definitions
static const CharFrame char_finnalt_frame[] = {    
  {finnalt_main_Idle0, {  0,  0,127,183}, {159,156}}, //0 idle 1
  {finnalt_main_Idle0, {127,  0,127,186}, {160,159}}, //1 idle 2
  {finnalt_main_Idle1, {  0,  0,126,187}, {158,160}}, //2 idle 3
  {finnalt_main_Idle1, {126,  0,124,186}, {157,159}}, //3 idle 4
  {finnalt_main_Idle2, {  0,  0,124,188}, {157,159}}, //4 idle 5
  {finnalt_main_Idle2, {124,  0,125,186}, {157,159}}, //5 idle 6
  {finnalt_main_Idle3, {  0,  0,125,186}, {157,159}}, //6 idle 7
  {finnalt_main_Idle3, {125,  0,125,186}, {157,159}}, //7 idle 8

  {finnalt_main_Idlealt0, {  0,  0,128,186}, {160,159}}, //8 idleb 1
  {finnalt_main_Idlealt0, {128,  0,124,186}, {157,160}}, //9 idleb 2
  {finnalt_main_Idlealt1, {  0,  0,125,188}, {157,160}}, //10 idleb 3
  {finnalt_main_Idlealt1, {125,  0,125,186}, {157,160}}, //11 idleb 4

  {finnalt_main_Left0, {  0,  0,127,173}, {184,146}}, //12 left 1
  {finnalt_main_Left1, {  0,  0,130,172}, {182,145}}, //13 left 2
  {finnalt_main_Left2, {  0,  0,130,172}, {181,145}}, //14 left 3

  {finnalt_main_Leftalt0, {  0,  0,162,173}, {186,147}}, //15 leftb 1
  {finnalt_main_Leftalt1, {  0,  0,131,173}, {185,147}}, //16 leftb 2

  {finnalt_main_Down0, {  0,  0,207,159}, {158,124}}, //17 down 1
  {finnalt_main_Down1, {  0,  0,150,181}, {167,155}}, //18 down 2
  {finnalt_main_Down2, {  0,  0,142,181}, {159,155}}, //19 down 3

  {finnalt_main_Downalt0, {  0,  0,207,190}, {158,154}}, //20 downb 1
  {finnalt_main_Downalt1, {  0,  0,150,180}, {167,154}}, //21 downb 2
  {finnalt_main_Downalt2, {  0,  0,143,181}, {159,155}}, //22 downb 3

  {finnalt_main_Up0, {  0,  0,156,183}, {181,156}}, //23 up 1
  {finnalt_main_Up1, {  0,  0,152,179}, {177,152}}, //24 up 2
  {finnalt_main_Up2, {  0,  0,151,178}, {176,152}}, //25 up 3

  {finnalt_main_Upalt0, {  0,  0,157,190}, {180,155}}, //26 upb 1
  {finnalt_main_Upalt1, {  0,  0,159,179}, {184,151}}, //27 upb 2

  {finnalt_main_Right0, {  0,  0,155,199}, {158,138}}, //28 right 1
  {finnalt_main_Right1, {  0,  0,172,165}, {166,139}}, //29 right 2
  {finnalt_main_Right2, {  0,  0,162,166}, {166,139}}, //30 right 3

  {finnalt_main_Rightalt0, {  0,  0,172,199}, {177,139}}, //31 rightb 1
  {finnalt_main_Rightalt1, {  0,  0,205,165}, {166,139}}, //32 rightb 2
  {finnalt_main_Rightalt2, {  0,  0,162,165}, {166,139}}, //33 rightb 3

};

static const Animation char_finnalt_anim[CharAnim_Max] = {
    {2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, ASCR_CHGANI, CharAnim_Idle}}, //CharAnim_Idle
    {2, (const u8[]){12, 13, 14, ASCR_BACK, 0}},         //CharAnim_Left
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
    {2, (const u8[]){17, 18, 19, ASCR_BACK, 0}},         //CharAnim_Down
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
    {2, (const u8[]){23, 24, 25, ASCR_BACK, 0}},         //CharAnim_Up
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
    {2, (const u8[]){28, 29, 30, ASCR_BACK, 0}},         //CharAnim_Right
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//finnalt character functions
void Char_finnalt_SetFrame(void *user, u8 frame)
{
    Char_finnalt *this = (Char_finnalt*)user;
    
    //Check if this is a new frame
    if (frame != this->frame)
    {
        //Check if new art shall be loaded
        const CharFrame *cframe = &char_finnalt_frame[this->frame = frame];
        if (cframe->tex != this->tex_id)
            Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
    }
}

void Char_finnalt_Tick(Character *character)
{
    Char_finnalt *this = (Char_finnalt*)character;
    
    //Perform idle dance
    if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
        Character_PerformIdle(character);
    
    //Animate and draw
    Animatable_Animate(&character->animatable, (void*)this, Char_finnalt_SetFrame);
    Character_Draw(character, &this->tex, &char_finnalt_frame[this->frame]);
}

void Char_finnalt_SetAnim(Character *character, u8 anim)
{
    //Set animation
    Animatable_SetAnim(&character->animatable, anim);
    Character_CheckStartSing(character);
}

void Char_finnalt_Free(Character *character)
{
    Char_finnalt *this = (Char_finnalt*)character;
    
    //Free art
    Mem_Free(this->arc_main);
}

Character *Char_finnalt_New(fixed_t x, fixed_t y)
{
    //Allocate finnalt object
    Char_finnalt *this = Mem_Alloc(sizeof(Char_finnalt));
    if (this == NULL)
    {
        sprintf(error_msg, "[Char_finnalt_New] Failed to allocate finnalt object");
        ErrorLock();
        return NULL;
    }
    
    //Initialize character
    this->character.tick = Char_finnalt_Tick;
    this->character.set_anim = Char_finnalt_SetAnim;
    this->character.free = Char_finnalt_Free;
    
    Animatable_Init(&this->character.animatable, char_finnalt_anim);
    Character_Init((Character*)this, x, y);
    
    //Set character information
    this->character.spec = 0;
    
    this->character.health_i = 1;

    //health bar color
    this->character.health_bar = 0xFFAD63D6;
    
    this->character.focus_x = FIXED_DEC(65,1);
    this->character.focus_y = FIXED_DEC(-115,1);
    this->character.focus_zoom = FIXED_DEC(1,1);
    
    //Load art
    this->arc_main = IO_Read("\\CHAR\\FINNALT.ARC;1");
    
    const char **pathp = (const char *[]){
      "idle0.tim",
      "idle1.tim",
      "idle2.tim",
      "idle3.tim",
      "idleb0.tim",
      "idleb1.tim",
      "left0.tim",
      "left1.tim",
      "left2.tim",
      "leftb0.tim",
      "leftb1.tim",
      "down0.tim",
      "down1.tim",
      "down2.tim",
      "downb0.tim",
      "downb1.tim",
      "downb2.tim",
      "up0.tim",
      "up1.tim",
      "up2.tim",
      "upb0.tim",
      "upb1.tim",
      "right0.tim",
      "right1.tim",
      "right2.tim",
      "rightb0.tim",
      "rightb1.tim",
      "rightb2.tim",
        NULL
    };
    IO_Data *arc_ptr = this->arc_ptr;
    for (; *pathp != NULL; pathp++)
        *arc_ptr++ = Archive_Find(this->arc_main, *pathp);
    
    //Initialize render state
    this->tex_id = this->frame = 0xFF;
    this->character.scale = FIXED_DEC(12,10);
    return (Character*)this;
}