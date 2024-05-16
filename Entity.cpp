#include "Entity.h"

Entity::Entity() {
	position.set(0, 0, 0);
	velocity.set(0, 0, 0);
	mass = 0;
	delta_time = 0.01667;
}

void Entity::update(double d_time) {

}

void Entity::display(){
	glPushMatrix();
		glTranslated(position.x, position.y, position.z);
		glColor3d(1.0, 0.0, 0.0);
		glutSolidSphere(300.0, 60, 60);
	glPopMatrix();
}

void Entity::default_update(double d_time) {
	Vector3 black_hole_position(0, 0, 0);
	Vector3 old_position = getPosition();
	Vector3 direction_to_blackhole = black_hole_position - old_position;
	if (!direction_to_blackhole.isZero()) {
		double distance_squared = old_position.getDistanceSquared(black_hole_position);
		assert(distance_squared > 0.0);

		double magnitude = G * m / distance_squared;
		Vector3 acceleration = direction_to_blackhole.getCopyWithNorm(magnitude);
		velocity += acceleration * d_time;
	}

	position = old_position + velocity * d_time;
}

double Entity::getOrbitSpeed(){
	return sqrt(G * m / position.getDistance(Vector3(0, 0, 0)));
}

Vector3 Entity::getTangentVector(){ //Calculates random direction tangent to origin (blackhole)

	Vector3 direction_to_blackhole(-position.x, -position.y, -position.z);

	Vector3 tangent_vector;
	tangent_vector = tangent_vector.getRandomUnitVector();
	tangent_vector = tangent_vector.getRejectionSafe(Vector3(-position.x, -position.y, -position.z)); //Get Rejection of direction to blackhole

	return tangent_vector;
}

Vector3 Entity::getPosition() const {
	return position;
}

void Entity::multiplyDeltaTime(double multiplier){ //Multiplies delta time by multiplier (speeds up physics rate)
	 delta_time = delta_time * multiplier;
}

void Entity::setVelocity(Vector3 vel) {
	velocity = vel;
}

double Entity::getMass() const {
	return mass;
}

Vector3 Entity::getVelocity() const {
	return velocity;
}

void Entity::setPosition(Vector3 pos) {
	position = pos;
}