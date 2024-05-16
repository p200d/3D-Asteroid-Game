#ifndef SPACESHIP_H
#define SPACESHIP_H

#include "CoordinateSystem.h"
#include "ObjLibrary\Vector3.h"
#include "Entity.h"
#include <iostream>
#include "GetGlut.h"

using namespace std;

class Spaceship : public Entity {
protected:
	unsigned int radius;
	double rotation_rate;
public:
	Spaceship();
	void update(double d_time);
	unsigned int getRadius() const;
	void turn_left(double radians);
	void turn_right(double radians);
	void tilt_up(double radians);
	void tilt_down(double radians);
	void roll_left(double radians);
	void roll_right(double radians);
	void accelerateForward(double acceleration_speed);
	void accelerateBackward(double acceleration_speed);
	void accelerateLeft(double acceleration_speed);
	void accelerateRight(double acceleration_speed);
	void accelerateUp(double acceleration_speed);
	void accelerateDown(double acceleration_speed);
	void updatePosition();
	Vector3 getCameraPosition();
	void drawFuturePath(double d_time);
	Vector3 getDronePosition(unsigned int drone_number);
};

#endif