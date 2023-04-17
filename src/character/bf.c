/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "bf.h"

#include "../mem.h"
#include "../archive.h"
#include "../stage.h"
#include "../random.h"
#include "../main.h"

int secret;

//Boyfriend skull fragments
static SkullFragment char_bf_skull[15] = {
	{ 1 * 8, -87 * 8, -13, -13},
	{ 9 * 8, -88 * 8,   5, -22},
	{18 * 8, -87 * 8,   9, -22},
	{26 * 8, -85 * 8,  13, -13},
	
	{-3 * 8, -82 * 8, -13, -11},
	{ 8 * 8, -85 * 8,  -9, -15},
	{20 * 8, -82 * 8,   9, -15},
	{30 * 8, -79 * 8,  13, -11},
	
	{-1 * 8, -74 * 8, -13, -5},
	{ 8 * 8, -77 * 8,  -9, -9},
	{19 * 8, -75 * 8,   9, -9},
	{26 * 8, -74 * 8,  13, -5},
	
	{ 5 * 8, -73 * 8, -5, -3},
	{14 * 8, -76 * 8,  9, -6},
	{26 * 8, -67 * 8, 15, -3},
};

//Boyfriend player types
enum
{
  bf_main_bf0,
  bf_main_bf1,
  bf_main_bf2,
  bf_main_bf3,
  bf_main_bf4,
  bf_main_bf5,
  bf_main_bf6,
  bf_main_bf7,
  bf_main_bf8,
	BF_ArcMain_Dead0, //BREAK
	
	BF_ArcMain_Max,
};

enum
{
	BF_ArcDead_Dead1, //Mic Drop
	BF_ArcDead_Dead2, //Twitch
	BF_ArcDead_Retry, //Retry prompt
	
	BF_ArcDead_Max,
};

#define BF_Arc_Max BF_ArcMain_Max

typedef struct
{
	//Character base structure
	Character character;
	
	//Render data and state
	IO_Data arc_main, arc_dead;
	CdlFILE file_dead_arc; //dead.arc file position
	IO_Data arc_ptr[BF_Arc_Max];
	
	Gfx_Tex tex, tex_retry;
	u8 frame, tex_id;
	
	u8 retry_bump;
	
	SkullFragment skull[COUNT_OF(char_bf_skull)];
	u8 skull_scale;
} Char_BF;

//Boyfriend player definitions
static const CharFrame char_bf_frame[] = {
  {bf_main_bf0, {  0,  0,100, 98}, {153,154}}, //0 idle 1
  {bf_main_bf0, {100,  0, 99, 98}, {152,154}}, //1 idle 2
  {bf_main_bf0, {  0, 98, 96, 98}, {149,154}}, //2 idle 3
  {bf_main_bf0, { 96, 98, 96, 98}, {148,154}}, //3 idle 4
  {bf_main_bf1, {  0,  0, 94,100}, {146,156}}, //4 idle 5
  {bf_main_bf1, { 94,  0,102,100}, {154,156}}, //5 idle 6
  {bf_main_bf1, {  0,100,107,102}, {159,157}}, //6 idle 7
  {bf_main_bf1, {107,100,105,102}, {158,157}}, //7 idle 8
  {bf_main_bf2, {  0,  0,104,104}, {155,160}}, //8 idle 9
  {bf_main_bf2, {104,  0,104,104}, {155,160}}, //9 idle 10
  {bf_main_bf2, {  0,104,104,104}, {155,160}}, //10 idle 11

  {bf_main_bf2, {104,104, 97,103}, {161,159}}, //11 left 1
  {bf_main_bf3, {  0,  0, 95,103}, {158,159}}, //12 left 2

  {bf_main_bf3, { 95,  0,101,109}, {162,164}}, //13 leftb 1
  {bf_main_bf3, {  0,109,102,108}, {162,163}}, //14 leftb 2

  {bf_main_bf3, {102,109, 97,103}, {161,157}}, //15 leftm 1
  {bf_main_bf4, {  0,  0, 94,120}, {157,164}}, //16 leftm 2

  {bf_main_bf4, { 94,  0, 95, 90}, {155,147}}, //17 down 1
  {bf_main_bf4, {  0,120, 94, 91}, {155,148}}, //18 down 2

  {bf_main_bf4, { 94,120, 96, 85}, {153,141}}, //19 downb 1
  {bf_main_bf5, {  0,  0, 94, 85}, {152,141}}, //20 downb 2

  {bf_main_bf5, { 94,  0, 95, 91}, {157,147}}, //21 downm 1
  {bf_main_bf5, {  0, 91, 95, 99}, {157,156}}, //22 downm 2

  {bf_main_bf5, { 95, 91, 94,112}, {147,166}}, //23 up 1
  {bf_main_bf6, {  0,  0, 95,111}, {149,165}}, //24 up 2

  {bf_main_bf6, { 95,  0,101,120}, {139,177}}, //25 upb 1
  {bf_main_bf6, {  0,120,101,120}, {140,177}}, //26 upb 2

  {bf_main_bf6, {101,120, 94,112}, {146,166}}, //27 upm 1
  {bf_main_bf7, {  0,  0, 91,108}, {149,163}}, //28 upm 2

  {bf_main_bf7, { 91,  0,104,102}, {148,158}}, //29 right 1
  {bf_main_bf7, {  0,108,104,103}, {148,159}}, //30 right 2

  {bf_main_bf7, {104,108,118,103}, {153,159}}, //31 rightb 1
  {bf_main_bf8, {  0,  0,119,102}, {154,159}}, //32 rightb 2

  {bf_main_bf8, {119,  0,104,102}, {148,157}}, //33 rightm 1
  {bf_main_bf8, {  0,102,100,109}, {149,165}}, //34 rightm 2
};

static const Animation char_bf_anim[PlayerAnim_Max] = {
	{2, (const u8[]){ 0,  1,  2,  3,  4,  5,  6, 7, 8, 9, 10, ASCR_BACK, 0}}, //CharAnim_Idle
	{2, (const u8[]){ 11,  12, ASCR_BACK, 0}},             //CharAnim_Left
	{2, (const u8[]){ 13, 14, ASCR_BACK, 0}},       //CharAnim_LeftAlt
	{2, (const u8[]){ 17,  18, ASCR_BACK, 0}},             //CharAnim_Down
	{2, (const u8[]){ 19,  20, ASCR_BACK, 0}},       //CharAnim_DownAlt
	{2, (const u8[]){ 23, 24, ASCR_BACK, 0}},             //CharAnim_Up
	{2, (const u8[]){ 25, 26, ASCR_BACK, 0}},       //CharAnim_UpAlt
	{2, (const u8[]){ 29, 30, ASCR_BACK, 0}},             //CharAnim_Right
	{2, (const u8[]){ 31, 32, ASCR_BACK, 0}},       //CharAnim_RightAlt
	
	{1, (const u8[]){ 5, 20, 20, 21, ASCR_BACK, 1}},     //PlayerAnim_LeftMiss
	{1, (const u8[]){ 7, 22, 22, 23, ASCR_BACK, 1}},     //PlayerAnim_DownMiss
	{1, (const u8[]){ 9, 24, 24, 25, ASCR_BACK, 1}},     //PlayerAnim_UpMiss
	{1, (const u8[]){11, 26, 26, 27, ASCR_BACK, 1}},     //PlayerAnim_RightMiss
	
	{2, (const u8[]){13, 14, 15, ASCR_BACK, 1}},         //PlayerAnim_Peace
	{2, (const u8[]){16, 17, 18, 19, ASCR_REPEAT}},      //PlayerAnim_Sweat
	
	{5, (const u8[]){28, 29, 30, 31, 31, 31, 31, 31, 31, 31, ASCR_CHGANI, PlayerAnim_Dead1}}, //PlayerAnim_Dead0
	{5, (const u8[]){31, ASCR_REPEAT}},                                                       //PlayerAnim_Dead1
	{3, (const u8[]){32, 33, 34, 35, 35, 35, 35, 35, 35, 35, ASCR_CHGANI, PlayerAnim_Dead3}}, //PlayerAnim_Dead2
	{3, (const u8[]){35, ASCR_REPEAT}},                                                       //PlayerAnim_Dead3
	{3, (const u8[]){36, 37, 35, 35, 35, 35, 35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead4
	{3, (const u8[]){38, 39, 35, 35, 35, 35, 35, ASCR_CHGANI, PlayerAnim_Dead3}},             //PlayerAnim_Dead5
	
	{10, (const u8[]){35, 35, 35, ASCR_BACK, 1}}, //PlayerAnim_Dead4
	{ 3, (const u8[]){38, 39, 35, ASCR_REPEAT}},  //PlayerAnim_Dead5
};

//Boyfriend player functions
void Char_BF_SetFrame(void *user, u8 frame)
{
	Char_BF *this = (Char_BF*)user;
	
	//Check if this is a new frame
	if (frame != this->frame)
	{
		//Check if new art shall be loaded
		const CharFrame *cframe = &char_bf_frame[this->frame = frame];
		if (cframe->tex != this->tex_id)
			Gfx_LoadTex(&this->tex, this->arc_ptr[this->tex_id = cframe->tex], 0);
	}
}

void Char_BF_Tick(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Secret icon
	if (secret)
		this->character.health_i = 3;
	else
		this->character.health_i = 0;

	if (pad_state.press & PAD_SELECT && stage.prefs.debug == 0)
		secret ++;
	
	if (secret == 2)
		secret = 0;
	//Handle animation updates
	if ((character->pad_held & (INPUT_LEFT | INPUT_DOWN | INPUT_UP | INPUT_RIGHT)) == 0 ||
	    (character->animatable.anim != CharAnim_Left &&
	     character->animatable.anim != CharAnim_LeftAlt &&
	     character->animatable.anim != CharAnim_Down &&
	     character->animatable.anim != CharAnim_DownAlt &&
	     character->animatable.anim != CharAnim_Up &&
	     character->animatable.anim != CharAnim_UpAlt &&
	     character->animatable.anim != CharAnim_Right &&
	     character->animatable.anim != CharAnim_RightAlt))
		Character_CheckEndSing(character);
	
	if (stage.flag & STAGE_FLAG_JUST_STEP)
	{
		//Perform idle dance
		if (Animatable_Ended(&character->animatable) &&
			(character->animatable.anim != CharAnim_Left &&
		     character->animatable.anim != CharAnim_LeftAlt &&
		     character->animatable.anim != PlayerAnim_LeftMiss &&
		     character->animatable.anim != CharAnim_Down &&
		     character->animatable.anim != CharAnim_DownAlt &&
		     character->animatable.anim != PlayerAnim_DownMiss &&
		     character->animatable.anim != CharAnim_Up &&
		     character->animatable.anim != CharAnim_UpAlt &&
		     character->animatable.anim != PlayerAnim_UpMiss &&
		     character->animatable.anim != CharAnim_Right &&
		     character->animatable.anim != CharAnim_RightAlt &&
		     character->animatable.anim != PlayerAnim_RightMiss) &&
			(stage.song_step & 0x7) == 0)
			character->set_anim(character, CharAnim_Idle);
		
		//Stage specific animations
		if (stage.note_scroll >= 0)
		{
			switch (stage.stage_id)
			{
				case StageId_1_4: //Tutorial peace
					if (stage.song_step > 64 && stage.song_step < 192 && (stage.song_step & 0x3F) == 60)
						character->set_anim(character, PlayerAnim_Peace);
					break;
				case StageId_1_1: //Bopeebo peace
					if ((stage.song_step & 0x1F) == 28)
						character->set_anim(character, PlayerAnim_Peace);
					break;
				default:
					break;
			}
		}
	}
	
	//Retry screen
	if (character->animatable.anim >= PlayerAnim_Dead3)
	{
		//Tick skull fragments
		if (this->skull_scale)
		{
			SkullFragment *frag = this->skull;
			for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
			{
				//Draw fragment
				RECT frag_src = {
					(i & 1) ? 112 : 96,
					(i >> 1) << 4,
					16,
					16
				};
				fixed_t skull_dim = (FIXED_DEC(16,1) * this->skull_scale) >> 6;
				fixed_t skull_rad = skull_dim >> 1;
				RECT_FIXED frag_dst = {
					character->x + (((fixed_t)frag->x << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.x,
					character->y + (((fixed_t)frag->y << FIXED_SHIFT) >> 3) - skull_rad - stage.camera.y,
					skull_dim,
					skull_dim,
				};
				Stage_DrawTex(&this->tex_retry, &frag_src, &frag_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
				
				//Move fragment
				frag->x += frag->xsp;
				frag->y += ++frag->ysp;
			}
			
			//Decrease scale
			this->skull_scale--;
		}
		
		//Draw input options
		u8 input_scale = 16 - this->skull_scale;
		if (input_scale > 16)
			input_scale = 0;
		
		RECT button_src = {
			 0, 96,
			16, 16
		};
		RECT_FIXED button_dst = {
			character->x - FIXED_DEC(32,1) - stage.camera.x,
			character->y - FIXED_DEC(88,1) - stage.camera.y,
			(FIXED_DEC(16,1) * input_scale) >> 4,
			FIXED_DEC(16,1),
		};
		
		//Cross - Retry
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Circle - Blueball
		button_src.x = 16;
		button_dst.y += FIXED_DEC(56,1);
		Stage_DrawTex(&this->tex_retry, &button_src, &button_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
		
		//Draw 'RETRY'
		u8 retry_frame;
		
		if (character->animatable.anim == PlayerAnim_Dead6)
		{
			//Selected retry
			retry_frame = 2 - (this->retry_bump >> 3);
			if (retry_frame >= 3)
				retry_frame = 0;
			if (this->retry_bump & 2)
				retry_frame += 3;
			
			if (++this->retry_bump == 0xFF)
				this->retry_bump = 0xFD;
		}
		else
		{
			//Idle
			retry_frame = 1 +  (this->retry_bump >> 2);
			if (retry_frame >= 3)
				retry_frame = 0;
			
			if (++this->retry_bump >= 55)
				this->retry_bump = 0;
		}
		
		RECT retry_src = {
			(retry_frame & 1) ? 48 : 0,
			(retry_frame >> 1) << 5,
			48,
			32
		};
		RECT_FIXED retry_dst = {
			character->x -  FIXED_DEC(7,1) - stage.camera.x,
			character->y - FIXED_DEC(92,1) - stage.camera.y,
			FIXED_DEC(48,1),
			FIXED_DEC(32,1),
		};
		Stage_DrawTex(&this->tex_retry, &retry_src, &retry_dst, FIXED_MUL(stage.camera.zoom, stage.bump));
	}
	
	//Animate and draw character
	Animatable_Animate(&character->animatable, (void*)this, Char_BF_SetFrame);
	Character_Draw(character, &this->tex, &char_bf_frame[this->frame]);
}

void Char_BF_SetAnim(Character *character, u8 anim)
{
	Char_BF *this = (Char_BF*)character;
	
	//Perform animation checks
	switch (anim)
	{
		case PlayerAnim_Dead0:
			//Begin reading dead.arc and adjust focus
			this->arc_dead = IO_AsyncReadFile(&this->file_dead_arc);
			character->focus_x = FIXED_DEC(0,1);
			character->focus_y = FIXED_DEC(-40,1);
			character->focus_zoom = FIXED_DEC(125,100);
			break;
		case PlayerAnim_Dead2:
			//Unload main.arc
			Mem_Free(this->arc_main);
			this->arc_main = this->arc_dead;
			this->arc_dead = NULL;
			
			//Find dead.arc files
			const char **pathp = (const char *[]){
				"dead1.tim", //BF_ArcDead_Dead1
				"dead2.tim", //BF_ArcDead_Dead2
				"retry.tim", //BF_ArcDead_Retry
				NULL
			};
			IO_Data *arc_ptr = this->arc_ptr;
			for (; *pathp != NULL; pathp++)
				*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
			
			//Load retry art
			Gfx_LoadTex(&this->tex_retry, this->arc_ptr[BF_ArcDead_Retry], 0);
			break;
	}
	
	//Set animation
	Animatable_SetAnim(&character->animatable, anim);
	Character_CheckStartSing(character);
}

void Char_BF_Free(Character *character)
{
	Char_BF *this = (Char_BF*)character;
	
	//Free art
	Mem_Free(this->arc_main);
	Mem_Free(this->arc_dead);
}

Character *Char_BF_New(fixed_t x, fixed_t y)
{
	//Allocate boyfriend object
	Char_BF *this = Mem_Alloc(sizeof(Char_BF));
	if (this == NULL)
	{
		sprintf(error_msg, "[Char_BF_New] Failed to allocate boyfriend object");
		ErrorLock();
		return NULL;
	}
	
	//Initialize character
	this->character.tick = Char_BF_Tick;
	this->character.set_anim = Char_BF_SetAnim;
	this->character.free = Char_BF_Free;
	
	Animatable_Init(&this->character.animatable, char_bf_anim);
	Character_Init((Character*)this, x, y);
	
	//Set character information
	this->character.spec = CHAR_SPEC_MISSANIM;

	//health bar color
	this->character.health_bar = 0xFF29B5D6;
	
	this->character.focus_x = FIXED_DEC(-50,1);
	this->character.focus_y = (stage.stage_id == StageId_1_4) ? FIXED_DEC(-85,1) : FIXED_DEC(-65,1);
	this->character.focus_zoom = FIXED_DEC(1,1);
	
	//Load art
	if (stage.stage_id >= StageId_5_1 && stage.stage_id <= StageId_5_3)
	{
		this->arc_main = IO_Read("\\CHAR\\XMASBF.ARC;1");
		this->arc_dead = NULL;
		IO_FindFile(&this->file_dead_arc, "\\CHAR\\BFDEAD.ARC;1");
		
		const char **pathp = (const char *[]){
			"xmasbf0.tim",   //BF_ArcMain_BF0
			"xmasbf1.tim",   //BF_ArcMain_BF1
			"xmasbf2.tim",   //BF_ArcMain_BF2
			"xmasbf3.tim",   //BF_ArcMain_BF3
			"xmasbf3.tim",   //BF_ArcMain_BF4
			"xmasbf4.tim",   //BF_ArcMain_BF5
			"xmasbf5.tim",   //BF_ArcMain_BF6
			"dead0.tim", //BF_ArcMain_Dead0
			NULL
		};
		IO_Data *arc_ptr = this->arc_ptr;
		for (; *pathp != NULL; pathp++)
			*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	}
	else
	{
		this->arc_main = IO_Read("\\CHAR\\BF.ARC;1");
		this->arc_dead = NULL;
		IO_FindFile(&this->file_dead_arc, "\\CHAR\\BFDEAD.ARC;1");
		
		const char **pathp = (const char *[]){
			"bf0.tim",   //BF_ArcMain_BF0
			"bf1.tim",   //BF_ArcMain_BF1
			"bf2.tim",   //BF_ArcMain_BF2
			"bf3.tim",   //BF_ArcMain_BF3
			"bf4.tim",   //BF_ArcMain_BF4
			"bf5.tim",   //BF_ArcMain_BF5
			"bf6.tim",   //BF_ArcMain_BF6
			"bf7.tim",   //BF_ArcMain_BF7
			"bf8.tim",   //BF_ArcMain_BF8
			"dead0.tim", //BF_ArcMain_Dead0
			NULL
		};
		IO_Data *arc_ptr = this->arc_ptr;
		for (; *pathp != NULL; pathp++)
			*arc_ptr++ = Archive_Find(this->arc_main, *pathp);
	}
	//Initialize render state
	this->tex_id = this->frame = 0xFF;
	
	//Initialize player state
	this->retry_bump = 0;
	
	//Copy skull fragments
	memcpy(this->skull, char_bf_skull, sizeof(char_bf_skull));
	this->skull_scale = 64;
	
	SkullFragment *frag = this->skull;
	for (size_t i = 0; i < COUNT_OF_MEMBER(Char_BF, skull); i++, frag++)
	{
		//Randomize trajectory
		frag->xsp += RandomRange(-4, 4);
		frag->ysp += RandomRange(-2, 2);
	}
	
	return (Character*)this;
}
