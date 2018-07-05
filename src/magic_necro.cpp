/**************************************************************************/
// magic_ce.cpp - spells/skills written by Celrion
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
//NECRO-CHANGE : ADD

#include "include.h"
#include "magic.h"

/**************************************************************************/
SPRESULT spell_wyldfire( int sn, int level, char_data *ch, void *vo,int  )
{
	char_data *victim = (char_data *) vo;
	int dam;
	int necro_dam;

	dam		= dice(level, 6)+10;
    necro_dam     = dice(UMAX((get_ambient_death(ch) * 5), 1), 6) + 10;

	switch(check_immune(victim,DAM_NEGATIVE))
	{
	case(IS_IMMUNE):
		necro_dam = 0;
		break;
	case(IS_RESISTANT):
		necro_dam -= necro_dam/2;
		break;
	case(IS_VULNERABLE):
		necro_dam += necro_dam;
		break;
	}

	ch->println("You harness liquid death into a weapon!");
	act( "$n pulls darkness to $mself and energy erupts!",ch, NULL, NULL, TO_ROOM );

	if ( saves_spell( level, victim, DAMTYPE(sn)))
		dam /= 2;

	damage_spell( ch, victim, dam, sn, DAMTYPE(sn),true);
	return FULL_MANA;
}
/**************************************************************************/
int get_ambient_death(char_data *ch)
{
	char_data *victim;
	char_data *v_next;
	int room_energy;
	int people_energy;
	int number_present;

	room_energy = ch->in_room->room_death;
	number_present = count_people_room( ch, 0 );
	people_energy = 0;

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        people_energy += victim->death_energy;
		if(victim->next_in_room)
			v_next = victim->next_in_room;
		else
			break;
    }
	
	return (room_energy + (people_energy / 100 / number_present));
}
/**************************************************************************/
int get_room_death(char_data *ch)
{
	int room_energy;

	room_energy = ch->in_room->room_death;	
	return room_energy;
}
/**************************************************************************/
int get_people_death(char_data *ch)
{
	char_data *victim;
	char_data *v_next;
	int people_energy;
	int number_present;

	number_present = count_people_room( ch, 0 );
	people_energy = 0;

	for ( victim = ch->in_room->people; victim != NULL; victim = v_next )
    {
        v_next = victim->next_in_room;
        people_energy += victim->death_energy;
    }
	
	return (people_energy / 100 / number_present);
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
