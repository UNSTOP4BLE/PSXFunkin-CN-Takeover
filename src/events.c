/*
  This Source Code Form is subject to the terms of the Mozilla Public
  License, v. 2.0. If a copy of the MPL was not distributed with this
  file, You can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "events.h"
#include "stage.h"
#include "timer.h"
#include "random.h"
#include "mutil.h"

Events event_speed;

static void Events_Check(Event* event)
{
    //Events
    switch(event->event & EVENTS_FLAG_VARIANT)
    {
        case EVENTS_FLAG_SPEED: //Scroll Speed!!
        {
            event_speed.value1 = event->value1;
            event_speed.value2 = event->value2;
            break;
        }
        case EVENTS_FLAG_GF: //Set GF Speed!!
        {
            stage.gf_speed = (event->value1 >> FIXED_SHIFT) * 4;
            break;
        }
        case EVENTS_FLAG_CAMZOOM: //Add Camera Zoom!!
        {

            stage.character_bump += event->value1;
            stage.bump += event->value2;
            break;
        }
        case EVENTS_FLAG_SHAKE: //Add Shake
        {
            stage.cam_shake.duration = event->value1;
            stage.cam_shake.intensity = event->value2 * 128;

            stage.hud_shake.duration = event->value3;
            stage.hud_shake.intensity = event->value4 * 128;
            break;
        }
        default: //nothing lol
        break;
    }
}

void Events_Tick(void)
{
    if (stage.hud_shake.duration > 0)
    {
        stage.hud_shake.shake += RandomRange(-stage.hud_shake.intensity, stage.hud_shake.intensity * 2);
        stage.hud_shake.duration -= timer_dt;
    }
    else
    {
        stage.hud_shake.shake = 0;
        stage.hud_shake.duration = 0;
    }

    if (stage.cam_shake.duration > 0)
    {
        stage.cam_shake.shake += RandomRange(-stage.cam_shake.intensity, stage.cam_shake.intensity * 2);
        stage.cam_shake.duration -= timer_dt;
    }
    else
    {
        stage.cam_shake.shake = 0;
        stage.cam_shake.duration = 0;
    }

    //Scroll Speed!
    stage.speed += (FIXED_MUL(stage.ogspeed, event_speed.value1) - stage.speed) / (((event_speed.value2 / 64) + 1));
}

void Events_StartEvents(void)
{
    for (Event *event = stage.cur_event; event->pos != 0xFFFF; event++)
    {
        //Update event pointer
        if (event->pos > (stage.note_scroll >> FIXED_SHIFT))
            break;

        else
            stage.cur_event++;

        if (event->event & EVENTS_FLAG_PLAYED)
            continue;

        Events_Check(event);
        event->event |= EVENTS_FLAG_PLAYED;
    }

    Events_Tick();
}

//Initialize some stuffs
void Events_Load(void)
{
    //Scroll Speed
    event_speed.value1 = FIXED_UNIT;
    event_speed.value2 = 0;
}