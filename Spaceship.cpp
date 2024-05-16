#include "Spaceship.h"

Spaceship::Spaceship() {
	rotation_rate = 0;
	radius = 4;  //meters
	mass = 1000; //Kg
	velocity.set(30, 0, 0); //Default velocity to yield orbit
	position.set(-1000, 1000, 0); //CHANGE THIS
}

void Spaceship::update(double d_time) {
	default_update(d_time); //Ship affected by gravity
}

unsigned int Spaceship::getRadius() const {
	return radius;
}

void Spaceship::turn_left(double radians) {
	local_coords.turn(radians);
}
void Spaceship::turn_right(double radians) {
	local_coords.turn(-radians);
}
void Spaceship::tilt_up(double radians) {
	local_coords.tilt(radians);
}
void Spaceship::tilt_down(double radians) {
	local_coords.tilt(-radians);
}
void Spaceship::roll_left(double radians) {
	local_coords.roll(-radians);
}
void Spaceship::roll_right(double radians) {
	local_coords.roll(radians);
}

void Spaceship::accelerateForward(double acceleration_speed) {
	velocity.set(
		velocity.x + local_coords.getForward().x * acceleration_speed, 
		velocity.y + local_coords.getForward().y * acceleration_speed, 
		velocity.z + local_coords.getForward().z * acceleration_speed
	);
}

void Spaceship::accelerateBackward(double acceleration_speed) {
	velocity.set(
		velocity.x - local_coords.getForward().x * acceleration_speed,
		velocity.y - local_coords.getForward().y * acceleration_speed,
		velocity.z - local_coords.getForward().z * acceleration_speed
	);
}

void Spaceship::accelerateLeft(double acceleration_speed) {
	velocity.set(
		velocity.x - local_coords.getRight().x * acceleration_speed,
		velocity.y - local_coords.getRight().y * acceleration_speed,
		velocity.z - local_coords.getRight().z * acceleration_speed
	);
}

void Spaceship::accelerateRight(double acceleration_speed) {
	velocity.set(
		velocity.x + local_coords.getRight().x * acceleration_speed,
		velocity.y + local_coords.getRight().y * acceleration_speed,
		velocity.z + local_coords.getRight().z * acceleration_speed
	);
}

void Spaceship::accelerateUp(double acceleration_speed) {
	velocity.set(
		velocity.x + local_coords.getUp().x * acceleration_speed,
		velocity.y + local_coords.getUp().y * acceleration_speed,
		velocity.z + local_coords.getUp().z * acceleration_speed
	);
}

void Spaceship::accelerateDown(double acceleration_speed) {
	velocity.set(
		velocity.x - local_coords.getUp().x * acceleration_speed,
		velocity.y - local_coords.getUp().y * acceleration_speed,
		velocity.z - local_coords.getUp().z * acceleration_speed
	);
}

void Spaceship::updatePosition() {
	position.set(position.x + velocity.x, position.y + velocity.y, position.z + velocity.z);
}

Vector3 Spaceship::getCameraPosition() { //Determines camera position based on velocity
	Vector3 camera_position;
	Vector3 offset(0, 5, 0);

	if (velocity.x == 0 && velocity.y == 0 && velocity.z == 0) { //Ship is not moving
		camera_position.set(position.x - 20, position.y + 5, position.z);
		return camera_position;
	}

	//REMOVE THIS STUFF???--------------------------------------------------
	double speed = sqrt(pow(velocity.x, 2) + pow(velocity.y, 2) + pow(velocity.z, 2));
	Vector3 unit_vector(velocity.x/speed, velocity.y/speed, velocity.z/speed);
	camera_position = position + offset - (unit_vector * 20) + offset;
	//REMOVE THIS STUFF???-----------------------------------------------------

	return camera_position;
}

void Spaceship::drawFuturePath(double d_time) {

	Vector3 virtual_position = position;
	Vector3 virtual_velocity = velocity;
	Vector3 black_hole_position(0, 0, 0);

	glColor3d(0, 0, 1);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 2000; i++) { //Updates virtual acceleration, velocity, then position. Draws a new line vertex at each new position.
		Vector3 old_virtual_position = virtual_position;
		Vector3 direction_to_blackhole = black_hole_position - old_virtual_position;
		if (!direction_to_blackhole.isZero()) {
			double distance_squared = old_virtual_position.getDistanceSquared(black_hole_position);
			assert(distance_squared > 0.0);
			double magnitude = G * m / distance_squared;
			Vector3 acceleration = direction_to_blackhole.getCopyWithNorm(magnitude);
			glVertex3d(virtual_position.x, virtual_position.y, virtual_position.z);

			virtual_velocity += acceleration * d_time;
		}

		virtual_position = old_virtual_position + virtual_velocity * d_time;
	}
	glEnd();
}

Vector3 Spaceship::getDronePosition(unsigned int drone_number) {
	Vector3 up = local_coords.getUp();
	Vector3 right = local_coords.getRight();
	switch (drone_number)
	{
	case 0: 
		return Vector3(position.x + up.x * 8, position.y + up.y * 8, position.z + up.z * 8);
	case 1:
		return Vector3(position.x + right.x * 8 + up.x * 4, position.y + right.y * 8 + up.y * 4, position.z + right.z * 8 + up.z * 4);
	case 2:
		return Vector3(position.x - right.x * 8 + up.x * 4, position.y - right.y * 8 + up.y * 4, position.z - right.z * 8 + up.z * 4);
	case 3:
		return Vector3(position.x + right.x * 4 - up.x * 8, position.y + right.y * 4 - up.y * 8, position.z + right.z * 4 - up.z * 8);
	case 4:
		return Vector3(position.x - right.x * 4 - up.x * 8, position.y - right.y * 4 - up.y * 8, position.z - right.z * 4 - up.z * 8);
	default:
		return Vector3(position.x + up.x * 8, position.y + up.y * 8, position.z + up.z * 8);
	}
}