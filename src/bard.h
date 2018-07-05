/**************************************************************************/
// bard.h - Bard Code
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#ifndef BARD_H
#define BARD_H

// bardic utilities
void 	song_update (void);

// bardic songs
DECLARE_SPELL_FUN( spell_bardic_visions		);
DECLARE_SPELL_FUN( spell_bardic_peace		);
DECLARE_SPELL_FUN( spell_enhanced_healing	);
DECLARE_SPELL_FUN( spell_summon_light		);
DECLARE_SPELL_FUN( spell_kill_magic		);
DECLARE_SPELL_FUN( spell_sober_drunks		);
DECLARE_SPELL_FUN( spell_fog_of_truth		);
DECLARE_SPELL_FUN( spell_battle_rage		);
DECLARE_SPELL_FUN( spell_create_sustinance	);
DECLARE_SPELL_FUN( spell_sleepytime		);
DECLARE_SPELL_FUN( spell_natures_howl		);
DECLARE_SPELL_FUN( spell_dreamscape		);
#endif // BARD_H
