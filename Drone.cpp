#include "Drone.h"

Drone::Drone() {
	radius = 2;
	mass = 100;
	rotation_rate = 1.0;
	main_speed = 250;
	maneuver_speed = 25;
	state = "escort";
}

Drone::Drone(Vector3 pos, Vector3 vel) {
	radius = 2;
	mass = 100;
	rotation_rate = 1.0;
	main_speed = 250;
	maneuver_speed = 25;
	state = "escort";
	position = pos;
	velocity = vel;
}

void Drone::update(double d_time) {
	default_update(d_time); //Ship affected by gravity
}

unsigned int Drone::getRadius() const {
	return radius;
}

void Drone::drawFuturePath(double d_time) {

	Vector3 virtual_position = position;
	Vector3 virtual_velocity = velocity;
	Vector3 black_hole_position(0, 0, 0);

	glColor3d(0, 1, 0);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < 1000; i++) { //Updates virtual acceleration, velocity, then position. Draws a new line vertex at each new position.
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

double Drone::arrivalTimeInterval(Vector3 current_position, Vector3 target_position){
	//d = distance to target
	//a = magnitude
	//sf = speed at target (?)
	//tarrival = (sqrt((sf) ^ 2 + 2 * a * d) – sf) / a.
	double mag = magnitude(velocity);
	double distance_to_target = current_position.getDistance(target_position);
	return sqrt(2 * mag * distance_to_target) / mag;
}

double Drone::calcSafeSpeed(Vector3 current_position, Vector3 target_position) {
	//Max relative approach speed at distance d ("safe speed") -> sd = sf + a * tarrival
	return magnitude(velocity) * arrivalTimeInterval(current_position, target_position);
}

Vector3 Drone::desiredRelativeVelocity(Vector3 current_position, Vector3 target_position) {
	Vector3 dir_drone_to_target = target_position - current_position;
	return dir_drone_to_target * calcSafeSpeed(current_position, target_position);;
}

void Drone::escort(Vector3 relative_position, Vector3 relative_velocity) {
	double time_to_arrive = arrivalTimeInterval(position, relative_position);
	cout << time_to_arrive << endl << endl;
	Vector3 future_drone_position = position + velocity * time_to_arrive;
	Vector3 future_escort_position = relative_position + relative_velocity * time_to_arrive;
	future_position = future_drone_position;

	//Vector3 move_between_velocity = desiredRelativeVelocity(future_drone_position, future_escort_position) + relative_velocity;
	//setEscortVelocity(move_between_velocity, future_drone_position, future_escort_position);

	Vector3 future_direction = future_escort_position - future_drone_position;
	if (position.getDistance(relative_position) <= .8)
		velocity = relative_velocity;
	else if (position.getDistance(relative_position) <= 5)
		velocity += future_direction * maneuver_speed * 0.001;
	else
		velocity += future_direction * maneuver_speed * 0.002;
}

void Drone::AI(Vector3 relative_position, Vector3 relative_velocity){
	if (state == "avoid")
		avoid();
	else
		escort(relative_position, relative_velocity);
}

double Drone::magnitude(Vector3 vector) {
	return sqrt(pow(vector.x, 2) + pow(vector.y, 2) + pow(vector.z, 2));
}

string Drone::getState() const {
	return state;
}