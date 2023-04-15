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

void Tab_Init(void)
{
    tab.x = -120;
    if (stage.prefs.widescreen)
        tab.x = -220;
    tab.y = -90;
}

void Tab_draw(void)
{
    RECT icon = geticon(stage.player_state[0].character->health_i);
    RECT bar_fill = {252, 252, 1, 1};
    if (stage.song_step >= 20 && !stage.paused)
        tab.x += Lerp(FIXED_DEC(tab.x, 1), FIXED_DEC(-screen.SCREEN_WIDTH + -40,1), FIXED_DEC(6,100)) / 1024;
    RECT_FIXED bar_dst = {FIXED_DEC(tab.x,1), FIXED_DEC(tab.y,1), FIXED_DEC(144,1), FIXED_DEC(24,1)};

    //draw text
    stage.font_cdr.draw(&stage.font_cdr, stage.songname, FIXED_DEC(tab.x + 135,1), FIXED_DEC(tab.y+1,1), FontAlign_Right);
    FntPrint("str 2%s2 %d, %d, %d, ", stage.credit, stage.tr, stage.tg, stage.tb );
    stage.font_cdr.draw_col(&stage.font_cdr, stage.credit, FIXED_DEC(tab.x + 155,1), FIXED_DEC(tab.y + 13,1), FontAlign_Right, stage.tr, stage.tg, stage.tb);
    Stage_BlendTex(&stage.tex_hud0, &bar_fill, &bar_dst, stage.bump, 1);
    Stage_DrawTex(&stage.tex_hud1, &icon, &bar_dst, stage.bump);

}