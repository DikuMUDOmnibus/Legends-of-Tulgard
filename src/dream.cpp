/**************************************************************************/
// dream.cpp - Random Dream Code by Brad Wilson
/***************************************************************************
 * Whispers of Times Lost (c)1998-2005 Brad Wilson (ixliam@gmail.com)      *
 * >> If you use this code you must give credit in your help file as well  *
 *    as leaving this header intact.                                       *
 * >> To use this source code, you must fully comply with all the licenses *
 *    in licenses below. In particular, you may not remove this copyright  *
 *    notice.                                                              *
 ***************************************************************************
 * Dreams 1-6 writen by Metamorph And Omega Maelstrom  (c)1999             *
 ***************************************************************************/

#include "include.h" // dawn standard includes

DECLARE_DO_FUN(do_wake      );
DECLARE_DO_FUN(do_yell      );
DECLARE_DO_FUN(do_say       );

/**************************************************************************/
void dream_kill(char_data *ch)
{
	ch->dreamp = 0;
	ch->dreamn = 0;
	save_char_obj(ch);
	return;
}
/**************************************************************************/
void dream_reset(char_data *ch)
{

	ch->dreamp = 0;
	ch->dreamn = number_range(0,26);
	save_char_obj(ch);
	return;
}
/**************************************************************************/
void do_dreamtest( char_data *ch, char *argument ) // Testing procedure
{
   	if (!(ch->position==POS_SLEEPING))
   	{
   		ch->printlnf("Putting you to sleep....");
   	}
   	ch->position=POS_SLEEPING;
   	dream_reset(ch);
   	return;
}
/**************************************************************************/
void dream_update(void)
{
     char_data *ch;

     for ( ch = char_list; ch; ch = ch->next )
     {
	if ( ch->desc == NULL || ch->desc->connected_state != CON_PLAYING )
	   continue;

        if(IS_NPC(ch))
                return;
                
	// If the character is asleep.
	if(!IS_AWAKE(ch))
	{
		if(ch->dreamn == 0)
		{
			ch->print("`#`SYou dream:`c ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou're walking on a dead, grey path in the dark woods...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cNothing looks familiar to you, yet this image is clear.`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cWherever you are, you must get out. They're watching you....`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cThey know who you are, and know where you've come from.`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cYou hear a voice, its telling you to be careful and prepare.`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 1)
		{
			ch->print("`#`SYou dream:`c ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cWhoa! you thought you were sleeping, but now you're standing...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cIn the center of a stage! The lights shine down on you...`^");
				ch->dreamp++;
				break;
			    case 2:
    				ch->println("`#`cThe crowd cheers as you are standing there dressed in your armor...`^");
				ch->dreamp++;
				break;
			    case 3:
    				ch->println("`#`cThe crowd settles..awaiting your next move.. you don't know what to do!`^");
				ch->dreamp++;
				break;
			    case 4:
    				ch->println("`#`cAn overwhelming effect of stage fright washes over you! You panic...`^");
				ch->dreamp++;
				break;
			    case 5:
    				ch->println("`#`cRisking everyone laughing at you, you start to sing a song..`^");
				ch->dreamp++;
				break;
			    case 6:
    				ch->println("`#`cThe crowd starts booing at your performance! You run off the stage!`^");
				act( "Mumbling strange words, $n passes into a deeper sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		// Nightmare Dream
		else if(ch->dreamn == 2)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cMasses are moving in a void. They are flowing though it...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cThey are closing in on you. You look closer. You start maneuvering the masses...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou clench your fist, but it feels smaller than usual. You open it..`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cAnd reclench it, but it still feels small.. and hard, it feels like a rock...`^");
				ch->print("`#`SYou dream:`^ ");
				ch->println("`#`cYou maneuver the masses with the power of your mind. `^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cIt's a game. The masses are closing in. You look in closer...`^");
				ch->print("`#`SYou dream:`^ ");
				ch->println("`#`cThey're getting closer...you can't win, you can't escape!`^");
				ch->println("`#`RYou wake up in a `Ccold sweat`R, fearing for your life.`x");
				do_wake(ch, "");
				dream_kill(ch);
				do_yell(ch,"AAAAAGGGGHHHH! The HORROR!!!");
				break;
			}
		}
		else if(ch->dreamn == 3)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou are walking in a forest, and you don't know where it is..`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou've never been here before, but yet it seems so familiar to you...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cAs you're walking, you come across a pond. You see your reflection...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cOr do you?.. It doesn't look like you.. but it has to be...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cYou somehow feel better about knowing your true self.`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		// Nightmare Dream
		else if(ch->dreamn == 4)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou think about vampires and one appears before you...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cIt reaches for you and you scream in terror...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cA swirl of eternal darkness flows around you...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cThe fangs sink into your neck, draining your blood...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`RYou wake up in a `Ccold sweat`R, fearing for your life.`x");
				do_wake(ch, "");
				dream_kill(ch);
				do_yell(ch,"AAAAAGGGGHHHH! The HORROR!!!");
				break;
			}
		}
		else if(ch->dreamn == 5)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou shudder from the cold as you see a scene of a vast frozen wasteland...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou begin to stumble around in the cold snow drifts and frozen ponds...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cThe cold begins to reach your fingers and toes as you begin to lose feeling...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cAll of a sudden a man dressed in black appears before you...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cIn one hand he is holding a dagger and in the other, `YYOUR HEAD!!`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cAs you begin to scream it slowly fades away as you feel cold...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cThe frost from his cold blade is cutting through you...`^");
				ch->println("`#`RYou wake up in a `Ccold sweat`R, fearing for your life.`x");
				do_wake(ch, "");
				dream_kill(ch);
				do_yell(ch,"AAAAAGGGGHHHH! The HORROR!!!");
				break;
			}
		} 
		else if(ch->dreamn == 6)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou see an ad on the television .. wait, whats a television ?!`^");
				ch->print("`#`SYou dream:`^ ");
				ch->println("`#`cThey don't exist yet! Hmm anyway back to your dream...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou see an ad on the television about a new lottery...`^");				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou run down to the ticket place and buy 4 tickets.....`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cIt is now a few days later, the drawing is about to begin!!`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cThey begin to draw the numbers.. you look down at your ticket...`^");
				ch->dreamp++;
				break;
			    case 5:
				if (ch->perm_stats[STAT_IN] >= 50)
      				{
       					ch->println("`#`cYou win the jackpot! You are rich rich! RICH!!!");
				ch->print("`#`SYou dream:`^ ");
       					ch->println("`#`cOr so you feel anyway...`^");
      					ch->gold=ch->gold+10;
       					ch->println("`#`YYou win 10 gold coins!!!`^");
       					act( "$n appears to be rolling around in gold coins in their sleep!", ch, NULL, NULL, TO_ROOM);
       					dream_kill(ch);
					break;
      				}
     				if (ch->perm_stats[STAT_IN] >= 30)
      				{
       					ch->println("`#`cYou win second prize! You see the world in a whole new aspect!!");
      					ch->gold=ch->gold+4;
       					ch->println("`#`YYou win 4 gold coins!!!");
       					act( "$n is clutching 4 gold coins in $s hand!!", ch, NULL, NULL, TO_ROOM);
       					dream_kill(ch);
					break;
      				}
     				if (ch->perm_stats[STAT_IN] >=20 )
      				{
       					ch->println("`#`cYou win third prize, not bad for your first lottery!`^");
      					ch->gold=ch->gold+3;
       					ch->println("`#`YYou win 3 gold coins!!");
       					act( "$n clutches 3 gold coins in $s hand!!", ch, NULL, NULL, TO_ROOM);
       					dream_kill(ch);
					break;
      				}
     				ch->println("`#`cNone of your tickets win.. bummer...`^"); 
				act( "Mumbling strange words, $n passes into a deeper sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		} 
		else if(ch->dreamn == 7)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cA warm incredible feeling of happiness washes over you....`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cIt feels like you are floating on a bed of air...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cThousands of butterflies are fluttering over you...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cA far off harp peacefully fills the air with soul-stirring music...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cAs you open your eyes, you realize the sweet aroma filling the air..`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cAre from the hundreds of flowers on and around you, with more floating in the air...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cYou close your eyes, hoping to retain these feelings forever.`^");
	    			act( "The air is filled with sweetness as hundreds of flower petals float down from the sky to cover $n as $e lies sleeping.", ch, NULL, NULL, TO_ROOM);
				dream_reset(ch);
				break;
			}
		} 
		else if(ch->dreamn == 8)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cCreak...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cSomeone... or something is approaching...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou struggle to awaken as you hear the silver whisper of a sword being drawn...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cRed eyes glare evilly at you as you frantically roll out of bed reaching...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cIn vain for your weapon...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cSwish...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cDarkness...`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		} 
		else if(ch->dreamn == 9)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou are walking down the street on a bright and sunny morning...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cPeople are laughing and smiling happily as you walk by and you smile back...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cIt sure is nice to see people happy for once...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cYou continue your walk down the street and stop to look in a store window...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cYou look at the beautiful dishes. Looking more closely in the window you gasp in horror..`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cYOU FORGOT TO PUT YOUR CLOTHES ON THIS MORNING!!!!`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cYou turn deep red from embarassment and hide behind a bush...`^");
				act( "$n turns beet red and mumbles in their sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		} 
		else if(ch->dreamn == 10)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cAhhhh...  Asleep at last, it's been a long time since you've had a good night's sleep...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cAll of a sudden you feel a burst of energy run through your body. You break out in a sweat...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou feel your body being pulled down, down, down.. deep into the earth...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cThen a deep voice speaks, 'I have been awaiting your arrival.' it booms...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cLooking around frantically you see a dark figure with horns dressed in a blood-stained cloak...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`c'You have been very bad...' it continues 'for this I will punish you!'`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cYou are suddenly thrown into a giant flaming pit, and screaming fills your ears...`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		} 
		else if(ch->dreamn == 11)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou hear flutes in the distance. As the fog clears you realize that...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou are standing in a quaint village filled with small happy faeries. A small male approaches...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cHe smiles, puts a pipe to his lips, and begins to play a tune...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cHe walks away - and you begin to follow. At first you are enchanted...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`c...but you slowly come to the realization you can't stop! You struggle against the sound...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cYou try throwing your hands over your ears...but it solves nothing!`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cHe leads you away from the village into a dark, ominous forest....`^");
				ch->dreamp++;
				break;
			    case 7:
				ch->println("`#`cThe sound begins to drill into your eardrums...then your brain...`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 12)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou are being led down a corridor. Your arms are shackled and held by zombies...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cThey force you into a large dark throne room. Before you sits Lord Ixliam...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cHe glares at you, as the other immortals around him whisper your secrets into his ear...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cIxliam extends his arm, makes a fist, and points his thumb down to the floor...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cExcruciating pain fills your entire being, and you look down at your body...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cYour skin dissolves into dust. You open your mouth, and a scream of hopelessness escapes...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cEverything goes dark......`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 13)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou're resting peacefully, dreaming about beautiful green fields, full of colorful flowers...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cThe sun shines brightly overhead, when an odd sensation awakens you...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou wake up to feel an odd burning sensation on your left hand, and look down...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cYou find your hand on fire! Bright flames and greasy orange smoke rise from the palm of your hand...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cYou sit helplessly and watch the skin drip and crackle, flesh peeling away from the bones...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cYou begin to scream in agony...`^");
				act( "$n moans as if in pain while they sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 14)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou hear them coming...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cThey're coming, there's too many of them. What will you do ?`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cSuddenly, they crest the ridge, and you are bowled over by them...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`c2000 cute puppies who want to lick your face...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cThey're all over you. Well, at least they're housebroken...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cAren't they ?`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 15)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou walk towards the top of a hill, searching for something...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou see something in the distance at the hilltop, you are almost there...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cA smell like no other assaults you as you reach the summit of the hill...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`c'YES!!' you exclaim, you have finally found it...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cAfter years of searching, you have found the Eternal Tree of Everlasting Cupcakes...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cYou reach into the tree, selecting a delicious cupcake from among the branches...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cNo matter how hard you try, you can't seem to get the cupcake into your mouth...`^");
				act( "$n passes into a deeper sleep...", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 16)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou walk along the edge of a beautiful seashore...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cBarefoot, the sand squishes between your toes...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou walk through the water as it crashes upon the shore...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cLooking just up ahead, you notice a large but strange rock...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cApproaching it, you ponder to yourself about it...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cThe rock transforms before you. It's not a rock...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cIT'S A ROCK LOBSTER!!! BOOOMshakalaka!!!!`^");
				act( "$n mutters 'BOOMshakalaka' in their sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 17)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou cruise into a bar on the shore...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cHer picture graced the grime on the door...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cShe's a long lost love at first bite...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cBut maybe you're wrong, but you know it's all right...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cThe dude looks like a lady!!!`^");
				act( "$n mutters something in their sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 18) // Strift
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou are surrounded by green fields, fresh water and lush trees...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou begin walking until you spot a brook. You jump in the water and begin swimming...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cSuddenly the trees begin to burn, the grass turns to piles of corpses and bones...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cThe water turns to blood before your eyes...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cYou find yourself surrounded with no way of escape...`^");
				act( "$n tosses about in their sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 19)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cWhile traveling you spot an entrance to a cave and tell your friends to hold up while you go explore....`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cAs you reach inside it is dark and dank, the walls feel like slime...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cThe floor has so many bugs you can hear the crunching of their bones and shells...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cYou begin to hear screams of pain and horror as you go deeper into the cave...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cWhen an impulse drives you to run screaming from the cave..`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cOnly to find your friends murdered and thier blood on your hands!`^");
				act( "$n mutters something in their sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 20)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou're walking through a forest blanketed by a heavy-mist...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cA chill is sent up your spine as there's a crunch of twigs behind you...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou panic and started running, swerving and dodging trees and low branches...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cYou trip on the path and scream as what feels like fingers start touching you...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cSomething bushy touches your nose and you open your eyes...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cYou see your foe before you... a squirrel.`^");
				act( "$n wiggles their nose in their sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 21)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou're floating on a rainbow cloud high in the sky...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cThe stars come out to greet you, smiling, laughing and waving as they twinkle...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cThe cloud swerves around and takes you to an even bigger star, wearing a golden crown...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`c'I am the Northern Star,' the star says. 'Welcome to my domain.  Here is a gift...'`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cThe Northern Star gives you a pouch, and you open it...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cStar dust goes everywhere, and you begin to sneeze...`^");
				act( "$n sneezed in their sleep.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 22)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou open your eyes and find yourself underwater...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cSharks swim around you in circles like vultures...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou scream out, losing the air you hold in your lungs and swallow water...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cUsing all the strength you can muster you swim for the surface...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cThe light from the surface just gets further away...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cYour body grows weak, everything fades to darkness...`^");
				act( "$n begins to gasp for breath.", ch, NULL, NULL,TO_ROOM);
				do_wake(ch, "");
				dream_kill(ch);
				act( "You awake gasping for breath.", ch, NULL, NULL,TO_CHAR);
				do_yell(ch,"AAAAAGGGGHHHH! The HORROR!!!");
				break;
			}
		}
		else if(ch->dreamn == 23)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou're standing on a floating chessboard in a void of nothing...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou stare over to the other side to see a giant tyrannosaurus rex...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cThe dinosaur's high-pitched roar echoes as it charges at you...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cIn your hand is a club... which is not a good weapon against it, so you run...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cSuddenly the squares fall from the board... you jump to remaining squares...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cEnding up on the dinosaurs square, it's jaws close in around you...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cCRUNCH......`^");
				act( "$n curls up into a tiny ball from a bad dream.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 24)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cLooking around you find yourself in a large castle...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cLying on a soft bed, you relax under under the silk covers...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou hear footsteps approaching down the hall...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cSomeone is coming, you wonder who it could be...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cYou look up, and a pasty white figure with black hair looms over you...`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cHe reaches out his hand, covered with a sparking glove...`^");
				act( "$n begins to pant deeply.", ch, NULL, NULL,TO_ROOM);
				do_wake(ch, "");
				dream_kill(ch);
				act( "You awake panting deeply.", ch, NULL, NULL,TO_CHAR);
				do_yell(ch,"AAAAAGGGGHHHH! The HORROR!!!");
				break;
			}
		} 
		else if(ch->dreamn == 25)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou look around and find yourself in a cemetery. It is very dark...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou hear a noise. Was it behind you? In front? Where did it come from ?`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cYou panic and start to run. You trip over a tombstone...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cYou land hard on the ground. A sound starts to come from the ground...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cA hand pops out of the ground and grabs your ankle!`^");
				ch->dreamp++;
				break;
			    case 5:
				ch->println("`#`cIt starts to pull you into the grave, then you notice the tombstone...`^");
				ch->dreamp++;
				break;
			    case 6:
				ch->println("`#`cInscribed on the tombstone, is YOUR OWN NAME!!!`^");
				act( "$n curls up into a tiny ball from a bad dream.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
		else if(ch->dreamn == 26)
		{
			ch->print("`#`SYou dream:`^ ");
			switch(ch->dreamp)
			{
			    case 0:
				ch->println("`#`cYou seem to be walking down a serene tree lined path...`^");
				ch->dreamp++;
				break;
			    case 1:
				ch->println("`#`cYou come to a clearing full of flowers and wildlife, seemingly a paradise...`^");
				ch->dreamp++;
				break;
			    case 2:
				ch->println("`#`cSuddenly, a dragon with scales that gleam brighter than rubies seems to swoop down from above, setting flame to the meadow...`^");
				ch->dreamp++;
				break;
			    case 3:
				ch->println("`#`cYou look down at your hands, and all you see are charred stumps, and then you realize you’re suddenly shorter and standing on a pile of ashes...`^");
				ch->dreamp++;
				break;
			    case 4:
				ch->println("`#`cA scream tries to issue from your mouth, but all that comes out is more ash, clouding your vision and filling it with a pitch dark haze...`^");
				act( "$n curls up into a tiny ball from a bad dream.", ch, NULL, NULL,TO_ROOM);
				dream_reset(ch);
				break;
			}
		}
 
	} // end awake
    } // end for loop
    return;
}
/**************************************************************************
Dead Grey Path
On Stage
The Masses
The Reflection
Vampires
Frozen Wasteland
Lotto
Flowers & Butterflies
Creak/Swish
No Clothes On
Going To Hell
Pied Piper
Ixliam Rules
Burning Hand
Puppies
Cupcakes
Rock Lobster
Dude Looks Like A Lady
Water to Blood
The Cave
The Squirrel
Star Dust
Underwater
Chessboard Dinosaur
Neverland
Tombstone
Dragonfire
**************************************************************************/
/**************************************************************************/
