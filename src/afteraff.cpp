/**************************************************************************/
// act_comm.cpp - primarily code relating to player communications
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"
#include "afteraff.h"
#include "altform.h"

/**************************************************************************/
void afteraff_raging_fever(char_data *ch, char *)
{
	AFFECT_DATA *fever;

	if(IS_NPC(ch))
		return;

	fever = affect_find( ch->affected, gsn_raging_fever);
	if(number_percent() <= 80 || fever->modifier == 1)
	{
		if(IS_SET(race_table[ch->race]->flags, RACEFLAG_CAN_WEREBEAST ))
		{
			SET_BIT(ch->affected_by2, AFF2_LYCANTHROPE);
			SET_BIT(ch->affected_by2, AFF2_NO_VAMP);
			if(setup_werebeast(ch) == true)
			{
				ch->println("The fever leaves your body and something primal takes its place.");
				return;
			}
		}

	 }

	AFFECT_DATA af;

	af.where     = WHERE_AFFECTS;
	af.type      = gsn_weaken;
	af.level     = ch->level;
	af.duration  = ch->level / 2;
	af.location  = APPLY_ST;
	af.modifier  = -1 * (ch->level / 3);
	affect_to_char( ch, &af );
	af.modifier  = (ch->max_hit / 10) * -1;
	af.location  = APPLY_HIT;
	affect_to_char( ch, &af );
	ch->pcdata->tired = 50;
	ch->println("The fever burns itself out, taking the last of your strength with it.");
	return;
}
/**************************************************************************/
void afteraff_second_wind(char_data *ch, char *)
{
	AFFECT_DATA *af;
	ch->subdued = false;
	ch->subdued_timer=0;
	
	af = affect_find(ch->affected, gsn_second_wind);
	if(!IS_NPC(ch))
	{
		// tiredness
		ch->pcdata->tired=UMIN(0, ch->pcdata->tired);

		// conditions
		ch->pcdata->condition[COND_THIRST]=
			UMAX(30, ch->pcdata->condition[COND_THIRST]);
		ch->pcdata->condition[COND_HUNGER]=
			UMAX(30,ch->pcdata->condition[COND_HUNGER]);
		ch->pcdata->condition[COND_FULL]=
			UMIN(0,ch->pcdata->condition[COND_FULL]);
	    ch->pcdata->condition[COND_DRUNK]=
			UMIN(0,ch->pcdata->condition[COND_DRUNK]);
	}
	ch->hit 	= UMIN(ch->max_hit, ch->hit + af->level);
	ch->mana	= UMIN(ch->max_mana, ch->max_mana + af->level);
	ch->move	= UMIN(ch->max_move, ch->max_move + af->level);
	
	update_pos( ch);
	return;
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

