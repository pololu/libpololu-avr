/*
 * This file contains the maze-sovling strategy.
 */

#include <pololu/3pi.h>

// Performs a turn.  m1 and m2 are set to -1 or 1, depending on
// whether the corresponding motors should be rotating backward or
// forward during the turn.
void turn(char m1, char m2, unsigned char angle)
{
	set_motors(0,0);
	delay_ms(100);

	set_motors(m1*80,m2*80);

	if(angle == 90)
		delay_ms(200);
	else if(angle == 180)
		delay_ms(400);

	set_motors(0,0);
	delay_ms(100);
}

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
unsigned char solved_maze = 0; // true after we have solved it
unsigned char follow_pos = 0;  // an index for following the path

// This function makes a turn according to the instruction stored in
// path[follow_pos], then increments follow_pos.  It is a lot faster
// than learn_new_intersection(), below, because it does not wait to
// display informative messages.
void follow_next_turn()
{
	if(path[follow_pos] == 'L')
		turn(-1,1,90);
	else if(path[follow_pos] == 'R')
		turn(1,-1,90);

	follow_pos ++;
}

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

// These variables record whether the robot has seen a line to the
// left, straight ahead, and right, whil examining the current
// intersection.
unsigned char found_left=0;
unsigned char found_straight=0;
unsigned char found_right=0;

// This function processes the current intersection during the
// learning phase of maze solving.  It uses the variables found_left,
// found_straight, and found_right to decide which way to turn,
// records that turn in path[], and makes the turn.
// Debugging information is displayed before making the turn so that
// you can confirm that the robot has correctly identified the
// intersection.
void learn_new_intersection()
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
		turn(-1,1,90);
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
		turn(1,-1,90);
		path[path_length] = 'R';
	}
	else
	{
		// This is a dead end - we have to go back.
		turn(1,-1,180);
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
	// Normally, we will be following a line.  The code below is
	// similar to the 3pi-linefollower-pid example, but the maximum
	// speed is turned down to 60 for reliability.

	// Get the position of the line.
	unsigned int sensors[5];
	unsigned int position = read_line(sensors,IR_EMITTERS_ON);

	// The "proportional" term should be 0 when we are on the line.
	int proportional = ((int)position) - 2000;

	// Compute the derivative (change) and integral (sum) of the
	// position.
	int last_proportional = 0;
	int derivative = proportional - last_proportional;

	// Remember the last position.
	last_proportional = proportional;

	// Compute the difference between the two motor power settings,
	// m1 - m2.  If this is a positive number the robot will turn
	// to the left.  If it is a negative number, the robot will
	// turn to the right, and the magnitude of the number determines
	// the sharpness of the turn.
	int power_difference = proportional/80 + derivative/20;

	// Compute the actual motor settings.  We never set either motor
	// to a negative value.
	const int max = 60; // the maximum speed
	if(power_difference > max)
		power_difference = max;
	if(power_difference < -max)
		power_difference = -max;
		
	if(power_difference < 0)
		set_motors(max+power_difference,max);
	else
		set_motors(max,max-power_difference);

	/////////////////////////////////////////////////////////////////
	// The rest of the code deals with finding intersections, dead
	// ends, and the ending spot.  We use the inner three sensors (1,
	// 2, and 3) for determining whether there is a line straight
	// ahead, and the sensors 0 and 4 for detecting lines going to the
	// left and right.

	// Reset the intersection detection variables.
	found_left = 0;
	found_right = 0;
	found_straight = 0;

	if(sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100)
	{
		// There is no line visible ahead, and we didn't see any
		// intersection.  Must be a dead end.

		// Do a U-turn.  Drive a bit forward before turning, or
		// else we might end up too close to the previous
		// intersection.
		set_motors(40,40);
		delay_ms(250);

		if(solved_maze)
			follow_next_turn();
		else
			learn_new_intersection();
	}
	else if(sensors[0] > 200 || sensors[4] > 200)
	{
		// Found an intersection.

		// Drive straight a bit.  This helps us in case we entered the
		// intersection at an angle.
		set_motors(50,50);
		delay_ms(50);

		// Check for left and right exits.
		read_line(sensors,IR_EMITTERS_ON);
		if(sensors[0] > 100)
			found_left = 1;
		if(sensors[4] > 100)
			found_right = 1;

		// Drive straight a bit more.  Note that we are slowing down -
		// this prevents the robot from tipping forward too much.
		set_motors(40,40);
		delay_ms(200);

		// Check for a straight exit.
		read_line(sensors,IR_EMITTERS_ON);
		if(sensors[1] > 200 || sensors[2] > 200 || sensors[3] > 200)
			found_straight = 1;

		// Check for the ending spot.
		if(sensors[1] > 600 && sensors[2] > 600 && sensors[3] > 600)
		{
			// If all three middle sensors are on dark black, we have
			// solved the maze.

			set_motors(0,0);
			play(">>a32");
			solved_maze = 1;
			follow_pos = 0;

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

			return;
		}

		// Intersection identification is complete.

		// If the maze has been solved, we can follow the existing
		// path.  Otherwise, we need to learn the solution.
		if(solved_maze)
			follow_next_turn();
		else
			learn_new_intersection();
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
