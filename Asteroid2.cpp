#include "Asteroid2.h"
#define _USE_MATH_DEFINES

Asteroid2::Asteroid2() {
	position.x = 0.0;
	position.y = 0.0;
	position.z = 0.0;
	rotation_axis.x = 0.0;
	rotation_axis.y = 0.0;
	rotation_axis.z = 0.0;
	radius = 225;					//Average radius
	inner_radius = radius / 2;		//Average inner/outer ratio
	holds_crystals = true;
	calcMass();
	safe = true;
}

void Asteroid2::initAsteroid(Vector3 pos, double outRadius, double inRadius, Vector3 rotationAxis, double rotationRate) {
	position = pos;
	radius = outRadius;
	inner_radius = inRadius;
	rotation_axis = rotationAxis;
	rotation_rate = rotationRate;
	calcMass();
	initOrbit();
	safe = true;
}

void Asteroid2::update(double d_time) {
	local_coords.rotateAroundArbitrary(rotation_axis, rotation_rate); //Rotate asteroid
	default_update(d_time);
}

void Asteroid2::setPosition(Vector3 pos){
	position = pos;

	local_coords.setPosition(pos); //Set local_coords position to match asteroid position
}

void Asteroid2::setRadius(double rad) {
	radius = rad;
	setDiameter();
}

void Asteroid2::setDiameter() {
	diameter = radius * 2;
}

double Asteroid2::getRadius() const {
	return radius;
}

double Asteroid2::getDiameter() const {
	return diameter;
}

void Asteroid2::setDisplayListNumber(unsigned int dListNumber) {
	display_list_number = dListNumber;
}

unsigned int Asteroid2::getDisplayListNumber() const {
	return display_list_number;
}

void Asteroid2::setRotationRate(double rate){
	rotation_rate = rate;
}

double Asteroid2::getRotationRate() const {
	return rotation_rate;
}

void Asteroid2::setRotationAxis(Vector3 axis) {
	rotation_axis = axis;
}
Vector3 Asteroid2::getRotationAxis() const {
	return rotation_axis;
}

void Asteroid2::setInnerRadius(double inRadius) {
	inner_radius = inRadius;
}

void Asteroid2::calcMass() {
	unsigned int density = 2710;
	mass = double((PI / 6) * pow(radius, 2) * inner_radius * density);
}

void Asteroid2::initOrbit() {
	double multipliers[11] = { 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5 };
	double initial_speed = multipliers[rand() % 11] * getOrbitSpeed(); //Initial orbit speed * random multiplier
	velocity = getTangentVector();
	velocity.set(velocity.x * initial_speed, velocity.y * initial_speed, velocity.z * initial_speed);
}

bool Asteroid2::holdsCrystals() const {
	return holds_crystals;
}

void Asteroid2::knockCrystals() {
	holds_crystals = false;
}