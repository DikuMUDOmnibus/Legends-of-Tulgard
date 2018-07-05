/**************************************************************************/
// territory.h - Territory Option
/***************************************************************************
 * Whispers of Times Lost (c)1998-2004 Brad Wilson (wotlmud@bellsouth.net) *
 * >> If you use this code you must give credit in your help file as well  *
 *    as leaving this header intact.                                       *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses below. In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/
#ifndef TEDIT_H
#define TEDIT_H

DECLARE_OLC_FUN( tedit_name		);
DECLARE_OLC_FUN( tedit_show		);
DECLARE_OLC_FUN( tedit_jailvnum		);
DECLARE_OLC_FUN( tedit_stockade		);
DECLARE_OLC_FUN( tedit_crimeflags	);
DECLARE_OLC_FUN( tedit_description	);
DECLARE_OLC_FUN( tedit_taxrate );
DECLARE_OLC_FUN( tedit_taxes );
DECLARE_OLC_FUN( tedit_mayor );
DECLARE_OLC_FUN( tedit_number );

/**************************************************************************/
const struct olc_cmd_type tedit_table[] =
{
//	{	COMMAND			FUNCTION				}

	{	"commands",		show_commands		},
	{	"description",		tedit_description	},
	{	"name",			tedit_name		},
	{	"show",			tedit_show		},
	{	"number",		tedit_number		},
	{	"taxrate",		tedit_taxrate		},
	{	"taxes",		tedit_taxes		},
	{	"mayor",		tedit_mayor		},
	{	"jailvnum",		tedit_jailvnum		},
	{	"stockade",		tedit_stockade		},
	{	"crimeflags",		tedit_crimeflags	},
	{	"?",			show_help				},
	{	NULL,			0,						}
};
/**************************************************************************/
#endif // TEDIT_H

