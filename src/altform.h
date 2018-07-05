/**************************************************************************/
// altform.h - Alternate Forms code by Reath
/***************************************************************************
 * The Dawn of Time v1.69r (c)1997-2004 Michael Garratt                    *
 * >> A number of people have contributed to the Dawn codebase, with the   *
 *    majority of code written by Michael Garratt - www.dawnoftime.org     *
 * >> To use this source code, you must fully comply with the dawn license *
 *    in licenses.txt... In particular, you may not remove this copyright  *
 *    notice.                                                              *
 **************************************************************************/

#ifndef ALTFORM_H
#define ALTFORM_H

/**************************************************************************/
bool setup_dragon(char_data *ch);
bool setup_werebeast(char_data *ch);
void revert_race(char_data *ch);
void setup_altform(char_data *ch, int alttype, int altsubtype);
void setup_altmodifiers(char_data *ch);
void setup_altshape(char_data *ch);
void setup_altbits(char_data *ch);
void setup_altappearance(char_data *ch);
bool form_alternate(char_data *ch);
bool form_original(char_data *ch);

#endif // ALTFORM_H
