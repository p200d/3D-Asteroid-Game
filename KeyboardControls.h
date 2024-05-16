
#ifndef KEYBOARD_CONTROLS_H
#define KEYBOARD_CONTROLS_H

#include <stdlib.h>
#include "CoordinateSystem.h"
#include "Spaceship.h"
#include "GetGlut.h"

//Contains necessary functions for all keyboard functions
void keyboard(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void special(int special_key, int x, int y);
void specialUp(int special_key, int x, int y);
void checkKeyPress(Spaceship& player);
void checkSpecialPress(Spaceship& player);
bool tToggled();
void slowPhysics();
void slowFramerate();
void speedUpPhysics();
void resetDeltaMultiplier();
void resetSlowFramerate();
void resetSlowPhysics();
double getSlowFrames();
double getSlowPhysics();
double getDeltaMultiplier();
void gameOver();
void knockCrystals();
bool getKnockCheck();
void setKnockCheck(bool value);

#endif
