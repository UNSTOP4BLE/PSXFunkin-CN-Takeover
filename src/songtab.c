/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "songtab.h"
#include "stage.h"
#include "gfx.h"

void Tab_draw(void)
{
	fixed_t x = -160;
	fixed_t y = -90;
	
	RECT bar_fill = {252, 252, 1, 1};
	RECT_FIXED bar_dst = {FIXED_DEC(x,1), FIXED_DEC(y,1), FIXED_DEC(144,1), FIXED_DEC(24,1)};

	//draw text
	stage.font_cdr.draw(&stage.font_cdr, stage.songname, FIXED_DEC(x + 45,1), FIXED_DEC(y+1,1), FontAlign_Center);
  	u32 col = stage.stage_def[stage.stage_id].col;
	stage.font_cdr.draw_col(&stage.font_cdr, stage.stage_def[stage.stage_id].credit, FIXED_DEC(x + 16,1), FIXED_DEC(y + 13,1), FontAlign_Right, ((col >> 16) & 0xFF) >> 1, ((col >> 8) & 0xFF) >> 1, ((col >> 0) & 0xFF) >> 1);

	Stage_BlendTex(&stage.tex_hud0, &bar_fill, &bar_dst, stage.bump, 1);

}