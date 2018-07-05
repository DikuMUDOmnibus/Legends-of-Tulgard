/**************************************************************************/
// plants.h
/***************************************************************************
 * Whispers of Times Lost (c)2001-2003 Brad Wilson                         *
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

/*
 * The amount above desired water a plant must be to increase its
 * health.
 */
#define PLANT_HEALTH_DELTA 100

/*
 * Plant appears simply as a sprout until this age is met.
 */
#define PLANT_RECOGNIZABLE_AGE 10


#define PLANT_TYPE_TREE   0
#define PLANT_TYPE_SHRUB  1
#define PLANT_TYPE_BUSH   2
#define PLANT_TYPE_STEM   3
#define PLANT_TYPE_CACTUS 4
#define PLANT_TYPE_MAX    5

struct plant_class
{
  struct plant_class	*next;
  char			*name;
  int			type;            /* tree, shrub, vine, etc. */
  int			health_flower;   /* Health to produce flowers/fruit */
  int			health_max;      /* Maximum Health */
  int			water_desired;   /* Water to maintain health. */
};


struct plant_species
{
  struct plant_species	*next;
  struct plant_class	*class;
  char			*name;
  int			flower_effort; /* plant updates to produce 1 fruit */
  int			flower_vnum;
  int			flower_max;    /* max flowers that plant can have  */
  int			fruit_vnum;
  int			fruit_max;     /* max fruits that plant can have   */
  int			growth_seed;   /* negative number, 0 = sprouts     */
  int			growth_flowering; /* age plant starts producing seeds */
  int			growth_max;    /* plant begins to die at this age  */
};


struct plant_instance
{
  struct plant_instance	*next;
  struct plant_species	*species;
  int			quantity;
  int			health;
  int			water;      /* plants can be watered individually */
  int			fruit;
  int			flower;
  int			growth;
  int			eiv_fruit;   /* last fruit generation */
};
