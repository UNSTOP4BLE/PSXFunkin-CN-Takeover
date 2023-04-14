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
} Tab;

Tab tab;

void Tab_Init(void)
{
    tab.x = -120;
    tab.y = -90;
}

void Tab_draw(void)
{
    
    RECT bar_fill = {252, 252, 1, 1};
    if (stage.song_step >= 20)
        tab.x += Lerp(FIXED_DEC(tab.x, 1), FIXED_DEC(-screen.SCREEN_WIDTH + -40,1), FIXED_DEC(6,100)) / 1024;
    RECT_FIXED bar_dst = {FIXED_DEC(tab.x,1), FIXED_DEC(tab.y,1), FIXED_DEC(144,1), FIXED_DEC(24,1)};

    //draw text
    stage.font_cdr.draw(&stage.font_cdr, stage.songname, FIXED_DEC(tab.x + 135,1), FIXED_DEC(tab.y+1,1), FontAlign_Right);
    u32 col = stage.stage_def[stage.stage_id].col;
    stage.font_cdr.draw_col(&stage.font_cdr, stage.stage_def[stage.stage_id].credit, FIXED_DEC(tab.x + 155,1), FIXED_DEC(tab.y + 13,1), FontAlign_Right, ((col >> 16) & 0xFF) >> 1, ((col >> 8) & 0xFF) >> 1, ((col >> 0) & 0xFF) >> 1);

    Stage_BlendTex(&stage.tex_hud0, &bar_fill, &bar_dst, stage.bump, 1);

}