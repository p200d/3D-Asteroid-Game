#include "KeyboardControls.h"

const double ROTATE_SPEED = 0.05; //rotation rate of all camera turns/tilts/rolls
const unsigned int KEY_COUNT = 258;
bool key_pressed[KEY_COUNT]; //Includes all keys
bool special_pressed[KEY_COUNT];
bool t_toggle = false;

double SLOW_FRAMES = 0.0;					//Used to slow down framerate
double SLOW_PHYSICS = 0.0;					//Used to slow down physics
double DELTA_MULTIPLIER = 1;

const double SLOW_ACCEL = 2;
const double FAST_ACCEL = 5;

bool GAME_OVER_KEYBOARD = false;
bool KNOCK_CHECK = false;

void keyboard(unsigned char key, int x, int y)
{
	if (key == 27) // [ESC] exit
		exit(0);

	if (!GAME_OVER_KEYBOARD){
		switch (key)
		{
		case ',': // [<] Roll left
			key_pressed[key] = true;
			break;
		case '.': // [>] Roll right
			key_pressed[key] = true;
			break;
		case ' ': // [SPACEBAR] Move forward quickly
			key_pressed[key] = true;
			break;
		case ';': // [;] Move forward slowly
			key_pressed[key] = true;
			break;
		case '/': // [/] Move backwards slowly
			key_pressed[key] = true;
			break;
		case 'w': // [W] Move up
			key_pressed[key] = true;
			break;
		case 's': // [S] Move down
			key_pressed[key] = true;
			break;
		case 'a': // [A] Move left
			key_pressed[key] = true;
			break;
		case 'd': // [D] Move right
			key_pressed[key] = true;
			break;
		case 'u': // [U] Slow physics
			key_pressed[key] = true;
			break;
		case 'y': // [Y] Slow framerate
			key_pressed[key] = true;
			break;
		case 'g': // [G] Speed up physics
			key_pressed[key] = true;
			break;
		case 'k': // [K] Knock crystals off asteroid
			key_pressed[key] = true;
			break;
		case 't': // [T] Toggle asteroid coords
			if (t_toggle) //T is toggled, turn off.
				t_toggle = false;
			else //T is not toggled, turn on.
				t_toggle = true;
			break;
		}
	}

	if (key >= 'a' && key <= 'z') //Treat all letters as uppercase
		key = key - 'a' + 'A';
}

void keyUp(unsigned char key, int x, int y)
{
	key_pressed[key] = false; //Key is released

	if (key == 'y')
		resetSlowFramerate();
	if (key == 'u')
		resetSlowPhysics();
	if (key == 'g')
		resetDeltaMultiplier();
	if (key == 'k')
		knockCrystals();
}

void special(int special_key, int x, int y) {
	switch (special_key) {
	case GLUT_KEY_LEFT: // [LEFT ARROW] rotate left, around up vector
		special_pressed[special_key] = true;
		break;
	case GLUT_KEY_RIGHT: // [RIGHT ARROW] rotate right, around up vector
		special_pressed[special_key] = true;
		break;
	case GLUT_KEY_UP: // [UP ARROW] rotate up
		special_pressed[special_key] = true;
		break;
	case GLUT_KEY_DOWN: // [DOWN ARROW] rotate down
		special_pressed[special_key] = true;
		break;
	}
}

void specialUp(int special_key, int x, int y) {
	special_pressed[special_key] = false;
}

void checkKeyPress(Spaceship& player) {
	if (!GAME_OVER_KEYBOARD) {
		if (key_pressed[' '])// [SPACEBAR] Accelerate forward
			player.accelerateForward(FAST_ACCEL * DELTA_MULTIPLIER);
		if (key_pressed[',']) // [<] Roll left
			player.roll_left(ROTATE_SPEED);
		if (key_pressed['.']) // [>] Roll right
			player.roll_right(ROTATE_SPEED);
		if (key_pressed[';']) // [;] Move forward slowly
			player.accelerateForward(SLOW_ACCEL * DELTA_MULTIPLIER);
		if (key_pressed['/']) // [/] Move backwards slowly
			player.accelerateBackward(SLOW_ACCEL * DELTA_MULTIPLIER);
		if (key_pressed['w']) // [W] Move up
			player.accelerateUp(SLOW_ACCEL * DELTA_MULTIPLIER);
		if (key_pressed['s']) // [S] Move down
			player.accelerateDown(SLOW_ACCEL * DELTA_MULTIPLIER);
		if (key_pressed['a']) // [A] Move left
			player.accelerateLeft(SLOW_ACCEL * DELTA_MULTIPLIER);
		if (key_pressed['d']) // [D] Move right
			player.accelerateRight(SLOW_ACCEL * DELTA_MULTIPLIER);
		if (key_pressed['u']) // [U] Slow physics
			slowPhysics();
		if (key_pressed['y']) // [Y] Slow framerate
			slowFramerate();
		if (key_pressed['g']) // [Y] x10 physics
			speedUpPhysics();
		if (key_pressed['k']) // [K] Knock crystals off asteroid
			//knockCrystals();
		if (key_pressed['t']) // [T] Toggle asteroid coords
			std::cout << "Toggle on";
	}
}

void checkSpecialPress(Spaceship& player) {
	if (!GAME_OVER_KEYBOARD) {
		if (special_pressed[GLUT_KEY_LEFT]) // [LEFT ARROW] rotate left, around up vector
			player.turn_left(ROTATE_SPEED);
		if (special_pressed[GLUT_KEY_RIGHT]) // [RIGHT ARROW] rotate right, around up vector
			player.turn_right(ROTATE_SPEED);
		if (special_pressed[GLUT_KEY_UP]) // [UP ARROW] rotate up, around right vector
			player.tilt_up(ROTATE_SPEED);
		if (special_pressed[GLUT_KEY_DOWN]) // [DOWN ARROW] rotate down
			player.tilt_down(ROTATE_SPEED);
	}
}

bool tToggled() {
	return t_toggle;
}

void slowPhysics() {
	SLOW_PHYSICS = double(0.1);
}

void slowFramerate() {
	SLOW_FRAMES = double(0.1);
}

void speedUpPhysics() {
	DELTA_MULTIPLIER = 10;
}

void resetDeltaMultiplier() {
	DELTA_MULTIPLIER = 1;
}

void resetSlowFramerate() {
	SLOW_FRAMES = 0.0;
}

void resetSlowPhysics() {
	SLOW_PHYSICS = 0.0;
}

double getSlowFrames() {
	return SLOW_FRAMES;
}

double getSlowPhysics() {
	return SLOW_PHYSICS;
}

double getDeltaMultiplier() {
	return DELTA_MULTIPLIER;
}

void gameOver() {
	GAME_OVER_KEYBOARD = true;
}

void knockCrystals(){
	KNOCK_CHECK = true;
}

bool getKnockCheck(){
	return KNOCK_CHECK;
}

void setKnockCheck(bool value) {
	KNOCK_CHECK = value;
}