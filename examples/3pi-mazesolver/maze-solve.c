#include <pololu/3pi.h>

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
// is simplifed at every step to the shortest path.
char path[100] = "";
unsigned char path_length = 0;
unsigned char follow = 0;
unsigned char follow_pos = 0;

void follow_intersection()
{
	switch(path[follow_pos])
	{
	case 'L':
		turn(-1,1,90);
		break;
	case 'S':
		// do nothing
		break;
	case 'R':
		turn(1,-1,90);
		break;
	}

	follow_pos ++;
}

unsigned char found_left=0;
unsigned char found_straight=0;
unsigned char found_right=0;

void display_path()
{
	clear();
	print(path);

	if(path_length > 8)
	{
		lcd_goto_xy(0,1);
		print(path+8);
	}
}

void handle_intersection()
{
	clear();
	print("Turn");
	lcd_goto_xy(0,1);
	print_long(found_left);
	print_long(found_straight);
	print_long(found_right);
	set_motors(0,0);
	delay_ms(1000);

	if(found_left)
	{
		turn(-1,1,90);
		path[path_length] = 'L';
	}
	else if(found_straight)
	{ 
		// do nothing - keep going!
		path[path_length] = 'S';
	}
	else if(found_right)
	{
		turn(1,-1,90);
		path[path_length] = 'R';
	}
	else
	{
		// have to go back
		turn(1,-1,180);
		path[path_length] = 'B';
	}    

	path_length ++;

	// note that overflow checking should be placed here

	if(path[path_length-2] == 'B')
	{
		char first = path[path_length-3];
		char last = path[path_length-1];
		char replace = 0;

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
		// otherwise, something went wrong
	}

	path[path_length] = 0;

	display_path();

	set_motors(0,0);
	delay_ms(1000);
}

void maze_solve()
{
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
	int power_difference = proportional/40 + derivative/8;

	// Compute the actual motor settings.  We never set either motor
	// to a negative value.
	const int max = 60;
	if(power_difference > max)
		power_difference = max;
	if(power_difference < -max)
		power_difference = -max;
		
	if(power_difference < 0)
		set_motors(max+power_difference,max);
	else
		set_motors(max,max-power_difference);

	found_left = 0;
	found_right = 0;
	found_straight = 0;

	if(sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100)
	{
		// Will do a U-turn.  Drive a bit forward before turning, or
		// else we might end up too close to the previous
		// intersection.
		set_motors(60,60);
		delay_ms(150);

		if(follow)
			follow_intersection();
		else
			handle_intersection();
	}
	else if(sensors[0] > 200 || sensors[4] > 200)
	{
		if(sensors[0] > 200)
			found_left = 1;
		if(sensors[4] > 200)
			found_right = 1;

		set_motors(60,60);
		delay_ms(50);

		read_line(sensors,IR_EMITTERS_ON);
		if(sensors[0] > 200)
			found_left = 1;
		if(sensors[4] > 200)
			found_right = 1;

		delay_ms(150);

		read_line(sensors,IR_EMITTERS_ON);
		if(sensors[1] > 200 || sensors[2] > 200 || sensors[3] > 200)
			found_straight = 1;
		
		if(sensors[1] > 600 && sensors[2] > 600 && sensors[3] > 600)
		{
			// If all three middle sensors are on black, we have
			// solved the maze.

			set_motors(0,0);
			play(">>a32");
			follow = 1;
			follow_pos = 0;

			while(!button_is_pressed(BUTTON_B))
			{
				if(get_ms() % 1000 < 500)
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
			delay_ms(1000);

			return;
		}

		if(follow)
			follow_intersection();
		else
			handle_intersection();
	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
