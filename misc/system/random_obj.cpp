/**************************************************************************/
// random_obj.cpp - Random Object Generator by Brad Wilson
/***************************************************************************
 * Whispers of Times Lost (c)1998-2005 Brad Wilson (wotlmud@bellsouth.net) *
 * >> If you use this code you must give credit in your help file as well  *
 *    as leaving this header intact.                                       *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses below. In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
 * The Dawn of Time v1.69q (c)1997-2002 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
 * >> Original Diku Mud copyright (c)1990, 1991 by Sebastian Hammer,       *
 *    Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, & Katja Nyboe.   *
 * >> Merc Diku Mud improvements copyright (C) 1992, 1993 by Michael       *
 *    Chastain, Michael Quan, and Mitchell Tse.                            *
 * >> ROM 2.4 is copyright 1993-1995 Russ Taylor and has been brought to   *
 *    you by the ROM consortium: Russ Taylor(rtaylor@pacinfo.com),         *
 *    Gabrielle Taylor(gtaylor@pacinfo.com) & Brian Moore(rom@rom.efn.org) *
 * >> Oblivion 1.2 is copyright 1996 Wes Wagner                            *
 **************************************************************************/

/*************************** INSTRUCTIONS **********************************
** Add the file random_object.o to your makefile
//
// GAMESET.H
** Put the following line with all of the obj_vnum assignments
//	int		obj_vnum_random_object; // Ixliam's Random Object Code
//
// GAMEEDIT.CPP
** Put this in below the obj_vnum_outfit_light
// GIO_INT_WITH_DEFAULT(obj_vnum_random_object, 100) 
// I used vnum 100, which is a reserved number but isn't used in 1.69q or 1.69r
// Vnum 412 might work as well.
** Then put this in below the obj_vnum_outfit_light line
//	{ GSVC_OBJECT, GSBYTE_OFFSET(obj_vnum_random_object), "random_object", "obj_vnum_random_object"},
//
//
// #define GAMESET4_RANDOM_OBJECTS_ENABLED				(H)
//
//
// PARAMS.H  - Add this in in the OBJ_VNUM area
// #define OBJ_VNUM_RANDOM_OBJ 	(game_settings->obj_vnum_random_object)
// 
// 
// FIGHT.CPP
// obj_data  *random_object args(( int level ));
// 
// then in void make_corpse routine, right before it puts the NPC
// corpse in the room.
// 
// 	if (GAMESETTING4(GAMESET4_RANDOM_OBJECTS_ENABLED))
// 	{
// 		obj_data		*random;
// 		if ( (number_percent() > 50) && IS_NPC(ch) )
// 		{
// 			random = random_object( level );
// 			obj_to_obj( random, corpse );
// 		}
// 	}
// 
// Now go create on object and name it rand_object_template for the name,
// short, and long. Then in gameedit set the value of the random object
// to that object vnum. Then go into the Flags4 and turn on Random_objects
// and you will now generate random objects. You may wish to adjust the
// percentage that the are generated, as well as the cost depending on your
// muds economy, as well as some of the names I used.
//
// The only real issues with it is that you get some strange sounding names
// like Earring of Wood Resistance or Pants of the Snake 
//
// I would appreciate hearing from you if you use this, and adding a brief
// credit to the help file (if you really nice).
//
// Ixliam - November 2003 (wotlmud@bellsouth.net)
//
************************************************************************/

#include "include.h" // dawn standard includes
#include "gameset.h"
#include "olc.h"
#include "lookup.h"
#include "tables.h"

// local functions
void random_affect args(( OBJ_DATA *obj, char *buf, int total ));

/**************************************************************************/
// weapon damage dice {1, 10} = 1d10  {2, 8} = 2d8 etc (From DOT 1.69q)
static const int weapon_balance_table[100][1][2] =
{
	{ { 1, 5  }}, 	{ { 1, 6  }}, 	{ { 2, 3  }}, 	{ { 1, 8  }}, 	{ { 2, 4  }}, 	{ { 1, 10 }},
	{ { 3, 3  }}, 	{ { 1, 12 }}, 	{ { 2, 6  }},	{ { 3, 4  }}, 	{ { 2, 7  }}, 	{ { 4, 3  }},
	{ { 3, 5  }}, 	{ { 2, 8  }},	{ { 4, 4  }},	{ { 3, 6  }},	{ { 2, 10 }},	{ { 2, 10 }},
	{ { 2, 11 }},	{ { 5, 4  }},	{ { 2, 12 }},	{ { 3, 8  }},	{ { 2, 13 }},	{ { 2, 13 }},
	{ { 3, 9  }},	{ { 2, 14 }},	{ { 2, 15 }},	{ { 3, 10 }},	{ { 2, 16 }},	{ { 5, 6  }},
	{ { 2, 17 }},	{ { 2, 17 }},	{ { 2, 18 }},	{ { 3, 12 }},	{ { 2, 19 }},	{ {10, 3  }},	
	{ { 3, 13 }},	{ { 2, 20 }},	{ { 2, 21 }},	{ { 3, 14 }},	{ { 2, 22 }},	{ { 2, 22 }},
	{ { 2, 23 }},	{ { 7, 6  }},	{ { 2, 24 }},	{ { 3, 16 }},	{ { 2, 25 }},	{ { 2, 25 }},
	{ { 2, 26 }},	{ { 5, 10 }},	{ { 2, 27 }},	{ { 3, 18 }},	{ { 2, 28 }},	{ { 2, 28 }},
	{ { 2, 29 }},	{ { 6, 9  }},	{ { 2, 30 }},	{ { 3, 20 }},	{ { 4, 15 }},	{ { 5, 12 }},
	{ { 3, 21 }},	{ { 3, 21 }},	{ { 4, 16 }},	{ { 3, 22 }},	{ { 5, 13 }},	{ { 5, 13 }},
	{ { 3, 23 }},	{ { 3, 23 }},	{ { 3, 24 }},	{ { 3, 24 }},	{ { 4, 18 }},	{ { 7, 10 }},
	{ { 3, 25 }},	{ { 3, 25 }},	{ { 4, 19 }},	{ { 3, 26 }},	{ { 3, 26 }},	{ { 7, 11 }},
	{ { 3, 27 }},	{ { 5, 16 }},	{ {17, 4  }},	{ { 3, 28 }},	{ { 4, 21 }},	{ {22, 3  }},
	{ { 3, 29 }},	{ { 7, 12 }},	{ { 4, 22 }},	{ { 3, 30 }},	{ { 3, 30 }},	{ { 5, 18 }},
	{ { 3, 31 }},	{ { 3, 31 }},	{ { 7, 13 }},	{ { 3, 32 }},	{ { 4, 24 }},	{ { 4, 24 }},
	{ { 3, 33 }},	{ { 3, 33 }},	{ { 4, 25 }},	{ { 7, 14 }},
};

/**************************************************************************/
int weapon_stat_lookup( int level, int pos, int die )
{
	// first off, return 1d4 for anything below level -2
	if ( level < -2 )
	{
		return ( die = 0 ? 1 : 4 );
	}

	// normalize values and bounds checking, just to be safe!!
	if ( pos   < 0	 )		pos		= 0;
	if ( pos   > 7	 )		pos		= 0;
	if ( level < 0	 )		level		= 0;
	if ( level > 99 )		level		= 99;
	if ( die   < 0	 )		die		= 0;
	if ( die   > 1	 )		die		= 1;

	// all safe now, return a number
	return ( weapon_balance_table[level][pos][die]);
}
/**************************************************************************/

obj_data  *random_object( int level )
{
	obj_data       	*obj;
  	OBJ_INDEX_DATA 	*pObjIndex;
	char           	buf[MSL];
	bool singlename 	= true;
	bool lowhigh 		= false;
	char long_desc[MSL], short_desc[MSL];

	if ((number_percent() + level) > 90 )
		lowhigh = true;

	if ( level > 91 )
		level = 91;

	pObjIndex = get_obj_index(OBJ_VNUM_RANDOM_OBJ);
	obj = create_object( pObjIndex );

   switch( dice( 1, 6 ) )
   {
   	case 1:
		case 2:
		case 3:
		case 4:
		case 5:
			obj->item_type = ITEM_ARMOR;

         switch( dice( 1, 18 ) )
         {
         	case 1: /* finger */
		      	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_FINGER;
		       	obj->value[0] = level/10;
	            obj->value[1] = level/10;
	            obj->value[2] = level/10;
	            obj->value[3] = level/10;
			   	sprintf( buf, "ring" );
					break;
            case 2: /* neck */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_NECK;
		       	obj->value[0] = level/6;
	            obj->value[1] = level/6;
	            obj->value[2] = level/6;
	            obj->value[3] = level/8;
			   	switch( dice( 1,7))
			   	{
			   		case 1:
				   		sprintf( buf, "necklace" ); break;
			   		case 2:
				   		sprintf( buf, "collar" ); break;
			   		case 3:
				   		sprintf( buf, "talisman" ); break;
			   		case 4:
				   		sprintf( buf, "gold chain" ); break;
			   		case 5:
				   		sprintf( buf, "silver amulet" ); break;
			   		case 6:
				   		sprintf( buf, "gorget" ); break;
			   		case 7:
				   		sprintf( buf, "pendant" ); break;
			   	} break;
            case 3: /* body */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_TORSO;
		       	obj->value[0] = level/3;
	            obj->value[1] = level/3;
	            obj->value[2] = level/3;
	            obj->value[3] = level/4;
			   	switch( dice( 1,3))
			   	{
			   		case 1:
				   		sprintf( buf, "tunic" ); break;
			   		case 2:
				   		sprintf( buf, "shirt" ); break;
			   		case 3:
				   		sprintf( buf, "scaled shirt" ); break;
			   		case 4:
				   		sprintf( buf, "armor" ); 
							break;
			   		case 5:
				   		sprintf( buf, "silk shirt" ); break;
			   		case 6:
				   		sprintf( buf, "breastplate" ); 
							break;
			   		case 7:
				   		sprintf( buf, "brigadine" );
							break;
			   		case 8:
				   		sprintf( buf, "long coat" ); break;
			   	} break;
      		case 4: /* head */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_HEAD;
		       	obj->value[0] = level/4;
	            obj->value[1] = level/4;
	            obj->value[2] = level/4;
	            obj->value[3] = level/5;
			   	switch( dice( 1,3))
			   	{
			   		case 1:
				   		sprintf( buf, "crown" ); break;
			   		case 2:
				   		sprintf( buf, "helmet" ); break;
			   		case 3:
				   		sprintf( buf, "helm" ); break;
			   	} break;
       		case 5: /* legs */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_LEGS;
		       	obj->value[0] = level/4;
	            obj->value[1] = level/4;
	            obj->value[2] = level/4;
	            obj->value[3] = level/5;
			   	switch( dice( 1,4))
			   	{
					   case 1:
						   sprintf( buf, "leggings" ); break;
					   case 2:
						   sprintf( buf, "pants" ); break;
			   		case 3:
						   sprintf( buf, "kilt" ); break;
					   case 4:
						   sprintf( buf, "skirt" ); break;
				   } break;
         	case 6: /* feet */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_FEET;
		       	obj->value[0] = level/5;
	            obj->value[1] = level/5;
	            obj->value[2] = level/5;
	            obj->value[3] = level/6;
			   	switch( dice( 1,5))
			   	{
			   		case 1:
						   sprintf( buf, "shoes" ); break;
					   case 2:
						   sprintf( buf, "boots" ); break;
					   case 3:
						   sprintf( buf, "sandals" ); break;
			   		case 4:
						   sprintf( buf, "low boots" ); break;
					   case 5:
						   sprintf( buf, "knee-high boots" ); break;
			   	} break;
				case 7: /* hands */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_HANDS;
		       	obj->value[0] = level/5;
	            obj->value[1] = level/5;
	            obj->value[2] = level/5;
	            obj->value[3] = level/6;
			   	switch( dice( 1,2))
			   	{
					   case 1:
						   sprintf( buf, "gloves" ); break;
				   	case 2:
						   sprintf( buf, "gauntlets" ); break;
				   } break;
			 	case 8: /* arm */
			   	obj->wear_flags = OBJWEAR_TAKE;
		      	obj->wear_flags += OBJWEAR_ARMS;
		     	  	obj->value[0] = level/4;
	            obj->value[1] = level/4;
	            obj->value[2] = level/4;
	            obj->value[3] = level/5;
			   	switch( dice( 1,6))
			   	{
			   		case 1:
						   sprintf( buf, "sleeves" ); break;
					   case 2:
						   sprintf( buf, "vambraces" );
						   break;
					   case 3:
						   sprintf( buf, "elbow pads" ); break;
			   		case 4:
						   sprintf( buf, "armguards" ); break;
					   case 5:
						   sprintf( buf, "padded sleeves" ); break;
					   case 6:
						   sprintf( buf, "armband" ); break;
			   	} break;
			 	case 9: /* shield */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_SHIELD;
		       	obj->value[0] = level/3;
	            obj->value[1] = level/3;
	            obj->value[2] = level/3;
	            obj->value[3] = level/4;
			   	switch( dice( 1,6))
			   	{
					   case 1:
						   sprintf( buf, "heater shield" ); break;
			   		case 2:
						   sprintf( buf, "buckler" ); break;
					   case 3:
						   sprintf( buf, "kite shield" ); break;
					   case 4:
						   sprintf( buf, "shield" ); break;
			   		case 5:
						   sprintf( buf, "round shield" ); break;
					   case 6:
						   sprintf( buf, "curved shield" ); break;
					} break;
				case 10: /* about */
			 		obj->wear_flags = OBJWEAR_TAKE;
			     	obj->wear_flags += OBJWEAR_ABOUT;
			     	obj->value[0] = level/4;
	   	      obj->value[1] = level/4;
	      	   obj->value[2] = level/4;
	         	obj->value[3] = level/5;
				   switch( dice( 1,7))
				   {
					   case 1:
						   sprintf( buf, "robe" ); break;
			   		case 2:
					   	sprintf( buf, "cloak" ); break;
					   case 3:
						   sprintf( buf, "cape" ); break;
					   case 4:
						   sprintf( buf, "overcoat" ); break;
			   		case 5:
					   	sprintf( buf, "velvet cape" ); break;
					   case 6:
						   sprintf( buf, "velvet cloak" ); break;
					   case 7:
						   sprintf( buf, "soft robe" ); break;
			   	} break;
			case 11: /* waist */
			   obj->wear_flags = OBJWEAR_TAKE;
		     	obj->wear_flags += OBJWEAR_WAIST;
		      obj->value[0] = level/5;
	         obj->value[1] = level/5;
	         obj->value[2] = level/5;
	         obj->value[3] = level/7;
			   switch( dice( 1,6))
			   {
				   case 1:
					   sprintf( buf, "belt" ); break;
				   case 2:
					   sprintf( buf, "sash" ); break;
				   case 3:
					   sprintf( buf, "cord" ); break;
			   	case 4:
					   sprintf( buf, "belly chain" ); break;
				   case 5:
					   sprintf( buf, "girdle" ); break;
				   case 6:
					   sprintf( buf, "navel ring" ); break;
			   } break;
			case 12: /* wrist */
				obj->wear_flags = OBJWEAR_TAKE;
		      obj->wear_flags += OBJWEAR_WRIST;
		      obj->value[0] = level/7;
	         obj->value[1] = level/7;
	         obj->value[2] = level/7;
	         obj->value[3] = level/9;
			   switch( dice( 1,6))
			   {
			   	case 1:
				   	sprintf( buf, "bracer" ); 
					   break;
				   case 2:
					   sprintf( buf, "bracelet" ); break;
				   case 3:
					   sprintf( buf, "fine bracelet" ); break;
				   case 4:
					   sprintf( buf, "engraved bracelet" ); break;
				   case 5:
					   sprintf( buf, "gold bracelet" ); break;
			   	case 6:
				   	sprintf( buf, "silver bracelet" ); break;
			   } break;
			case 13: /* hold */
				obj->wear_flags = OBJWEAR_TAKE;
		      obj->wear_flags += OBJWEAR_HOLD;
		      obj->value[0] = 0;
	         obj->value[1] = 0;
	         obj->value[2] = 0;
	         obj->value[3] = 0;
				sprintf( buf, "sceptre" );
				break;
			case 14: /* orbit */
				obj->wear_flags = OBJWEAR_TAKE;
		      obj->wear_flags += OBJWEAR_FLOAT;
		      obj->value[0] = 0;
	         obj->value[1] = 0;
	         obj->value[2] = 0;
	         obj->value[3] = 0;
			   switch( dice( 1,5))
			   {
			   	case 1:
				   	sprintf( buf, "orb" ); break;
				   case 2:
					   sprintf( buf, "stone" ); break;
				   case 3:
					   sprintf( buf, "aura" ); break;
			   	case 4:
				   	sprintf( buf, "fragrent odor" ); break;
				   case 5:
					   sprintf( buf, "nasty odor" ); break;
			   } break;
			case 15: /* face */
				obj->wear_flags = OBJWEAR_TAKE;
		      obj->wear_flags += OBJWEAR_FACE;
		      obj->value[0] = level/10;
	         obj->value[1] = level/10;
	         obj->value[2] = level/10;
	         obj->value[3] = level/10;
			   switch( dice( 1,5))
			   {
			   	case 1:
				   	sprintf( buf, "mask" ); break;
				   case 2:
					   sprintf( buf, "veil" ); break;
				   case 3:
					   sprintf( buf, "tattoos" ); break;
			   	case 4:
				   	sprintf( buf, "scars" ); break;
				   case 5:
					   sprintf( buf, "scabs" ); break;
			 	} break;
			case 16: /* contact */
				obj->wear_flags = OBJWEAR_TAKE;
		      obj->wear_flags += OBJWEAR_EYES;
		      obj->value[0] = level/10;
	         obj->value[1] = level/10;
	         obj->value[2] = level/10;
	         obj->value[3] = level/10;
			   switch( dice( 1,5))
			   {
			   	case 1:
				   	sprintf( buf, "tear" ); break;
				   case 2:
					   sprintf( buf, "pair of spectacles" ); break;
				   case 3:
					   sprintf( buf, "pair of glasses" ); break;
			   	case 4:
				   	sprintf( buf, "visor" ); break;
				   case 5:
					   sprintf( buf, "eyes" ); break;
				} break;
			case 17: /* ears */
				obj->wear_flags = OBJWEAR_TAKE;
		      obj->wear_flags += OBJWEAR_EAR;
		      obj->value[0] = level/10;
	         obj->value[1] = level/10;
	         obj->value[2] = level/10;
	         obj->value[3] = level/10;
				sprintf( buf, "earring" );
				break;
			case 18: /* ankle */
				obj->wear_flags = OBJWEAR_TAKE;
		      obj->wear_flags += OBJWEAR_ANKLE;
		      obj->value[0] = level/8;
	         obj->value[1] = level/8;
	         obj->value[2] = level/8;
	         obj->value[3] = level/10;
			   switch( dice( 1,2))
			   {
			   	case 1:
				   	sprintf( buf, "anklet" ); break;
				   case 2:
					   sprintf( buf, "socks" ); break;
			   } break;
			} break;
      case 6:
			obj->item_type = ITEM_WEAPON;
			obj->value[1] = 0;
			obj->value[1] = 0;

			switch( dice( 1, 6 ) )
			{
				case 1: /* bash */
				   obj->wear_flags = OBJWEAR_TAKE;
				   obj->wear_flags += OBJWEAR_WIELD;
				   obj->value[0] = 4;
			   	obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level, 0, 1 );
				   obj->value[3] = 8;
				   switch( dice( 1,12))
				   {
				   case 1:
					   sprintf( buf, "hammer" ); break;
				   case 2:
					   sprintf( buf, "mace" ); break;
				   case 3:
					   sprintf( buf, "club" ); break;
				   case 4:
					   sprintf( buf, "warhammer" ); break;
				   case 5:
					   sprintf( buf, "spiked club" ); break;
				   case 6:
					   sprintf( buf, "battle mace" ); break;
				   case 7:
					   sprintf( buf, "battle hammer" ); break;
				   case 8:
					   sprintf( buf, "fine mace" ); break;
				   case 9:
					   sprintf( buf, "large hammer" ); break;
				   case 10:
					   sprintf( buf, "small hammer" ); break;
				   case 11:
					   sprintf( buf, "fine hammer" ); break;
				   case 12:
					   sprintf( buf, "wooden club" ); break;
				   } break;
				case 2: /* pierce */
				   obj->wear_flags = OBJWEAR_TAKE;
				   obj->wear_flags += OBJWEAR_WIELD;
				   obj->value[0] = 2;
			   	obj->value[1] = weapon_stat_lookup( level-7, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level-7, 0, 1 );
				   obj->value[3] = 11;
				   switch( dice( 1,8))
				   {
				   case 1:
					   sprintf( buf, "dagger" ); break;
				   case 2:
					   sprintf( buf, "knife" ); break;
				   case 3:
					   sprintf( buf, "dirk" ); break;
				   case 4:
					   sprintf( buf, "claw" ); break;
				   case 5:
					   sprintf( buf, "curved dagger" ); break;
				   case 6:
					   sprintf( buf, "sharp dagger" ); break;
				   case 7:
					   sprintf( buf, "bone-hilted dagger" ); break;
				   case 8:
					   sprintf( buf, "sharp tooth" ); break;
				   } break;
				case 3: /* slash */
				   obj->wear_flags = OBJWEAR_TAKE;
				   obj->wear_flags += OBJWEAR_WIELD;
				   obj->value[0] = 1;
			   	obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level, 0, 1 );
				   obj->value[3] = 3;
				   switch( dice( 1,8))
				   {
				   case 1:
					   sprintf( buf, "sword" ); break;
				   case 2:
					   sprintf( buf, "longsword" ); break;
				   case 3:
					   sprintf( buf, "short sword" ); break;
				   case 4:
					   sprintf( buf, "broadsword" ); break;
				   case 5:
					   sprintf( buf, "scimitar" ); break;
				   case 6:
					   sprintf( buf, "rapier" ); 
					   obj->value[3] = 11;
					   break;
				   case 7:
					   sprintf( buf, "katana" ); break;
				   case 8:
					   sprintf( buf, "claymore" ); break;
				   } break;
				case 4: /* chop */
				   obj->wear_flags = OBJWEAR_TAKE;
				   obj->wear_flags += OBJWEAR_WIELD;
				   obj->value[0] = 5;
			   	obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
			   	obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
				   obj->value[3] = 25;
				   switch( dice( 1,8))
				   {
					   case 1:
						   sprintf( buf, "axe" ); break;
				   	case 2:
						   sprintf( buf, "battleaxe" ); break;
					   case 3:
						   sprintf( buf, "sharp axe" ); break;
					   case 4:
						   sprintf( buf, "sharp battleaxe" ); break;
				   	case 5:
						   sprintf( buf, "fine axe" ); break;
					   case 6:
						   sprintf( buf, "fine battleaxe" ); break;
					   case 7:
						   sprintf( buf, "large axe" ); break;
				   	case 8:
						   sprintf( buf, "large battleaxe" ); break;

				   } break;
				case 5: /* whip */
			   	obj->wear_flags = OBJWEAR_TAKE;
			   	obj->wear_flags += OBJWEAR_WIELD;
				   obj->value[0] = 7;
					obj->value[1] = weapon_stat_lookup( level-3, 0, 0 );
					obj->value[2] = weapon_stat_lookup( level-3, 0, 1 );
					obj->value[3] = 4;
					switch( dice( 1,3))
					{
						case 1:
							sprintf( buf, "whip" ); break;
						case 2:
						   sprintf( buf, "leather whip" ); break;
						case 3:
						   sprintf( buf, "multi-headed whip" ); break;
					} break;

			case 6: /* flail */
				obj->wear_flags = OBJWEAR_TAKE;
				obj->wear_flags += OBJWEAR_WIELD;
				obj->value[0] = 6;
				obj->value[1] = weapon_stat_lookup( level-2, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level-2, 0, 1 );
				obj->value[3] = 13;
				switch( dice( 1,10))
				{
					case 1:
					   sprintf( buf, "flail" ); break;
					case 2:
					   sprintf( buf, "morningstar" ); break;
					case 3:
					   sprintf( buf, "fine flail" ); break;
					case 4:
					   sprintf( buf, "fine morningstar" ); break;
					case 5:
					   sprintf( buf, "etched flail" ); break;
					case 6:
					   sprintf( buf, "large morningstar" ); break;
					case 7:
					   sprintf( buf, "large flail" ); break;
					case 8:
					   sprintf( buf, "morningstar" ); break;
					case 9:
						sprintf( buf, "huge flail" ); break;
					case 10:
						sprintf( buf, "small morningstar" ); break;
				} break;
			} break;
		} /* big switch */
	if ( lowhigh )
		switch ( dice( 1, 10 ) )
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5: /* no applies */
			break;
		case 6:
			singlename = false;
			random_affect( obj, buf, 1); break;
		case 7:
			singlename = false;
			random_affect(obj, buf, 2); break;
		case 8:
			singlename = false;
			random_affect(obj, buf, 3); break;
		case 9:
			break;
		  	case 10:
			break;
	} 
   obj->cost = number_range(level*10, level*75)/2;
	if ( singlename )
	{
		obj->name = str_dup( buf );
		if (!str_prefix("axe", obj->name) 	|| !str_prefix("earring", obj->name) ||
		    !str_prefix("armband", obj->name) 	|| !str_prefix("anklet", obj->name))
		{
			sprintf( long_desc, "An %s has been left here.", buf );
			sprintf( short_desc, "an %s", buf );
			obj->short_descr = str_dup( short_desc );
			obj->description = str_dup( long_desc );
		}
		else
		if (!str_prefix("a", obj->name) 	|| !str_prefix("e", obj->name) ||
		    !str_prefix("i", obj->name) 	|| !str_prefix("o", obj->name) ||
		    !str_prefix("gaunt", obj->name) 	|| !str_prefix("gloves", obj->name) ||
		    !str_prefix("shoes", obj->name) 	|| !str_prefix("boots", obj->name) ||
		    !str_prefix("sandals", obj->name)	|| !str_prefix("low", obj->name) ||
		    !str_prefix("knee", obj->name) 	|| !str_prefix("sleeves", obj->name) ||
		    !str_prefix("vambrace", obj->name) 	|| !str_prefix("elbow", obj->name) ||
		    !str_prefix("armor", obj->name) 	|| !str_prefix("padded", obj->name) ||
		    !str_prefix("eyes", obj->name) 	|| !str_prefix("socks", obj->name) ||
		    !str_prefix("u", obj->name))
		{
			sprintf( long_desc, "Some %s has been left here.", buf );
			sprintf( short_desc, "some %s", buf );
			obj->short_descr = str_dup( short_desc );
			obj->description = str_dup( long_desc );
		}
		else
		{
			sprintf( long_desc, "A %s has been left here.", buf );
			sprintf( short_desc, "a %s", buf );
			obj->short_descr = str_dup( short_desc );
			obj->description = str_dup( long_desc );
		}

	obj->cost = 0;
	}
	obj->level = level;
	return obj;
}

void random_affect( obj_data *obj, char *buf, int total )
{
	char long_desc[MSL];
   AFFECT_DATA *paf;
	char name[MSL];
	int number;

	for ( number = 0; number < total; number++ )
	{
		switch( dice( 1, 69 ) )
		{
		case 1: /* Resist Charm */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector = RES_CHARM;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Charm Resistance", capitalize(buf) );
			break;
		case 2: /* Resist Magic */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector = RES_MAGIC;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Magic Resistance", capitalize(buf) );
			break;
		case 3: /* Resist Fire */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_FIRE;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Fire Resistance", capitalize(buf) );
			break;
		case 4: /* Resist Cold */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_COLD;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Cold Resistance", capitalize(buf) );
			break;
		case 5: /* Resist Lightning */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector = RES_LIGHTNING;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Lightning Resistance", capitalize(buf) );
			break;
		case 6: /* Resist Acid */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_ACID;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Acid Resistance", capitalize(buf) );
			break;
		case 7: /* Resist Poison */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_POISON;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Poison Resistance", capitalize(buf) );
			break;
		case 8: /* Resist Negative */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_NEGATIVE;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Life Protection", capitalize(buf) );
			break;
		case 9: /* Resist Holy */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector = RES_HOLY;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Unholy Protection", capitalize(buf) );
			break;
		case 10: /* Resist MENTAL */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_MENTAL;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Mind Resistance", capitalize(buf) );
			break;
		case 11: /* Resist Disease */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_DISEASE;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Disease Resistance", capitalize(buf) );
			break;
		case 12: /* Resist Sound */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_SOUND;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Sound Resistance", capitalize(buf) );
			break;
		case 13: /* Resist Wood */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_WOOD;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Wood Resistance", capitalize(buf) );
			break;
		case 14: /* Resist Illusion */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector = RES_ILLUSION;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Illusion Resistance", capitalize(buf) );
			break;
		case 15: /* Resist Scry */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_RESIST;
			paf->duration = -1;
			paf->bitvector     = RES_SCRY;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Scry Resistance", capitalize(buf) );
			break;
		case 16: /* Invisibility */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_invisibility;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Invisibility", capitalize(buf) );
			break;
		case 17: /* Blindness */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_blindness;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Blindness", capitalize(buf) );
			break;
		case 18: /* Detect Evil */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_detect_evil;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Evil Detection", capitalize(buf) );
			break;
		case 19: /* Detect Invis */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_detect_invis;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Invisibility Detection", capitalize(buf) );
			break;
		case 20: /* Detect Magic */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_detect_magic;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Magic Detection", capitalize(buf) );
			break;
		case 21: /* Detect Hidden */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_detect_hidden;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Detection", capitalize(buf) );
			break;
		case 22: /* Detect Good */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_detect_good;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Unholy Vision", capitalize(buf) );
			break;
		case 23: /* FAERIE_FIRE */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_faerie_fire;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of the Faerie", capitalize(buf) );
			break;
		case 24: /* Cursed */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_curse;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Enchantment", capitalize(buf) );
			break;
		case 25: /* Otterlungs */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_otterlungs;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Water Breathing", capitalize(buf) );
			break;
		case 26: /* Poison */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_poison;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of the Snake", capitalize(buf) );
			break;
		case 27: /* Evil Protection */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_protection_good;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Unholy Protection", capitalize(buf) );
			break;
		case 28: /* Good Protection */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_protection_evil;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Holy Protection", capitalize(buf) );
			break;
		case 29: /* Sneak */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_AFFECTS;
			paf->duration = -1;
			paf->bitvector     = AFF_SNEAK;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Sneaking", capitalize(buf) );
			break;
		case 30: /* Hide */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_AFFECTS;
			paf->duration = -1;
			paf->bitvector     = AFF_HIDE;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Hiding", capitalize(buf) );
			break;
		case 31: /* Flying */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_fly;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Flying", capitalize(buf) );
			break;
		case 32: /* Pass Door */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_pass_door;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Passage", capitalize(buf) );
			break;
		case 33: /* Weaken */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_weaken;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Strength", capitalize(buf) );
			break;
		case 34: /* Night Vision */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = -1;
			paf->where     = WHERE_AFFECTS;
			paf->duration = -1;
			paf->bitvector     = AFF_DARK_VISION;
			paf->duration = -1;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of the Drow", capitalize(buf) );
			break;
		case 35: /* Fear */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_cause_fear;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Fright", capitalize(buf) );
			break;
		case 36: /* Vice Grip */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_vicegrip;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of the Iron Fist", capitalize(buf) );
			break;
		case 37: /* Strength */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_ST;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Strength", capitalize(buf) );
			break;
		case 38: /* Quickness */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_QU;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Speed", capitalize(buf) );
			break;
		case 39: /* Presence */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_PR;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Presence", capitalize(buf) );
			break;
		case 40: /* Intuition */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_IN;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of the Mind", capitalize(buf) );
			break;
		case 41: /* Empathy */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_EM;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Fortitude", capitalize(buf) );
			break;
		case 42: /* Constitution */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_CO;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Health", capitalize(buf) );
			break;
		case 43: /* Agility */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_AG;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Agility", capitalize(buf) );
			break;
		case 44: /* Self Discipline */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_SD;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Discipline", capitalize(buf) );
			break;
		case 45: /* Memory */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_ME;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Memory", capitalize(buf) );
			break;
		case 46: /* Resoning */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -10, 10 );
			paf->location = APPLY_RE;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Reason", capitalize(buf) );
			break;
		case 47: /* Mana */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -100, 200 );
			paf->location = APPLY_MANA;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Power", capitalize(buf) );
			break;
		case 48: /* Hit Points */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -5, 10 );
			paf->location = APPLY_AG;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Power", capitalize(buf) );
			break;
		case 49: /* Moves */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( 10, 200 );
			paf->location = APPLY_MOVE;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Travel", capitalize(buf) );
			break;
		case 50: /* Saves */
			paf = new_affect();
			paf->where = WHERE_MODIFIER;
			paf->modifier = number_range( -15, 5 );
			paf->location = APPLY_AG;
			paf->duration = -1;
			paf->type =   -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Protection", capitalize(buf) );
			break;
		case 51: /* Barkskin */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_barkskin;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of the Woods", capitalize(buf) );
			break;
		case 52: /* Aura of Temperance */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_aura_of_temperance;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Temperance", capitalize(buf) );
			break;
		case 53: /* Sex Change */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_change_sex;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Persuasion", capitalize(buf) );
			break;
		case 54: /* Deafness */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_deafness;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Hearing", capitalize(buf) );
			break;
		case 55: /* Despair */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_despair;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Defeat", capitalize(buf) );
			break;
		case 56: /* Detect Poison */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_detect_poison;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Poison Detection", capitalize(buf) );
			break;
		case 57: /* Induce Sleep */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_induce_sleep;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Rest", capitalize(buf) );
			break;
		case 58: /*Hallucinate */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_hallucinate;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Visions", capitalize(buf) );
			break;
		case 59: /* Know Alignment */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_know_alignment;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Balance", capitalize(buf) );
			break;
		case 60: /* Pass without Trace */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_pass_without_trace;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Passage", capitalize(buf) );
			break;
		case 61: /* Slow */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_slow;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Speed", capitalize(buf) );
			break;
		case 62: /* Teleport */
			paf = new_affect();
			paf->location =  APPLY_NONE;
			paf->modifier = 0;
			paf->where = WHERE_OBJECTSPELL;
			paf->type = gsn_teleport;
			paf->level = obj->level;
			paf->duration = -1;
			paf->bitvector = 0;
			paf->next = obj->affected;
			obj->affected = paf;
			sprintf( name, "%s of Travel", capitalize(buf) );
			break;
		case 63: /* Flaming */
			SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
			if( obj->item_type != ITEM_WEAPON)
			{
				sprintf( name, "%s of the Dawn", capitalize(buf) );
				break;
			}
			sprintf( name, "%s of `#`RFlame`^", capitalize(buf) );
			SET_BIT(obj->value[4], WEAPON_FLAMING);
			break;
		case 64: /* Frost */
			SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
			if( obj->item_type != ITEM_WEAPON)
			{
				sprintf( name, "%s of the Khreed", capitalize(buf) );
				break;
			}
			sprintf( name, "%s of `#`CIce`^", capitalize(buf) );
			SET_BIT(obj->value[4], WEAPON_FROST);
			break;
		case 65: /* Lightning */
			SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
			if( obj->item_type != ITEM_WEAPON)
			{
				sprintf( name, "%s of the Codite", capitalize(buf) );
				break;
			}
			sprintf( name, "%s of `#`YLightning`^", capitalize(buf) );
			SET_BIT(obj->value[4], WEAPON_SHOCKING);
			break;
		case 66: /* Vampiric */
			SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
			if( obj->item_type != ITEM_WEAPON)
			{
				sprintf( name, "%s of the Mydar", capitalize(buf) );
				break;
			}
			sprintf( name, "%s of `#`SDraining`^", capitalize(buf) );
			SET_BIT(obj->value[4], WEAPON_VAMPIRIC);
			break;

		case 67: /* Poison */
			SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
			if( obj->item_type != ITEM_WEAPON)
			{
				sprintf( name, "%s of the Sloth", capitalize(buf) );
				break;
			}
			sprintf( name, "%s of the `#`gSerpent`^", capitalize(buf) );
			SET_BIT(obj->value[4], WEAPON_POISON);
			break;

		case 68: /* Suckle */
			SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
			if( obj->item_type != ITEM_WEAPON)
			{
				sprintf( name, "%s of the Grilth", capitalize(buf) );
				break;
			}
			sprintf( name, "%s of `#`YMagic-Bane`^", capitalize(buf) );
			SET_BIT(obj->value[4], WEAPON_SUCKLE);
			break;

		case 69: /* Annealed */
			SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
			if( obj->item_type != ITEM_WEAPON)
			{
				sprintf( name, "%s of the Flarth", capitalize(buf) );
				break;
			}
			sprintf( name, "%s of `#`SStunning`^", capitalize(buf) );
			SET_BIT(obj->value[4], WEAPON_ANNEALED);
			break;

		} /* aff switch */
	}

	obj->name = str_dup( name );
	if (!str_prefix("a", buf) || !str_prefix("e", buf) ||
	    !str_prefix("i", buf) || !str_prefix("o", buf) ||
	    !str_prefix("u", buf))
	{
		sprintf( long_desc, "%s has been left here.", name );
		obj->short_descr = str_dup( name );
		obj->description = str_dup( long_desc );
	}
	else
	{
		sprintf( long_desc, "%s has been left here.", name );
		obj->short_descr = str_dup( name );
		obj->description = str_dup( long_desc );
	}
	return;
}

/**************************************************************************/
// This is my testing procedure for the routine, use it if you want to
// If you do, make sure to place it in interp.h and interp.cpp, and I would
// set the level to ML+1.

void do_randtest( char_data *ch, char *argument )
{ 
	obj_data *random;
	random = random_object( 50 );
	obj_to_char( random, ch );
	ch->printlnf("%s suddenly appears in your inventory.", random->name);
	return;
}
/**************************************************************************/
void make_weapon( char_data *ch, int weaptype, char *name, char *shdesc, int level, char *material )
{
	obj_data       	*obj;
  	OBJ_INDEX_DATA 	*pObjIndex;
	char long_desc[MSL];

	pObjIndex = get_obj_index(OBJ_VNUM_RANDOM_OBJ);
	obj = create_object( pObjIndex );

	obj->item_type = ITEM_WEAPON;
	obj->level = level;
	obj->name = str_dup( name );
	sprintf( long_desc, "%s has been left here.", capitalize(shdesc) );
	obj->short_descr = str_dup( shdesc );
	obj->description = str_dup( long_desc );
	obj->material = str_dup(material);

	switch( weaptype )
	{
		case 1: /* MACE */
		   	obj->wear_flags = OBJWEAR_TAKE;
		   	obj->wear_flags += OBJWEAR_WIELD;
		   	obj->value[0] = 4;
	   		obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			obj->value[3] = 8;
			break;	
		case 2: /* DAGGER */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WIELD;
			obj->value[0] = 2;
			obj->value[1] = weapon_stat_lookup( level-7, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level-7, 0, 1 );
			obj->value[3] = 11;
			break;			
		case 3: /* SWORD */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WIELD;
			obj->value[0] = 1;
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			obj->value[3] = 3;
			break;	
		case 4: /* AXE */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WIELD;
			obj->value[0] = 5;
			obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			obj->value[3] = 25;
			break;
		case 5: /* WHIP */
		  	obj->wear_flags = OBJWEAR_TAKE;
		   	obj->wear_flags += OBJWEAR_WIELD;
		   	obj->value[0] = 7;
			obj->value[1] = weapon_stat_lookup( level-3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level-3, 0, 1 );
			obj->value[3] = 4;
			break;
		case 6: /* FLAIL */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WIELD;
			obj->value[0] = 6;
			obj->value[1] = weapon_stat_lookup( level-2, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level-2, 0, 1 );
			obj->value[3] = 13;
			break;
		// TWO HANDED WEAPONS BELOW
		case 7: /* MACE */
		   	obj->wear_flags = OBJWEAR_TAKE;
		   	obj->wear_flags += OBJWEAR_WIELD;
		   	obj->value[0] = 4;
	   		obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			obj->value[3] = 8;
			SET_BIT(obj->value[4], WEAPON_TWO_HANDS);
			break;	
		case 8: /* SWORD */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WIELD;
			obj->value[0] = 1;
			obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			obj->value[3] = 3;
			SET_BIT(obj->value[4], WEAPON_TWO_HANDS);
			break;	
		case 9: /* AXE */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WIELD;
			obj->value[0] = 5;
			obj->value[1] = weapon_stat_lookup( level+9, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+9, 0, 1 );
			obj->value[3] = 25;
			SET_BIT(obj->value[4], WEAPON_TWO_HANDS);
			break;
		case 10: /* FLAIL */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WIELD;
			obj->value[0] = 6;
			obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			obj->value[3] = 13;
			SET_BIT(obj->value[4], WEAPON_TWO_HANDS);
			break;
		}

   	if (strstr(obj->material, "mithril"))
	{
		SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
	}
   	if (strstr(obj->material, "vampiric"))
	{
		SET_BIT(obj->value[4], WEAPON_VAMPIRIC);
	}
   	if (strstr(obj->material, "adamantium"))
	{
		SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
	}

	SET_BIT(obj->extra2_flags,OBJEXTRA2_NOSELL);
	SET_BIT(obj->extra2_flags,OBJEXTRA2_PC_CRAFTED);
	obj->cost = number_range(level*10, level*75)/2;
	obj_to_char( obj, ch );
	act( "$n holds $p, newly forged, in $s hand.", ch, obj, NULL, TO_ROOM );
	act( "You hold $p, newly forged, in your hand.", ch, obj, NULL, TO_CHAR );
	if (!IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS)) 
	{
		equip_char( ch, obj, WEAR_HOLD );
	}
	return;
}
/**************************************************************************/
void do_smithweapon( char_data *ch, char *argument )
{
	obj_data *ore=NULL;
    	obj_data *obj;
    	obj_data *obj_next;
    	char arg[MIL];
	char alevel[MIL];
    	char name[MIL];
    	char *material=NULL;
	bool forge = false;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, name);
	argument = one_argument(argument, alevel);
	
	if(IS_NULLSTR(arg))
	{
        	ch->println("What type of weapon are you wanting to smith ?");
        	ch->println("`#`BSyntax:`w smithweapon <weapontype> <weaponname> <level> <weapon short desc>`^");
		ch->println("`#`cValid types: `wmace dagger sword axe whip flail`^");
		ch->println("`#`cValid 2-handed types: `w2hmace 2hsword 2haxe  2hflail`^");
		return;
	}

	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED ))
	{
		if(ch->master)
		{
			ch->master->println( "Not going to happen.");
		}
		return;
	}

    	if ( !IS_NPC( ch ) && ch->level < skill_table[gsn_weaponsmith].skill_level[ch->clss] )
    	{                                          
        	ch->println("You do not know how to smith weapons.");
	        return;
    	}

	int weaptype = 0;

	if(strstr(arg, "mace")) 	weaptype = 1;
	if(strstr(arg, "dagger")) 	weaptype = 2;
	if(strstr(arg, "sword")) 	weaptype = 3;
	if(strstr(arg, "axe")) 		weaptype = 4;
	if(strstr(arg, "whip")) 	weaptype = 5;
	if(strstr(arg, "flail")) 	weaptype = 6;

	if(strstr(arg, "2hmace")) 	weaptype = 7;
	if(strstr(arg, "2hsword")) 	weaptype = 8;
	if(strstr(arg, "2haxe")) 	weaptype = 9;
	if(strstr(arg, "2hflail")) 	weaptype = 10;

	if(weaptype == 0)
    	{                                          
        	ch->println("What type of weapon are you wanting to smith ?");
        	ch->println("`#`BSyntax:`w smithweapon <weapontype> <weaponname> <level> <weapon short desc>`^");
		ch->println("`#`cValid types: `wmace dagger sword axe whip flail`^");
	        return;
    	}

	if(IS_NULLSTR(name)){
		ch->println("You must give the weapon a short description.");
        	ch->println("`#`BSyntax:`w smithweapon <weapontype> <weaponname> <level> <weapon short desc>`^");
	       	ch->println("`#`c(i.e. `wsmithweapon sword `Rbroadsword`^ 15 `Ya large broadsword`c)`^");
		return;
	}

	if(IS_NULLSTR(alevel)){
		ch->println("You must give the weapon a level.");
         	ch->println("`#`BSyntax:`w smithweapon <weapontype> <weaponname> <level> <weapon short desc>`^");
	       	ch->println("`#`c(i.e. `wsmithweapon sword broadsword `R15`^ `ya `Ylarge broadsword`c)`^");
		return;
	}

	if(IS_NULLSTR(argument)){
		ch->println("You must give the weapon a short description.");
         	ch->println("`#`BSyntax:`w smithweapon <weapontype> <weaponname> <level> <weapon short desc>`^");
	       	ch->println("`#`c(i.e. `wsmithweapon sword broadsword 15 `ya `Ylarge broadsword`c)`^");
		return;
	}

	int olevel = 0;
	olevel = atoi(alevel);

	if(olevel < 1)
	{
		ch->println("Unknown level.");
         	ch->println("`#`BSyntax:`w smithweapon <weapontype> <weaponname> <level> <weapon short desc>`^");
	       	ch->println("`#`c(i.e. `wsmithweapon sword broadsword 15 `ya `Ylarge broadsword`c)`^");
		return;
	}

	if(ch->level < olevel)
	{
		ch->println("You cannot make it higher than your level.");
         	ch->println("`#`BSyntax:`w smithweapon <weapontype> <weaponname> <level> <weapon short desc>`^");
	       	ch->println("`#`c(i.e. `wsmithweapon sword broadsword 15 `ya `Ylarge broadsword`c)`^");
		return;
	}

	for ( obj = ch->in_room->contents; obj; obj = obj_next ) // Look For Forge
	{
		obj_next = obj->next_content;

		if(obj->pIndexData->vnum == 418) // Look For Forge
		{
			forge = true;
			break;
		}
	}

	if(forge == false)
	{
		ch->println("There is no forge here for you to smith weapons with.");
		return;
	}

    	for ( ore = ch->carrying; ore; ore = ore->next_content )
    	{
        	if ( ore->item_type == ITEM_ORE && ore->wear_loc == WEAR_HOLD )
            		break;
        	if ( ore->item_type == ITEM_MATERIAL && ore->wear_loc == WEAR_HOLD )
            		break;
    	}

    	if ( !ore )
    	{
        	ch->println("You are not holding any ore to forge.");
        	return;
    	}

	bool goodore = false;

   	if (strstr(ore->material, "mithril"))
	{
		material = str_dup("mithril");
		goodore = true;
	}
   	if (strstr(ore->material, "vampiric"))
	{
		material = str_dup("vampiric");
		goodore = true;
	}
   	if (strstr(ore->material, "steel"))
	{	
		material = str_dup("steel");
		goodore = true;
	}
   	if (strstr(ore->material, "adamantium"))
	{
		material = str_dup("adamantium");
		goodore = true;
	}
   	if (strstr(ore->material, "iron"))
	{
		material = str_dup("iron");
		goodore = true;
	}

   	if (strstr(ore->material, "silver"))
	{
		material = str_dup("silver");
		goodore = true;
	}

   	if (goodore == false)
    	{
        	ch->println("This material will not stand up to the trials of combat.");
        	return;
    	}

  	act( "$n begins hammering $p on the forge.", ch, ore, NULL, TO_ROOM );
        act( "You begin working the ore into a weapon.", ch, ore, NULL, TO_CHAR );

	if (!IS_IMMORTAL( ch ))
		WAIT_STATE( ch, skill_table[gsn_weaponsmith].beats ); 

    	/* Check the skill percentage, memory and reasoning checks) */

    	if ( !IS_NPC(ch) && ( number_percent( ) > get_skill(ch, gsn_weaponsmith) ||
              number_percent( ) > ((ch->modifiers[STAT_ST]-7)*5 + (ch->modifiers[STAT_RE]-7)*3) ))
    	{
        	act( "You fail to forge $p into anything useful.", ch, ore, NULL, TO_CHAR );
	        act( "$n fails to forge $p into anything useful.", ch, ore, NULL, TO_ROOM );
		check_improve(ch, gsn_weaponsmith, false, 1);
        	extract_obj( ore );
	        return;
    	}

       	extract_obj( ore );
	int modifier = 101 - ch->pcdata->learned[gsn_weaponsmith];
    	int chance = number_range(1, modifier);
	olevel -= chance;
	make_weapon(ch, weaptype, name, argument, olevel, material);
	check_improve(ch, gsn_weaponsmith, true, 1);
	return;
}
/**************************************************************************/
void do_repair_armor( char_data *ch, char *argument )
{
    	char arg1[MIL];
    	char arg2[MIL];
    	obj_data *obj;
    	int cost=0;

    	argument=one_argument(argument,arg1);
    	argument=one_argument(argument,arg2);

    	if ( !IS_NPC( ch ) && ch->level < skill_table[gsn_armorsmith].skill_level[ch->clss] )
    	{                                          
        	ch->println("You do not know how to repair armor.");
	        return;
    	}

    	if (IS_NULLSTR(arg1))
    	{
		ch->println("`YRepairarmor `wwhat ?`x");
		ch->println("`#`BSyntax:`w repairarmor <item> <estimate or confirm>`^");
		ch->println("Estimate will provide material cost, confirm will repair.");
		return;
	}

    	if (IS_NULLSTR(arg2))
    	{
		ch->println("`YEstimate `wor `YConfirm`w ?`x");
		ch->println("`#`BSyntax:`w repairarmor <item> <estimate or confirm>`^");
		ch->println("Estimate will provide material cost, confirm will repair.");
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
		ch->println( "You do not have that item." );
		return;
	}

	if(IS_OBJ_STAT( obj, OBJEXTRA_NONMETAL ))
	{
		ch->println("You can only repair metal armor.");
		return;
	}

	if(obj->item_type != ITEM_ARMOR)
	{
		ch->println("This item is not armor.");
		return;
	}

    	if(obj->cost == 0)
       		cost += 150;
    	else
       		cost = obj->cost/4;

	if(strstr(arg2, "estimate"))
    	{
    		ch->printlnf("It will cost %d silver and %d gold coins to repair %s.",	
			      cost - (cost/100) * 100, cost/100, obj->short_descr );
		return;
    	}

	if(!strstr(arg2, "confirm"))
    	{
		ch->println("`YEstimate `wor `YConfirm`w ?`x");
		ch->println("`#`BSyntax:`w repairarmor <item> <estimate or confirm>`^");
		ch->println("Estimate will provide material cost, confirm will repair.");
		return;
    	}
	
	if(obj->condition == 100)
	{
		ch->println("This item is does not need repair.");
		return;
	}

	if(obj->condition < 39)
	{
		ch->println("This item is too badly damaged.");
		return;
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		ch->println("You do not have enough money to purchase repair materials for this item.");
		return;
	}

	deduct_cost(ch,cost);

	obj->condition = 100;
	if( (obj->item_type == ITEM_ARMOR) && obj->pIndexData->vnum != OBJ_VNUM_RANDOM_OBJ)
	{
		obj->value[0]= obj->pIndexData->value[0];
		obj->value[1]= obj->pIndexData->value[1];
		obj->value[2]= obj->pIndexData->value[2];
		obj->value[3]= obj->pIndexData->value[3];
	} // End Normal Items
		
	if( obj->item_type == ITEM_ARMOR && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
	{
	   	int level = obj->level;
	   	if(CAN_WEAR(obj, OBJWEAR_FINGER ))
	       	{
	       		obj->value[0] = level/10;
               		obj->value[1] = level/10;
               		obj->value[2] = level/10;
               		obj->value[3] = level/10;		
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_NECK ))
		{
		       		obj->value[0] = level/6;
	               		obj->value[1] = level/6;
	               		obj->value[2] = level/6;
	              	 	obj->value[3] = level/8;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_TORSO ))
		{
			obj->value[0] = level/3;
	               	obj->value[1] = level/3;
	               	obj->value[2] = level/3;
	               	obj->value[3] = level/4;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_HEAD ))
		{
			obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_LEGS ))
		{
		  	obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/6;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_FEET ))
		{
	       		obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/6;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_ARMS ))
		{
	       		obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_SHIELD ))
		{
			obj->value[0] = level/3;
	               obj->value[1] = level/3;
	               obj->value[2] = level/3;
	               obj->value[3] = level/4;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_ABOUT ))
		{
	      		obj->value[0] = level/4;
	               obj->value[1] = level/4;
	               obj->value[2] = level/4;
	               obj->value[3] = level/5;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_WAIST ))
		{
	       		obj->value[0] = level/5;
	               obj->value[1] = level/5;
	               obj->value[2] = level/5;
	               obj->value[3] = level/7;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_WRIST ))
		{
	 		obj->value[0] = level/7;
	               obj->value[1] = level/7;
	               obj->value[2] = level/7;
	               obj->value[3] = level/9;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_HOLD ))
		{
	      		obj->value[0] = 0;
	               obj->value[1] = 0;
	               obj->value[2] = 0;
	               obj->value[3] = 0;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_FLOAT ))
		{
	      		obj->value[0] = 0;
	               obj->value[1] = 0;
	               obj->value[2] = 0;
	               obj->value[3] = 0;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_FACE ))
		{
	       		obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_EYES ))
		{
	      		obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_EAR ))
		{
	      		obj->value[0] = level/10;
	               obj->value[1] = level/10;
	               obj->value[2] = level/10;
	               obj->value[3] = level/10;
		}
		else
		if(CAN_WEAR(obj, OBJWEAR_ANKLE ))
		{
	       		obj->value[0] = level/8;
	               obj->value[1] = level/8;
	               obj->value[2] = level/8;
	               obj->value[3] = level/10;
		}
	}
  	act( "$n begins repairing $p.", ch, obj, NULL, TO_ROOM );
        act( "You begin repairing $p.", ch, obj, NULL, TO_CHAR );	

	if (!IS_IMMORTAL( ch ))
		WAIT_STATE( ch, skill_table[gsn_armorsmith].beats ); 
  	act( "$n has finished repairing $p.", ch, obj, NULL, TO_ROOM );
        act( "You have repaired $p.", ch, obj, NULL, TO_CHAR );
	check_improve(ch, gsn_armorsmith, true, 1);
	return;
}
/**************************************************************************/
void do_repair_weapon( char_data *ch, char *argument )
{
    	char arg1[MIL];
    	char arg2[MIL];
    	obj_data *obj;
    	int cost=0;

    	argument=one_argument(argument,arg1);
    	argument=one_argument(argument,arg2);

    	if ( !IS_NPC( ch ) && ch->level < skill_table[gsn_weaponsmith].skill_level[ch->clss] )
    	{                                          
        	ch->println("You do not know how to repair weapons.");
	        return;
    	}

    	if (IS_NULLSTR(arg1))
    	{
		ch->println("`YRepairarmor `wwhat ?`x");
		ch->println("`#`BSyntax:`w repairweapon <item> <estimate or confirm>`^");
		ch->println("Estimate will provide material cost, confirm will repair.");
		return;
	}

    	if (IS_NULLSTR(arg2))
    	{
		ch->println("`YEstimate `wor `YConfirm`w ?`x");
		ch->println("`#`BSyntax:`w repairweapon <item> <estimate or confirm>`^");
		ch->println("Estimate will provide material cost, confirm will repair.");
		return;
	}

	if ( ( obj = get_obj_carry( ch, arg1 ) ) == NULL )
	{
		ch->println( "You do not have that item." );
		return;
	}

	if(obj->item_type != ITEM_WEAPON)
	{
		ch->println("This item is not a weapon.");
		return;
	}

    	if(obj->cost == 0)
       		cost += 150;
    	else
       		cost = obj->cost/4;

	if(strstr(arg2, "estimate"))
    	{
    		ch->printlnf("It will cost %d silver and %d gold coins to repair %s.",	
			      cost - (cost/100) * 100, cost/100, obj->short_descr );
		return;
    	}

	if(!strstr(arg2, "confirm"))
    	{
		ch->println("`YEstimate `wor `YConfirm`w ?`x");
		ch->println("`#`BSyntax:`w repairweapon <item> <estimate or confirm>`^");
		ch->println("Estimate will provide material cost, confirm will repair.");
		return;
    	}
	
	if(obj->condition == 100)
	{
		ch->println("This item is does not need repair.");
		return;
	}

	if(obj->condition < 39)
	{
		ch->println("This item is too badly damaged.");
		return;
	}

    	if (cost > (ch->gold * 100 + ch->silver))
    	{
		ch->println("You do not have enough money to purchase repair materials for this item.");
		return;
	}

	deduct_cost(ch,cost);

	obj->condition = 100;
	if( (obj->item_type == ITEM_WEAPON ) && obj->pIndexData->vnum != OBJ_VNUM_RANDOM_OBJ)
	{
		obj->value[1]= obj->pIndexData->value[1];
		obj->value[2]= obj->pIndexData->value[2];
	} // End Normal Items
		
	if( obj->item_type == ITEM_WEAPON && obj->pIndexData->vnum == OBJ_VNUM_RANDOM_OBJ)
	{
		int level = obj->level;
		if (obj->value[0] == 4)
		{
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			}
		}
		if (obj->value[0] == 2)
		{
		   	obj->value[1] = weapon_stat_lookup( level-8, 0, 0 );
		   	obj->value[2] = weapon_stat_lookup( level-8, 0, 1 );
		}
		if( obj->value[0] == 1)
		{
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			}
		}
		if(obj->value[0] == 5)
		{
			obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+9, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+9, 0, 1 );
			}
		}
		if(obj->value[0] == 7)
		{
			obj->value[1] = weapon_stat_lookup( level-3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level-3, 0, 1 );
		}
		if(obj->value[0] == 6)
		{
		   	obj->value[1] = weapon_stat_lookup( level-2, 0, 0 );
		   	obj->value[2] = weapon_stat_lookup( level-2, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			}
		}
	}

  	act( "$n begins repairing $p.", ch, obj, NULL, TO_ROOM );
        act( "You begin repairing $p.", ch, obj, NULL, TO_CHAR );	

	if (!IS_IMMORTAL( ch ))
		WAIT_STATE( ch, skill_table[gsn_armorsmith].beats ); 
  	act( "$n has finished repairing $p.", ch, obj, NULL, TO_ROOM );
        act( "You have repaired $p.", ch, obj, NULL, TO_CHAR );
	check_improve(ch, gsn_weaponsmith, true, 1);
	return;
}
/**************************************************************************/
void make_armor( char_data *ch, int armortype, char *name, char *shdesc, int level, char *material )
{
	obj_data       	*obj;
  	OBJ_INDEX_DATA 	*pObjIndex;
	char long_desc[MSL];

	pObjIndex = get_obj_index(OBJ_VNUM_RANDOM_OBJ);
	obj = create_object( pObjIndex );

	obj->item_type = ITEM_ARMOR;
	obj->level = level;
	obj->name = str_dup( name );
	sprintf( long_desc, "%s has been left here.", capitalize(shdesc) );
	obj->short_descr = str_dup( shdesc );
	obj->description = str_dup( long_desc );
	obj->material = str_dup(material);

	switch( armortype )
	{
        	case 1: /* neck */
			   	obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_NECK;
		       	obj->value[0] = level/6;
	            	obj->value[1] = level/6;
	            	obj->value[2] = level/6;
	            	obj->value[3] = level/8;
			break;
            	case 2: /* body */
			obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_TORSO;
		       	obj->value[0] = level/3;
	            	obj->value[1] = level/3;
	            	obj->value[2] = level/3;
	            	obj->value[3] = level/4;
			break;
      		case 3: /* head */
			obj->wear_flags = OBJWEAR_TAKE;
		   	obj->wear_flags += OBJWEAR_HEAD;
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
			break;
       		case 4: /* legs */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_LEGS;
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
			break;
         	case 5: /* feet */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_FEET;
			obj->value[0] = level/5;
	            	obj->value[1] = level/5;
	            	obj->value[2] = level/5;
	            	obj->value[3] = level/6;
			break;
		case 6: /* hands */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_HANDS;
			obj->value[0] = level/5;
	            	obj->value[1] = level/5;
	            	obj->value[2] = level/5;
	            	obj->value[3] = level/6;
			break;
		case 7: /* arm */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_ARMS;
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
			break;
		case 8: /* shield */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_SHIELD;
			obj->value[0] = level/3;
	            	obj->value[1] = level/3;
	            	obj->value[2] = level/3;
	            	obj->value[3] = level/4;
		 	break;
		case 9: /* wrist */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WRIST;
			obj->value[0] = level/7;
	         	obj->value[1] = level/7;
	         	obj->value[2] = level/7;
	         	obj->value[3] = level/9;
			break;
		case 10: /* face */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_FACE;
		      	obj->value[0] = level/10;
	         	obj->value[1] = level/10;
	         	obj->value[2] = level/10;
	         	obj->value[3] = level/10;
			break;
		case 11: /* ankle */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_ANKLE;
			obj->value[0] = level/8;
	         	obj->value[1] = level/8;
	         	obj->value[2] = level/8;
	         	obj->value[3] = level/10;
			break;
	}

   	if (strstr(obj->material, "mithril"))
	{
		SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
	}
   	if (strstr(obj->material, "adamantium"))
	{
		SET_BIT(obj->extra_flags, OBJEXTRA_BURN_PROOF);
	}

	SET_BIT(obj->extra2_flags,OBJEXTRA2_NOSELL);
	SET_BIT(obj->extra2_flags,OBJEXTRA2_PC_CRAFTED);
	obj->cost = number_range(level*10, level*75)/2;
	obj->condition = 100;
	obj_to_char( obj, ch );
	act( "$n holds $p, newly forged, in $s hand.", ch, obj, NULL, TO_ROOM );
	act( "You hold $p, newly forged, in your hand.", ch, obj, NULL, TO_CHAR );
	return;
}
/**************************************************************************/
void do_smitharmor( char_data *ch, char *argument )
{
	obj_data *ore=NULL;
    	obj_data *obj;
    	obj_data *obj_next;
    	char arg[MIL];
	char alevel[MIL];
    	char name[MIL];
    	char *material=NULL;
	bool forge = false;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, name);
	argument = one_argument(argument, alevel);
	
	if(IS_NULLSTR(arg))
	{
        	ch->println("What type of armor are you wanting to smith ?");
        	ch->println("`#`BSyntax:`w smitharmor <armortype> <armorname> <level> <armor short desc>`^");
		ch->println("`#`cValid types: `wneck torso head legs feet hands arm`^");
		ch->println("`#`cValid types: `wshield wrist face ankle`^");
		return;
	}

	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED ))
	{
		if(ch->master)
		{
			ch->master->println( "Not going to happen.");
		}
		return;
	}

    	if ( !IS_NPC( ch ) && ch->level < skill_table[gsn_armorsmith].skill_level[ch->clss] )
    	{                                          
        	ch->println("You do not know how to smith armor.");
	        return;
    	}

	int armortype = 0;

	if(strstr(arg, "neck")) 	armortype = 1;
	if(strstr(arg, "torso")) 	armortype = 2;
	if(strstr(arg, "head")) 	armortype = 3;
	if(strstr(arg, "legs")) 	armortype = 4;
	if(strstr(arg, "feet")) 	armortype = 5;
	if(strstr(arg, "hands")) 	armortype = 6;
	if(strstr(arg, "arm")) 		armortype = 7;
	if(strstr(arg, "shield")) 	armortype = 8;
	if(strstr(arg, "wrist")) 	armortype = 9;
	if(strstr(arg, "face")) 	armortype = 10;
	if(strstr(arg, "ankle")) 	armortype = 11;

	if(armortype == 0)
    	{                                          
        	ch->println("What type of armor are you wanting to smith ?");
        	ch->println("`#`BSyntax:`w smitharmor <armortype> <armorname> <level> <armor short desc>`^");
		ch->println("`#`cValid types: `wneck torso head legs feet hands arm`^");
		ch->println("`#`cValid types: `wshield body wrist face ankle`^");
	        return;
    	}

	if(IS_NULLSTR(name)){
		ch->println("You must give the armor a short description.");
        	ch->println("`#`BSyntax:`w smitharmor <armortype> <armorname> <level> <armor short desc>`^");
	       	ch->println("`#`c(i.e. `wsmitharmor head `Rhelmet`^ 15 `Sa large steel helmet`c)`^");
		return;
	}

	if(IS_NULLSTR(alevel)){
		ch->println("You must give the armor a level.");
         	ch->println("`#`BSyntax:`w smitharmor <armortype> <armorname> <level> <armor short desc>`^");
	       	ch->println("`#`c(i.e. `wsmitharmor head helmet `R15 `Sa large steel helmet`c)`^");
		return;
	}

	if(IS_NULLSTR(argument)){
		ch->println("You must give the armor a short description.");
         	ch->println("`#`BSyntax:`w smitharmor <armortype> <armorname> <level> <armor short desc>`^");
	       	ch->println("`#`c(i.e. `wsmitharmor head helmet 15 `R``#a ``Wlarge ``Ssteel ``Whelmet`c)`^");
		return;
	}

	int olevel = 0;
	olevel = atoi(alevel);

	if(olevel < 1)
	{
		ch->println("Unknown level.");
         	ch->println("`#`BSyntax:`w smitharmor <armortype> <armorname> <level> <armor short desc>`^");
	       	ch->println("`#`c(i.e. `wsmitharmor head helmet 15 `R``#a ``Wlarge ``Ssteel ``Whelmet`c)`^");
		return;
	}

	if(ch->level < olevel)
	{
		ch->println("You cannot make it higher than your level.");
         	ch->println("`#`BSyntax:`w smitharmor <armortype> <armorname> <level> <armor short desc>`^");
	       	ch->println("`#`c(i.e. `wsmitharmor head helmet 15 `R``#a ``Wlarge ``Ssteel ``Whelmet`c)`^");
		return;
	}

	for ( obj = ch->in_room->contents; obj; obj = obj_next ) // Look For Forge
	{
		obj_next = obj->next_content;

		if(obj->pIndexData->vnum == 418) // Look For Forge
		{
			forge = true;
			break;
		}
	}

	if(forge == false)
	{
		ch->println("There is no forge here for you to smith armors with.");
		return;
	}

    	for ( ore = ch->carrying; ore; ore = ore->next_content )
    	{
        	if ( ore->item_type == ITEM_ORE && ore->wear_loc == WEAR_HOLD )
            	break;
    	}
    	if ( !ore )
    	{
        	ch->println("You are not holding any ore to forge.");
        	return;
    	}

	bool goodore = false;

   	if (strstr(ore->material, "mithril"))
	{
		material = str_dup("mithril");
		goodore = true;
	}
   	if (strstr(ore->material, "steel"))
	{	
		material = str_dup("steel");
		goodore = true;
	}
   	if (strstr(ore->material, "adamantium"))
	{
		material = str_dup("adamantium");
		goodore = true;
	}
   	if (strstr(ore->material, "iron"))
	{
		material = str_dup("iron");
		goodore = true;
	}

   	if (goodore == false)
    	{
        	ch->println("This ore will not stand up to the trials of combat.");
        	return;
    	}

  	act( "$n begins hammering $p on the forge.", ch, ore, NULL, TO_ROOM );
        act( "You begin working the ore into armor.", ch, ore, NULL, TO_CHAR );

	if (!IS_IMMORTAL( ch ))
		WAIT_STATE( ch, skill_table[gsn_armorsmith].beats ); 

    	/* Check the skill percentage, memory and reasoning checks) */

    	if ( !IS_NPC(ch) && ( number_percent( ) > ch->pcdata->learned[gsn_armorsmith] ||
              number_percent( ) > ((ch->modifiers[STAT_ST]-7)*5 + (ch->modifiers[STAT_RE]-7)*3) ))
    	{
        	act( "You fail to forge $p into anything useful.", ch, ore, NULL, TO_CHAR );
	        act( "$n fails to forge $p into anything useful.", ch, ore, NULL, TO_ROOM );
		check_improve(ch, gsn_armorsmith, false, 1);
        	extract_obj( ore );
	        return;
    	}

       	extract_obj( ore );
	int modifier = 101 - ch->pcdata->learned[gsn_armorsmith];
    	int chance = number_range(1, modifier);
	olevel -= chance;
	make_armor(ch, armortype, name, argument, olevel, material);
	check_improve(ch, gsn_armorsmith, true, 1);
	return;
}
/**************************************************************************/
void do_skinhide( char_data *ch, char *argument )
{
    static char *header1[] = { "the corpse of the ", "corpse of the ",
                               "the corpse of an ", "corpse of an ",
                               "the corpse of a ", "corpse of a ",
                               "the corpse of " }; // (This one must be last)

    static char *header2[] = { "the corpse ", "corpse "}; // (This one must be last)
	
    char arg[MIL];
    char buf[MSL];
    OBJ_DATA *obj;
    
    one_argument(argument, arg);

	if (IS_SET(ch->affected_by2, AFF2_STONE_GARGOYLE))
	{
		ch->printlnf( "Not while you are a statue." );
		return;
    	}


	if (IS_SET(ch->affected_by2, AFF2_STOCKADE))
	{
		ch->printlnf( "The stocks prevent you from doing that." );
		return;
   }


    if (arg[0] == '\0')
    {
	    ch->println( "`BSyntax: `xskinhide corpse" );
	    return;
    }

    obj = get_obj_list(ch, arg, ch->carrying);
    
    if (obj == NULL)
    {
	    ch->println( "You not carrying any corpse." );
	    return;
    }

    if ( obj->item_type != ITEM_CORPSE_NPC && obj->item_type != ITEM_CORPSE_PC)
    {
	    ch->println( "You cannot skin that." );
	    return;
    }

    if (IS_SET(obj->extra2_flags, OBJEXTRA2_NON_SKINABLE))
    {
	    ch->println( "That corpse cannot be skinned." );
	    return;
    }

    if(!IS_IMMORTAL(ch))
    {
	obj_data *cont;
	obj_data *cont_next;
	for ( cont = obj->contains; cont != NULL; cont = cont_next )
	{
		cont_next = cont->next_content;
		ch->println("The corpse is too full to be skinned.");
		return;
	}
    }
	
    int i;
    for (i = 0; i < 7; i++)
       {
       int len = strlen(header1[i]);
       if ( memcmp(obj->short_descr, header1[i], len) == 0 )
          {
          sprintf( buf, "hide %s", obj->short_descr+len );
          free_string( obj->name );
          obj->name = str_dup(buf);

          break;
          }
       }

    for (i = 0; i < 2; i++)
       {
       int len = strlen(header2[i]);
       if ( memcmp(obj->short_descr, header2[i], len) == 0 )
          {
          sprintf( buf, "A skinned hide %s catches your eye.  ",
             obj->short_descr+len );
          free_string( obj->description );
          obj->description = str_dup( buf );

          sprintf( buf, "the hide %s", obj->short_descr+len );
          free_string( obj->short_descr );
          obj->short_descr = str_dup( buf );

          break;
          }
       }

    obj->item_type = ITEM_HIDE;
    obj->wear_flags = OBJWEAR_HOLD|OBJWEAR_TAKE;
    obj->condition = 100;
    obj->weight = 10;
    obj->level = 1;
    obj->cost = 50;
    obj->pIndexData = get_obj_index( 2033 );    /* So it's not a corpse */

    act( "You get $p from the corpse by skinning it.", ch, obj, NULL, TO_CHAR );
    act( "$n's gets $p from a corpse by skinning it.", ch, obj, NULL, TO_ROOM );
    return;
}
/**************************************************************************/
void make_leather( char_data *ch, int armortype, char *name, int level )
{

	obj_data       	*obj;
  	OBJ_INDEX_DATA 	*pObjIndex;
	char long_desc[MSL];
	char short_desc[MSL];
	char *newshort=NULL;
	char buf[MSL];

	pObjIndex = get_obj_index(OBJ_VNUM_RANDOM_OBJ);
	obj = create_object( pObjIndex );
	obj->item_type = ITEM_ARMOR;

       int len = strlen("hide ");
       if ( memcmp(name, "hide ", len) == 0 )
          {
         	sprintf( buf, "%s", name+len );
          	free_string( newshort );
          	newshort = str_dup( buf );
          }

	switch( armortype )
	{
        	case 1: /* neck gorget*/
			obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_NECK;
		       	obj->value[0] = level/6;
	            	obj->value[1] = level/6;
	            	obj->value[2] = level/6;
	            	obj->value[3] = level/8;
			sprintf( long_desc, "A gorget of %s hide has been left here.", newshort );
			sprintf( short_desc, "gorget of %s hide", newshort );
			obj->name = str_dup("gorget leather hide");
			break;
            	case 2: /* body armor*/
			obj->wear_flags = OBJWEAR_TAKE;
		       	obj->wear_flags += OBJWEAR_TORSO;
		       	obj->value[0] = level/3;
	            	obj->value[1] = level/3;
	            	obj->value[2] = level/3;
	            	obj->value[3] = level/4;
			sprintf( long_desc, "A jerkin of %s hide has been left here.", newshort );
			sprintf( short_desc, "a jerkin of %s hide", capitalize(newshort) );
			obj->name = str_dup("jerkin armor leather hide");
			break;
       		case 3: /* legs greaves*/
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_LEGS;
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
			sprintf( long_desc, "Leather %s hide greaves has been left here.", newshort );
			sprintf( short_desc, "%s hide greaves", newshort );
			obj->name = str_dup("greaves leather hide");
			break;
         	case 4: /* feet boots*/
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_FEET;
			obj->value[0] = level/5;
	            	obj->value[1] = level/5;
	            	obj->value[2] = level/5;
	            	obj->value[3] = level/6;
			sprintf( long_desc, "Leather %s hide boots has been left here.", newshort );
			sprintf( short_desc, "%s hide boots", newshort );
			obj->name = str_dup("boots leather hide");
			break;
		case 5: /* hands gloves*/
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_HANDS;
			obj->value[0] = level/5;
	            	obj->value[1] = level/5;
	            	obj->value[2] = level/5;
	            	obj->value[3] = level/6;
			sprintf( long_desc, "Leather %s hide gloves has been left here.", newshort );
			sprintf( short_desc, "%s hide gloves", capitalize(newshort) );
			obj->name = str_dup("gloves leather hide");
			break;
		case 6: /* arm bracer*/
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_ARMS;
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
			sprintf( long_desc, "A %s hide bracer has been left here.", newshort );
			sprintf( short_desc, "a %s hide bracer", newshort );
			obj->name = str_dup("bracer leather hide");
			break;
		case 7: /* about cloak */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_ABOUT;
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
			sprintf( long_desc, "A cloak of %s hide has been left here.", newshort );
			sprintf( short_desc, "a cloak of %s hide", newshort );
			obj->name = str_dup("cloak leather hide");
		 	break;
		case 8: /* wrist wristguard*/
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WRIST;
			obj->value[0] = level/7;
	         	obj->value[1] = level/7;
	         	obj->value[2] = level/7;
	         	obj->value[3] = level/9;
			sprintf( long_desc, "A %s hide wristguard has been left here.", newshort );
			sprintf( short_desc, "a %s hide wristguard", newshort );
			obj->name = str_dup("wristguard leather hide");
			break;
		case 9: /* waist belt */
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags += OBJWEAR_WAIST;
		      	obj->value[0] = level/5;
	         	obj->value[1] = level/5;
	         	obj->value[2] = level/5;
	         	obj->value[3] = level/7;
			sprintf( long_desc, "A %s hide belt has been left here.", newshort );
			sprintf( short_desc, "a %s hide belt", newshort );
			obj->name = str_dup("belt leather hide");
			break;
		case 10: /* quiver */
    			obj->item_type = ITEM_QUIVER;
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags +=OBJWEAR_BACK;
    			obj->value[0] = 50;
    			obj->value[1] = 1; 
    			obj->value[2] = 0;
			sprintf( long_desc, "A %s hide quiver has been left here.", newshort );
			sprintf( short_desc, "a %s hide quiver", newshort );
			obj->name = str_dup("quiver leather hide");
			break;
		case 11: /* backpack */
    			obj->item_type = ITEM_CONTAINER;
			obj->wear_flags = OBJWEAR_TAKE;
			obj->wear_flags +=OBJWEAR_BACK;
    			obj->value[0] = ch->level * 10;
    			obj->value[1] = 1; 
    			obj->value[2] = 0;
			sprintf( long_desc, "A backpack of %s hide has been left here.", newshort );
			sprintf( short_desc, "a %s hide backpack", newshort );
			obj->name = str_dup("backpack leather hide");
			break;
	}

	obj->level = level;
	obj->short_descr = str_dup( short_desc );
	obj->description = str_dup( long_desc );
	obj->material = str_dup("leather");

	SET_BIT(obj->extra_flags,OBJEXTRA_NONMETAL);
	SET_BIT(obj->extra2_flags,OBJEXTRA2_NOSELL);
	SET_BIT(obj->extra2_flags,OBJEXTRA2_PC_CRAFTED);
	obj->cost = number_range(level*10, level*75)/2;
	obj->condition = 100;
	obj_to_char( obj, ch );
	act( "$n holds $p, newly made, in $s hand.", ch, obj, NULL, TO_ROOM );
	act( "You hold $p, newly made, in your hand.", ch, obj, NULL, TO_CHAR );
	return;
}
/**************************************************************************/
void do_leathercraft( char_data *ch, char *argument )
{
	obj_data *hide=NULL;
    	char arg[MIL];
	char alevel[MIL];
	char *name=NULL;

	argument = one_argument(argument, arg);
	argument = one_argument(argument, alevel);
	
	if(IS_NULLSTR(arg))
	{
        	ch->println("What type of leather product are you wanting to make ?");
        	ch->println("`#`BSyntax:`w leathercraft <armortype> <level>`^");
		ch->println("`#`cValid types: `wgorget armor legs boots gloves bracer`^");
		ch->println("`#`cValid types: `wcloak wrist belt backpack quiver`^");
		return;
	}

	// Check if they're being ordered to do this
	if ( IS_SET( ch->dyn, DYN_IS_BEING_ORDERED ))
	{
		if(ch->master)
		{
			ch->master->println( "Not going to happen.");
		}
		return;
	}

    	if ( !IS_NPC( ch ) && ch->level < skill_table[gsn_leathercraft].skill_level[ch->clss] )
    	{                                          
        	ch->println("You do not know how to work leather.");
	        return;
    	}

	int armortype = 0;

	if(strstr(arg, "gorget")) 	armortype = 1;
	if(strstr(arg, "armor")) 	armortype = 2;
	if(strstr(arg, "legs")) 	armortype = 3;
	if(strstr(arg, "boots")) 	armortype = 4;
	if(strstr(arg, "gloves")) 	armortype = 5;
	if(strstr(arg, "bracer"))	armortype = 6;
	if(strstr(arg, "cloak")) 	armortype = 7;
	if(strstr(arg, "wrist")) 	armortype = 8;
	if(strstr(arg, "belt")) 	armortype = 9;
	if(strstr(arg, "quiver")) 	armortype = 10;
	if(strstr(arg, "backpack")) 	armortype = 11;

	if(armortype == 0)
    	{                                          
        	ch->println("What type of leather product are you wanting to make ?");
        	ch->println("`#`BSyntax:`w leathercraft <armortype> <level>`^");
		ch->println("`#`cValid types: `wgorget armor legs boots gloves bracer`^");
		ch->println("`#`cValid types: `wcloak wrist belt backpack quiver`^");
		return;
    	}


	if(IS_NULLSTR(alevel)){
		ch->println("You must give the object a level.");
         	ch->println("`#`BSyntax:`w leathercraft <armortype> <level>`^");
	       	ch->println("`#`c(i.e. `wleathercraft gloves`^ `R15`c)`^");
		return;
	}

	int olevel = 0;
	olevel = atoi(alevel);

	if(olevel < 1)
	{
		ch->println("Unknown level.");
         	ch->println("`#`BSyntax:`w leathercraft <armortype> <level>`^");
	       	ch->println("`#`c(i.e. `wleathercraft gloves`^ `R15`c)`^");
		return;
	}

	if(ch->level < olevel)
	{
		ch->println("You cannot make it higher than your level.");
          	ch->println("`#`BSyntax:`w leathercraft <armortype> <level>`^");
	       	ch->println("`#`c(i.e. `wleathercraft gloves`^ `R15`c)`^");
		return;
	}

    	for ( hide = ch->carrying; hide; hide = hide->next_content )
    	{
        	if ( hide->item_type == ITEM_HIDE && hide->wear_loc == WEAR_HOLD )
            	break;
    	}
    	if ( !hide )
    	{
        	ch->println("You are not holding any hides to work.");
        	return;
    	}

  	act( "$n begins working $p into something useful.", ch, hide, NULL, TO_ROOM );
        act( "You begin working the hide.", ch, hide, NULL, TO_CHAR );

	if (!IS_IMMORTAL( ch ))
		WAIT_STATE( ch, skill_table[gsn_leathercraft].beats ); 

    	/* Check the skill percentage, memory and reasoning checks) */

    	if ( !IS_NPC(ch) && ( number_percent( ) > ch->pcdata->learned[gsn_leathercraft] ||
              number_percent( ) > ((ch->modifiers[STAT_ST]-7)*5 + (ch->modifiers[STAT_RE]-7)*3) ))
    	{
        	act( "You fail to make $p into anything useful.", ch, hide, NULL, TO_CHAR );
	        act( "$n fails to make $p into anything useful.", ch, hide, NULL, TO_ROOM );
		check_improve(ch, gsn_leathercraft, false, 1);
        	extract_obj( hide );
	        return;
    	}

	name = str_dup(hide->name);

       	extract_obj( hide );
	int modifier = 101 - ch->pcdata->learned[gsn_leathercraft];
    	int chance = number_range(1, modifier);
	olevel -= chance;
	make_leather(ch, armortype, name, olevel);
	check_improve(ch, gsn_leathercraft, true, 1);
	return;
}
/**************************************************************************/
void do_price( char_data *ch, char *argument )
{
    char arg1[MIL];
    char price[MIL];
    OBJ_DATA *obj;
    
    argument = one_argument(argument, arg1);
    argument = one_argument(argument, price);

    if (IS_NULLSTR(arg1))
    {
	    ch->println( "`BSyntax: `xprice <item> <cost>" );
	    return;
    }

    if (IS_NULLSTR(price))
    {
	    ch->println( "`BSyntax: `xprice <item> <cost in silver>" );
	    return;
    }

    obj = get_obj_list(ch, arg1, ch->carrying);
    
    if (obj == NULL)
    {
	    ch->println( "You not carrying that." );
	    return;
    }

    if ( !IS_SET(obj->extra2_flags, OBJEXTRA2_PC_CRAFTED))
    {
	    ch->println( "That is not a crafted item." );
	    return;
    }

    int cost=0;
    int minprice=0;
    cost = atoi(price);
  
    minprice = 10 * obj->level;
    if(cost < obj->cost)
    {
	    ch->printlnf( "That price must be above %d silver.", minprice );
	    return;
    }

    obj->cost = cost;
    ch->printlnf("The price of %s has been set to %d silver.", obj->short_descr, obj->cost);
    return;
}
/**************************************************************************/
void runic_armor( char_data *ch, obj_data *obj )
{
	int		level;

	level = obj->level +1;
	if(level > ch->level)
		level = ch->level;

	obj->level = level;

	if((obj->item_type = ITEM_ARMOR) && IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
	{
		obj->level = level;
		if(IS_SET(obj->wear_flags, OBJWEAR_NECK))
		{
		       	obj->value[0] = level/6;
	            	obj->value[1] = level/6;
	            	obj->value[2] = level/6;
	            	obj->value[3] = level/8;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_TORSO))
		{
		       	obj->value[0] = level/3;
	            	obj->value[1] = level/3;
	            	obj->value[2] = level/3;
	            	obj->value[3] = level/4;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_HEAD))
		{
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_LEGS))
		{
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_FEET))
		{
			obj->value[0] = level/5;
	            	obj->value[1] = level/5;
	            	obj->value[2] = level/5;
	            	obj->value[3] = level/6;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_HANDS))
		{
			obj->value[0] = level/5;
	            	obj->value[1] = level/5;
	            	obj->value[2] = level/5;
	            	obj->value[3] = level/6;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_ARMS))
		{
			obj->value[0] = level/4;
	            	obj->value[1] = level/4;
	            	obj->value[2] = level/4;
	            	obj->value[3] = level/5;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_SHIELD))
		{
			obj->value[0] = level/3;
	            	obj->value[1] = level/3;
	            	obj->value[2] = level/3;
	            	obj->value[3] = level/4;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_WRIST))
		{
			obj->value[0] = level/7;
	         	obj->value[1] = level/7;
	         	obj->value[2] = level/7;
	         	obj->value[3] = level/9;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_FACE))
		{
		      	obj->value[0] = level/10;
	         	obj->value[1] = level/10;
	         	obj->value[2] = level/10;
	         	obj->value[3] = level/10;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_ANKLE))
		{
			obj->value[0] = level/8;
	         	obj->value[1] = level/8;
	         	obj->value[2] = level/8;
	         	obj->value[3] = level/10;
		}
		if(IS_SET(obj->wear_flags, OBJWEAR_EYES))
		{
		       	obj->value[0] = level/10;
	            	obj->value[1] = level/10;
	            	obj->value[2] = level/10;
	            	obj->value[3] = level/10;
		}
		obj->condition = 100;
		act( "$n's $p glows with runic power.", ch, obj, NULL, TO_ROOM );
		act( "Your $p glows with runic power.", ch, obj, NULL, TO_CHAR );
	}
	return;
}
/**************************************************************************/
void runic_weapon( char_data *ch, obj_data *obj )
{
	int		level;

	level = obj->level +1;
	if(level > ch->level)
		level = ch->level;

	obj->level = level;

	if( obj->item_type == ITEM_WEAPON && IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
	{
		if( obj->value[0] == 0) //exotic
		{
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			}
		}
		if( obj->value[0] == 1) //sword
		{
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			}
		}
		if (obj->value[0] == 2) //dagger
		{
		   	obj->value[1] = weapon_stat_lookup( level-8, 0, 0 );
		   	obj->value[2] = weapon_stat_lookup( level-8, 0, 1 );
		}
		if (obj->value[0] == 3) //staff
		{
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			}
		}
		if (obj->value[0] == 4) //mace
		{
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			}
		}
		if(obj->value[0] == 5) //axe
		{
			obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+9, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+9, 0, 1 );
			}
		}
		if(obj->value[0] == 6) //flail
		{
		   	obj->value[1] = weapon_stat_lookup( level-2, 0, 0 );
		   	obj->value[2] = weapon_stat_lookup( level-2, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			}
		}
		if(obj->value[0] == 7) //whip
		{
			obj->value[1] = weapon_stat_lookup( level-3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level-3, 0, 1 );
		}
		if(obj->value[0] == 8) //polearm
		{
			obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+9, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+9, 0, 1 );
			}
		}
		if( obj->value[0] == 9) //sickle
		{
			obj->value[1] = weapon_stat_lookup( level, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+6, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+6, 0, 1 );
			}
		}
		if (obj->value[0] == 10) //spear
		{
			obj->value[1] = weapon_stat_lookup( level-3, 0, 0 );
			obj->value[2] = weapon_stat_lookup( level-3, 0, 1 );
			if (IS_WEAPON_STAT(obj,WEAPON_TWO_HANDS))
			{
				obj->value[1] = weapon_stat_lookup( level+3, 0, 0 );
				obj->value[2] = weapon_stat_lookup( level+3, 0, 1 );
			}
		}
		obj->condition = 100;
		act( "$n's $p glows with runic power.", ch, obj, NULL, TO_ROOM );
		act( "Your $p glows with runic power.", ch, obj, NULL, TO_CHAR );
	}
	return;
}
/**************************************************************************/
void update_runic( char_data *ch, char *argument )
{
	obj_data       	*obj;

	obj = get_eq_char( ch, WEAR_WIELD );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_weapon(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_SECONDARY );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_weapon(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_2WIELD );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_weapon(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_2SECONDARY );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_weapon(ch, obj);
	}

	obj = get_eq_char( ch, WEAR_NECK_1 );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_NECK_2 );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_TORSO );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_HEAD );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_LEGS );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_FEET );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_HANDS );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_ARMS );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_SHIELD );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_ABOUT );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_WAIST );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_WRIST_L );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_WRIST_R );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_EYES );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_EAR_R );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_EAR_L );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_FACE );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_ANKLE_L );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_ANKLE_R );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	obj = get_eq_char( ch, WEAR_BACK );
	if (  obj != NULL )
	{	
		if(IS_SET(obj->extra2_flags, OBJEXTRA2_RUNIC))
			runic_armor(ch, obj);
	}
	return;
}
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
