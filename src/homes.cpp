/**************************************************************************/
//  Homes.cpp - Player Homes         
/***************************************************************************
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

#include "include.h"
#include "recycle.h"
#include "tables.h"

#define HOME_ITEM_1   700 /* fish */
#define HOME_ITEM_2   701 /* turtle */
#define HOME_ITEM_3   702 /* snake */
#define HOME_ITEM_4   703 /* spider */
#define HOME_ITEM_5   704 /* carved chair */
#define HOME_ITEM_6   705 /* highback chair */
#define HOME_ITEM_7   706 /* hard chair */
#define HOME_ITEM_8   707 /* sturdy chair */
#define HOME_ITEM_9   708 /* oak desk */
#define HOME_ITEM_10  709 /* pine desk */
#define HOME_ITEM_11  710 /* carved desk */
#define HOME_ITEM_12  711 /* polished desk */
#define HOME_ITEM_13  712 /* teak desk */
#define HOME_ITEM_14  713 /* plain desk */
#define HOME_ITEM_15  714 /* plush sofa */
#define HOME_ITEM_16  715 /* padded sofa */
#define HOME_ITEM_17  716 /* comfy sofa */
#define HOME_ITEM_18  717 /* fluffed sofa */
// #define HOME_ITEM_19  718 /* comfy sofa */
#define HOME_ITEM_20  719 /* oak end */
#define HOME_ITEM_21  720 /* pine end */
#define HOME_ITEM_22  721 /* carved end */
#define HOME_ITEM_23  722 /* teak end */
#define HOME_ITEM_24  723 /* norm end */
#define HOME_ITEM_25  724 /* oak dining table */
#define HOME_ITEM_26  725 /* pine table */
#define HOME_ITEM_27  726 /* carved table */
#define HOME_ITEM_28  727 /* polished table */
#define HOME_ITEM_29  728 /* teak table */
#define HOME_ITEM_30  729 /* comfy recliner */
#define HOME_ITEM_31  730 /* lamp */
#define HOME_ITEM_32  731 /* lantern */
#define HOME_ITEM_33  732 /* torch */
#define HOME_ITEM_34  733 /* oak dresser */
#define HOME_ITEM_35  734 /* pine dresser */
#define HOME_ITEM_36  735 /* carved dresser */
#define HOME_ITEM_37  736 /* polished dresser */
#define HOME_ITEM_38  737 /* teak dresser */
// #define HOME_ITEM_39  738 norm dresser */
#define HOME_ITEM_40  739 /* oak foot */
#define HOME_ITEM_41  740 /* pine foot */
#define HOME_ITEM_42  741 /* carved foot */
#define HOME_ITEM_43  742 /* polished foot */
#define HOME_ITEM_44  743 /* teak foot */
// #define HOME_ITEM_45  744 norm foot */
#define HOME_ITEM_46  745 /* oak arm */
#define HOME_ITEM_47  746 /* pine arm */
#define HOME_ITEM_48  747 /* carved arm */
#define HOME_ITEM_49  748 /* polished arm */
#define HOME_ITEM_50  749 /* teak arm */
// #define HOME_ITEM_51  750 norm arm */
#define HOME_ITEM_52  751 /* oak war */
#define HOME_ITEM_53  752 /* pine arm */
#define HOME_ITEM_54  753 /* carved arm */
#define HOME_ITEM_55  754 /* polished arm */
#define HOME_ITEM_56  755 /* teak arm */
// #define HOME_ITEM_57  756 norm arm */
#define HOME_ITEM_58  757 /* oak book */
#define HOME_ITEM_59  758 /* pine book */
#define HOME_ITEM_60  759 /* carved book */
#define HOME_ITEM_61  760 /* polished book */
#define HOME_ITEM_62  761 /* teak book */
// #define HOME_ITEM_63  762  norm book 
#define HOME_ITEM_64  763 /* oak chiff */
#define HOME_ITEM_65  764 /* pine chiff */
#define HOME_ITEM_66  765 /* carved chiff */
#define HOME_ITEM_67  766 /* polished chiff */
#define HOME_ITEM_68  767 /* teak chiff */
// #define HOME_ITEM_69  768  norm chiff */
#define HOME_ITEM_70  769 /* royalbed */
#define HOME_ITEM_71  770 /* cot */
#define HOME_ITEM_72  771 /* featherbed */
#define HOME_ITEM_73  772 /* canopybed */
#define HOME_ITEM_74  773 /* postedbed */
#define HOME_ITEM_75  774 /* twinbed */

RESET_DATA *new_reset_data args ( ( void ) );
void add_reset args ( ( ROOM_INDEX_DATA *room, RESET_DATA *loc_reset, int index ) );
void free_reset_data args ( ( RESET_DATA *pReset ) );
void home_buy ( char_data *ch );
void home_sell ( char_data *ch, char *argument );
void home_describe ( char_data *ch );

void do_home ( char_data *ch, char *argument )
{

    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
    RESET_DATA *loc_reset;
    OBJ_DATA *furn;
	char arg1[MIL];
	char arg2[MIL];
	char arg3[MIL];
	char buf[MSL];

	loc = ch->in_room;
	loc_area = ch->in_room->area;
	buf[0] = '\0';

	argument = one_argument( argument, arg1 );
	argument = one_argument( argument, arg2 );
	argument = one_argument( argument, arg3 );

	if ( IS_NPC(ch)
	|| ch == NULL )
		return;

	if ( arg1[0] == '\0' || !strcmp ( arg1, "list" ) )
	{
		ch->printlnf("`#`cWhat about a home ?`^");
    		ch->printlnf("\n\r`#`BSyntax:`^ Home     (buy, sell, furnish, describe)");
		ch->printlnf("\n\rExtended:\n\rHome (buy, sell, furnish, describe)");
		ch->printlnf("`#`cBuy`^      (purchase a home in the room you are standing in)");
		ch->printlnf("`#`cSell`^     (confirm - does not prompt for confirmation!)");
		ch->printlnf("`#`cFurnish`^  (allows purchases of items)");
		ch->printlnf("`#`cDescribe`^ (describe the room - uses the OLC editor)");
		return;
	}
	if ( !is_name( ch->in_room->area->file_name, "houses.are" ) )
	{
		ch->printlnf("There is no house here!");
		return;
	}

	/* Find out what the argument is, if any */
	if      ( !strcmp ( arg1, "buy" ) )        home_buy ( ch );
	else if ( !strcmp ( arg1, "sell" ) )       home_sell ( ch, arg2 );
	else if ( !strcmp ( arg1, "describe" ) )   home_describe ( ch );
	else if ( !strcmp ( arg1, "furnish" ) )
	/* Home furnish was left in here because I didn't feel like
	 * redoing all the arguments - Dalsor
	 */
	{
		if ( !is_room_owner ( ch, loc ) )
		{
			ch->printlnf("But you do not own this room!");
			return;
		}
		if ( arg2[0] == '\0' )
		{
		ch->printlnf("This command allows you to furnish your home.");
		ch->printlnf("You must be carrying gold to purchase furnishings,");
		ch->printlnf("and be standing in your home. You cannot have more");
		ch->printlnf("than five items in your home.");
		ch->printlnf("\n\rSyntax: Home (furnish) (item name)");
		ch->printlnf("   Aquarium    75 gold (fish, turtle, snake, spider)");
		ch->printlnf("   Chair       50 gold (sturdy, highback, carved, hard)");
		ch->printlnf("   Desk        75 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Sofa        75 gold (plush, padded, comfortable, fluffed)");
		ch->printlnf("   Endtable    10 gold (oak, pine, carved, teak)");
		ch->printlnf("   Table       75 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Recliner    75 gold");
		ch->printlnf("   Lamp        20 gold");
		ch->printlnf("   Lantern     15 gold");
		ch->printlnf("   Torch       10 gold");
		ch->printlnf("   Dresser     50 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Footchest   50 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Armoire     75 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Wardrobe    75 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Bookcase    75 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Chifferobe  75 gold (oak, pine, carved, polished, teak)");
		ch->printlnf("   Cot         10 gold");
		ch->printlnf("   Featherbed  25 gold");
		ch->printlnf("   Twinbed     50 gold");
		ch->printlnf("   Postedbed   75 gold");
		ch->printlnf("   Canopybed   85 gold");
		ch->printlnf("   Royalbed    99 gold");
		return;
		}
		else if ( !strcmp ( arg2, "aquarium" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
		        
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "fish" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_1)); }
				else if ( !strcmp ( arg3, "turtle" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_2)); }
				else if ( !strcmp ( arg3, "snake" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_3)); }
				else if ( !strcmp ( arg3, "spider" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_4)); }
				else {
					ch->printlnf("Invalid aquarium type.");
				    return;	}
			}
			else
			{
				ch->printlnf("Invalid aquarium type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
			SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			        
			return;
		}
		else if ( !strcmp ( arg2, "chair" ) )
		{
			if ( ch->gold < 50 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "sturdy" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_8)); }
				else if ( !strcmp ( arg3, "highback" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_6)); }
				else if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_5)); }
				else if ( !strcmp ( arg3, "hard" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_7)); }
				else
				{
					ch->printlnf("Invalid chair type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid chair type.");
			    return;
			}
			ch->gold -= 50;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "desk" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_9)); }
				else if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_10)); }
				else if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_11)); }
				else if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_12)); }
				else if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_13)); }
				else
				{
					ch->printlnf("Invalid desk type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid desk type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "sofa" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "plush" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_15)); }
				else if ( !strcmp ( arg3, "padded" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_16)); }
				else if ( !strcmp ( arg3, "comfortable" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_17)); }
				else if ( !strcmp ( arg3, "fluffed" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_18)); }
				else
				{
					ch->printlnf("Invalid sofa type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid sofa type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "endtable" ) )
		{
			if ( ch->gold < 10 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_20)); }
				else if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_21)); }
				else if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_22)); }
				else if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_23)); }
				else
				{
					ch->printlnf("Invalid endtable type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid endtable type.");
			    return;
			}
			ch->gold -= 10;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        		SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "table" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_25)); }
				else if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_26)); }
				else if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_27)); }
				else if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_28)); }
				else if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_29)); }
				else
				{
					ch->printlnf("Invalid table type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid table type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "recliner" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->gold             -= 75;
		    furn = create_object(get_obj_index(HOME_ITEM_30));
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();

			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "lamp" ) )
		{
			if ( ch->gold < 20 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->printlnf("Your lamp will provide light for your home for a time.");
			ch->gold             -= 20;
		    furn = create_object(get_obj_index(HOME_ITEM_31));
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}

		else if ( !strcmp ( arg2, "lantern" ) )
		{
			if ( ch->gold < 15 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->printlnf("Your lantern will provide light for your home for a time.");
			ch->gold             -= 15;
		    furn = create_object(get_obj_index(HOME_ITEM_32));
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "torch" ) )
		{
			if ( ch->gold < 10 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->printlnf("Your torch will provide light for your home for a time.");
			ch->gold             -= 10;
		    furn = create_object(get_obj_index(HOME_ITEM_33));
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "dresser" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_34)); }
				else if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_35)); }
				else if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_36)); }
				else if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_37)); }
				else if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_38)); }
				else
				{
					ch->printlnf("Invalid dresser type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid dresser type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "footchest" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_40)); }
				else if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_41)); }
				else if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_42)); }
				else if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_43)); }
				else if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_44)); }
				else
				{
					ch->printlnf("Invalid footchest type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid footchest type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "armoire" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_46)); }
				if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_47)); }
				if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_48)); }
				if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_49)); }
				if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_50)); }
				else
				{
					ch->printlnf("Invalid armoire type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid armoire type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "wardrobe" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_52)); }
				if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_53)); }
				if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_54)); }
				if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_55)); }
				if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_56)); }
				else
				{
					ch->printlnf("Invalid wardrobe type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid wardrobe type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "bookcase" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_58)); }
				if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_59)); }
				if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_60)); }
				if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_61)); }
				if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_62)); }
				else
				{
					ch->printlnf("Invalid bookcase type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid bookcase type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "chifferobe" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
		    if ( arg3[0] != '\0' )
		    {
				if ( !strcmp ( arg3, "oak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_64)); }
				if ( !strcmp ( arg3, "pine" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_65)); }
				if ( !strcmp ( arg3, "carved" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_66)); }
				if ( !strcmp ( arg3, "polished" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_67)); }
				if ( !strcmp ( arg3, "teak" ) )
				    {furn = create_object(get_obj_index(HOME_ITEM_68)); }
				else
				{
					ch->printlnf("Invalid chifferobe type.");
				    return;
				}
			}
			else
			{
				ch->printlnf("Invalid chifferobe type.");
			    return;
			}
			ch->gold -= 75;
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "cot" ) )
		{
			if ( ch->gold < 10 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->gold             -= 10;
		    furn = create_object(get_obj_index(HOME_ITEM_71));
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "featherbed" ) )
		{
			if ( ch->gold < 25 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->gold             -= 25;
		    furn = create_object(get_obj_index(HOME_ITEM_72));
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "twinbed" ) )
		{
			if ( ch->gold < 5000 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->gold             -= 50;
		    furn = create_object(get_obj_index(HOME_ITEM_75));
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "postedbed" ) )
		{
			if ( ch->gold < 75 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->gold             -= 75;
		    furn = create_object(get_obj_index(HOME_ITEM_74));
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "canopybed" ) )
		{
			if ( ch->gold < 85 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->gold             -= 85;
		    furn = create_object(get_obj_index(HOME_ITEM_73));
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			obj_to_room ( furn,ch->in_room );
	        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else if ( !strcmp ( arg2, "royalbed" ) )
		{
			if ( ch->gold < 99 )
			{
				ch->printlnf("You do not have enough gold for this purchase.");
				return;
			}
			ch->gold             -= 99;
		    furn = create_object(get_obj_index(HOME_ITEM_70));
			loc_reset               = new_reset_data();
			loc_reset->command      = 'O';
			loc_reset->arg1         = furn->pIndexData->vnum;
			loc_reset->arg2         = 0;
			loc_reset->arg3         = loc->vnum;
			loc_reset->arg4         = 0;
			add_reset( loc, loc_reset, 0 );
			ch->printlnf("You have been deducted %d for your purchase.",furn->cost);
			obj_to_room ( furn,ch->in_room );
	        SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
			return;
		}
		else
		{
			ch->printlnf("You have not provided the name of the item to purchase.");
			return;
		}
	}
	else
	{
		ch->printlnf("No such Home Command. See HELP HOME for more informatio.");
		return;
	}
	return;
}

void home_buy ( char_data *ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
	char buf[MSL];

	loc = ch->in_room;
	loc_area = ch->in_room->area;
	if ( loc->owner[0] == '\0' )
	{
		if ( ch->gold < 1000 )
		{
		ch->printlnf("This command allows you to buy a home.");
		ch->printlnf("You must be standing in the room to buy.");
		ch->printlnf("You start with a blank, untitled room that is void of");
		ch->printlnf("furnishings and light. A single door allows entrance.");
		ch->printlnf("A home costs 1000 GOLD. You must be carrying the coins.");
		ch->printlnf("\n\rSyntax: Home buy");
		return;
		}
		else
		{
	    free_string ( loc->owner );
	    loc->owner = str_dup ( ch->name );
	    ch->gold -= 1000;
	    free_string ( loc->name );
	    sprintf ( buf, "%s's Home", ch->name );
	    loc->name = str_dup ( buf );
	    ch->printlnf("Congratulations on purchasing your new home!");
	    ch->printlnf("You are now 1,000 gold coins lighter and the owner");
	    ch->printlnf("of the room in which you now stand! Be sure to describe");
	    ch->printlnf("your home and purchase furnishings for it.");
 	    SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
	    return;
		}
	}
	else
	{
		ch->printlnf("This home is already owned.");
		return;
	}
	return;
}

void home_sell ( char_data *ch, char *argument )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;
	RESET_DATA *current_reset;
	RESET_DATA *next_reset;
    OBJ_DATA *furn;
    OBJ_DATA *furn_next;

	loc = ch->in_room;
	loc_area = ch->in_room->area;

	if ( !is_room_owner ( ch, loc ) )
	{
		ch->printlnf("But you do not own this room!");
		return;
	}
	if ( argument[0] == '\0' )
	{
		ch->printlnf("This command allows you to sell your home.");
		ch->printlnf("You will no longer own your home once it is sold,");
		ch->printlnf("and you will be compensated half the cost of the home,");
		ch->printlnf("not including items you have purchased.");
		ch->printlnf("You must be standing in the room which you own.");
		ch->printlnf("\n\rSyntax: Home (sell) (confirm)");
		return;
	}
	else if ( !strcmp ( argument, "confirm" ) )
	{
	    free_string ( loc->owner );
	    loc->owner = str_dup ( "" );
	    ch->gold += 500;
	    free_string ( loc->name );
	    loc->name = str_dup ( "An Abandoned Home" );

	    if ( !ch->in_room->reset_first )
	    {
			return;
		}
		/* Thanks to Edwin and Kender for the help with killing
		 * resets. Pointed out some real problems with my handling
		 * of it. Thanks a bunch guys.
		 * This is the exact way Kender put it, and it works great!
		 */
		for( current_reset = ch->in_room->reset_first; current_reset;
		    current_reset = next_reset)
		{
		   next_reset = current_reset->next;
		   free_reset_data(current_reset);
		}

		ch->in_room->reset_first = NULL;

		for ( furn = ch->in_room->contents; furn != NULL; furn = furn_next )
		{
		    furn_next = furn->next_content;
	        extract_obj( furn );
		}
                SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
		ch->printlnf("Your home has been sold and you are now 500 gold coins richer!");
        
        return;
	}
	else
	{
		ch->printlnf("This command allows you to sell your home.");
		ch->printlnf("You will no longer own your home once it is sold,");
		ch->printlnf("and you will be compensated half the cost of the home,");
		ch->printlnf("not including items you have purchased.");
		ch->printlnf("You must be standing in the room which you own.");
		ch->printlnf("\n\rSyntax: Home (sell) (confirm)");
		return;
	}
}

void home_describe ( char_data *ch )
{
    ROOM_INDEX_DATA *loc;
    AREA_DATA *loc_area;

	loc = ch->in_room;
	loc_area = ch->in_room->area;

	if ( !is_room_owner ( ch, loc ) )
	{
		ch->printlnf("But you do not own this room!");
		return;
	}
	else
	{
		ch->printlnf("This command allows you to describe your home.");
		ch->printlnf("You should not describe items that are in the room,");
		ch->printlnf("rather allowing the furnishing of the home to do that.");
		ch->printlnf("If you currently own this room, you will be placed into.");
		ch->printlnf("the room editor. Be warned that while in the room editor,");
		ch->printlnf("you are only allowed to type the description. If you are");
		ch->printlnf("unsure or hesitant about this, please note the Immortals,");
		ch->printlnf("or better, discuss the how-to's with a Builder.");
		ch->printlnf("Syntax: Home (describe)");
		SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
    		string_append( ch, &loc->description );
        	SET_BIT( loc_area->olc_flags, OLCAREA_CHANGED );
		return;
	}
}





/**************************************************************************/


