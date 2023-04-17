/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "finn.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../main.h"

//finn character structure
enum
{

  finn_main_Idle0,
  finn_main_Idle1,
  finn_main_Idle2,
  finn_main_Idle3,
  finn_main_Idle4,
  finn_main_Idle5,
  finn_main_Idle6,
  finn_main_Idle7,
  finn_main_Idle8,
  finn_main_Idle9,
  finn_main_Idle10,
  finn_main_Idle11,
  finn_main_Idle12,
  finn_main_Idle13,
  finn_main_Idle14,
  finn_main_Idle15,
  finn_main_Left0,
  finn_main_Left1,
  finn_main_Left2,
  finn_main_Left3,
  finn_main_Left4,
  finn_main_Left5,
  finn_main_Left6,
  finn_main_Left7,
  finn_main_Down0,
  finn_main_Down1,
  finn_main_Down2,
  finn_main_Up0,
  finn_main_Up1,
  finn_main_Up2,
  finn_main_Up3,
  finn_main_Up4,
  finn_main_Up5,
  finn_main_Up6,
  finn_main_Right0,
  finn_main_Right1,
  finn_main_Right2,
  finn_main_Right3,

    finn_Arc_Max,
};

typedef struct
{
    //Character base structure
    Character character;
    
    //Render data and state
    IO_Data arc_main;
    IO_Data arc_ptr[finn_Arc_Max];
    
    Gfx_Tex tex;
    u8 frame, tex_id;
} Char_finn;

//finn character definitions
static const CharFrame char_finn_frame[] = {  
  {finn_main_Idle0, {  0,  0,150,141}, {156,150}}, //0 idle 1
  {finn_main_Idle1, {  0,  0,150,137}, {160,145}}, //1 idle 2
  {finn_main_Idle2, {  0,  0,119,131}, {160,140}}, //2 idle 3
  {finn_main_Idle3, {  0,  0,166,133}, {154,141}}, //3 idle 4
  {finn_main_Idle4, {  0,  0,163,141}, {153,149}}, //4 idle 5
  {finn_main_Idle5, {  0,  0,161,151}, {154,160}}, //5 idle 6
  {finn_main_Idle6, {  0,  0,159,149}, {156,157}}, //6 idle 7
  {finn_main_Idle7, {  0,  0,158,150}, {159,159}}, //7 idle 8
  {finn_main_Idle8, {  0,  0,163,149}, {155,158}}, //8 idle 9
  {finn_main_Idle9, {  0,  0,163,149}, {156,158}}, //9 idle 10
  {finn_main_Idle10, {  0,  0,163,149}, {156,158}}, //10 idle 11
  {finn_main_Idle11, {  0,  0,162,149}, {156,158}}, //11 idle 12
  {finn_main_Idle12, {  0,  0,161,149}, {156,158}}, //12 idle 13
  {finn_main_Idle13, {  0,  0,162,150}, {156,159}}, //13 idle 14
  {finn_main_Idle14, {  0,  0,162,150}, {155,159}}, //14 idle 15
  {finn_main_Idle15, {  0,  0,163,150}, {156,159}}, //15 idle 16

  {finn_main_Left0, {  0,  0,182,129}, {264,138}}, //16 left 1
  {finn_main_Left1, {  0,  0,179,128}, {262,138}}, //17 left 2
  {finn_main_Left2, {  0,  0,181,135}, {262,141}}, //18 left 3
  {finn_main_Left3, {  0,  0,183,115}, {264,125}}, //19 left 4
  {finn_main_Left3, {  0,115,183,115}, {264,125}}, //20 left 5
  {finn_main_Left4, {  0,  0,184,115}, {264,125}}, //21 left 6
  {finn_main_Left4, {  0,115,185,115}, {264,125}}, //22 left 7
  {finn_main_Left5, {  0,  0,182,115}, {264,125}}, //23 left 8
  {finn_main_Left5, {  0,115,181,115}, {263,125}}, //24 left 9
  {finn_main_Left6, {  0,  0,181,115}, {263,125}}, //25 left 10
  {finn_main_Left6, {  0,115,182,115}, {263,125}}, //26 left 11
  {finn_main_Left7, {  0,  0,183,115}, {264,125}}, //27 left 12
  {finn_main_Left7, {  0,115,183,115}, {264,125}}, //28 left 13

  {finn_main_Down0, {  0,  0,128,125}, {158,134}}, //29 down 1
  {finn_main_Down0, {  0,125,129,126}, {158,134}}, //30 down 2
  {finn_main_Down1, {  0,  0,129,122}, {158,130}}, //31 down 3
  {finn_main_Down1, {  0,122,128,127}, {158,135}}, //32 down 4
  {finn_main_Down2, {  0,  0,128,125}, {158,134}}, //33 down 5
  {finn_main_Down2, {128,  0,127,125}, {158,134}}, //34 down 6
  {finn_main_Down2, {  0,125,128,125}, {158,134}}, //35 down 7

  {finn_main_Up0, {  0,  0,231,145}, {173,154}}, //36 up 1
  {finn_main_Up1, {  0,  0,232,145}, {172,154}}, //37 up 2
  {finn_main_Up2, {  0,  0,239,171}, {175,179}}, //38 up 3
  {finn_main_Up3, {  0,  0,241,166}, {175,175}}, //39 up 4
  {finn_main_Up4, {  0,  0,241,166}, {175,175}}, //40 up 5
  {finn_main_Up5, {  0,  0,241,167}, {175,175}}, //41 up 6
  {finn_main_Up6, {  0,  0,240,167}, {175,175}}, //42 up 7

  {finn_main_Right0, {  0,  0,178,123}, {164,133}}, //43 right 1
  {finn_main_Right0, {  0,123,179,125}, {164,135}}, //44 right 2
  {finn_main_Right1, {  0,  0,173,117}, {161,126}}, //45 right 3
  {finn_main_Right1, {  0,117,173,118}, {162,128}}, //46 right 4
  {finn_main_Right2, {  0,  0,172,121}, {162,131}}, //47 right 5
  {finn_main_Right2, {  0,121,172,118}, {162,128}}, //48 right 6
  {finn_main_Right3, {  0,  0,172,118}, {162,128}}, //49 right 7
};

static const Animation char_finn_anim[CharAnim_Max] = {
    {2, (const u8[]){ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, ASCR_BACK, 0}}, //CharAnim_Idle
    {2, (const u8[]){16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, ASCR_BACK, 0}},         //CharAnim_Left
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_LeftAlt
    {2, (const u8[]){29, 30, 31, 32, 33, 34, 35, 36, ASCR_BACK, 0}},         //CharAnim_Down
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_DownAlt
    {2, (const u8[]){37, 38, 39, 40, 41, 42, 43, 44, 45, ASCR_BACK, 0}},         //CharAnim_Up
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_UpAlt
    {2, (const u8[]){46, 47, 48, 49, 50, 51, 52, 53, ASCR_BACK, 0}},         //CharAnim_Right
    {0, (const u8[]){ASCR_CHGANI, CharAnim_Idle}},   //CharAnim_RightAlt
};

//finn character functions
void Char_finn_SetFrame(void *user, u8 frame)
{
    Char_finn *this = (Char_finn*)user;
    
    //Check if this is a new frame
    if (frame != this->frame)
    {
        //Check if new art shall be loaded
        const CharFrame *cframe = &char_finn_frame[this->frame = frame];
        if (cframe->tex != this->tex_id)
            Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
    }
}

void Char_finn_Tick(Character *character)
{
    Char_finn *this = (Char_finn*)character;
    
    //Perform idle dance
    if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0)
        Character_PerformIdle(character);
    
    //Animate and draw
    Animatable_Animate(&character->animatable, (void*)this, Char_finn_SetFrame);
    Character_Draw(character, &this->tex, &char_finn_frame[this->frame]);
}

void Char_finn_SetAnim(Character *character, u8 anim)
{
    //Set animation
    Animatable_SetAnim(&character->animatable, anim);
    Character_CheckStartSing(character);
}

void Char_finn_Free(Character *character)
{
    Char_finn *this = (Char_finn*)character;
    
    //Free art
    Mem_Free(this->arc_main);
}

Character *Char_finn_New(fixed_t x, fixed_t y)
{
    //Allocate finn object
    Char_finn *this = Mem_Alloc(sizeof(Char_finn));
    if (this == NULL)
    {
        sprintf(error_msg, "[Char_finn_New] Failed to allocate finn object");
        ErrorLock();
        return NULL;
    }
    
    //Initialize character
    this->character.tick = Char_finn_Tick;
    this->character.set_anim = Char_finn_SetAnim;
    this->character.free = Char_finn_Free;
    
    Animatable_Init(&this->character.animatable, char_finn_anim);
    Character_Init((Character*)this, x, y);
    
    //Set character information
    this->character.spec = 0;
    
    this->character.health_i = 1;

    //health bar color
    this->character.health_bar = 0xFFADBFCA;
    
    this->character.focus_x = FIXED_DEC(-193 - -120,1);
    this->character.focus_y = FIXED_DEC(-6 - 123,1);
    this->character.focus_zoom = FIXED_DEC(1,1);
    
    //Load art
    this->arc_main = IO_Read("\\CHAR\\FINN.ARC;1");
    
    const char **pathp = (const char *[]){
      "idle0.tim",
      "idle1.tim",
      "idle2.tim",
      "idle3.tim",
      "idle4.tim",
      "idle5.tim",
      "idle6.tim",
      "idle7.tim",
      "idle8.tim",
      "idle9.tim",
      "idle10.tim",
      "idle11.tim",
      "idle12.tim",
      "idle13.tim",
      "idle14.tim",
      "idle15.tim",
      "left0.tim",
      "left1.tim",
      "left2.tim",
      "left3.tim",
      "left4.tim",
      "left5.tim",
      "left6.tim",
      "left7.tim",
      "down0.tim",
      "down1.tim",
      "down2.tim",
      "up0.tim",
      "up1.tim",
      "up2.tim",
      "up3.tim",
      "up4.tim",
      "up5.tim",
      "up6.tim",
      "right0.tim",
      "right1.tim",
      "right2.tim",
      "right3.tim",
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
