/**************************************************************************/
// bard.cpp - Spells/Skills & Bard Singing
/***************************************************************************
 * Whispers of Times Lost (c)1998-2005 Brad Wilson (ixliam@gmail.com)      *
 * >> If you use this code you must give credit in your help file as well  *
 *    as leaving this header intact.                                       *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses below. In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "include.h"
#include "magic.h"
#include "msp.h"
#include "song.h"

DECLARE_DO_FUN(do_bug		);
DECLARE_DO_FUN(do_wear		);

//DECLARE_SONG_FUN(sfun_bardic_peace);

/**************************************************************************/ 
SPRESULT spell_bardic_visions( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 1;
     ch->printlnf("You prepare to sing the song 'Bardic Visions'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_bardic_peace( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 2;
     ch->printlnf("You prepare to sing the song 'Bardic Peace'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_enhanced_healing( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 3;
     ch->printlnf("You prepare to sing the song 'Enhanced Healing'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_summon_light( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 4;
     ch->printlnf("You prepare to sing the song 'Summon Light'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_kill_magic( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 5;
     ch->printlnf("You prepare to sing the song 'Kill Magic'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_sober_drunks( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 6;
     ch->printlnf("You prepare to sing the song 'Sobriety'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_fog_of_truth( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 7;
     ch->printlnf("You prepare to sing the song 'Fog Of Truth'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_battle_rage( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 8;
     ch->printlnf("You prepare to sing the song 'Battle Rage'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_create_sustinance( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 9;
     ch->printlnf("You prepare to sing the song 'Create Sustinance'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_sleepytime( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 10;
     ch->printlnf("You prepare to sing the song 'Sleepytime'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_natures_howl( int sn, int level, char_data *ch, void *vo, int )
{
     if ( ch->pet != NULL )
     {
	ch->printlnf( "You already have a pet." );
	return NO_MANA;
     }
     if (is_affected(ch,gsn_natures_howl))
     {
	ch->println("You cannot sing for any more creatures now.");
	return HALF_MANA;
     }
     ch->songp = 0;
     ch->songn = 11;
     ch->printlnf("You prepare to sing the song 'Natures Howl'.");
     return FULL_MANA;
}
/**************************************************************************/ 
SPRESULT spell_dreamscape( int sn, int level, char_data *ch, void *vo, int )
{
     ch->songp = 0;
     ch->songn = 12;
     ch->printlnf("You prepare to sing the song 'Dreamscape'.");
     return FULL_MANA;
}
/**************************************************************************/
void song_update(void)
{
     char_data *ch;
     char_data *victim;
     AFFECT_DATA af;
     OBJ_DATA  *hold;
     char song1[MSL], song2[MSL];

     for ( ch = char_list; ch; ch = ch->next )
     {
		if ( ch->desc == NULL || ch->desc->connected_state != CON_PLAYING )
		   continue;

			//Inserted here only because songs are the right speed.
			//I'm too lazy\thrifty to create a whole new update for it.
			if(!IS_NPC(ch) && IS_WERETIGER(ch))
			{
				if(room_is_dark(ch->in_room))
				{	
					if(ch->catseye_timer > 0)
						ch->catseye_timer--;
					else;
						SET_WEREAFF(ch, WAFF_CATSEYE);
				}else{
					REMOVE_WEREAFF(ch, WAFF_CATSEYE);
					if(IS_WEREFORM(ch))
						ch->catseye_timer = UMIN(ch->catseye_timer + number_range(1, 3), 15);
					else
						ch->catseye_timer = UMIN(ch->catseye_timer + number_range(1, 5), 120-2*(13-ch->werecircle));
				}
			}

		if ( class_table[ch->clss].class_cast_type != CCT_BARD )
		   continue;

		if (ch->songn ==0)
		   continue;

		hold = get_eq_char(ch, WEAR_HOLD);

		if (hold == NULL || hold->item_type != ITEM_INSTRUMENT){
			ch->songp = 0;
			ch->songn = 0;
			ch->printlnf("You must be holding an instrument to accompany your singing.");
			continue;
		}
		if(hold->value[2] > 0)
		{

			sprintf( song1, "$p plays a few notes of '%s'`x",skill_table[hold->value[2]].name );
			sprintf( song2, "Your instrument plays '%s'`x", skill_table[hold->value[2]].name );
   			act(song1, ch, hold, NULL, TO_ROOM );
			act(song2, ch, hold, NULL, TO_CHAR); 
			hold->value[2]--;
			if(hold->value[3] == gsn_bardic_visions)
			{

     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
     	 			if ( IS_AFFECTED2( victim, AFF2_HALLUCINATE ) )
		 			continue;

					if ( victim == ch)
						continue;
 
     	 				if ( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell( ch->level, victim, DAM_MENTAL ))
		 				continue;

     	 				if (IS_SET(victim->in_room->room2_flags,ROOM2_ARENA))
		 				continue;

     	 				if (IS_SET(victim->in_room->room2_flags,ROOM2_WAR))
		 				continue;

     	 				af.where 	= WHERE_AFFECTS;
     	 				af.type 	= gsn_bardic_visions;
     	 				af.level	= ch->level;
     	 				af.duration  	= number_fuzzy( ch->level / 6 );
     	 				af.location  	= APPLY_IN;
     	 				af.modifier  	= -4;
     	 				af.bitvector 	= 0;
     	 				affect_to_char(victim,&af);
     	 				af.where 	= WHERE_AFFECTS2;
     	 				af.bitvector 	= AFF2_HALLUCINATE;
     	 				affect_to_char(victim,&af);

     	 				act("`#$N's body is `?su`?rr`?ou`?nd`?ed `?by `?da`?nc`?in`?g l`?ig`?hts`^.",ch,NULL,victim,TO_ROOM);
     	 				act("`#$N's body is `?su`?rr`?ou`?nd`?ed `?by `?da`?nc`?in`?g l`?ig`?hts`^.",ch,NULL,victim,TO_CHAR);
     	 				victim->printlnf("`#`W`?Thous`?ands `?of `?danc`?ing `?lig`?hts `?surr`?ound `?you.`^");
     					ch->songp = 0;
     					ch->songn = 0;
				} // End For
			}

			if(hold->value[3] == gsn_bardic_peace)
			{
				for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
					if ( IS_AFFECTED( victim, AFF_CALM )
					||   IS_AFFECTED( victim, AFF_BERSERK )
					||   is_affected( victim, gsn_bardic_peace ))
						continue;
					if (victim->fighting || victim->position == POS_FIGHTING )
						stop_fighting( victim, false );


					af.where = WHERE_AFFECTS;
					af.type = gsn_bardic_peace;
					af.level = ch->level;
					af.duration = 1;
					af.location = APPLY_HITROLL;

					if (!IS_NPC(victim))
						af.modifier = -5;
					else
						af.modifier = -2;

					af.bitvector = AFF_CALM;
					affect_to_char(victim,&af);
					af.location = APPLY_DAMROLL;
					affect_to_char(victim,&af);
				}
			}

			if(hold->value[3] == gsn_enhanced_healing)
			{
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
					if(IS_NPC(victim))
						continue;
					if ( is_affected( victim, gsn_enhanced_healing ))
						affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
					af.where = WHERE_AFFECTS;
					af.type = gsn_enhanced_healing;
					af.level = ch->level;
					af.duration = 1;
					af.location  = APPLY_NONE;
					af.modifier  = 0;
					af.bitvector = 0;
					affect_to_char(victim,&af);
				}
			}

			if(hold->value[3] == gsn_sober_drunks)
			{
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
					if(IS_NPC(victim))
					continue;
					victim->println("Your head feels clearer.");
					victim->pcdata->condition[COND_DRUNK] = 0;
				}

			}
		}

//S Bardic Visions 1
	if(ch->songn == 1)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_0)) {
				sprintf( song1, "$n sings '%s'`x",  ch->s1_0 );
				sprintf( song2, "You sing '%s'`x", ch->s1_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Be not dismayed by those who mock,'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Be not dismayed by those who mock,'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1:   
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_1)) {
				sprintf( song1, "$n sings '%s'`x", ch->s1_1 );
				sprintf( song2, "You sing '%s'`x", ch->s1_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else { 
     				act( "$n sings 'And all endeavors noble scorn.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'And all endeavors noble scorn.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_2)) {
				sprintf( song1, "$n sings '%s'`x", ch->s1_2 );
				sprintf( song2, "You sing '%s'`x", ch->s1_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'Abandon not thy honor's flock;'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'Abandon not thy honor's flock;'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_3)) {
				sprintf( song1, "$n sings '%s'`x", ch->s1_3 );
				sprintf( song2, "You sing '%s'`x", ch->s1_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'From lofty virtue be not torn.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'From lofty virtue be not torn.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_4)) {
				sprintf( song1, "$n sings '%s'`x", ch->s1_4 );
				sprintf( song2, "You sing '%s'`x", ch->s1_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'Seek grains of truth in every voice;'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'Seek grains of truth in every voice;'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_5)) {
				sprintf( song1, "$n sings '%s'`x", ch->s1_5 );
				sprintf( song2, "You sing '%s'`x", ch->s1_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'Be not thou fain to these eschew.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'Be not thou fain to these eschew.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 6:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_6)) {
				sprintf( song1, "$n sings '%s'`x", ch->s1_6 );
				sprintf( song2, "You sing '%s'`x", ch->s1_6 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'But when tumultuous thy choice,'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'But when tumultuous thy choice,'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 7:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s1_7)) {
				sprintf( song1, "$n sings '%s'`x", ch->s1_7 );
				sprintf( song2, "You sing '%s'`x", ch->s1_7 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'To thine own heart, thou must be true.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'To thine own heart, thou must be true.'", ch, NULL, NULL,TO_CHAR);
			}

     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
     	 			if ( IS_AFFECTED2( victim, AFF2_HALLUCINATE ) )
		 			continue;

				if ( victim == ch)
					continue;
     
     	 			if ( IS_AFFECTED( victim, AFF_BLIND ) || saves_spell( ch->level, victim, DAM_MENTAL ))
		 			continue;
 
     	 			if (IS_SET(victim->in_room->room2_flags,ROOM2_ARENA))
		 			continue;
 
     	 			if (IS_SET(victim->in_room->room2_flags,ROOM2_WAR))
		 			continue;

     	 			af.where 	= WHERE_AFFECTS;
     	 			af.type 	= gsn_bardic_visions;
     	 			af.level	= ch->level;
     	 			af.duration  	= number_fuzzy( ch->level / 6 );
     	 			af.location  	= APPLY_IN;
     	 			af.modifier  	= -4;
     	 			af.bitvector 	= 0;
     	 			affect_to_char(victim,&af);
     	 			af.where 	= WHERE_AFFECTS2;
     	 			af.bitvector 	= AFF2_HALLUCINATE;
     	 			affect_to_char(victim,&af);
 
     	 			act("`#$N's body is `?su`?rr`?ou`?nd`?ed `?by `?da`?nc`?in`?g l`?ig`?hts`^.",ch,NULL,victim,TO_ROOM);
     	 			act("`#$N's body is `?su`?rr`?ou`?nd`?ed `?by `?da`?nc`?in`?g l`?ig`?hts`^.",ch,NULL,victim,TO_CHAR);
     	 			victim->printlnf("`#`W`?Thous`?ands `?of `?danc`?ing `?lig`?hts `?surr`?ound `?you.`^");
     				ch->songp = 0;
     				ch->songn = 0;
				break;
			} // End For
		} //end cases
	} // end if
//S Bardic Peace 2
	else if(ch->songn == 2)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_0 );
				sprintf( song2, "You sing '%s'`x", ch->s2_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
	     			act( "$n sings 'The sky is the stage, with a storm all around; The audience helplessly waits on the ground.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'The sky is the stage, with a storm all around; The audience helplessly waits on the ground.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 1:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_1 );
				sprintf( song2, "You sing '%s'`x", ch->s2_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'The dragons above claim the sky as their own, and flame marks the path over which they have flown.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'The dragons above claim the sky as their own, and flame marks the path over which they have flown.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;

		case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_2 );
				sprintf( song2, "You sing '%s'`x", ch->s2_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Then up from below comes a thunderous cry; The paladin airborne appears in the sky!'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Then up from below comes a thunderous cry; The paladin airborne appears in the sky!'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;

		case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_3 );
				sprintf( song2, "You sing '%s'`x", ch->s2_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Each knight on his pegasus, lances at hand; To battle they ride, in a glorious stand.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Each knight on his pegasus, lances at hand; To battle they ride, in a glorious stand.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;

		case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_4 );
				sprintf( song2, "You sing '%s'`x", ch->s2_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Mere words can't describe the magnificent fight, as dragon and paladin battle this night.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Mere words can't describe the magnificent fight, as dragon and paladin battle this night.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;

		case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_5)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_5 );
				sprintf( song2, "You sing '%s'`x", ch->s2_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Raw courage and steel against talon and breath, as more than one hero earns honor in death.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Raw courage and steel against talon and breath, as more than one hero earns honor in death.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;

		case 6:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_6)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_6 );
				sprintf( song2, "You sing '%s'`x", ch->s2_6 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'The blood of both evil and good falls like rain, but when it is over, no dragons remain.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'The blood of both evil and good falls like rain, but when it is over, no dragons remain.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;

		case 7:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s2_7)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s2_7 );
				sprintf( song2, "You sing '%s'`x", ch->s2_7 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Perhaps but a dream, or a vision, and yet, those sharing this vision shall never forget.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Perhaps but a dream, or a vision, and yet, those sharing this vision shall never forget.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if ( IS_AFFECTED( victim, AFF_CALM )
				||   IS_AFFECTED( victim, AFF_BERSERK )
				||   is_affected( victim, gsn_bardic_peace ))
					continue;
				if (victim->fighting || victim->position == POS_FIGHTING )
					stop_fighting( victim, false );


				af.where = WHERE_AFFECTS;
				af.type = gsn_bardic_peace;
				af.level = ch->level;
				af.duration = 1;
				af.location = APPLY_HITROLL;

				if (!IS_NPC(victim))
					af.modifier = -5;
				else
					af.modifier = -2;

				af.bitvector = AFF_CALM;
				affect_to_char(victim,&af);
				af.location = APPLY_DAMROLL;
				affect_to_char(victim,&af);
			}
			ch->songp = 0;
     			ch->songn = 0;
			break;

		}
	}
//S Enhanced Healing 3
	else if(ch->songn == 3)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_0 );
				sprintf( song2, "You sing '%s'`x", ch->s3_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'You soon will be out facing danger and strife, you soon will be fighting and risking your life.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'You soon will be out facing danger and strife, you soon will be fighting and risking your life.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 1:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_1 );
				sprintf( song2, "You sing '%s'`x", ch->s3_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'I know there is use for my saber and knife; Alas, I will not be along.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'I know there is use for my saber and knife; Alas, I will not be along.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_2 );
				sprintf( song2, "You sing '%s'`x", ch->s3_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'We all have our limits, and I'm nearing mine; And so, with your pardon, I'll have to decline.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'We all have our limits, and I'm nearing mine; And so, with your pardon, I'll have to decline.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_3 );
				sprintf( song2, "You sing '%s'`x", ch->s3_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'For I've an appointment with women and wine, and gaming and music and song.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'For I've an appointment with women and wine, and gaming and music and song.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_4 );
				sprintf( song2, "You sing '%s'`x", ch->s3_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'It's been far too long since I've gambled and drank, too long in that dungeon, disgusting and dank.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'It's been far too long since I've gambled and drank, too long in that dungeon, disgusting and dank.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_5)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_5 );
				sprintf( song2, "You sing '%s'`x", ch->s3_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Too long amidst refuse, repulsive and rank, too long without love and romance.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Too long amidst refuse, repulsive and rank, too long without love and romance.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 6:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_6)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_6 );
				sprintf( song2, "You sing '%s'`x", ch->s3_6 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Before I embark on some hazardous quest, before once again I am put to the test.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Before I embark on some hazardous quest, before once again I am put to the test.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 7:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_7)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_7 );
				sprintf( song2, "You sing '%s'`x", ch->s3_7 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'If I don't take time to recover and rest, my sanity hasn't a chance.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'If I don't take time to recover and rest, my sanity hasn't a chance.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 8:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_8)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_8 );
				sprintf( song2, "You sing '%s'`x", ch->s3_8 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'It's not that your company isn't a thrill, I'm truly impressed with your courage and skill.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'It's not that your company isn't a thrill, I'm truly impressed with your courage and skill.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 9:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_9)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_9 );
				sprintf( song2, "You sing '%s'`x", ch->s3_9 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'And happy to journey beside you; but still, I need to be free for a time.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'And happy to journey beside you; but still, I need to be free for a time.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 10:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_10)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_10 );
				sprintf( song2, "You sing '%s'`x", ch->s3_10 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'When we meet again, we'll have stories to share, so off you go, good luck, and take care.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'When we meet again, we'll have stories to share, so off you go, good luck, and take care.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp++;
			break;
		case 11:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s3_11)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s3_11 );
				sprintf( song2, "You sing '%s'`x", ch->s3_11 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'May travel be pleasant, and fortune be fair, and may you find meter and rhyme.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'May travel be pleasant, and fortune be fair, and may you find meter and rhyme.'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
			    	if(IS_NPC(victim))
					continue;
				if ( is_affected( victim, gsn_enhanced_healing ))
					affect_parentspellfunc_strip( victim, gsn_enhanced_healing);
				af.where = WHERE_AFFECTS;
				af.type = gsn_enhanced_healing;
				af.level = ch->level;
				af.duration = 1;
				af.location  = APPLY_NONE;
				af.modifier  = 0;
				af.bitvector = 0;
				affect_to_char(victim,&af);
			}
			ch->songp = 0;
     			ch->songn = 0;
			break;
		}
	}
//S Summon Light 4
	else if(ch->songn == 4)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s4_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s4_0 );
				sprintf( song2, "You sing '%s'`x", ch->s4_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'A candle to light my way, she begs, a light to get the mother home.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'A candle to light my way, she begs, a light to get the mother home.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1: 
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s4_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s4_1 );
				sprintf( song2, "You sing '%s'`x", ch->s4_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {   
     				act( "$n sings 'She clutches her infant and cries again, can't you help get my babe safely home ?'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'She clutches her infant and cries again, can't you help get my babe safely home ?'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s4_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s4_2 );
				sprintf( song2, "You sing '%s'`x", ch->s4_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'She spies the the fire burning in the hearth, the dancing light setting the stones aglow.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'She spies the the fire burning in the hearth, the dancing light setting the stones aglow.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s4_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s4_3 );
				sprintf( song2, "You sing '%s'`x", ch->s4_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'She quickly steals a torch from the fire and carries her babe out into the night alone.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'She quickly steals a torch from the fire and carries her babe out into the night alone.'", ch, NULL, NULL,TO_CHAR);
			}
 			OBJ_DATA *light;
			light = create_object( get_obj_index( OBJ_VNUM_LIGHT_BALL ));
			obj_to_room( light, ch->in_room );
			act( "$n finishes $s song and $p appears.",   ch, light, NULL, TO_ROOM );
			act( "You finish your song and $p appears.", ch, light, NULL, TO_CHAR );
			ch->songp = 0;
     			ch->songn = 0;
			break;

		} //end cases
	} // end if
//S Kill Magic 5
	else if(ch->songn == 5)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s5_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s5_0 );
				sprintf( song2, "You sing '%s'`x", ch->s5_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Though heavy in heart and weary worn, yet in my breast is hope reborn.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Though heavy in heart and weary worn, yet in my breast is hope reborn.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1: 
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s5_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s5_1 );
				sprintf( song2, "You sing '%s'`x", ch->s5_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {   
     				act( "$n sings 'The glorious skies, the windswept plains, thy memory they cannot profane.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'The glorious skies, the windswept plains, thy memory they cannot profane.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s5_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s5_2 );
				sprintf( song2, "You sing '%s'`x", ch->s5_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'No more to fear, no more to roam, there is a path that leads me home.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'No more to fear, no more to roam, there is a path that leads me home.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s5_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s5_3 );
				sprintf( song2, "You sing '%s'`x", ch->s5_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'I lift mine eyes unto the sky, the road to Paradox is nigh.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'I lift mine eyes unto the sky, the road to Paradox is nigh.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s5_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s5_4 );
				sprintf( song2, "You sing '%s'`x", ch->s5_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'The road to Paradox is nigh.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'The road to Paradox is nigh.'", ch, NULL, NULL,TO_CHAR);
			}
			if ( IS_SET( ch->in_room->affected_by, ROOMAFF_UTTERDARK )) {
				ch->println( "The darkness around you disappears." );
				act( "The darkness around you disappears.", ch, NULL, NULL, TO_ROOM );
				REMOVE_BIT( ch->in_room->affected_by, ROOMAFF_UTTERDARK );
		
			}

     		for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     		{
 			check_dispel(ch->level,victim, gsn_regeneration );
			check_dispel(ch->level,victim, gsn_resist_poison );
			check_dispel(ch->level,victim, gsn_poison_immunity );
			check_dispel(ch->level,victim, gsn_prismatic_spray );
			check_dispel(ch->level,victim, gsn_illusions_grandeur );
			check_dispel(ch->level,victim, gsn_protection_fire );
			check_dispel(ch->level,victim, gsn_protection_cold );
			check_dispel(ch->level,victim, gsn_protection_lightning );
			check_dispel(ch->level,victim, gsn_holy_aura );
			check_dispel(ch->level,victim, gsn_true_sight );
			check_dispel(ch->level,victim, gsn_barkskin );
			check_dispel(ch->level,victim, gsn_magic_resistance );
			check_dispel(ch->level,victim, gsn_fire_shield );
			check_dispel(ch->level,victim, gsn_chill_shield );
			check_dispel(ch->level,victim, gsn_animal_essence);
    			check_dispel(ch->level,victim, gsn_armor );
			check_dispel(ch->level,victim, gsn_bless );
			check_dispel(ch->level,victim, gsn_hallucinate );
			check_dispel(ch->level,victim, gsn_bardic_visions );	
			check_dispel(ch->level,victim, gsn_despair);
			check_dispel(ch->level,victim, gsn_curse );
			check_dispel(ch->level,victim, gsn_detect_evil );
			check_dispel(ch->level,victim, gsn_detect_good );
    			check_dispel(ch->level,victim, gsn_detect_hidden );
			check_dispel(ch->level,victim, gsn_detect_invis );
			check_dispel(ch->level,victim, gsn_detect_magic );
			check_dispel(ch->level,victim, gsn_infravision );
			check_dispel(ch->level,victim, gsn_darkvision );
			check_dispel(ch->level,victim, gsn_pass_door );
			check_dispel(ch->level,victim, gsn_protection_evil );
			check_dispel(ch->level,victim, gsn_protection_good );
			check_dispel(ch->level,victim, gsn_rage );
			check_dispel(ch->level,victim, gsn_sleep );

    			if (check_dispel(ch->level, victim, gsn_polymorph ))
    			{
        			act("$n looks more like $mself again.",victim,NULL,NULL,TO_ROOM);
    			}
			if (check_dispel(ch->level,victim, gsn_blindness ))
			{
				act("$n is no longer blinded.",victim,NULL,NULL,TO_ROOM);
    			}
			if (check_dispel(ch->level,victim, gsn_calm ))
			{
				act("$n no longer looks so peaceful...",victim,NULL,NULL,TO_ROOM);
			}
    			if (check_dispel(ch->level,victim, gsn_change_sex ))
    			{
        			act("$n looks more like $mself again.",victim,NULL,NULL,TO_ROOM);
    			}
    			if (check_dispel(ch->level,victim, gsn_charm_person))
			{
				act("$n regains $s free will.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_chill_touch ))
			{
				act("$n looks warmer.",victim,NULL,NULL,TO_ROOM);
			}

			if (check_dispel(ch->level,victim, gsn_faerie_fire ))
			{
				act("$n's outline fades.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_fly ))
			{
				act("$n falls to the ground!",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_frenzy ))
			{
				act("$n no longer looks so wild.",victim,NULL,NULL,TO_ROOM);;
			}
			if (check_dispel(ch->level,victim, gsn_giant_strength ))
			{
				act("$n no longer looks so mighty.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_haste ))
			{
				act("$n is no longer moving so quickly.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_invisibility ))
			{
				act("$n fades into existance.",victim,NULL,NULL,TO_ROOM);
			}
    			if (check_dispel(ch->level,victim, gsn_mass_invis ))
    			{
				act("$n fades into existance.",victim,NULL,NULL,TO_ROOM);	
			}
			if (check_dispel(ch->level,victim, gsn_sanctuary ))
			{
				act("The white aura around $n's body vanishes.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_shield ))
			{
				act("The shield protecting $n vanishes.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_slow ))
			{
				act("$n is no longer moving so slowly.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_stone_skin ))
			{
				act("$n's skin regains its normal texture.",victim,NULL,NULL,TO_ROOM);
			}
			if (check_dispel(ch->level,victim, gsn_weaken ))
			{
				act("$n looks stronger.",victim,NULL,NULL,TO_ROOM);
			}
			if ( IS_AFFECTED2(victim, AFF2_VANISH))
			{
				affect_strip ( victim, gsn_vanish);
				REMOVE_BIT( victim->affected_by2, AFF2_VANISH );
				act( "A swirl of dust reveals $n.", victim, NULL, NULL, TO_ROOM );
				victim->println( "You have been revealed." );
			}
		} // end dispel for
			ch->songp = 0;
     			ch->songn = 0;
			break;

		} //end cases
	} // end if
//S Sober Drunks 6
	else if(ch->songn == 6)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s6_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s6_0 );
				sprintf( song2, "You sing '%s'`x", ch->s6_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Ale, ale, jolly good ale, oh pour it fine and free.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Ale, ale, jolly good ale, oh pour it fine and free.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s6_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s6_1 );
				sprintf( song2, "You sing '%s'`x", ch->s6_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'In a bucket or a pail, dipper up that jolly ale, aye, ale's the drink for me!'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'In a bucket or a pail, dipper up that jolly ale, aye, ale's the drink for me!'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s6_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s6_2 );
				sprintf( song2, "You sing '%s'`x", ch->s6_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Beer, beer, jolly good beer, oh pour it fine and free.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Beer, beer, jolly good beer, oh pour it fine and free.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s6_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s6_3 );
				sprintf( song2, "You sing '%s'`x", ch->s6_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Both far and near we'll sing of beer, aye, beer's the drink for me!'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Both far and near we'll sing of beer, aye, beer's the drink for me!'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s6_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s6_4 );
				sprintf( song2, "You sing '%s'`x", ch->s6_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Mead, mead, jolly good mead, fermented golden mead.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Mead, mead, jolly good mead, fermented golden mead.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s6_5)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s6_5 );
				sprintf( song2, "You sing '%s'`x", ch->s6_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Let's shout Huzzah for golden mead, aye, mead's the drink for me!'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Let's shout Huzzah for golden mead, aye, mead's the drink for me!'", ch, NULL, NULL,TO_CHAR);
			}
     			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (IS_NPC(victim))
				{
					act("$n looks extremely sober all of a sudden.",victim,NULL,ch,TO_CHAR);
				}
				else
				{
					victim->println("You feel suddenly more sober and awake.");
					act("$n looks more sober all of a sudden.",victim,NULL,NULL,TO_ROOM); 
					victim->pcdata->condition[COND_DRUNK] = 0;
				}
			}
			ch->songp = 0;
     			ch->songn = 0;
			break;
		} //end cases
	} // end if
//S Revealing spell 7
	else if(ch->songn == 7)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s7_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s7_0 );
				sprintf( song2, "You sing '%s'`x", ch->s7_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'The misty fog rolls in from off the shore.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'The misty fog rolls in from off the shore.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s7_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s7_1 );
				sprintf( song2, "You sing '%s'`x", ch->s7_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Children sit and watch the tendrils from their doors.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Children sit and watch the tendrils from their doors.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s7_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s7_2 );
				sprintf( song2, "You sing '%s'`x", ch->s7_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'The faeries dance in the misty air.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'The faeries dance in the misty air.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s7_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s7_3 );
				sprintf( song2, "You sing '%s'`x", ch->s7_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Their sparkling dust lights the area and shows their faces fair.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Their sparkling dust lights the area and shows their faces fair.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s7_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s7_4 );
				sprintf( song2, "You sing '%s'`x", ch->s7_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Magic is woven in the darkened swirls.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Magic is woven in the darkened swirls.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s7_5)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s7_5 );
				sprintf( song2, "You sing '%s'`x", ch->s7_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Mischief cannot be concealed by any boy or girl.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Mischief cannot be concealed by any boy or girl.'", ch, NULL, NULL,TO_CHAR);
			}
			for ( victim = ch->in_room->people; victim != NULL; victim = victim->next_in_room )
			{
				long tempflag;
				bool changed= false;

				if (INVIS_LEVEL(victim)> 0)
					continue;

				if ( victim == ch || saves_spell( ch->level, victim,DAMTYPE(gsn_fog_of_truth)) )
					continue;

				tempflag=victim->affected_by;

				if (is_affected( victim, gsn_invisibility ))
				{
					affect_strip ( victim, gsn_invisibility );
					changed = true;	
				}
				if (is_affected( victim, gsn_mass_invis))
				{
					affect_strip ( victim, gsn_mass_invis);
					changed = true;
				}
				if (is_affected( victim, gsn_sneak))
				{
					affect_strip ( victim, gsn_sneak);
					changed = true;
				}
		
				if (IS_AFFECTED2( victim, AFF2_VANISH))
				{
					affect_strip ( victim, gsn_vanish);
					changed = true;
				}

				REMOVE_BIT( victim->affected_by,	AFF_HIDE	);
				REMOVE_BIT( victim->affected_by,	AFF_INVISIBLE	);
				REMOVE_BIT( victim->affected_by,	AFF_SNEAK	);
				REMOVE_BIT( victim->affected_by2,	AFF2_VANISH 	);

				if (victim->affected_by!=tempflag || changed)
				{
					act( "$n is revealed!", victim, NULL, NULL, TO_ROOM );
					victim->printlnf( "You are revealed!" );
				}
			}

			ch->songp = 0;
     			ch->songn = 0;
			break;
		} //end cases
	} // end if
//S Battle Rage 8
	else if(ch->songn == 8)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_0 );
				sprintf( song2, "You sing '%s'`x", ch->s8_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Storm clouds are gathering above my citadel, and on the plain before me I can see.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Storm clouds are gathering above my citadel, and on the plain before me I can see.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 1:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_1 );
				sprintf( song2, "You sing '%s'`x", ch->s8_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Armies are clashing like a garage sale in hell, And I know what my destiny must be.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Armies are clashing like a garage sale in hell, And I know what my destiny must be.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_2 );
				sprintf( song2, "You sing '%s'`x", ch->s8_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'All my defences are cut and burned away, all my retreats are closed and gone.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'All my defences are cut and burned away, all my retreats are closed and gone.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_3 );
				sprintf( song2, "You sing '%s'`x", ch->s8_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'And all my hope has perished, this night will have no day, all I can do is battle on.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'And all my hope has perished, this night will have no day, all I can do is battle on.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_4 );
				sprintf( song2, "You sing '%s'`x", ch->s8_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me a sword, and a foeman I can face.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me a sword, and a foeman I can face.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_5)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_5 );
				sprintf( song2, "You sing '%s'`x", ch->s8_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me a sword, and someone I can slay.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me a sword, and someone I can slay.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 6:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_6)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_6 );
				sprintf( song2, "You sing '%s'`x", ch->s8_6 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me the chance, to take some of them with me.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me the chance, to take some of them with me.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 7:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_7)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_7 );
				sprintf( song2, "You sing '%s'`x", ch->s8_7 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'In this darkest hour I see no other way.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'In this darkest hour I see no other way.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 8:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_8)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_8 );
				sprintf( song2, "You sing '%s'`x", ch->s8_8 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'I am no fighter, I hate the thought of war. I've never struck a man nor borne a scar.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'I am no fighter, I hate the thought of war. I've never struck a man nor borne a scar.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 9:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_9)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_9 );
				sprintf( song2, "You sing '%s'`x", ch->s8_9 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'I am a poet, that's what my life was for, but even poets can be pushed too far.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'I am a poet, that's what my life was for, but even poets can be pushed too far.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 10:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_10)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_10 );
				sprintf( song2, "You sing '%s'`x", ch->s8_10 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Once I believed that a man could choose his course, to live in peace and sing a peaceful song.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Once I believed that a man could choose his course, to live in peace and sing a peaceful song.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 11:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_11)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_11 );
				sprintf( song2, "You sing '%s'`x", ch->s8_11 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'But now my innocence is swallowed in remorse, my dreams brought pain and heartache: I was wrong.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'But now my innocence is swallowed in remorse, my dreams brought pain and heartache: I was wrong.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 12:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_12)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_12 );
				sprintf( song2, "You sing '%s'`x", ch->s8_12 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me a sword, and a foeman I can face.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me a sword, and a foeman I can face.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 13:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_13)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_13 );
				sprintf( song2, "You sing '%s'`x", ch->s8_13 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me a sword, and someone I can slay.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me a sword, and someone I can slay.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 14:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_14)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_14 );
				sprintf( song2, "You sing '%s'`x", ch->s8_14 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me the chance, to take some of them with me.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me the chance, to take some of them with me.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 15:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_15)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_15 );
				sprintf( song2, "You sing '%s'`x", ch->s8_15 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'In this darkest hour I see no other way.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'In this darkest hour I see no other way.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 16:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_16)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_16 );
				sprintf( song2, "You sing '%s'`x", ch->s8_16 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'The storm is breaking, here comes the hardest rain, and I must go and face my final fight.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'The storm is breaking, here comes the hardest rain, and I must go and face my final fight.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 17:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_17)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_17 );
				sprintf( song2, "You sing '%s'`x", ch->s8_17 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Look for my body out on the darkening plain, where ignorant armies clash by endless night.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Look for my body out on the darkening plain, where ignorant armies clash by endless night.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 18:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_18)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_18 );
				sprintf( song2, "You sing '%s'`x", ch->s8_18 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me a sword, and a foeman I can face.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me a sword, and a foeman I can face.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 19:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_19)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_19 );
				sprintf( song2, "You sing '%s'`x", ch->s8_19 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me a sword, and someone I can slay.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me a sword, and someone I can slay.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 20:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_20)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_20 );
				sprintf( song2, "You sing '%s'`x", ch->s8_20 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Give me the chance, to take some of them with me.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Give me the chance, to take some of them with me.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			break;
		    case 21:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s8_21)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s8_21 );
				sprintf( song2, "You sing '%s'`x", ch->s8_21 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'In this darkest hour I see no other way.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'In this darkest hour I see no other way.'", ch, NULL, NULL,TO_CHAR);
			}
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if (is_affected(victim,gsn_calm))
					continue;
				if (IS_NPC(victim))
					continue;
				if (!is_affected(victim, gsn_battle_rage))
				{
					if (!IS_NPC(victim)) {
						victim->printlnf( "The song fills you with battle rage!" );
					}
					act("$n gets a wild look in $s eyes!",victim,NULL,NULL,TO_ROOM);
				}
				affect_strip ( victim, gsn_battle_rage);

				af.where     = WHERE_AFFECTS;
				af.type 	 = gsn_battle_rage;
				af.level	 = ch->level;
				af.duration	 = 1;
				af.modifier  = ch->level / 6;
				af.bitvector = 0;
				af.location  = APPLY_HITROLL;
				affect_to_char(victim,&af);
				af.location  = APPLY_DAMROLL;
				affect_to_char(victim,&af);
				af.modifier  = 10 * (ch->level / 12);
				af.location  = APPLY_AC;
				affect_to_char(victim,&af);
			}
			ch->songp = 0;
     			ch->songn = 0;
			break;
		} //end cases
	} // end if
//S Create Sustinencew 9
	else if(ch->songn == 9)
	{
		OBJ_DATA *fooditem;

		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_0 );
				sprintf( song2, "You sing '%s'`x", ch->s9_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Oh, fill up the tankard, lad, and send it round to me.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Oh, fill up the tankard, lad, and send it round to me.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1:  
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_1 );
				sprintf( song2, "You sing '%s'`x", ch->s9_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {  
     				act( "$n sings 'I'd rather sit here drinking with my friends so cheerfully.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'I'd rather sit here drinking with my friends so cheerfully.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_2 );
				sprintf( song2, "You sing '%s'`x", ch->s9_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'And if perchance you catch my eye, then I can get to know you.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'And if perchance you catch my eye, then I can get to know you.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:  
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_3 );
				sprintf( song2, "You sing '%s'`x", ch->s9_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {  
     				act( "$n sings 'For otherwise then all I have is an empty bed to go to.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'For otherwise then all I have is an empty bed to go to.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			fooditem = create_object( get_obj_index( 1619 ));
			obj_to_room( fooditem, ch->in_room );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_ROOM );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_CHAR );
			break;
		    case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_4 );
				sprintf( song2, "You sing '%s'`x", ch->s9_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
      				act( "$n sings 'An Intori doesn't smell so bad when you have got a cold.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'An Intori doesn't smell so bad when you have got a cold.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_5)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_5 );
				sprintf( song2, "You sing '%s'`x", ch->s9_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
				if(ch->sex == 1)
				{
       					act( "$n sings 'And judging by the dirt upon her shirt she's not to old.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'And judging by the dirt upon her shirt she's not to old.'", ch, NULL, NULL,TO_CHAR);
				} else
				{
       					act( "$n sings 'And judging by the dirt upon his shirt he's not to old.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'And judging by the dirt upon his shirt he's not to old.'", ch, NULL, NULL,TO_CHAR);
				}
			}
			ch->songp++;
			break;
		    case 6:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_6)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_6 );
				sprintf( song2, "You sing '%s'`x", ch->s9_6 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
				if(ch->sex == 1)
				{
					act( "$n sings 'She will always cuddle up when winter nights are cold.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'She will always cuddle up when winter nights are cold.'", ch, NULL, NULL,TO_CHAR);
				}else
				{
					act( "$n sings 'He will always cuddle up when winter nights are cold.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'He will always cuddle up when winter nights are cold.'", ch, NULL, NULL,TO_CHAR);
				}
			}
			ch->songp++;
			break;
		    case 7:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_7)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_7 );
				sprintf( song2, "You sing '%s'`x", ch->s9_7 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'For its only in the summer that an Intori likes to roam.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'For its only in the summer that an Intori likes to roam.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			fooditem = create_object( get_obj_index( 803 ));
			fooditem->value[0] = number_fuzzy(ch->level);
			fooditem->value[1] = number_fuzzy(ch->level)*2;
			fooditem->timer = (ch->level*2)+10;
			obj_to_room( fooditem, ch->in_room );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_ROOM );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_CHAR );
			break;
		    case 8:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_8)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_8 );
				sprintf( song2, "You sing '%s'`x", ch->s9_8 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'The fiercest fighters are the Cthuls, or so I have been told.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'The fiercest fighters are the Cthuls, or so I have been told.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 9:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_9)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_9 );
				sprintf( song2, "You sing '%s'`x", ch->s9_9 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'For when they go to battle they are always wearing woad.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'For when they go to battle they are always wearing woad.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 10:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_10)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_10 );
				sprintf( song2, "You sing '%s'`x", ch->s9_10 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'So if you see a Cthul, you know his body's painted blue.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'So if you see a Cthul, you know his body's painted blue.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 11:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_11)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_11 );
				sprintf( song2, "You sing '%s'`x", ch->s9_11 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'And if you sleep with one you're sure to get some on you.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'And if you sleep with one you're sure to get some on you.'", ch, NULL, NULL,TO_CHAR);
			}
			fooditem = create_object( get_obj_index( 208 ));
			fooditem->value[0] = number_fuzzy(ch->level);
			fooditem->value[1] = number_fuzzy(ch->level)*2;
			fooditem->timer = (ch->level*2)+10;
			obj_to_room( fooditem, ch->in_room );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_ROOM );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_CHAR );
			ch->songp++;
			break;
		    case 12:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_12)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_12 );
				sprintf( song2, "You sing '%s'`x", ch->s9_12 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'The Markians travel round a lot and take whate'er they see.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'The Markians travel round a lot and take whate'er they see.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 13:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_13)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_13 );
				sprintf( song2, "You sing '%s'`x", ch->s9_13 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
				if(ch->sex == 1)
				{
	       				act( "$n sings 'If there's a Markian here she's not the girl for me.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'If there's a Markian here she's not the girl for me.'", ch, NULL, NULL,TO_CHAR);
				} else
				{
	       				act( "$n sings 'If there's a Markian here he's not the man for me.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'If there's a Markian here he's not the man for me.'", ch, NULL, NULL,TO_CHAR);
				}
			}
			ch->songp++;
			break;
		    case 14:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_14)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_14 );
				sprintf( song2, "You sing '%s'`x", ch->s9_14 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'They always stay up late at night and never seem to sleep.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'They always stay up late at night and never seem to sleep.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 15:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_15)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_15 );
				sprintf( song2, "You sing '%s'`x", ch->s9_15 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
				if(ch->sex == 1)
				{
       					act( "$n sings 'Your virgin boys are very safe but you'd better watch your sheep.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'Your virgin boys are very safe but you'd better watch your sheep.'", ch, NULL, NULL,TO_CHAR);
				} else
				{
       					act( "$n sings 'Your virgin girls are very safe but you'd better watch your sheep.'", ch, NULL, NULL,TO_ROOM);
      					act( "You sing 'Your virgin girls are very safe but you'd better watch your sheep.'", ch, NULL, NULL,TO_CHAR);
				}	
			}
			fooditem = create_object( get_obj_index( 1619 ));
			obj_to_room( fooditem, ch->in_room );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_ROOM );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_CHAR );
			ch->songp++;
			break;
		    case 16:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_16)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_16 );
				sprintf( song2, "You sing '%s'`x", ch->s9_16 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'Fill up the tankard lad and send it round to me.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'Fill up the tankard lad and send it round to me.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 17:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_17)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_17 );
				sprintf( song2, "You sing '%s'`x", ch->s9_17 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'I'd rather sit here drinking with my friends so cheerfully.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'I'd rather sit here drinking with my friends so cheerfully.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 18:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_18)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_18 );
				sprintf( song2, "You sing '%s'`x", ch->s9_18 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'And if perchance you catch my eye, then I can get to know you.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'And if perchance you catch my eye, then I can get to know you.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 19:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s9_19)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s9_19 );
				sprintf( song2, "You sing '%s'`x", ch->s9_19 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
       				act( "$n sings 'For otherwise then all I have is an empty bed to go to.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'For otherwise then all I have is an empty bed to go to.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp = 0;
     			ch->songn = 0;
			fooditem = create_object( get_obj_index( 27070 ));
			obj_to_room( fooditem, ch->in_room );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_ROOM );
			act( "$p suddenly appears.", ch, fooditem, NULL, TO_CHAR );
			break;
		} //end cases
	} // end if
//S Sleepyland 10
 	else if(ch->songn == 10)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s10_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s10_0 );
				sprintf( song2, "You sing '%s'`x", ch->s10_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Rock-a-bye baby, in the treetop.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Rock-a-bye baby, in the treetop.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1: 
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s10_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s10_1 );
				sprintf( song2, "You sing '%s'`x", ch->s10_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {   
     				act( "$n sings 'When the wind blows, the cradle will rock.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'When the wind blows, the cradle will rock.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2: 
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s10_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s10_2 );
				sprintf( song2, "You sing '%s'`x", ch->s10_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {   
     				act( "$n sings 'When the bow breaks, the cradle will fall.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'When the bow breaks, the cradle will fall.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:    
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s10_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s10_3 );
				sprintf( song2, "You sing '%s'`x", ch->s10_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'And down will come baby, cradle and all.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'And down will come baby, cradle and all.'", ch, NULL, NULL,TO_CHAR);
			}
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if(IS_NPC(victim))
					continue;
				if(victim->is_trying_sleep)
				{
					victim->println("You drift off into dreamland.");
					victim->position=POS_SLEEPING;
					act( "$n goes quietly to sleep.", victim, NULL, NULL, TO_ROOM );
				}
			}
			ch->songp = 0;
     			ch->songn = 0;
			break;
		} //end cases
	} // end if
//S Nature's Howl 11
	else if(ch->songn == 11)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_0 );
				sprintf( song2, "You sing '%s'`x", ch->s11_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'I am stretched on your grave, and will lie there forever.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'I am stretched on your grave, and will lie there forever.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 1:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_1 );
				sprintf( song2, "You sing '%s'`x", ch->s11_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'If your hands were in mine, I'd be sure they'd not sever.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'If your hands were in mine, I'd be sure they'd not sever.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 2:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_2 );
				sprintf( song2, "You sing '%s'`x", ch->s11_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'My appletree, my brightness, 'Tis time we were together.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'My appletree, my brightness, 'Tis time we were together.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 3:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_3 );
				sprintf( song2, "You sing '%s'`x", ch->s11_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'For I smell of the earth, and am worn by the weather.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'For I smell of the earth, and am worn by the weather.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 4:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_4)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_4 );
				sprintf( song2, "You sing '%s'`x", ch->s11_4 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'When my family thinks, that I'm safe in my bed.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'When my family thinks, that I'm safe in my bed.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 5:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_5)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_5 );
				sprintf( song2, "You sing '%s'`x", ch->s11_5 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'From night until morning, I am stretched at your head.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'From night until morning, I am stretched at your head.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 6:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_6)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_6 );
				sprintf( song2, "You sing '%s'`x", ch->s11_6 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Calling out to the air, with tears both hot and wild.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Calling out to the air, with tears both hot and wild.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 7:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_7)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_7 );
				sprintf( song2, "You sing '%s'`x", ch->s11_7 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
				if(ch->sex == 1)
				{
	     				act( "$n sings 'My grief for the girl, that I loved as a child.'", ch, NULL, NULL,TO_ROOM);
     					act( "You sing 'My grief for the girl, that I loved as a child.'", ch, NULL, NULL,TO_CHAR);
				} else 
				if(ch->sex == 2)
				{
     					act( "$n sings 'My grief for the boy, that I loved as a child.'", ch, NULL, NULL,TO_ROOM);
     					act( "You sing 'My grief for the boy, that I loved as a child.'", ch, NULL, NULL,TO_CHAR);
				}else 
				{
     					act( "$n sings 'My grief for the one, that I loved as a child.'", ch, NULL, NULL,TO_ROOM);
     					act( "You sing 'My grief for the one, that I loved as a child.'", ch, NULL, NULL,TO_CHAR);
				}
			}
			ch->songp++;
			break;
		    case 8:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_8)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_8 );
				sprintf( song2, "You sing '%s'`x", ch->s11_8 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'The priests and the friars, approach me in dread.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'The priests and the friars, approach me in dread.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 9:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_9)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_9 );
				sprintf( song2, "You sing '%s'`x", ch->s11_9 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Because I still love you, my love, and your dead.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Because I still love you, my love, and your dead.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 10:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_10)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_10 );
				sprintf( song2, "You sing '%s'`x", ch->s11_10 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'And still would be your shelter, through rain and through storm.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'And still would be your shelter, through rain and through storm.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			break;
		    case 11:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s11_11)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s11_11 );
				sprintf( song2, "You sing '%s'`x", ch->s11_11 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'For with you in the cold ground, I cannot sleep warm.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'For with you in the cold ground, I cannot sleep warm.'", ch, NULL, NULL,TO_CHAR);
			}
    			if (number_percent() > 75)
    			{
				ch->println("No animals hear your song.");
				ch->songp = 0;
     				ch->songn = 0;
				break;
    			}

    	    		char_data *animal;
	    		char buf[MSL];
			int anum = 0;

			if(ch->in_room->sector_type == SECT_CITY)
				anum = 15714; // War Dog
			if(ch->in_room->sector_type == SECT_FIELD)
				anum = 13002; // Horse
			if(ch->in_room->sector_type == SECT_FOREST)
				anum = 52021; // Deer
			if(ch->in_room->sector_type == SECT_HILLS)
				anum = 3033; // Wolf
			if(ch->in_room->sector_type == SECT_MOUNTAIN)
				anum = 3040; // Mountain Lion
			if(ch->in_room->sector_type == SECT_TRAIL)
				anum = 52006; // Monkey
			if(ch->in_room->sector_type == SECT_SWAMP)
				anum = 29801; // Giant Frog
			if(ch->in_room->sector_type == SECT_DESERT)
				anum = 10104; // King Cobra Snake
			if(ch->in_room->sector_type == SECT_SNOW || ch->in_room->sector_type == SECT_ICE)
				anum = 3413; // Yeti
			if(ch->in_room->sector_type == SECT_AIR)
				anum = 3037; // Eagle
			if(ch->in_room->sector_type == SECT_CAVE)
				anum = 55; // Bear
			
    			if ( anum == 0)
    			{
   				ch->println("You realize this doesn't look like a good place to attract a follower.");
 				ch->songp = 0;
     				ch->songn = 0;
				break;
    			}

	    		animal = create_mobile(get_mob_index(anum), 0 );
	    		SET_BIT(animal->act, ACT_PET);
 	    		SET_BIT(animal->affected_by, AFF_CHARM);
	    		animal->comm = COMM_NOTELL|COMM_NOSHOUT|COMM_NOCHANNELS;
	    		animal->level = ch->level;
	    		animal->max_hit = ch->max_hit + dice(ch->level, 5);
	    		animal->damroll += ch->level*3/4;
	    		animal->hitroll += ch->level/3;
	    		animal->alliance = ch->alliance;

	    		sprintf( buf, "%sA neck tag says 'I belong to %s'.\r\n", animal->description, ch->short_descr );
	    		free_string( animal->description );
	    		animal->description = str_dup( buf );
	    		char_to_room(animal,ch->in_room);
	    		add_follower(animal,ch);
	    		animal->leader = ch;
	    		ch->pet = animal;
	    		act("$n arrives to journey with $N.",animal,NULL,ch,TO_NOTVICT);
	    		animal->hit = animal->max_hit;

	    		af.where = WHERE_AFFECTS;
	    		af.level = ch->level;
	    		af.duration = 48;
			af.location  = APPLY_NONE;
			af.modifier  = 0;
	    		af.bitvector = 0;
	  	  	af.type = gsn_natures_howl;
		    	affect_to_char(ch,&af);
			ch->songp = 0;
     			ch->songn = 0;
			break;
		} //end cases
	} // end if	
//S Dreamland 12	
	else if(ch->songn == 12)
	{
		switch(ch->songp)
		{
		    case 0:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s12_0)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s12_0 );
				sprintf( song2, "You sing '%s'`x", ch->s12_0 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'Rock-a-bye baby, in the treetop.'", ch, NULL, NULL,TO_ROOM);
     				act( "You sing 'Rock-a-bye baby, in the treetop.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if(IS_NPC(victim))
					continue;
				if(ch == victim)
					continue;
				victim->println("You feel slightly sleepy.");
				victim->pcdata->tired=10;
			}
			break;
		    case 1:
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s12_1)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s12_1 );
				sprintf( song2, "You sing '%s'`x", ch->s12_1 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {    
     				act( "$n sings 'When the wind blows, the cradle will rock.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'When the wind blows, the cradle will rock.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if(IS_NPC(victim))
					continue;
				if(ch == victim)
					continue;
				victim->pcdata->tired=15;
			}
			break;
		    case 2:    
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s12_2)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s12_2 );
				sprintf( song2, "You sing '%s'`x", ch->s12_2 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'When the bow breaks, the cradle will fall.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'When the bow breaks, the cradle will fall.'", ch, NULL, NULL,TO_CHAR);
			}
			ch->songp++;
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if(IS_NPC(victim))
					continue;
				if(ch == victim)
					continue;
				if(IS_IMMORTAL(victim))
					continue;
					victim->println("You feel sleepy.");
					victim->pcdata->tired=20;
			}
			break;
		    case 3:    
			if (!HAS_CONFIG2(ch, CONFIG2_NOSONGMODS) && !IS_NULLSTR(ch->s12_3)) 
			{
				sprintf( song1, "$n sings '%s'`x", ch->s12_3 );
				sprintf( song2, "You sing '%s'`x", ch->s12_3 );
   				act(song1, ch, NULL, NULL, TO_ROOM );
				act(song2, ch, NULL, ch, TO_CHAR);
			} else {
     				act( "$n sings 'And down will come baby, cradle and all.'", ch, NULL, NULL,TO_ROOM);
      				act( "You sing 'And down will come baby, cradle and all.'", ch, NULL, NULL,TO_CHAR);
			}
			for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     			{
				if(IS_NPC(victim))
					continue;
				if(ch == victim)
					continue;
				if(IS_IMMORTAL(victim))
					continue;
				victim->println("You drift off into dreamland.");
				victim->pcdata->tired=40;
				victim->position=POS_SLEEPING;
				act( "$n goes quietly to sleep.", victim, NULL, NULL, TO_ROOM );
			}
			ch->songp = 0;
     			ch->songn = 0;
			break;
		} //end cases
	} // end if
     return;
}
}
/**************************************************************************/
void do_songcraft( char_data *ch, char *argument )
{
	char songnum[MIL];
	char songline[MIL];

	argument = one_argument(argument, songnum);
	argument = one_argument(argument, songline);

	if(IS_NPC(ch))
	{
 		do_huh(ch,"");
		return;
	}

	if ( class_table[ch->clss].class_cast_type != CCT_BARD )
	{
 		do_huh(ch,"");
		return;
	}

    	if ( HAS_CONFIG2(ch, CONFIG2_NOSONGMODS))
    	{
 		ch->println("You are not allowed to modify songs.");
		return;
	}
	
	if(IS_NULLSTR(songnum))
	{
        	ch->println("What song are you wanting to compose ?");
        	ch->println("`#`BSyntax:`w songcraft <song num> <song line> <words>`^");
		return;
	}

	if(IS_NULLSTR(songline))
	{
        	ch->println("What song line are you wanting to compose ?");
        	ch->println("`#`BSyntax:`w songcraft <song num> <song line> <words>`^");
		return;
	}

	if(IS_NULLSTR(argument))
	{
        	ch->println("You cannot compose a song with no verse !");
        	ch->println("`#`BSyntax:`w songcraft <song num> <song line> <words>`^");
		return;
	}

	int snum = 0;
	snum = atoi(songnum);
	int sline = 0;
	sline = atoi(songline);

	if(snum < 0 || snum > 11)
	{
        	ch->println("Please choose a song between 1 and 11.");
        	return;
	}

	if(snum == 1)
	{		
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_5 = str_dup( capitalize(argument));
				break;
			case 6:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_6 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_6 = str_dup( capitalize(argument));
				break;
			case 7:
				if( !str_prefix( argument, "-"))
				{
					ch->s1_7 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s1_7 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 2)
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_5 = str_dup( capitalize(argument));
				break;
			case 6:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_6 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_6 = str_dup( capitalize(argument));
				break;
			case 7:
				if( !str_prefix( argument, "-"))
				{
					ch->s2_7 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s2_7 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 3) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_5 = str_dup( capitalize(argument));
				break;
			case 6:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_6 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_6 = str_dup( capitalize(argument));
				break;
			case 7:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_7 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_7 = str_dup( capitalize(argument));
				break;
			case 8:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_8 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_8 = str_dup( capitalize(argument));
				break;
			case 9:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_9 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_9 = str_dup( capitalize(argument));
				break;
			case 10:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_10 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_10 = str_dup( capitalize(argument));
				break;
			case 11:
				if( !str_prefix( argument, "-"))
				{
					ch->s3_11 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s3_11 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 4) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s4_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s4_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s4_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s4_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s4_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s4_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s4_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s4_3 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 5) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s5_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s5_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s5_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s5_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s5_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s5_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s5_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s5_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s5_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s5_4 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 6) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s6_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s6_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s6_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s6_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s6_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s6_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s6_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s6_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s6_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s6_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s6_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s6_5 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 7) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s7_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s7_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s7_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s7_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s7_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s7_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s7_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s7_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s7_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s7_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s7_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s7_5 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 8) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_5 = str_dup( capitalize(argument));
				break;
			case 6:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_6 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_6 = str_dup( capitalize(argument));
				break;
			case 7:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_7 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_7 = str_dup( capitalize(argument));
				break;
			case 8:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_8 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_8 = str_dup( capitalize(argument));
				break;
			case 9:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_9 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_9 = str_dup( capitalize(argument));
				break;
			case 10:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_10 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_10 = str_dup( capitalize(argument));
				break;
			case 11:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_11 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_11 = str_dup( capitalize(argument));
				break;
			case 12:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_12 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_12 = str_dup( capitalize(argument));
				break;
			case 13:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_13 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_13 = str_dup( capitalize(argument));
				break;
			case 14:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_14 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_14 = str_dup( capitalize(argument));
				break;
			case 15:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_15 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_15 = str_dup( capitalize(argument));
				break;
			case 16:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_16 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_16 = str_dup( capitalize(argument));
				break;
			case 17:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_17 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_17 = str_dup( capitalize(argument));
				break;
			case 18:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_18 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_18 = str_dup( capitalize(argument));
				break;
			case 19:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_19 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_19 = str_dup( capitalize(argument));
				break;
			case 20:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_20 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_20 = str_dup( capitalize(argument));
				break;
			case 21:
				if( !str_prefix( argument, "-"))
				{
					ch->s8_21 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s8_21 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 9) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_5 = str_dup( capitalize(argument));
				break;
			case 6:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_6 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_6 = str_dup( capitalize(argument));
				break;
			case 7:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_7 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_7 = str_dup( capitalize(argument));
				break;
			case 8:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_8 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_8 = str_dup( capitalize(argument));
				break;
			case 9:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_9 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_9 = str_dup( capitalize(argument));
				break;
			case 10:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_10 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_10 = str_dup( capitalize(argument));
				break;
			case 11:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_11 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_11 = str_dup( capitalize(argument));
				break;
			case 12:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_12 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_12 = str_dup( capitalize(argument));
				break;
			case 13:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_13 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_13 = str_dup( capitalize(argument));
				break;
			case 14:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_14 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_14 = str_dup( capitalize(argument));
				break;
			case 15:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_15 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_15 = str_dup( capitalize(argument));
				break;
			case 16:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_16 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_16 = str_dup( capitalize(argument));
				break;
			case 17:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_17 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_17 = str_dup( capitalize(argument));
				break;
			case 18:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_18 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_18 = str_dup( capitalize(argument));
				break;
			case 19:
				if( !str_prefix( argument, "-"))
				{
					ch->s9_19 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s9_19 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 10) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s10_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s10_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s10_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s10_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s10_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s10_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s10_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s10_3 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	} else if(snum == 11) 
	{
		switch(sline)
		{
			case 0:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_0 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_0 = str_dup( capitalize(argument));
				break;
			case 1:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_1 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_1 = str_dup( capitalize(argument));
				break;
			case 2:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_2 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_2 = str_dup( capitalize(argument));
				break;
			case 3:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_3 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_3 = str_dup( capitalize(argument));
				break;
			case 4:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_4 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_4 = str_dup( capitalize(argument));
				break;
			case 5:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_5 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_5 = str_dup( capitalize(argument));
				break;
			case 6:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_6 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_6 = str_dup( capitalize(argument));
				break;
			case 7:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_7 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_7 = str_dup( capitalize(argument));
				break;
			case 8:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_8 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_8 = str_dup( capitalize(argument));
				break;
			case 9:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_9 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_9 = str_dup( capitalize(argument));
				break;
			case 10:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_10 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_10 = str_dup( capitalize(argument));
				break;
			case 11:
				if( !str_prefix( argument, "-"))
				{
					ch->s11_11 = NULL;
		        		ch->println("Line Cleared.");
				} else
				ch->s11_11 = str_dup( capitalize(argument));
				break;
			default:
		        	ch->println("Invalid Line.");
				break;
		}
		ch->printlnf("Ok.");
		return;
	}

 	ch->println("Invalid Line.");
	return;
}
/**************************************************************************/
void do_songrecite( char_data *ch, char *argument )
{

    	if ( HAS_CONFIG2(ch, CONFIG2_NOSONGMODS))
    	{
 		ch->println("You are not allowed to modify songs.");
		return;
	}

	if(IS_NPC(ch))
	{
 		do_huh(ch,"");
		return;
	}

	if ( class_table[ch->clss].class_cast_type != CCT_BARD )
	{
 		do_huh(ch,"");
		return;
	}
	
	if(IS_NULLSTR(argument))
	{
        	ch->println("What song are you wanting to recite ?");
        	ch->println("`#`BSyntax:`w songrecite <song num>`^");
		return;
	}


	int snum = 0;
	snum = atoi(argument);

	if(snum < 0 || snum > 11)
	{
        	ch->println("Please choose a song between 1 and 11.");
        	return;
	}

	switch(snum)
   	{
   		case 1:
			ch->printlnf("Song 1: Bardic Visions");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s1_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s1_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s1_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s1_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s1_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s1_5);
			ch->printf("Line 6: ");
			ch->printlnf(ch->s1_6);
			ch->printf("Line 7: ");
			ch->printlnf(ch->s1_7);
			break;
		case 2:
			ch->printlnf("Song 1: Bardic Peace");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s2_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s2_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s2_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s2_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s2_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s2_5);
			ch->printf("Line 6: ");
			ch->printlnf(ch->s2_6);
			ch->printf("Line 7: ");
			ch->printlnf(ch->s2_7);
			break;
		case 3:
			ch->printlnf("Song 3: Enhanced Visions");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s3_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s3_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s3_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s3_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s3_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s3_5);
			ch->printf("Line 6: ");
			ch->printlnf(ch->s3_6);
			ch->printf("Line 7: ");
			ch->printlnf(ch->s3_7);
			ch->printf("Line 8: ");
			ch->printlnf(ch->s3_8);
			ch->printf("Line 9: ");
			ch->printlnf(ch->s3_9);
			ch->printf("Line 10: ");
			ch->printlnf(ch->s3_10);
			ch->printf("Line 11: ");
			ch->printlnf(ch->s3_11);
			break;
		case 4:
			ch->printlnf("Song 4: Summon Light");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s4_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s4_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s4_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s4_3);
			break;
		case 5:
			ch->printlnf("Song 5: Kill Magic");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s5_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s5_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s5_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s5_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s5_4);
			break;
		case 6:
			ch->printlnf("Song 6: Sobriety");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s6_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s6_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s6_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s6_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s6_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s6_5);
			break;
		case 7:
			ch->printlnf("Song 7: Fog of Truth");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s7_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s7_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s7_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s7_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s7_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s7_5);
			break;
		case 8:
			ch->printlnf("Song 8: Battle Rage");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s8_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s8_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s8_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s8_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s8_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s8_5);
			ch->printf("Line 6: ");
			ch->printlnf(ch->s8_6);
			ch->printf("Line 7: ");
			ch->printlnf(ch->s8_7);
			ch->printf("Line 8: ");
			ch->printlnf(ch->s8_8);
			ch->printf("Line 9: ");
			ch->printlnf(ch->s8_9);
			ch->printf("Line 10: ");
			ch->printlnf(ch->s8_10);
			ch->printf("Line 11: ");
			ch->printlnf(ch->s8_11);
			ch->printf("Line 12: ");
			ch->printlnf(ch->s8_12);
			ch->printf("Line 13: ");
			ch->printlnf(ch->s8_13);
			ch->printf("Line 14: ");
			ch->printlnf(ch->s8_14);
			ch->printf("Line 15: ");
			ch->printlnf(ch->s8_15);
			ch->printf("Line 16: ");
			ch->printlnf(ch->s8_16);
			ch->printf("Line 17: ");
			ch->printlnf(ch->s8_17);
			ch->printf("Line 18: ");
			ch->printlnf(ch->s8_18);
			ch->printf("Line 19: ");
			ch->printlnf(ch->s8_19);
			ch->printf("Line 20: ");
			ch->printlnf(ch->s8_20);
			ch->printf("Line 21: ");
			ch->printlnf(ch->s8_21);
			break;
		case 9:
			ch->printlnf("Song 9: Create Sustinance");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s9_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s9_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s9_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s9_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s9_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s9_5);
			ch->printf("Line 6: ");
			ch->printlnf(ch->s9_6);
			ch->printf("Line 7: ");
			ch->printlnf(ch->s9_7);
			ch->printf("Line 8: ");
			ch->printlnf(ch->s9_8);
			ch->printf("Line 9: ");
			ch->printlnf(ch->s9_9);
			ch->printf("Line 10: ");
			ch->printlnf(ch->s9_10);
			ch->printf("Line 11: ");
			ch->printlnf(ch->s9_11);
			ch->printf("Line 12: ");
			ch->printlnf(ch->s9_12);
			ch->printf("Line 13: ");
			ch->printlnf(ch->s9_13);
			ch->printf("Line 14: ");
			ch->printlnf(ch->s9_14);
			ch->printf("Line 15: ");
			ch->printlnf(ch->s9_15);
			ch->printf("Line 16: ");
			ch->printlnf(ch->s9_16);
			ch->printf("Line 17: ");
			ch->printlnf(ch->s9_17);
			ch->printf("Line 18: ");
			ch->printlnf(ch->s9_18);
			ch->printf("Line 19: ");
			ch->printlnf(ch->s9_19);
			break;
		case 10:
			ch->printlnf("Song 10: Sleepytime");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s10_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s10_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s10_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s10_3);
			break;
		case 11:
			ch->printlnf("Song 11: Natures Howl");
			ch->printf("Line 0: ");
			ch->printlnf(ch->s11_0);
			ch->printf("Line 1: ");
			ch->printlnf(ch->s11_1);
			ch->printf("Line 2: ");
			ch->printlnf(ch->s11_2);
			ch->printf("Line 3: ");
			ch->printlnf(ch->s11_3);
			ch->printf("Line 4: ");
			ch->printlnf(ch->s11_4);
			ch->printf("Line 5: ");
			ch->printlnf(ch->s11_5);
			ch->printf("Line 6: ");
			ch->printlnf(ch->s11_6);
			ch->printf("Line 7: ");
			ch->printlnf(ch->s11_7);
			ch->printf("Line 8: ");
			ch->printlnf(ch->s11_8);
			ch->printf("Line 9: ");
			ch->printlnf(ch->s11_9);
			ch->printf("Line 10: ");
			ch->printlnf(ch->s11_10);
			ch->printf("Line 11: ");
			ch->printlnf(ch->s11_11);
			break;
	}
	return;
}
/**************************************************************************/
void do_disallowsongs( char_data *ch, char *argument )
{
	char arg[MIL];
	char_data *victim;
	
	one_argument( argument, arg );
	
    if ( arg[0] == '\0' )
    {
		ch->println( "Disallow Bardic Song modifications on whom ?" );
		return;
    }
	
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
		ch->println( "They aren't here." );
		return;
    }
	
    if ( IS_NPC(victim) )
    {
		ch->println( "Not on NPC's." );
		return;
    }

    if ( HAS_CONFIG2(victim, CONFIG2_NOSONGMODS))
    {
		ch->printlnf("%s can now create songs.", victim->name);
		victim->println( "You can now create your own songs." );
		REMOVE_CONFIG2(victim, CONFIG2_NOSONGMODS);
    		save_char_obj(victim);
		return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) && ch != victim )
    {
		ch->println( "You failed." );
		return;
    }

    SET_CONFIG2(victim, CONFIG2_NOSONGMODS);
    save_char_obj(victim);
    ch->printlnf("%s can no longer create songs.", victim->name);
    victim->println("You can no longer create songs.");
    return;
}
/**************************************************************************/
void do_shutup( char_data *ch, char *argument )
{
	char arg[MIL];
	char_data *victim;
	
	one_argument( argument, arg );
	
    if ( arg[0] == '\0' )
    {
		ch->println( "Make who Shut Up ?" );
		return;
    }
	
    if ( ( victim = get_char_world( ch, arg ) ) == NULL )
    {
		ch->println( "They aren't here." );
		return;
    }
	
    if ( IS_NPC(victim) )
    {
		ch->println( "Not on NPC's." );
		return;
    }

    if ( IS_SET(victim->config2, CONFIG2_SHUTUP))
    {
    		ch->printlnf("%s can now run their mouth.", victim->name);
		REMOVE_CONFIG2(victim, CONFIG2_SHUTUP);
    		save_char_obj(victim);
		return;
    }

    if ( get_trust( victim ) >= get_trust( ch ) && ch != victim )
    {
		ch->println( "You failed." );
		return;
    }

    SET_CONFIG2(victim, CONFIG2_SHUTUP);
    save_char_obj(victim);
    ch->printlnf("%s can no longer run their mouth.", victim->name);
    return;
}
/*************************************************************************
SONG_FUN sfun_bardic_peace(char_data *ch, void *)
{
	char_data *victim;
	affect_data af;
	for( victim = ch->in_room->people; victim; victim = victim->next_in_room )
     	{
		if ( IS_AFFECTED( victim, AFF_CALM )
		||   IS_AFFECTED( victim, AFF_BERSERK )
		||   is_affected( victim, gsn_bardic_peace ))
			continue;
		if (victim->fighting || victim->position == POS_FIGHTING )
			stop_fighting( victim, false );


		af.where = WHERE_AFFECTS;
		af.type = gsn_bardic_peace;
		af.level = ch->level;
		af.duration = 1;
		af.location = APPLY_HITROLL;

		if (!IS_NPC(victim))
			af.modifier = -5;
		else
			af.modifier = -2;

		af.bitvector = AFF_CALM;
		affect_to_char(victim,&af);
		af.location = APPLY_DAMROLL;
		affect_to_char(victim,&af);
	}
	return true;
}
**************************************************************************/
/**************************************************************************/
