#include "Crystal.h"

Crystal::Crystal() {
	mass = 1.0;
	radius = 2.0;
}

Crystal::Crystal(Vector3 pos, Vector3 vel) {
	mass = 1.0;
	radius = 2.0;
	position = pos;
	velocity = vel;
}

void Crystal::update(double d_time) {
	default_update(d_time);
}

double Crystal::getRadius() const {
	return radius;
}