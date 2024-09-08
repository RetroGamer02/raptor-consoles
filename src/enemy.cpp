#include <stdio.h>
#include <string.h>
#include "common.h"
#include "enemy.h"
#include "gfxapi.h"
#include "rap.h"
#include "glbapi.h"
#include "tile.h"
#include "loadsave.h"
#include "fx.h"
#include "eshot.h"
#include "shadow.h"
#include "anims.h"
#include "objects.h"
#include "bonus.h"
#include "flame.h"
#include "joyapi.h"
#include "input.h"
#include "fileids.h"

#define NORM_SHOOT  -1
#define START_SHOOT   0

int spriteflag[4];
int spriteitm[4] = {
     FILE1fc_SPRITE1_ITM, FILE200_SPRITE2_ITM, FILE300_SPRITE3_ITM, FILE400_SPRITE4_ITM
};

SPRITE *slib[4];

int cur_visable;
int boss_sound;
int g_numslibs;
int numslibs[4];

int numboss, numships;
int end_waveflag;

SPRITE_SHIP ships[30];
SPRITE_SHIP last_enemy, first_enemy;
SPRITE_SHIP *free_enemy;

CSPRITE *end_enemy, *cur_enemy;

int tiley;

SPRITE_SHIP *onscreen[MAX_ONSCREEN], *rscreen[MAX_ONSCREEN];

/***************************************************************************
   MoveEobj() - gets next postion for an Object at speed
 ***************************************************************************/
int 
MoveEobj(
    MOVEOBJ *cur,          // INPUT : pointer to MOVEOBJ
    int speed              // INPUT : speed to plot at
)      
{
    if (!speed)
        return 0;
    
    if (cur->delx >= cur->dely)
    {
        while (speed)
        {
            speed--;
            cur->maxloop--;
            
            if (cur->maxloop == 0)
            {
                cur->done = 1;
                return speed;
            }
            
            cur->x += cur->addx;
            cur->err += cur->dely;
            
            if (cur->err > 0)
            {
                cur->y += cur->addy;
                cur->err -= cur->delx;
            }
        }
    }
    else
    {
        while (speed)
        {
            speed--;
            cur->maxloop--;
            
            if (cur->maxloop == 0)
            {
                cur->done = 1;
                return speed;
            }
            
            cur->y += cur->addy;
            cur->err += cur->delx;
            
            if (cur->err > 0)
            {
                cur->x += cur->addx;
                cur->err -= cur->dely;
            }
        }
    }
    
    if (cur->maxloop < 1)
        cur->done = 1;
    
    return speed;
}

/***************************************************************************
ENEMY_FreeSprites () - Free Memory Used by Sprites use in last level
 ***************************************************************************/
void 
ENEMY_FreeSprites(
    void
)
{
    int loop, i;
    CSPRITE *curfld;
    SPRITE *curlib;

    for (loop = 0; loop < 4; loop++)
    {
        if (spriteflag[loop])
            GLB_FreeItem(spriteitm[loop]);
    }
    
    #ifdef __PPC__
    for (loop = 0; loop < mapmem->numsprites.get_value(); loop++)
    #else
    for (loop = 0; loop < mapmem->numsprites; loop++)
    #endif
    {
        curfld = &csprite[loop];
        #ifdef __PPC__
        curlib = &slib[csprite[loop].game.get_value()][csprite[loop].slib.get_value()];
        #else
        curlib = &slib[csprite[loop].game][csprite[loop].slib];
        #endif
        
        #ifdef __PPC__
        if (cur_diff & curfld->level.get_value())
        #else
        if (cur_diff & curfld->level)
        #endif
        {
            #ifdef __PPC__
            for (i = 0; i < curlib->num_frames.get_value(); i++)
            {
                GLB_FreeItem(curlib->item.get_value() + i);
            }
            #else
            for (i = 0; i < curlib->num_frames; i++)
            {
                GLB_FreeItem(curlib->item + i);
            }
            #endif
        }
    }
}

/***************************************************************************
ENEMY_LoadSprites() -
 ***************************************************************************/
void 
ENEMY_LoadSprites(
    void
)
{
    int loop, i, item;
    CSPRITE *curfld;
    SPRITE *curlib;
    
    ENEMY_Clear();
    cur_visable = 0;
    boss_sound = 0;
    
    #ifdef __PPC__
    for (loop = 0; loop < mapmem->numsprites.get_value(); loop++)
    #else
    for (loop = 0; loop < mapmem->numsprites; loop++)
    #endif
    {
        curfld = &csprite[loop];
        #ifdef __PPC__
        curlib = &slib[csprite[loop].game.get_value()][csprite[loop].slib.get_value()];
        #else
        curlib = &slib[csprite[loop].game][csprite[loop].slib];
        #endif
        curlib->item = GLB_GetItemID(curlib->iname);
        
        #ifdef __PPC__
        switch (curfld->level.get_value())
        #else
        switch (curfld->level)
        #endif
        {
        default:
            curfld->level = EB_NOT_USED;
            break;
        
        case E_SECRET_1:
            curfld->level = EB_SECRET_1;
            break;
        
        case E_SECRET_2:
            curfld->level = EB_SECRET_2;
            break;
        
        case E_SECRET_3:
            curfld->level = EB_SECRET_3;
            break;
        
        case E_EASY_LEVEL:
            curfld->level = EB_EASY_LEVEL;
            break;
        
        case E_MED_LEVEL:
            curfld->level = EB_MED_LEVEL;
            break;
        
        case E_HARD_LEVEL:
            curfld->level = EB_HARD_LEVEL;
            break;
        }
        
        #ifdef __PPC__
        if (cur_diff & curfld->level.get_value())
        #else
        if (cur_diff & curfld->level)
        #endif
        {
            #ifdef __PPC__
            if (curlib->item.get_value() != -1)
            {
                for (i = 0; i < curlib->num_frames.get_value(); i++)
                {
                    item = curlib->item.get_value() + i;
                    
                    GLB_CacheItem(item);
                }
            }
            else
            {
                curfld->level = EB_NOT_USED;
            }
            #else
            if (curlib->item != -1)
            {
                for (i = 0; i < curlib->num_frames; i++)
                {
                    item = curlib->item + i;
                    
                    GLB_CacheItem(item);
                }
            }
            else
            {
                curfld->level = EB_NOT_USED;
            }
            #endif
        }
        else
            curfld->level = EB_NOT_USED;
    }
}

/***************************************************************************
ENEMY_LoadLib () - Loads and Locks spritelib's MUST becalled b4 LoadSprites
 ***************************************************************************/
void ENEMY_LoadLib(
    void
)
{
    int loop;
    
    memset(spriteflag, 0, sizeof(spriteflag));
    
    #ifdef __PPC__
    for (loop = 0; loop < mapmem->numsprites.get_value(); loop++)
    #else
    for (loop = 0; loop < mapmem->numsprites; loop++)
    #endif
    {
        #ifdef __PPC__
        spriteflag[csprite[loop].game.get_value()] = 1;
        #else
        spriteflag[csprite[loop].game] = 1;
        #endif
    }
    g_numslibs = 0;
    for (loop = 0; loop < 4; loop++)
    {
        slib[loop] = NULL;
        numslibs[loop] = 0;
        
        if (spriteflag[loop])
            g_numslibs++;
    }
    
    if (g_numslibs > 1 && !gameflag[2] && !gameflag[3])
        EXIT_Error("ENEMY_LoadSprites() - F:%d  G1:%d G2:%d G3:%d G4:%d", g_numslibs, spriteflag[0], spriteflag[1], spriteflag[2], spriteflag[3]);
    
    for (loop = 0; loop < 4; loop++)
    {
        if (spriteflag[loop])
        {
            slib[loop] = (SPRITE*)GLB_LockItem(spriteitm[loop]);
            
            if (!slib[loop])
                EXIT_Error("ENEMY_LoadSprites() - memory");
            
            numslibs[loop] = GLB_ItemSize(spriteitm[loop]);
            numslibs[loop] /= sizeof(SPRITE);
        }
    }
}

/***************************************************************************
ENEMY_Clear()
 ***************************************************************************/
void ENEMY_Clear(
    void
)
{
    int loop;
    
    numboss = 0;
    numships = 0;
    end_waveflag = 0;
    
    first_enemy.prev = NULL;
    first_enemy.next = &last_enemy;
    
    last_enemy.prev = &first_enemy;
    last_enemy.next = NULL;
    
    free_enemy = ships;
    
    memset(ships, 0, sizeof(ships));
    
    for (loop = 0; loop < MAX_ONSCREEN - 1; loop++)
    {
        ships[loop].next = &ships[loop + 1];
    }
    #ifdef __PPC__
    if (mapmem->numsprites.get_value())
    {
        end_enemy = csprite + mapmem->numsprites.get_value() - 1;
        cur_enemy = csprite;
    }
    #else
    if (mapmem->numsprites)
    {
        end_enemy = csprite + mapmem->numsprites - 1;
        cur_enemy = csprite;
    }
    #endif
    else
    {
        end_enemy = NULL;
        cur_enemy = NULL;
    }
}

/*-------------------------------------------------------------------------*
ENEMY_Get() - Gets An Free Enemy from link list
 *-------------------------------------------------------------------------*/
SPRITE_SHIP
*ENEMY_Get(
    void
)
{
    SPRITE_SHIP *sh;
    
    if (!free_enemy)
        EXIT_Error("ENEMY_Get() - Max Sprites");
    
    numships++;
    
    sh = free_enemy;
    free_enemy = free_enemy->next;
    
    memset(sh, 0, sizeof(SPRITE_SHIP));
    
    sh->next = &last_enemy;
    sh->prev = last_enemy.prev;
    last_enemy.prev = sh;
    sh->prev->next = sh;
    
    return sh;
}

/*-------------------------------------------------------------------------*
ENEMY_Remove () - Removes an Enemy OBJECT from linklist
 *-------------------------------------------------------------------------*/
SPRITE_SHIP
*ENEMY_Remove(
    SPRITE_SHIP *sh
)
{
    SPRITE_SHIP *next;
    
    #ifdef __PPC__
    if (sh->lib->bossflag.get_value())
    #else
    if (sh->lib->bossflag)
    #endif
        numboss--;
    
    numships--;
    
    if (end_waveflag && numships < 1)
        startendwave = END_DURATION;
    
    next = sh->prev;
    
    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;
    
    memset(sh, 0, sizeof(SPRITE_SHIP));
    sh->item = -1;
    
    sh->next = free_enemy;
    
    free_enemy = sh;
    
    return next;
}

/*-------------------------------------------------------------------------*
ENEMY_Add () - Adds Enemy to attack player
 *-------------------------------------------------------------------------*/
void 
ENEMY_Add(
    CSPRITE *sprite
)
{
    SPRITE *curlib;
    SPRITE_SHIP *newe;
    char *pic;
    GFX_PIC *h;
    #ifdef __PPC__
    curlib = &slib[sprite->game.get_value()][sprite->slib.get_value()];
    #else
    curlib = &slib[sprite->game][sprite->slib];
    #endif
    
    newe = ENEMY_Get();
    #ifdef __PPC__
    pic = GLB_GetItem(curlib->item.get_value());
    #else
    pic = GLB_GetItem(curlib->item);
    #endif
    h = (GFX_PIC*)pic;
    
    #ifdef __PPC__
    newe->item = curlib->item.get_value();
    newe->width = h->width.get_value();
    newe->height = h->height.get_value();
    newe->hlx = h->width.get_value() >> 1;
    newe->hly = h->height.get_value() >> 1;
    #else
    newe->item = curlib->item;
    newe->width = h->width;
    newe->height = h->height;
    newe->hlx = h->width >> 1;
    newe->hly = h->height >> 1;
    #endif
    
    newe->kami = KAMI_FLY;
    
    #ifdef __PPC__
    newe->hits = curlib->hits.get_value();
    newe->lib = &slib[sprite->game.get_value()][sprite->slib.get_value()];
    newe->y = tileyoff - (tiley - sprite->y.get_value()) * 32 - 97;
    newe->x = sprite->x.get_value() * 32 + MAP_LEFT;
    #else
    newe->hits = curlib->hits;
    newe->lib = &slib[sprite->game][sprite->slib];
    newe->y = tileyoff - (tiley - sprite->y) * 32 - 97;
    newe->x = sprite->x * 32 + MAP_LEFT;
    #endif
   
    
    
    newe->edir = E_FORWARD;
    newe->x += 16;
    newe->y += 16;
    newe->x -= newe->hlx;
    newe->y -= newe->hly;
    newe->x2 = newe->x + newe->width;
    newe->y2 = newe->y + newe->height;
    newe->move.x = newe->sx = newe->x;
    newe->move.y = newe->sy = newe->y;
    #ifdef __PPC__
    newe->frame_rate = curlib->frame_rate.get_value();
    newe->speed = curlib->movespeed.get_value();
    newe->countdown = curlib->countdown.get_value() - newe->move.y;
    #else
    newe->frame_rate = curlib->frame_rate;
    newe->speed = curlib->movespeed;
    newe->countdown = curlib->countdown - newe->move.y;
    #endif
    
    
    
    newe->shoot_disable = 0;
    newe->shoot_on = 0;
    newe->shootagain = NORM_SHOOT;
    #ifdef __PPC__
    newe->shootcount = curlib->shootcnt.get_value();
    newe->shootflag = curlib->shootstart.get_value();
    #else
    newe->shootcount = curlib->shootcnt;
    newe->shootflag = curlib->shootstart;
    #endif
    
    #ifdef __PPC__
    if (curlib->bossflag.get_value() && curplr_diff <= DIFF_1)
    #else
    if (curlib->bossflag && curplr_diff <= DIFF_1)
    #endif
    {
        newe->hits -= newe->hits >> 1;
        newe->shootcount -= newe->shootcount >> 2;
    }
    
    #ifdef __PPC__
    switch (curlib->animtype.get_value())
    {
    default:
        EXIT_Error("ENEMY_Add() - Invalid ANIMTYPE");
        break;
    case GANIM_NORM:
        newe->anim_on = 1;
        newe->num_frames = curlib->num_frames.get_value();
        break;
    
    case GANIM_SHOOT:
        newe->anim_on = 0;
        newe->num_frames = curlib->num_frames.get_value();
        break;
    
    case GANIM_MULTI:
        newe->anim_on = 1;
        newe->num_frames = curlib->rewind.get_value();
        break;
    }
    #else
    switch (curlib->animtype)
    {
    default:
        EXIT_Error("ENEMY_Add() - Invalid ANIMTYPE");
        break;
    case GANIM_NORM:
        newe->anim_on = 1;
        newe->num_frames = curlib->num_frames;
        break;
    
    case GANIM_SHOOT:
        newe->anim_on = 0;
        newe->num_frames = curlib->num_frames;
        break;
    
    case GANIM_MULTI:
        newe->anim_on = 1;
        newe->num_frames = curlib->rewind;
        break;
    }
    #endif
    
    #ifdef __PPC__
    switch (curlib->flighttype.get_value())
    #else
    switch (curlib->flighttype)
    #endif
    {
    case F_REPEAT:
    case F_LINEAR:
    case F_KAMI:
        newe->groundflag = 0;
        newe->sy = 100 - newe->hly;
        #ifdef __PPC__
        newe->move.x2 = newe->sx + curlib->flightx[0].get_value();
        newe->move.y2 = newe->sy + curlib->flighty[0].get_value();
        #else
        newe->move.x2 = newe->sx + curlib->flightx[0];
        newe->move.y2 = newe->sy + curlib->flighty[0];
        #endif
        newe->movepos = 1;
        InitMobj(&newe->move);
        MoveMobj(&newe->move);
        break;
    
    case F_GROUND:                                          
        newe->groundflag = 1;
        newe->move.x2 = newe->x;
        newe->move.y2 = 211;
        break;
    
    case F_GROUNDRIGHT:                                           
        newe->x -= newe->width;
        newe->move.x = newe->sx = newe->x;
        newe->groundflag = 1;
        newe->move.x2 = 335;
        newe->move.y2 = 211;
        break;
    
    case F_GROUNDLEFT:                                           
        newe->x += newe->width;
        newe->move.x = newe->sx = newe->x;
        newe->groundflag = 1;
        newe->move.x2 = -newe->hlx;
        newe->move.y2 = 211;
        break;
    }
    
    #ifdef __PPC__
    newe->suckagain = curlib->hits.get_value() >> 4;

    if (curlib->song.get_value() != -1)
        boss_sound = 1;
    #else
    newe->suckagain = curlib->hits >> 4;

    if (curlib->song != -1)
        boss_sound = 1;
    #endif
    
    
}

/***************************************************************************
ENEMY_GetRandom () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP
*ENEMY_GetRandom(
    void
)
{
    int pos;
    
    if (!cur_visable)
        return NULL;
    
    pos = wrand() % cur_visable;
    
    return onscreen[pos];
}

/***************************************************************************
ENEMY_GetRandomAir () - Returns a random ship thats visable
 ***************************************************************************/
SPRITE_SHIP
*ENEMY_GetRandomAir(
    void
)
{
    int pos;
    int loop;
    
    if (!cur_visable)
        return NULL;
    
    pos = 0;
    for (loop = 0; loop < cur_visable; loop++)
    {
        if (onscreen[loop]->groundflag)
            continue;
        
        rscreen[pos] = onscreen[loop];
        pos++;
    }
    
    if (pos > 0)
    {
        pos = wrand() % pos;
        return rscreen[pos];
    }
    
    return NULL;
}

/***************************************************************************
ENEMY_DamageAll () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
int 
ENEMY_DamageAll(
    int x,           // INPUT : x position            
    int y,           // INPUT : y position
    int damage       // INPUT : damage
)
{
    int loop;
    SPRITE_SHIP *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits -= damage;
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************
ENEMY_DamageGround () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
int 
ENEMY_DamageGround(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int damage             // INPUT : damage
)
{
    int loop;
    SPRITE_SHIP *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (!cur->groundflag)
            continue;
        
        if (x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits -= damage;
            if (curplr_diff == DIFF_0)
                cur->hits -= damage;
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************
ENEMY_DamageAir () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
int 
ENEMY_DamageAir(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int damage             // INPUT : damage
)
{
    int loop;
    SPRITE_SHIP *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (cur->groundflag)
            continue;
        
        if (x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits -= damage;
            if (curplr_diff == DIFF_0)
                cur->hits -= damage;
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************
ENEMY_DamageEnergy () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
SPRITE_SHIP
*ENEMY_DamageEnergy(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int damage             // INPUT : damage
)
{
    int loop;
    SPRITE_SHIP *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (cur->groundflag)
            continue;
        
        if (x > cur->x&& x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits--;
            
            #ifdef __PPC__
            if (cur->lib->suck.get_value())
            #else
            if (cur->lib->suck)
            #endif
            {
                if (cur->suckagain > 0)
                {
                    cur->suckagain -= damage;
                }
                else
                {
                    cur->shoot_on = 0;
                    cur->shoot_disable = 1;
                    cur->shootagain = NORM_SHOOT;
                    SND_3DPatch(FX_EGRAB, cur->x + cur->hlx, cur->y + cur->hlx);
                }
            }
            return cur;
        }
    }
    
    return NULL;
}

/***************************************************************************
ENEMY_Think() - Does all thinking for enemy ships ( ground/air )
 ***************************************************************************/
void ENEMY_Think(
    void
)
{
    CSPRITE *old_enemy;
    SPRITE_SHIP *sprite;
    SPRITE *curlib;
    int speed, loop, x, y, suben, area;
    
    if (boss_sound)
    {
        if (!SND_IsPatchPlaying(FX_BOSS1))
            SND_Patch(FX_BOSS1, 127);
    }
    
    tiley = tilepos / MAP_COLS - 3;
    
    #ifdef __PPC__
    while (!end_waveflag && cur_enemy->y.get_value() == tiley)
    #else
    while (!end_waveflag && cur_enemy->y == tiley)
    #endif
    {
        do
        {
            old_enemy = cur_enemy;
            
            #ifdef __PPC__
            if (cur_enemy->level.get_value() != EB_NOT_USED)
            #else
            if (cur_enemy->level != EB_NOT_USED)
            #endif
                ENEMY_Add(cur_enemy);
            
            if (cur_enemy == end_enemy)
            {
                end_waveflag = 1;
                break;
            }
            
            cur_enemy++;
        #ifdef __PPC__
        } while (old_enemy->link.get_value() != -1 && old_enemy->link.get_value() != 1);
        #else
        } while (old_enemy->link != -1 && old_enemy->link != 1);
        #endif
    }
    
    cur_visable = 0;
    
    for (sprite = first_enemy.next; &last_enemy != sprite; sprite = sprite->next)
    {
        curlib = sprite->lib;
        
        #ifdef __PPC__
        if (curlib->num_frames.get_value() > 1)
        {
            sprite->item = curlib->item.get_value() + sprite->curframe;

            if (sprite->frame_rate < 1)
            {
                sprite->frame_rate = curlib->frame_rate.get_value();

                if (sprite->anim_on)
                {
                    sprite->curframe++;
                    if ((unsigned int)sprite->curframe >= (unsigned int)sprite->num_frames)
                    {
                        sprite->curframe -= curlib->rewind.get_value();
                        switch (curlib->animtype.get_value())
                        {
                        default:
                            EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE1");
                        case GANIM_NORM:
                            break;
                        
                        case GANIM_SHOOT:
                            sprite->anim_on = 0;
                            sprite->shoot_on = 1;
                            break;
                        
                        case GANIM_MULTI:
                            switch (sprite->multi)
                            {
                            case MULTI_START:
                                sprite->num_frames = curlib->num_frames.get_value();
                                sprite->multi = 2;
                                break;
                            
                            case MULTI_END:
                                sprite->shoot_on = 1;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
                sprite->frame_rate--;
            
            if (sprite->countdown < 1)
            {
                switch (curlib->animtype.get_value())
                {
                default:
                    EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE2");
                case GANIM_NORM:
                    sprite->shoot_on = 1;
                    break;
                
                case GANIM_SHOOT:
                    sprite->anim_on = 1;
                    break;
                
                case GANIM_MULTI:
                    if (!sprite->multi)
                        sprite->multi = MULTI_START;
                    break;
                }
            }
            else
                sprite->countdown -= curlib->movespeed.get_value();
        }
        else
        {
            if (sprite->countdown < 1)
            {
                sprite->countdown = -1;
                sprite->shoot_on = 1;
            }
            else
                sprite->countdown -= curlib->movespeed.get_value();
        }
        #else
        if (curlib->num_frames > 1)
        {
            sprite->item = curlib->item + sprite->curframe;
            
            if (sprite->frame_rate < 1)
            {
                sprite->frame_rate = curlib->frame_rate;
                
                if (sprite->anim_on)
                {
                    sprite->curframe++;
                    if ((unsigned int)sprite->curframe >= (unsigned int)sprite->num_frames)
                    {
                        sprite->curframe -= curlib->rewind;
                        switch (curlib->animtype)
                        {
                        default:
                            EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE1");
                        case GANIM_NORM:
                            break;
                        
                        case GANIM_SHOOT:
                            sprite->anim_on = 0;
                            sprite->shoot_on = 1;
                            break;
                        
                        case GANIM_MULTI:
                            switch (sprite->multi)
                            {
                            case MULTI_START:
                                sprite->num_frames = curlib->num_frames;
                                sprite->multi = 2;
                                break;
                            
                            case MULTI_END:
                                sprite->shoot_on = 1;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
                sprite->frame_rate--;
            
            if (sprite->countdown < 1)
            {
                switch (curlib->animtype)
                {
                default:
                    EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE2");
                case GANIM_NORM:
                    sprite->shoot_on = 1;
                    break;
                
                case GANIM_SHOOT:
                    sprite->anim_on = 1;
                    break;
                
                case GANIM_MULTI:
                    if (!sprite->multi)
                        sprite->multi = MULTI_START;
                    break;
                }
            }
            else
                sprite->countdown -= curlib->movespeed;
        }
        else
        {
            if (sprite->countdown < 1)
            {
                sprite->countdown = -1;
                sprite->shoot_on = 1;
            }
            else
                sprite->countdown -= curlib->movespeed;
        }
        #endif
        
        #ifdef __PPC__
        switch (curlib->flighttype.get_value())
        #else
        switch (curlib->flighttype)
        #endif
        {
        case F_REPEAT:
            sprite->x = sprite->move.x;
            sprite->y = sprite->move.y;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            
            #ifdef __PPC__
            speed = curlib->movespeed.get_value();
            #else
            speed = curlib->movespeed;
            #endif
            
            speed = MoveEobj(&sprite->move, speed);
            
            if (sprite->move.done)
            {
                sprite->move.x = sprite->move.x2;
                sprite->move.y = sprite->move.y2;
                #ifdef __PPC__
                sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos].get_value();
                sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos].get_value();
                #else
                sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos];
                #endif
                
                InitMobj(&sprite->move);
                MoveMobj(&sprite->move);
                
                speed = MoveEobj(&sprite->move, speed);
                
                if (!sprite->edir)
                {
                    sprite->movepos++;
                    #ifdef __PPC__
                    if (sprite->movepos >= curlib->numflight.get_value())
                    {
                        sprite->edir = E_BACKWARD;
                        sprite->movepos = curlib->numflight.get_value() - 1;
                    }
                    #else
                    if (sprite->movepos >= curlib->numflight)
                    {
                        sprite->edir = E_BACKWARD;
                        sprite->movepos = curlib->numflight - 1;
                    }
                    #endif
                }
                else
                {
                    sprite->movepos--;
                    #ifdef __PPC__
                    if (sprite->movepos <= curlib->repos.get_value())
                    {
                        sprite->edir = E_FORWARD;
                        sprite->movepos = curlib->repos.get_value();
                    }
                    #else
                    if (sprite->movepos <= curlib->repos)
                    {
                        sprite->edir = E_FORWARD;
                        sprite->movepos = curlib->repos;
                    }
                    #endif
                }
            }
            break;
        
        case F_KAMI:
            sprite->x = sprite->move.x;
            sprite->y = sprite->move.y;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            
            #ifdef __PPC__
            speed = curlib->movespeed.get_value();
            #else
            speed = curlib->movespeed;
            #endif
            speed = MoveEobj(&sprite->move, speed);
            
            if (sprite->kami == KAMI_END)
            {
                if (sprite->move.y > 0xc9)
                    sprite->doneflag = 1;
                
                if (sprite->hlx + 320 < sprite->move.x)
                    sprite->doneflag = 1;
                
                if (sprite->move.y + sprite->width < 0)
                    sprite->doneflag = 1;
                
                if (sprite->move.x + sprite->width < 0)
                    sprite->doneflag = 1;
            }
            
            if (sprite->move.done && sprite->kami != KAMI_END)
            {
                sprite->move.x = sprite->move.x2;
                sprite->move.y = sprite->move.y2;
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;
                
                if (sprite->kami == KAMI_CHASE)
                {
                    sprite->move.x2 = player_cx;
                    sprite->move.y2 = player_cy;
                    sprite->kami = KAMI_END;
                }
                else
                {
                    #ifdef __PPC__
                    sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos].get_value();
                    sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos].get_value();
                    #else
                    sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                    sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos];
                    #endif
                }
                
                InitMobj(&sprite->move);
                MoveMobj(&sprite->move);
                speed = MoveEobj(&sprite->move, speed);
                
                #ifdef __PPC__
                if (curlib->numflight.get_value() - 1 > sprite->movepos)
                    sprite->movepos++;
                else if (sprite->kami == KAMI_FLY)
                    sprite->kami = KAMI_CHASE;
                #else
                if (curlib->numflight - 1 > sprite->movepos)
                    sprite->movepos++;
                else if (sprite->kami == KAMI_FLY)
                    sprite->kami = KAMI_CHASE;
                #endif
            }
            break;
        
        case F_LINEAR:
            sprite->x = sprite->move.x;
            sprite->y = sprite->move.y;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            
            #ifdef __PPC__
            speed = curlib->movespeed.get_value();
            #else
            speed = curlib->movespeed;
            #endif
            
            speed = MoveEobj(&sprite->move, speed);
            
            if (sprite->move.done)
            {
                sprite->move.x = sprite->move.x2;
                sprite->move.y = sprite->move.y2;
                #ifdef __PPC__
                sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos].get_value();
                sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos].get_value();
                #else
                sprite->move.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                sprite->move.y2 = sprite->sy + curlib->flighty[sprite->movepos];
                #endif
                
                InitMobj(&sprite->move);
                MoveMobj(&sprite->move);
                
                speed = MoveEobj(&sprite->move, speed);
                
                sprite->movepos++;
                
                #ifdef __PPC__
                if (sprite->movepos > curlib->numflight.get_value())
                    sprite->doneflag = 1;
                #else
                if (sprite->movepos > curlib->numflight)
                    sprite->doneflag = 1;
                #endif
            }
            break;
        
        case F_GROUND:
            if (scroll_flag)
                sprite->y++;
            if (sprite->y > sprite->move.y2)
                sprite->doneflag = 1;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            break;
        
        case F_GROUNDRIGHT:
            if (scroll_flag)
                sprite->y++;
            if (sprite->y >= 0)
            {
                #ifdef __PPC__
                sprite->x += curlib->movespeed.get_value();
                #else
                sprite->x += curlib->movespeed;
                #endif
                if (sprite->x > sprite->move.x2)
                    sprite->doneflag = 1;
                else if (sprite->y > sprite->move.y2)
                    sprite->doneflag = 1;
            }
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            break;
        
        case F_GROUNDLEFT:
            if (scroll_flag)
                sprite->y++;
            if (sprite->y >= 0)
            {
                #ifdef __PPC__
                sprite->x -= curlib->movespeed.get_value();
                #else
                sprite->x -= curlib->movespeed;
                #endif
                if (sprite->x < sprite->move.x2)
                    sprite->doneflag = 1;
                else if (sprite->y > sprite->move.y2)
                    sprite->doneflag = 1;
            }
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            break;
        }
        
        if (sprite->shoot_on)
        {
            switch (sprite->shootagain)
            {
            case NORM_SHOOT:
                sprite->shootflag--;
                
                if (sprite->shootflag < 0)
                {
                    #ifdef __PPC__
                    sprite->shootflag = curlib->shootspace.get_value();
                    #else
                    sprite->shootflag = curlib->shootspace;
                    #endif
                    
                    if (sprite->shoot_disable == 0)
                    {
                        #ifdef __PPC__
                        for (loop = 0; loop < curlib->numguns.get_value(); loop++)
                        {
                            ESHOT_Shoot(sprite, loop);
                        }
                        #else
                        for (loop = 0; loop < curlib->numguns; loop++)
                        {
                            ESHOT_Shoot(sprite, loop);
                        }
                        #endif
                    }
                    sprite->shootcount--;

                    #ifdef __PPC__
                    if (sprite->shootcount < 1)
                        sprite->shootagain = curlib->shootframe.get_value();
                    #else
                    if (sprite->shootcount < 1)
                        sprite->shootagain = curlib->shootframe;
                    #endif
                }
                break;
            
            case START_SHOOT:
                sprite->shootagain = NORM_SHOOT;
                #ifdef __PPC__
                sprite->shootcount = curlib->shootcnt.get_value();
                sprite->shootflag = curlib->shootspace.get_value();
                #else
                sprite->shootcount = curlib->shootcnt;
                sprite->shootflag = curlib->shootspace;
                #endif
                break;
            
            default:
                sprite->shootagain--;
                break;
            }
        }
        
        if (sprite->doneflag)
        {
            sprite = ENEMY_Remove(sprite);
            continue;
        }
        
        #ifdef __PPC__
        if (curlib->shadow.get_value())
        #else
        if (curlib->shadow)
        #endif
        {
            if (sprite->groundflag)
            {
                SHADOW_GAdd(sprite->item, sprite->x, sprite->y);
            }
            else
            {
                SHADOW_Add(sprite->item, sprite->x, sprite->y);
            }
        }
        
        #ifdef __PPC__
        if (curlib->bossflag.get_value())
        #else
        if (curlib->bossflag)
        #endif
        {
            numboss++;
            if (sprite->hits < 50 && (gl_cnt & 2) != 0)
            {
                x = sprite->x + (wrand() % sprite->width);
                y = sprite->y + (wrand() % sprite->height);
                ANIMS_StartAnim(A_SMALL_AIR_EXPLO, x, y);
            }
        }
        
        if (!sprite->groundflag)
        {
            if (player_cx > sprite->x && player_cx < sprite->x2)
            {
                if (player_cy > sprite->y && player_cy < sprite->y2)
                {
                    if ((haptic) && (control == 2))
                    {
                        IPT_CalJoyRumbleMedium();                                                            //Rumble when enemy hit
                    }
                    sprite->hits -= (PLAYERWIDTH / 2);
                    if (sprite->width > sprite->height)
                        suben = sprite->width;
                    else
                        suben = sprite->height;
                    
                    OBJS_SubEnergy(suben >> 2);
                    x = player_cx + (wrand() % 8) - 4;
                    y = player_cy + (wrand() % 8) - 4;
                    ANIMS_StartAnim(A_SMALL_AIR_EXPLO, x, y);
                    SND_Patch(FX_CRASH, 127);
                }
            }
        }
        
        if (sprite->hits <= 0)
        {
            #ifdef __PPC__
            plr.score += curlib->money.get_value();
            #else
            plr.score += curlib->money;
            #endif
            
            SND_3DPatch(FX_AIREXPLO, sprite->x + sprite->hlx, sprite->x + sprite->hlx);
            
            #ifdef __PPC__
            switch (curlib->exptype.get_value())
            #else
            switch (curlib->exptype)
            #endif
            {
            case EXP_ENERGY:
                ANIMS_StartAnim(A_ENERGY_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                BONUS_Add(S_ITEMBUY6, sprite->x, sprite->y);
                break;
            
            case EXP_AIRSMALL1:
                ANIMS_StartAnim(A_MED_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_AIRSMALL2:
                ANIMS_StartAnim(A_MED_AIR_EXPLO2, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_AIRMED:
                ANIMS_StartAnim(A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_AIRLARGE:
                ANIMS_StartAnim(A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                area = (sprite->width>>4) * (sprite->height>>4);
                for (loop = 0; loop < area; loop++)
                {
                    x = sprite->x + (wrand() % sprite->width);
                    y = sprite->y + (wrand() % sprite->height);
                    if (loop & 1)
                        ANIMS_StartAnim(A_MED_AIR_EXPLO, x, y);
                    else
                        ANIMS_StartAAnim(A_MED_AIR_EXPLO2, x, y);
                }
                break;
            
            case EXP_GRDSMALL:
                ANIMS_StartAnim(A_SMALL_GROUND_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_GRDMED:
                x = sprite->x + (wrand() % sprite->width);
                y = sprite->y + (wrand() % sprite->height);
                if ((wrand() % 2) == 0)
                    ANIMS_StartAnim(A_GROUND_SPARKLE, x, y);
                else
                    ANIMS_StartAnim(A_GROUND_FLARE, x, y);
                ANIMS_StartAnim(A_LARGE_GROUND_EXPLO1, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_GRDLARGE:
                ANIMS_StartAnim(A_LARGE_GROUND_EXPLO1, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                area = (sprite->width>>4) * (sprite->height>>4);
                for (loop = 0; loop < area; loop++)
                {
                    x = sprite->x + (wrand() % sprite->width);
                    y = sprite->y + (wrand() % sprite->height);
                    
                    if ((wrand()%2) == 0)
                        ANIMS_StartAnim(A_GROUND_FLARE, x, y);
                    else
                        ANIMS_StartAnim(A_GROUND_SPARKLE, x, y);
                    
                    ANIMS_StartAnim(A_SMALL_GROUND_EXPLO, x, y);
                }
                break;
            
            case EXP_BOSS:
                ANIMS_StartAnim(A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                area = (sprite->width>>4) * (sprite->height>>4);
                for (loop = 0; loop < area; loop++)
                {
                    x = sprite->x + (wrand() % sprite->width);
                    y = sprite->y + (wrand() % sprite->height);
                    ANIMS_StartAAnim(A_GROUND_FLARE, x, y);
                    if (loop & 1)
                        ANIMS_StartAnim(A_LARGE_AIR_EXPLO, x, y);
                    else
                        ANIMS_StartAnim(A_MED_AIR_EXPLO2, x, y);
                }
                break;
            
            case EXP_PERSON:
                ANIMS_StartAnim(A_PERSON, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_PLATOON:
                ANIMS_StartAnim(A_PLATOON, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            }
            
            #ifdef __PPC__
            if (curlib->bonus.get_value() != -1)
                BONUS_Add(curlib->bonus.get_value(), sprite->x, sprite->y);
            #else
            if (curlib->bonus != -1)
                BONUS_Add(curlib->bonus, sprite->x, sprite->y);
            #endif
            
            sprite = ENEMY_Remove(sprite);
            
            continue;
        }
        
        y = sprite->y + sprite->height;
        
        if (y > 0 && sprite->y < 200)
        {
            x = sprite->x + sprite->width;
            
            if (x > 0 && sprite->x < 320)
            {
                onscreen[cur_visable] = sprite;
                cur_visable++;
            }
        }
    }
}

/***************************************************************************
ENEMY_DisplayGround () - Displays Ground ENEMY pics
 ***************************************************************************/
void 
ENEMY_DisplayGround(
    void
)
{
    SPRITE_SHIP *spt;
    
    for (spt = first_enemy.next; &last_enemy != spt; spt = spt->next)
    {
        if (!spt->groundflag)
            continue;
        GFX_PutSprite((char*)GLB_GetItem(spt->item), spt->x, spt->y);
    }
}

/***************************************************************************
ENEMY_DisplaySky () - Displays AIR ENEMY SHIPS
 ***************************************************************************/
void 
ENEMY_DisplaySky(
    void
)
{
    int i;
    SPRITE_SHIP *spt;
    
    for (spt = first_enemy.next; &last_enemy != spt; spt = spt->next)
    {
        if (spt->groundflag)
            continue;
        
        GFX_PutSprite((char*)GLB_GetItem(spt->item), spt->x, spt->y);
        
        #ifdef __PPC__
        for (i = 0; i < spt->lib->numengs.get_value(); i++)
        {
            FLAME_Up(spt->x + spt->lib->engx[i].get_value(), spt->y + spt->lib->engy[i].get_value(), spt->lib->englx[i].get_value(), spt->eframe);
        }
        #else
        for (i = 0; i < spt->lib->numengs; i++)
        {
            FLAME_Up(spt->x + spt->lib->engx[i], spt->y + spt->lib->engy[i], spt->lib->englx[i], spt->eframe);
        }
        #endif
        
        spt->eframe ^= 1;
    }
}

/***************************************************************************
ENEMY_GetBaseDamage() - Gets Base Ship damage
 ***************************************************************************/
int 
ENEMY_GetBaseDamage(
    void
)
{
    static int nums;
    int total, damage;
    SPRITE_SHIP *spt;

    total = 0;
    nums = 0;
    
    for (spt = first_enemy.next; &last_enemy != spt; spt = spt->next)
    {
        #ifdef __PPC__
        if (!spt->lib->bossflag.get_value())
            continue;
        #else
        if (!spt->lib->bossflag)
            continue;
        #endif
        if (spt->y + spt->hly >= 0)
        {
            #ifdef __PPC__
            damage = (spt->hits * 100) / spt->lib->hits.get_value();
            #else
            damage = (spt->hits * 100) / spt->lib->hits;
            #endif
            total += damage;
            nums++;
        }
    }
    
    if (nums)
        total /= nums;
    
    return total;
}
