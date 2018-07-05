/**************************************************************************/
// craft.cpp - handles all the various crafting systems!
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#include "craft.h"
#include "include.h"

char *grade(int percentage);
const char *ore_string(int material_flag, int alloy);
char *wstyle_string(int weapon_type, int weapon_style);
void transfer_qualities(int quality_flag, obj_data *obj);
bool material_check(obj_data *obj, char *argument);
void craftedit( char_data *ch, char *argument );
bool cstep_weaponsmith_table(char_data *ch, char *argument);
bool weaponsmith_mixing(char_data *ch, char *argument);
bool craft_done( char_data *ch);


void do_weaponsmith( char_data *ch, char *argument )
{
	obj_data *craft;
	int i;

	if (IS_NULLSTR(argument)){		
		ch->titlebar("WEAPONSMITH");
		ch->println("syntax: weaponsmith <ore>");
		ch->println("This command will begin an indepth crafting project.");
		ch->println("`x");
		return;
		}
		
	
	char arg[MIL];
	argument=one_argument(argument,arg);

	// check if the ore is actually there.
	craft=get_obj_carry(ch, arg);

	if(craft == NULL
	|| craft->item_type != ITEM_MATERIAL
	|| craft->value[0] != MATERIAL_ORE //Material type
	|| craft->value[1] <= 0 //Material purity
	|| craft->value[2] <= 20 //Material grade
	
	){
		ch->println("That's not suitable at all.");
		return;
	}

	act("You begin to smith $p.", ch, craft, NULL, TO_CHAR);

	extract_obj(craft);
	ch->craft_choices[0] = craft->value[1];
	ch->craft_choices[1] = 1;
	ch->craft_choices[2] = craft->weight * craft->value[2] / 100;
	ch->craft_choices[3] = craft->value[3];
	for(i = 0; str_cmp(ore_table[i].name, ""); i++)
	{
		if(ore_table[i].bit == craft->value[1])
			break;
	}
	ch->craft_choices[4] = ore_table[i].quality_flags;
	ch->craft_choices[5] = 0;
	ch->craft_choices[6] = 0;
	ch->craft_choices[7] = 7;
	ch->craft_choices[8] = 100;
	ch->craft_choices[9] = 100;
	ch->step_number = STEP_1;
    ch->desc->pEdit	= (void*)craft;
	ch->desc->editor = ED_CRAFT;
	ch->craft_type = CRAFT_WEAPONSMITH;
	craftedit(ch, "show");
	return;
}
/**************************************************************************/
// Kalahn - April 2003 (rehash yet again) - should one day become generic

void craftedit( char_data *ch, char *argument )
{
	obj_data *pCraft;
	char arg[MIL];
	EDIT_CRAFT( ch, pCraft);
	one_argument(argument, arg);
	int i;
	if(!str_cmp(arg, "\0"))
	{
		craftedit(ch, "show");
		return;
	}

	if ( !str_cmp(arg, "abort"))
	{
		ch->println("You stop your crafting, wasting your materials.");
		craft_done(ch);
		return;
	}
	
	if(ch->craft_type != CRAFT_NONE)
	{
		for(i = 0; craft_table[i].type != CRAFT_NONE; i++)
			if(craft_table[i].type == ch->craft_type)
			{	
				if(((craft_table[i].cmdlist_table)(ch, argument)) == true)
					return;
				else{
					interpret( ch, argument );
					return;
				}
			}
	}
	// Default to Standard Interpreter
	interpret( ch, argument );
	return;
}
bool cstep_weaponsmith_table(char_data *ch, char *argument)
{
	int i;
	for(i = 0; weaponsmith_table[i].step != 0;i++)
		if(ch->step_number == weaponsmith_table[i].step)
		{	
			return ((weaponsmith_table[i].cmd_table)( ch, argument));
			break;
		}
	return false;
}

bool weaponsmith_mixing(char_data *ch, char *argument)
{
	char arg[MIL];
	char command[MIL];
	int i;

	smash_tilde( argument );
	argument = one_argument(argument, command);
	strcpy( arg, argument );

	for(i = 0; !IS_NULLSTR(weapsmith_mixing[i].name); i++)
    {
        if(!str_prefix(command,weapsmith_mixing[i].name))
            	return (weapsmith_mixing[i].craft_fun)( ch, argument);				
    }
	
	return false;
}
/********************************************************************************/
//Mixing steps
/********************************************************************************/
bool weaponsmith_mixing_add(char_data *ch, char *argument)
{
	obj_data *ore;
	char arg[MIL];
	int i;
	argument = one_argument(argument, arg);
	
	if(IS_NULLSTR(arg))
	{
		ch->println("Syntax: Add <ore>.");
		return true;
	}
	if((ore = get_obj_carry(ch, arg)) == NULL)
	{
		ch->println("You do not have that.");
		return true;
	}

	if(ore->item_type != ITEM_MATERIAL
	|| ore->value[0] != MATERIAL_ORE
	|| ore->weight * ore->value[2] / 100 < 10
	|| ore->value[3] < 20)
	{
		ch->printlnf("%s cannot be added.", ore->short_descr);
		return true;
	}

	if(ch->craft_choices[9] < 10 * ((120 - ore->value[3]) / 20))
	{
		ch->printlnf("You'd surely fail trying to add %s.", ore->short_descr);
		return true;
	}
	for(i = 0 ; ore_table[i].bit != 0 ; i++)
		if(ore_table[i].bit == ore->value[1])
			break;
	SET_BIT(ch->craft_choices[4], ore_table[i].quality_flags);
	ch->craft_choices[2] += ore->weight * ore->value[2] / 100;
	if(!IS_SET(ch->craft_choices[0], ore->value[1]))
	{
		SET_BIT(ch->craft_choices[0], ore->value[1]);
		ch->craft_choices[1]++;
		ch->craft_choices[3] = ((ch->craft_choices[3] * (ch->craft_choices[1] - 1)) + ore->value[3]) /  ch->craft_choices[3];
	}
		else 
			ch->craft_choices[3]=((ch->craft_choices[3] * ch->craft_choices[1]) + ore->value[3]) / ch->craft_choices[1];
	ch->craft_choices[9]-=10 * ((120 - ore->value[3]) / 20);
	ch->printlnf("You add %s in.", ore->short_descr);
	extract_obj(ore);
	return true;
}
/********************************************************************************/
bool weaponsmith_mixing_refine(char_data *ch, char *argument)
{
	int percent;
	int skill = get_skill(ch, gsn_weaponsmith);
	percent = UMAX(1, (((number_percent() + skill) / 12) + 1));
	if(ch->craft_choices[9] - 2 * (percent / (skill/20) < 0)
	|| ((ch->craft_choices[2] * (100 - percent) / 100) <= 0)
	|| ch->craft_choices[3] == 100)
	{
		ch->println("You'd fail if you tried to refine this.");
	}
	else{
		ch->println("You perform the careful task of refining your material.");
		ch->craft_choices[9] -= 2 * (percent / (skill/30));
	    ch->craft_choices[2] = ch->craft_choices[2] * (100 - percent) / 100;
		ch->craft_choices[3] = UMIN(ch->craft_choices[3] + percent, 100);	
	}
	return true;

}
/********************************************************************************/
bool weaponsmith_mixing_speedup(char_data *ch, char *argument)
{
	int skill = get_skill(ch, gsn_weaponsmith);
	if(ch->craft_choices[7] == 10
	|| ch->craft_choices[9] - ((110 - skill) / 2))
		ch->println("Speeding the process up any more would destroy it!");
	else
	{
		ch->println("You speed your work up.");
		ch->craft_choices[7]++;
		ch->craft_choices[9] -= (110 - skill) / 2; 
	}
	return true;
}
/********************************************************************************/
bool weaponsmith_mixing_slowdown(char_data *ch, char *argument)
{
	int skill = get_skill(ch, gsn_weaponsmith);
	if(ch->craft_choices[7] == 1)
		ch->println("Slowing the process down any further would destroy it!");
	else
	{
		ch->println("You slow down your work down.");
		ch->craft_choices[7]--;
		ch->craft_choices[9] += (110 - skill) / 2; 
	}
	return true;
}
/********************************************************************************/
bool weaponsmith_mixing_show(char_data *ch, char *)
{
	ch->println( " `#`c_____________________________________________________________________________");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|`w Material:`W %-10s                           `wSpeed:`W %-2d                  `C|`c|", ore_string(ch->craft_choices[0], ch->craft_choices[1]), ch->craft_choices[7] );
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|`w      Quantity:`W %-4d pounds`w               Grade:`W %-32s`C|`c|", ch->craft_choices[2], grade(ch->craft_choices[3]));
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|      `wQualities:`W %-58s`C|`c|", flag_string(craftquality_flags, ch->craft_choices[4]));
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->println( "|`C|  `wOptions:`W Show, Add, Refine, Speedup, Slowdown, Done, Abort               `C|`c|");
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|  `wStarting Success Percentage: `W%-3d    `wCurrent Percentage:`W %-17d`C|`c|", ch->craft_choices[8], ch->craft_choices[9] );
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	return true;
}
/********************************************************************************/
bool weaponsmith_mixing_done(char_data *ch, char *argument)
{
	obj_data *pCraft;
	EDIT_CRAFT( ch, pCraft);
	int success = ch->craft_choices[9] + (ch->modifiers[STAT_SD] / 3) + (ch->modifiers[STAT_IN] / 3);
	ch->println("You heat the metal to a liquid state over the fire.");
	if(number_percent() >  success - 10){
		ch->println("But the metal sizzles and sparks and turns unusable.");
		ch->craft_choices[0] = 0;
		ch->craft_choices[1] = 0;
		ch->craft_choices[2] = 0;
		ch->craft_choices[3] = 0;
		ch->craft_choices[4] = 0;
		ch->craft_choices[5] = 0;
		ch->craft_choices[6] = 0;
		ch->craft_choices[7] = 0;
		ch->craft_choices[8] = 0;
		ch->craft_choices[9] = 0;
		craft_done(ch);
	}
	else{
		ch->println("After reaching a proper temperature you begin to cool it again.");
		pCraft->item_type		= ITEM_WEAPON;
		pCraft->wear_flags		= OBJWEAR_TAKE|OBJWEAR_WIELD;
		pCraft->value[0]		= 0;
		pCraft->value[1]		= 0;
		pCraft->value[2]		= 0;
		pCraft->value[3]		= 0;
		pCraft->value[4]		= 0;
		pCraft->durability		= 5;
		if(IS_SET(ch->craft_choices[4], QUALITY_MALLEABLE))
			ch->craft_choices[9]+= 5 * (get_skill(ch, gsn_weaponsmith) / 20);
		if(IS_SET(ch->craft_choices[4], QUALITY_MALLEABLE))
			ch->craft_choices[9]+= 5 * (get_skill(ch, gsn_weaponsmith) / 20);
		transfer_qualities(ch->craft_choices[4], pCraft);
		ch->craft_choices[4] = 0;
		ch->craft_choices[5] = 0;
		ch->craft_choices[9] -= 10;
	}
	return true;
}	
/********************************************************************************/
//Shaping steps
/********************************************************************************/
bool weaponsmith_shaping_show(char_data *ch, char *)
{
	ch->println( " `#`c_____________________________________________________________________________");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|`w Weapon type:`W %-10s        `wWeapon Style:`W %-10s                    `C|`c|",
		(ch->craft_choices[4] > 0 ? weapon_table[ch->craft_choices[4]].name : "none"), wstyle_string(ch->craft_choices[4], ch->craft_choices[5]));
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|`w      Original Quantity:`W %-4d pounds`w            Quantity:`W %-2d pounds                  `C|`c|", ch->craft_choices[2], ch->craft_choices[3]);
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|      `wStability Rating:`W %-2d          `wDamage Rating:`W %-2d                   `C|`c|", ch->craft_choices[6], ch->craft_choices[7]);
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->println( "|`C| `wOptions:`W Show, Type, Style, Stability, Damage, Speedup, Slowdown, Done, Abort`C|`c|");
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	ch->println( "|`C\\___________________________________________________________________________/`c|");
	ch->printlnf("|`C|  `wStarting Success Percentage: `W%-3d    `wCurrent Percentage:`W %-17d`C|`c|", ch->craft_choices[8], ch->craft_choices[9] );
	ch->println( "|`C|___________________________________________________________________________|`c|");
	ch->println( "|`C/`c___________________________________________________________________________`C\\`c|");
	return true;
}

/********************************************************************************/
//Utility Functions
/********************************************************************************/
char *wstyle_string(int weapon_type, int weapon_style)
{
	switch(weapon_type)
	{
		case WEAPON_SWORD : return wstyle_sword[weapon_style].name;
							break;
		default			  : return "none";
	}
}

const char *ore_string(int material, int alloy)
{	
	int i; 
	for(i = 0; !IS_NULLSTR(ore_table[i].name); i++)
	{
		if(ore_table[i].bit == material)
			break;
	}

	return (alloy > 1 ? "alloy" : ore_table[i].name);
}

bool craft_done(char_data *ch)
{
    ch->desc->pEdit = NULL;
	ch->desc->editor = 0;
	ch->step_number = 0;
	ch->craft_type = CRAFT_NONE;
	return false;
}

char *grade(int percentage)
{
	if(percentage < 0 
	|| percentage > 100)
		return "`#`RError`^";
	else if(percentage > 90)
		return "`#`YA`^";
	else if(percentage > 80)
		return "`#`yB`^";
	else if(percentage > 70)
		return "`#`gC`^";
	else if(percentage > 60)
		return "`#`GD`^";
	else if(percentage > 50)
		return "`#`BE`^";
	else
		return "`#`bF`^";
}

void transfer_qualities(int quality_flag, obj_data *obj)
{
	int dur_mod = number_range(1,3);
	int weight_mod = number_percent();
	obj->extra_flags	= 0;
	obj->extra2_flags	= 0;
	obj->ospec_value[0] = 100;
	if(IS_SET(quality_flag, QUALITY_EVIL)){
		SET_BIT(obj->extra_flags, OBJEXTRA_EVIL);
		SET_BIT(obj->extra_flags, OBJEXTRA_ANTI_GOOD);
	}
	if(IS_SET(quality_flag, QUALITY_BLESSED)){
		SET_BIT(obj->extra_flags, OBJEXTRA_BLESS);
		SET_BIT(obj->extra_flags, OBJEXTRA_ANTI_EVIL);
	}
	if(IS_SET(quality_flag, QUALITY_CHAOS)){
		obj->chaos = true;
		SET_BIT(obj->extra_flags, OBJEXTRA_CHAOS);
	}
	if(IS_SET(quality_flag, QUALITY_MAGICAL)){
		SET_BIT(obj->extra_flags, OBJEXTRA_MAGIC);
	}
	if(IS_SET(quality_flag, QUALITY_BURNPROOF)){
		SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
	}
	if(IS_SET(quality_flag, QUALITY_TRANSPARENT)){
		SET_BIT(obj->extra_flags, OBJEXTRA_GLOW);
	}
	if(IS_SET(quality_flag, QUALITY_FRAGILE)){
		obj->durability = 5 - dur_mod;
	}
	if(IS_SET(quality_flag, QUALITY_DURABLE)){
		obj->durability = 5 + dur_mod;
	}
	if(IS_SET(quality_flag, QUALITY_DRAIN)
	&& obj->item_type == ITEM_WEAPON){
		SET_BIT(obj->value[4], WEAPON_VAMPIRIC);
	}
	if(IS_SET(quality_flag, QUALITY_SUCKLE)
	&& obj->item_type == ITEM_WEAPON){
		SET_BIT(obj->value[4], WEAPON_SUCKLE);
	}
	if(IS_SET(quality_flag, QUALITY_HEAVY)){
		obj->ospec_value[0] += weight_mod;
	}
	if(IS_SET(quality_flag, QUALITY_LIGHT)){
		obj->ospec_value[0] -= weight_mod;
	}
}

bool material_check(obj_data *obj, char *argument)
{
	char arg[MIL];
	for(argument = one_argument(argument, arg);!IS_NULLSTR(arg);argument = one_argument(argument, arg))
		if(!str_prefix(obj->material, strip_colour(arg)))
			return true;
	return false;
}

void do_materialcheck(char_data *ch, char *argument)
{
	char arg[MIL];
	obj_data *obj;
	argument = one_argument(argument, arg);

	if(IS_NULLSTR(arg)
	|| (obj = get_obj_carry(ch, arg)) == NULL)
	{
		ch->println("You don't have that object.");
		return;
	}
	else if(!material_check(obj, argument))
	{
		ch->println("That string does not contain the required material.");
		return;
	}
	else{
		ch->println("Yes, that string is acceptable.");
	}
}





