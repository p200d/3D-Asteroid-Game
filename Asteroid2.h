#ifndef ASTEROID2_H
#define ASTEROID2_H

#include "ObjLibrary\Vector3.h"
#include "CoordinateSystem.h"
#include "Entity.h"
#include <math.h>

const double PI = 3.1415926535;

using namespace ObjLibrary;
using namespace std;

//const unsigned int NUM_ASTEROID_MODELS = 25;

class Asteroid2 : public Entity {
private:
	Vector3 rotation_axis;
	double radius, diameter, rotation_rate, inner_radius;
	unsigned int display_list_number;
	void setDiameter();
	bool holds_crystals;
public:
	Asteroid2();
	void initAsteroid(Vector3 pos, double outRadius, double inRadius, Vector3 rotationAxis, double rotationRate);
	CoordinateSystem local_coords;
	void update(double d_time);
	void setPosition(Vector3 pos);
	void setRadius(double rad);
	double getRadius() const;
	double getDiameter() const;
	void setDisplayListNumber(unsigned int dListNumber);
	unsigned int getDisplayListNumber() const;
	void setRotationRate(double rate);
	double getRotationRate() const;
	void setRotationAxis(Vector3 axis);
	Vector3 getRotationAxis() const;
	void setInnerRadius(double inRadius);
	void calcMass();
	void initOrbit();
	bool holdsCrystals() const;
	void knockCrystals();
	bool safe;
};

#endif