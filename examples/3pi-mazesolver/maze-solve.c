/*
 * This file contains the maze-solving strategy.
 */

#include <pololu/3pi.h>
#include "follow-segment.h"
#include "turn.h"

// The path variable will store the path that the robot has taken.  It
// is stored as an array of characters, each of which represents the
// turn that should be made at one intersection in the sequence:
//  'L' for left
//  'R' for right
//  'S' for straight (going straight through an intersection)
//  'B' for back (U-turn)
//
// Whenever the robot makes a U-turn, the path can be simplified by
// removing the dead end.  The follow_next_turn() function checks for
// this case every time it makes a turn, and it simplifies the path
// appropriately.
char path[100] = "";
unsigned char path_length = 0; // the length of the path

// Displays the current path on the LCD, using two rows if necessary.
void display_path()
{
	// Set the last character of the path to a 0 so that the print()
	// function can find the end of the string.  This is how strings
	// are normally terminated in C.
	path[path_length] = 0;

	clear();
	print(path);

	if(path_length > 8)
	{
		lcd_goto_xy(0,1);
		print(path+8);
	}
}

// This function processes the current intersection during the
// learning phase of maze solving.  It uses the variables found_left,
// found_straight, and found_right to decide which way to turn,
// records that turn in path[], and makes the turn.
// Debugging information is displayed before making the turn so that
// you can confirm that the robot has correctly identified the
// intersection.
void learn_new_intersection(unsigned char found_left, unsigned char found_straight, unsigned char found_right)
{
	// Debugging: this displays the detected path nicely on the LCD.
	clear();
	if(found_straight)
		print("   \xff\xff"); // print bars leading straight ahead
	lcd_goto_xy(0,1);
	if(found_left)
		print("\xff\xff\xff"); // print bars leading left
	else
		print("   ");
	print("\xff\xff"); // always display black under the robot
	
	if(found_right)
		print("\xff\xff\xff"); // print bars leading left
	else
		print("   ");

	// Stop the motors - this gives you a second to read the debugging
	// information on the screen.
	set_motors(0,0);
	delay_ms(1000);

	// Make a decision about how to turn.  The following code
	// implements a left-hand-on-the-wall strategy, where we always
	// turn as far to the left as possible.
	if(found_left)
	{
		// Turn left if possible.
		turn_left();
		path[path_length] = 'L';
	}
	else if(found_straight)
	{ 
		// Otherwise, go straight if possible.  No turn necessary!
		path[path_length] = 'S';
	}
	else if(found_right)
	{
		// Otherwise, turn right if possible.
		turn_right();
		path[path_length] = 'R';
	}
	else
	{
		// This is a dead end - we have to go back.
		turn_around();
		path[path_length] = 'B';
	}    

	path_length ++;

	if(path_length > sizeof(path))
	{
		// For long mazes, if the number of intersections your robot
		// crosses ever reaches 100, you need to do something about
		// it.
		//
		// Some options for the { ... } are:
		// - display an error and stop
		// - stop recording the path
		// - try to allocate more memory with malloc()
		//
		// What you should do depends on your contest format and
		// programming skill level.  Most people will just set the
		// size of the array to be big enough that they never have to
		// worry about this situation.  Here we'll take the first
		// option:
		
		clear();
		print("Out of");
		lcd_goto_xy(0,1);
		print("memory!");
		set_motors(0,0);
		while(1);
	}

	// Path simplification.  The strategy is that whenever we
	// encounter a sequence xBx, we can simplify it by cutting out the
	// dead end.  For example, LBL -> S, because a single S bypasses
	// the dead end represented by LBL.
	if(path[path_length-2] == 'B')
	{
		char first = path[path_length-3];
		char last = path[path_length-1];
		char replace = 0;

		// Consider all of the possible cases, and product appropriate
		// replacements.
		if( (first == 'L' && last == 'R') || (first == 'R' && last == 'L') ||
			(first == 'S' && last == 'S') )
			replace = 'B';
		else if(first == 'L' && last == 'L')
			replace = 'S';
		else if( (first == 'S' && last == 'L') || (first == 'L' && last == 'S') )
			replace = 'R';

		if( replace != 0 )
		{
			path[path_length - 3] = replace;
			path_length -= 2;
		}

		// If replace was not set, something went wrong.  There are
		// some impossible sequences, such as RBR that could only
		// happen if the robot incorrectly measured an intersection or
		// performed a turn badly.  You should think about doing
		// something useful in this situation, like guessing what must
		// have happened; we'll just ignore it in this example.
	}

	// Display the path on the LCD.
	display_path();
}

// This function is called over and over from the main loop in main.c.
void maze_solve()
{
	// Loop until we have solved the maze.
	while(1)
	{
		follow_segment();

		// Drive straight a bit.  This helps us in case we entered the
		// intersection at an angle.
		// Note that we are slowing down - this prevents the robot
		// from tipping forward too much.
		set_motors(50,50);
		delay_ms(50);

		// These variables record whether the robot has seen a line to the
		// left, straight ahead, and right, whil examining the current
		// intersection.
		unsigned char found_left=0;
		unsigned char found_straight=0;
		unsigned char found_right=0;

		// Now read the sensors and check the intersection type.
		unsigned int sensors[5];
		read_line(sensors,IR_EMITTERS_ON);

		// Check for left and right exits.
		if(sensors[0] > 100)
			found_left = 1;
		if(sensors[4] > 100)
			found_right = 1;

		// Drive straight a bit more - this is enough to line up our
		// wheels with the intersection.
		set_motors(40,40);
		delay_ms(200);

		// Check for a straight exit.
		read_line(sensors,IR_EMITTERS_ON);
		if(sensors[1] > 200 || sensors[2] > 200 || sensors[3] > 200)
			found_straight = 1;

		// Check for the ending spot.
		// If all three middle sensors are on dark black, we have
		// solved the maze.
		if(sensors[1] > 600 && sensors[2] > 600 && sensors[3] > 600)
			break;

		// Intersection identification is complete.
		// If the maze has been solved, we can follow the existing
		// path.  Otherwise, we need to learn the solution.
		learn_new_intersection(found_left, found_straight, found_right);
	}

	// Solved the maze!

	// Now enter an infinite loop - we can re-run the maze as many
	// times as we want to.
	while(1)
	{
		// Beep to show that we solved the maze.
		set_motors(0,0);
		play(">>a32");

		// Wait for the user to press a button, while displaying
		// the solution.
		while(!button_is_pressed(BUTTON_B))
		{
			if(get_ms() % 2000 < 1000)
			{
				clear();
				print("Solved!");
				lcd_goto_xy(0,1);
				print("Press B");
			}
			else
				display_path();
			delay_ms(30);
		}
		while(button_is_pressed(BUTTON_B));
	
		delay_ms(1000);

		// Re-run the maze.  It's not necessary to identify the
		// intersections, so this loop is really simple.
		int i;
		for(i=0;i<path_length;i++)
		{
			follow_segment();

			// Drive straight while slowing down, as before.
			set_motors(50,50);
			delay_ms(50);
			set_motors(40,40);
			delay_ms(200);

			// Make a turn according to the instruction stored in
			// path[i].
			if(path[i] == 'L')
				turn_left();
			else if(path[i] == 'R')
				turn_right();
		}
		
		// Follow the last segment up to the finish.
		follow_segment();

		// Now we should be at the finish!  Restart the loop.
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
