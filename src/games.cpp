/*
01234567890123456789012345678901234567890123456789012345678901234567890123456789
Game Code v2 for ROM based muds. Robert Schultz, Sembiance  -  bert@ncinter.net
Snippets of mine can be found at http://www.ncinter.net/~bert/mud/
This file (games.c) contains all the game functions.
*/

#include "include.h"
#include "recycle.h"
#include "tables.h"

void do_game_seven( char_data *ch, char *argument )
{
    char msg    [ MSL ];
    char wager  [ MIL ];
    char choice [ MIL ];
    char_data    *dealer;
    char buf	[ MSL ];
    int  ichoice;
    int  amount;
    int	 limit = 100;
    int  die1;
    int  die2;
    int  total;

    argument = one_argument( argument, wager );
    one_argument( argument, choice );

  for ( dealer = ch->in_room->people; dealer; dealer = dealer->next_in_room )
    {
      if ( IS_NPC(dealer) && IS_SET(dealer->act2, ACT2_DEALER) && can_see(ch,dealer))
	break;
    }
  
  if ( dealer == NULL )
    {
      ch->printlnf( "You do not see any dice dealer here." );
      return;
    }

    if ( wager[0] == '\0' || !is_number( wager ) )
    {
        ch->println("How many gold coins would you like to wager?");
        return;
    }

    amount = atoi( wager );

	if ( amount <= 0 )
	{
		ch->println("The wager must be a positive amount greater than zero.");
		return;
	}

    if ( amount > ch->gold )
    {
        ch->println("You don't have enough gold!");
        return;
    }

    if ( amount > limit )
    {
	ch->printlnf("`W%s says to you 'Sorry, the house limit is %d.`x'",
        dealer->short_descr, limit );
        return;
    }

		 if ( !str_cmp( choice, "under" ) ) ichoice = 1;
    else if ( !str_cmp( choice, "over"  ) ) ichoice = 2;
    else if ( !str_cmp( choice, "seven" ) ) ichoice = 3;
    else if ( !str_cmp( choice, "7"     ) ) ichoice = 3;
    else
    {
        ch->println("What do you wish to bet: Under, Over, or Seven?");
        return;
    }

	ch->printlnf("You place %d gold coins on the table, and wager '%s'.", amount, choice );

    act( "$n places a bet with you.", ch, NULL, dealer, TO_VICT );
	act( "$n places some coins on the table.", ch, NULL, NULL, TO_ROOM );

    ch->gold -= amount;

    die1 = number_range( 1, 6 );
    die2 = number_range( 1, 6 );
    total = die1 + die2;

	sprintf( buf, "%d and %d", die1, die2);
    sprintf( msg, "%s rolls the dice: they come up %d, and %d", dealer->short_descr, die1, die2 );
	act( "$n rolls the dice: they come up $t.", dealer, buf, ch, TO_NOTVICT );

    if( total == 7 )
    {
        strcat( msg, "." );
        act( msg, dealer, NULL, ch, TO_VICT );

        if ( ichoice == 3 )
        {
            amount *= 5;
            ch->printlnf("It's a SEVEN!  You win %d gold coins!", amount );
			act( "$n rejoices as $e wins some gold!!", ch, NULL, NULL, TO_ROOM );
            ch->gold += amount;
        }
        else
		{
            ch->println("It's a SEVEN!  You lose!");
			act( "$n mutters something as $e loses some coins!!", ch, NULL, NULL, TO_ROOM );
		}
		return;
    }

    ch->printlnf("%s, totalling %d.", msg, total );

    if ((( total < 7 ) && ( ichoice == 1 ))
        || (( total > 7 ) && ( ichoice == 2 )))
    {
        amount *= 2;
        ch->printlnf("You win %d gold coins!", amount );
		act( "$n cheers as $e wins a some coins!!", ch, NULL, NULL, TO_ROOM );
        ch->gold += amount;
    }
    else
	{
        ch->println("Sorry, better luck next time!");
		act( "$n loses $s wager!!", ch, NULL, NULL, TO_ROOM );
	}
	WAIT_STATE( ch, PULSE_VIOLENCE / 2 );
    return;
}

/*************************************************************************/
void do_slots(char_data *ch, char *argument )
{
  OBJ_DATA            *slotMachine;
  char                buf[MSL];
  char                arg[MIL];
  int counter, winArray[11];
  int cost, jackpot, bars, winnings, numberMatched;
  int bar1, bar2, bar3, bar4, bar5;
  bool partial, won, wonJackpot, frozen, foundSlot;

  char *bar_messages[] = {
    "<------------>",
    "`YGold Coin`^",               /* 1 */
    "`R A Banana`^",
    "`M  Lucian `^",               /* 3 */
    "`c    Lena `^",
    "`CShang-Ti `^",               /* 5 */
    "`yAn Orange`^",
    "`r  Ixliam `^",
    "`GA Dragon `^",
    "`WA Sword  `^",
    "`B A Dagger`^",              /* 10 */
  };

  argument = one_argument(argument, arg);
  
  if(arg[0] == '\0')
    {
      ch->printlnf("Syntax: gamble slots <which machine>");
      return;
    }

  foundSlot = false; 

  for (slotMachine = ch->in_room->contents; slotMachine != NULL; slotMachine = slotMachine->next_content)
    {
      if ( (slotMachine->item_type == ITEM_SLOTMACHINE) && (can_see_obj(ch,slotMachine)))
	{
	  if(is_name(arg, slotMachine->name))
	    {
	      foundSlot = true;
	      break;
	    }
	  else
	    {
	      foundSlot = false;
	    }
	}
    }
  
  if(foundSlot == false)
    {
      ch->printlnf("That slot machine is not here.");
      return;
    }

  cost = slotMachine->value[0];
  if(cost <= 0)
    {
      ch->printlnf("This slot machine seems to be broken.");
      return;
    }

  if(cost > ch->gold)
    {
      sprintf(buf, "This slot machine costs %d gold to play.", cost);
      ch->printlnf(buf);
      return;
    }

  ch->gold -= cost;

  jackpot = slotMachine->value[1];
  bars = slotMachine->value[2];

  if(slotMachine->value[3] == 1)
    partial = true;
  else
    partial = false;

  if(slotMachine->value[4] == 1)
    frozen = true;
  else
    frozen = false;

  bar1 = number_range( 1, 10 );
  bar2 = number_range( 1, 10 );
  bar3 = number_range( 1, 10 );
  if(bars > 3)
    {
      bar4 = number_range( 1, 10 );
      bar5 = number_range( 1, 10 );
    }
  else
    {
      bar4 = 0;
      bar5 = 0;
    }

  if(bars == 3)
    {
      ch->printlnf("`g////------------`MSlot Machine`g------------\\\\\\\\`^");
      sprintf(buf, "`g|`C{{}`g|`^  %s  %s  %s  `S|`C{{}`g|`^", bar_messages[bar1],
	      bar_messages[bar2], bar_messages[bar3]);
      ch->printlnf(buf);
      ch->printlnf("`g\\\\\\\\------------------------------------////`^");
    }
  else
    {
      ch->printlnf("`g////-----------------------`MSlot Machine`g----------------------\\\\\\\\`^");
      sprintf(buf, "`g|`C{{}`g|`^  %s  %s  %s  %s  %s  `g|`C{{}`g|`^", bar_messages[bar1],
	      bar_messages[bar2], bar_messages[bar3], bar_messages[bar4], bar_messages[bar5]);
      ch->printlnf(buf);
      ch->printlnf("`g\\\\\\\\---------------------------------------------------------////`^");
    }

  wonJackpot = false;
  winnings = 0;
  won = false;
  numberMatched = 0;

  if(bars == 3)
    {
      if( (bar1 == bar2) && (bar2 == bar3) )
        {
          winnings = jackpot;  /* they won the jackpot, make it */
          won = true;          /* worth their while!            */
          slotMachine->value[1] = cost*10;   /* put it back to something */
          wonJackpot = true;
        }
      else
        {
          if(!frozen)
            slotMachine->value[1] += cost;
        }
    }
  else if(bars == 5)
    {
      if( (bar1 == bar2) && (bar2 == bar3) && (bar3 == bar4) && (bar4 == bar5) )
        {
          winnings = jackpot;  /* if no partial, better have a  */
          won = true;          /* kick butt jackpot for them    */
          slotMachine->value[1] = cost*25;
          wonJackpot = true;
        }
      else
        {
          if(!frozen)
            slotMachine->value[1] += cost;
        }
    }
  else
    {
      ch->printlnf("This is a bad slot machine. Contact casino administration.");
      return;
    }
  if(!frozen)
    if(slotMachine->value[1] >= 32000)
      slotMachine->value[1] = 31000;

  for(counter = 0; counter <= 12; counter++)
    {
      winArray[counter] = 0;
    }


  if(!won && partial)
    {
      if(bars == 3)
        {
          if(bar1 == bar2)
            {
              winnings += cost/2;
              won = true;
              numberMatched++;
            }
          if(bar1 == bar3)
            {
              numberMatched++;
              if(won)
                winnings += cost;
              else
                {
                  winnings += cost/2;
                  won = true;
                }
            }
          if(bar2 == bar3)
            {
              numberMatched++;
              if(won)
                winnings += cost;
              else
                {
                  winnings += cost/2;
                  won = true;
                }
            }
          if(!frozen)
            {
              if(!won)
                slotMachine->value[1] += cost;
              else
                slotMachine->value[1] -= winnings;
            }
        }
      if( bars == 5)
        {
          winArray[bar1]++;
          winArray[bar2]++;
          winArray[bar3]++;
          winArray[bar4]++;
          winArray[bar5]++;

          for(counter = 0; counter <= 12; counter++)
            {
              if(winArray[counter] > 1)
                numberMatched += winArray[counter];
            }

          if(numberMatched == 5)
            {
              if(!frozen)
                slotMachine->value[1] -= (cost*7)/2;
              winnings += cost*7;
            }
          if(numberMatched == 4)
            {
              if(!frozen)
                slotMachine->value[1] -= (cost*5)/2;
              winnings += cost*5;
            }
          if(numberMatched == 3)
            {
              winnings += cost/2;
              if(!frozen)
                slotMachine->value[1] += cost/2;
            }
          if(numberMatched == 2)
            {
              if(!frozen)
                slotMachine->value[1] += cost-1;
              winnings = 1;
            }
          if(numberMatched == 0)
            {
              winnings = 0;
              if(!frozen)
                slotMachine->value[1] += cost;
            }
          if(winnings > 0)
            won = true;
        }
    }

  ch->gold += winnings;

  if(won && wonJackpot)
    {
      sprintf(buf, "You won the jackpot worth %d gold!! The jackpot now stands at %d gold.",
	      winnings, slotMachine->value[1]);
      ch->printlnf(buf);
    }
  if(won && !wonJackpot)
    {
      sprintf(buf, "You matched %d bars and won %d gold! The jackpot is now worth %d gold.",
	      numberMatched, winnings, slotMachine->value[1]);
      ch->printlnf(buf);
    }
  if(!won)
    {
      sprintf(buf, "Sorry you didn't win anything. The jackpot is now worth %d gold.",
	      slotMachine->value[1]);
      ch->printlnf(buf);
    }

  if(slotMachine->value[1] >= 32000)
    slotMachine->value[1] = 31000;

  return;
}

void do_high_dice( char_data *ch, char *argument)
{
  char                buf[MSL];
  char                arg[MIL];
  char_data           *dealer;
  int die, dealerDice, playerDice;
  int bet;
  
  argument = one_argument(argument, arg);

  if(arg[0] == '\0' || !is_number(arg))
    {
      ch->printlnf("Syntax is: gamble highdice <bet>");
      return;
    }

  bet = atoi(arg);

  if(bet < 10)
    {
      ch->printlnf("Minimum bet is 10 gold coins.");
      return;
    }

  if(bet > 1000)
    {
      ch->printlnf("Maximum bet is 1,000 gold coins.");
      return;
    }

  for ( dealer = ch->in_room->people; dealer; dealer = dealer->next_in_room )
    {
      if ( IS_NPC(dealer) && IS_SET(dealer->act2, ACT2_DEALER) && can_see(ch,dealer))
	break;
    }
  
  if ( dealer == NULL )
    {
      ch->printlnf( "You do not see any dice dealer here." );
      return;
    }

  if(bet > ch->gold)
    {
      ch->printlnf("You can not afford to bet that much!");
      return;
    }

  dealerDice = 0;
  playerDice = 0;


  die = number_range(1, 6);
  dealerDice += die;
  die = number_range(1, 6);
  dealerDice += die;

  die = number_range(1, 6);
  playerDice += die;
  die = number_range(1, 6);
  playerDice += die;

  sprintf(buf, "`c%s`g rolled two dice with a total of `W%d!`^", dealer->short_descr, 
	  dealerDice);
  ch->printlnf(buf);
  sprintf(buf, "`gYou rolled two dice with a total of `W%d!`^", playerDice);
  ch->printlnf(buf);

  if(dealerDice > playerDice)
    {
      sprintf(buf, "`RYou lost! `c%s`g takes your bet of `y%d gold`g.`^",
	      dealer->short_descr, bet);
      ch->printlnf(buf);
      ch->gold -= bet;
    }

  if(dealerDice < playerDice)
    {
      sprintf(buf, "`GYou won! `c%s `ggives you your winnings of `y%d gold`g.`^",
	      dealer->short_descr, bet);
      ch->printlnf(buf);
      ch->gold += bet;
    }

  if(dealerDice == playerDice)
    {
      sprintf(buf, "`RYou lost! `gThe dealer always wins in a tie. You lose `y%d gold`g.`^",
	      bet);
      ch->printlnf(buf);
      ch->gold -= bet;
    }

  return;
}


void do_game( char_data *ch, char *argument )
{
  char arg1[MIL];
  int whichGame;
  
  argument = one_argument(argument, arg1);
  
  if ( arg1[0] == '\0' )
    {
      ch->printlnf("Type 'help gamble' for more information on games.");
      return;
    }
  
  if(IS_NPC(ch))
    {
      ch->printlnf("Sorry, only player characters may gamble.");
      return;
    }
  
  if ( !strcmp(arg1, "slot") || !strcmp(arg1, "slots"))
    whichGame = GAME_SLOTS;
  else if( !strcmp(arg1, "highdice"))
    whichGame = GAME_HIGH_DICE;
  else if( !strcmp(arg1, "seven"))
    whichGame = GAME_SEVEN;
  else
    whichGame = GAME_NONE;
  
  switch(whichGame)
    {
    case GAME_SLOTS:
      do_slots(ch, argument);
      break;
    case GAME_HIGH_DICE:
      do_high_dice(ch, argument);
      break;
    case GAME_SEVEN:
      do_game_seven(ch, argument);
      break;
    default:
      ch->printlnf("Thats not a game. Type 'help gamble' for a list.");
      break;
    }
	      

  return;
}

