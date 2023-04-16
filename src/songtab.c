/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "songtab.h"
#include "stage.h"
#include "gfx.h"
#include "mutil.h"

typedef struct 
{
    fixed_t x, y;
    fixed_t iconzoom;
} Tab;

Tab tab;

RECT geticon(int i)
{
    //Get src
    RECT src = {
        (i % 1) * 114,
        16 + (i / 1) * 46,
        46,
        46,
    };
    return src;
}

void bump(fixed_t *bump)
{
    if ((bump = FIXED_UNIT + FIXED_MUL(bump - FIXED_UNIT, FIXED_DEC(95,100))) <= FIXED_DEC(1003,1000))
        bump = FIXED_UNIT;
      
    if (stage.flag & STAGE_FLAG_JUST_STEP)
    { 
        //Bump screen
        if ((stage.song_step & 0xF) == 0)
        	bump = FIXED_DEC(103,100); 
    }
}

void Tab_Init(void)
{
    tab.x = -120;
    if (stage.prefs.widescreen)
        tab.x = -220;
    tab.y = -90;
    tab.iconzoom = FIXED_UNIT;
}

void Tab_draw(void)
{
    RECT icon = geticon((stage.mode == StageMode_Swap ? stage.player_state[0].character->health_i : stage.player_state[1].character->health_i));
    RECT_FIXED icon_dst = {FIXED_DEC(tab.x,1), FIXED_DEC(tab.y,1), FIXED_DEC(46,1), FIXED_DEC(46,1)};
    bump(&tab.iconzoom);

    RECT bar_fill = {252, 252, 1, 1};
    if (stage.song_step >= 20 && !stage.paused)
        tab.x += Lerp(FIXED_DEC(tab.x, 1), FIXED_DEC(-screen.SCREEN_WIDTH + -40,1), FIXED_DEC(6,100)) / 1024;
    RECT_FIXED bar_dst = {FIXED_DEC(tab.x,1), FIXED_DEC(tab.y,1), FIXED_DEC(144,1), FIXED_DEC(24,1)};

    //draw text
    stage.font_cdr.draw(&stage.font_cdr, stage.songname, FIXED_DEC(tab.x + 135,1), FIXED_DEC(tab.y+1,1), FontAlign_Right);
    Stage_DrawTex(&stage.tex_hud1, &icon, &icon_dst, tab.iconzoom);
    stage.font_cdr.draw_col(&stage.font_cdr, stage.credit, FIXED_DEC(tab.x + 155,1), FIXED_DEC(tab.y + 13,1), FontAlign_Right, stage.tr, stage.tg, stage.tb);
    Stage_BlendTex(&stage.tex_hud0, &bar_fill, &bar_dst, stage.bump, 1);

}