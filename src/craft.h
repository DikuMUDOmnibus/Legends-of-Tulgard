#ifndef CRAFT_H
#define CRAFT_H


#include "security.h"
#include "o_lookup.h"
#include "include.h" 

#define EDIT_CRAFT(Ch, Obj)		( Obj = (OBJ_DATA *)Ch->desc->pEdit )

#ifndef DECLARE_CRAFT_FUN
typedef	bool CRAFT_FUN(class char_data *ch, char *argument);
#define DECLARE_CRAFT_FUN( fun )	CRAFT_FUN fun
#endif


#define STEP_1					1
#define STEP_2					2
#define STEP_3					3
#define STEP_4					4
#define STEP_5					5
#define STEP_6					6 
#define STEP_7					7
#define STEP_8					8

#define CRAFT_NONE				0
#define CRAFT_WEAPONSMITH		1
/**************************************************************************/
/*
 * Structures for crafting!
 */
struct craft_cmd_type
{
    char		*name;
    CRAFT_FUN	*craft_fun;
};

struct craft_cmdlist_table
{
	int			step;
	CRAFT_FUN	*cmd_table;
};

struct craft_type
{
    int			type;
	CRAFT_FUN	*cmdlist_table;
};
/**************************************************************************/
//weaponsmith commands
DECLARE_CRAFT_FUN( weaponsmith_mixing_add );
DECLARE_CRAFT_FUN( weaponsmith_mixing_refine );
DECLARE_CRAFT_FUN( weaponsmith_mixing_show );
DECLARE_CRAFT_FUN( weaponsmith_mixing_slowdown );
DECLARE_CRAFT_FUN( weaponsmith_mixing_speedup );

/**************************************************************************/
//Weaponsmith meta-table functions
DECLARE_CRAFT_FUN( cstep_weaponsmith_table );
DECLARE_CRAFT_FUN( weaponsmith_mixing );
/**************************************************************************/
//DECLARE NEW CRAFT COMMAND LISTS HERE
struct craft_cmd_type weapsmith_mixing[] =
{
//  {   command,     craft_function  },
	{   "show",		weaponsmith_mixing_show		},
	{   "add",		weaponsmith_mixing_add		},
	{   "refine",	weaponsmith_mixing_refine	},
	{   "slowdown", weaponsmith_mixing_slowdown	},
	{   "speedup",	weaponsmith_mixing_speedup	},
	{	NULL,		0							}
};
/**************************************************************************/
//DECLARE NEW CRAFT STEP LISTS HERE

struct craft_cmdlist_table weaponsmith_table[] =
{
//  {   craft_step,     craft_command_table	},
    {   STEP_1,	weaponsmith_mixing   },
//	{   STEP_2, weaponsmith_shaping	 },
//	{   STEP_3, weaponsmith_final    },
    {	0,			0						}
};

/**************************************************************************/
//DECLARE NEW CRAFT TYPES LISTS HERE
struct craft_type craft_table[] =
{
//  {   craft number	 ,	craft_step_table	},
    {   CRAFT_WEAPONSMITH,  cstep_weaponsmith_table },
    {	0,			0,					}
};

/**************************************************************************/
//DECLARE NEW WEAPON STYLE LISTS HERE
struct wstyle_type	wstyle_sword[] =
{// { Name,		   Subtype, Amount,	  Diff,Damword,Dammod, Hitmod } 
	{ "longsword",		-1,		10,		15,		 3,		5,		5 }, //0
	{ "broadsword",		-1,		30,		25,		25,		2,		8 },
	{ "rapier",			-1,		20,		 7,		 1,		7,		3 },
	{ "scimitar",		-1,		15,		20,		21,		3,		7 },
	{ "shortsword",		-1,		15,		10,		 3,		8,		2 },
	{ "katana",			-1,		40,		25,		 1,		5,		6 }, //5
	{ "cutlass",		 3,		20,		7,		 1,		7,		3 },
	{ "nodachi",		 1,		20,		7,		 1,		7,		3 },
	{ "wakizashi",		 4,		20,		7,		 1,		7,		3 },
	{ "saber",			-1,		20,		7,		 1,		7,		3 },
	{ NULL,				-1,		 0,		0,		 0,		0,		0 }
};

struct wstyle_type	wstyle_staff[] =
{// { Name,		   Subtype, Amount,	  Diff,Damword,Dammod, Hitmod } 
	{ "quarterstaff",	-1,		20,		20,		 3,		5,		5 }, //0
	{ "longstaff",		-1,		30,		25,		25,		2,		8 },
	{ "shortstaff",		-1,		15,		15,		 1,		7,		3 },
	{ "bo",				-1,		17,		16,		21,		3,		7 },
	{ "hanbo",			-1,		12,		12,		 3,		8,		2 },
	{ NULL,				-1,		 0,		0,		 0,		0,		0 }
};

#endif // CRAFT_H

