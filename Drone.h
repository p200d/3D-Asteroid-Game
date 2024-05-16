#ifndef DRONE_H
#define DRONE_H

#include "ObjLibrary\Vector3.h"
#include "Entity.h"
#include <iostream>
#include "GetGlut.h"
#include <string>
#include <math.h>

using namespace std;

class Drone : public Entity {
protected:
	unsigned int radius;
	double rotation_rate, main_speed, maneuver_speed;
	Vector3 relative_position;
	string state;
public:
	Drone();
	Drone(Vector3 pos, Vector3 vel);
	void AI(Vector3 relative_position, Vector3 relative_velocity);
	void escort(Vector3 relative_position, Vector3 relative_velocity);
	void avoid();
	void update(double d_time);
	unsigned int getRadius() const;
	void drawFuturePath(double d_time);
	double arrivalTimeInterval(Vector3 current_position, Vector3 target_position);
	double calcSafeSpeed(Vector3 current_position, Vector3 target_position);
	Vector3 desiredRelativeVelocity(Vector3 current_position, Vector3 target_position);
	Vector3 future_position;
	double magnitude(Vector3 vector);
	void setEscortVelocity(Vector3 move_between_velocity, Vector3 drone_position, Vector3 target_position);
	string getState() const;
};

#endif