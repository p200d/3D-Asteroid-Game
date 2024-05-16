#include "CoordinateSystem.h"

CoordinateSystem::CoordinateSystem() {
	forward.set(1, 0, 0);
	up.set(0, 1, 0);
	right.set(0, 0, 1);
	position.set(0, 0, 0);
}

CoordinateSystem::CoordinateSystem(Vector3 pos) {
	forward.set(1, 0, 0);
	up.set(0, 1, 0);
	right.set(0, 0, 1);
	position.set(pos.x, pos.y, pos.z);
}

const Vector3& CoordinateSystem::getPosition() const {
	return position;
}

const Vector3& CoordinateSystem::getForward() const {
	return forward;
}

const Vector3& CoordinateSystem::getUp() const {
	return up;
}

const Vector3& CoordinateSystem::getRight() const {
	return right;
}

void CoordinateSystem::setupCamera() const { //Set camera starting position and look direction vector
	Vector3 look_at = position + forward; //Set up look_at position of camera
	gluLookAt(position.x, position.y, position.z, // (x, y, z) camera position
		look_at.x, look_at.y, look_at.z, //(x, y, z) camera look_at position
		up.x, up.y, up.z); //(x, y, z) camera up direction
}

void CoordinateSystem::setPosition(const Vector3& pos) {
	position.x = pos.x;
	position.y = pos.y;
	position.z = pos.z;
}

void CoordinateSystem::roll(double radians) {
	rotateAroundArbitrary(forward, radians);
}

void CoordinateSystem::turn(double radians) {
	rotateAroundArbitrary(up, radians);
}

void CoordinateSystem::tilt(double radians) {
	rotateAroundArbitrary(right, radians);
}

void CoordinateSystem::rotateAroundArbitrary(Vector3 axis, double radians) {
	forward.rotateArbitrary(axis, radians);
	right.rotateArbitrary(axis, radians);
	up.rotateArbitrary(axis, radians);
}

void CoordinateSystem::moveForward(double distance) {
	position += forward * distance;
}

void CoordinateSystem::moveUp(double distance) {
	position += up * distance;
}

void CoordinateSystem::moveRight(double distance) {
	position -= right * distance;
}

void CoordinateSystem::rotateToVector(const Vector3& desired_forward, double max_radians) {
	Vector3 axis = forward.crossProduct(desired_forward);
	if (desired_forward.isZero()) //special case where forward vector is pointing exact same way or exact opposite of desired_forward vector
		return;
	else
		axis.normalize();

	double radians = forward.getAngleSafe(desired_forward); //Calculate angle in radians to rotate camera by "Pointing the camera at the origin section"

	if (radians > max_radians)	//Fixes the issue where rotateCameratoVector "overshoots" the origin because it is too close to the origin to move by max_radians
		radians = max_radians;
	forward.rotateArbitrary(axis, radians); //Rotate three camera vectors by max_radians (max angle in radians to rotate in one frame)
	up.rotateArbitrary(axis, radians);
	right.rotateArbitrary(axis, radians);
}

void CoordinateSystem::applyDrawTransformations() const { //Apply coordinate system transformations
	//glTranslated(position.x, position.y, position.z);
	double a_matrix[16];
	calculateOrientationMatrix(a_matrix);
	glMultMatrixd(a_matrix); //Apply orientation matrix
}

/* Takes an array of 16 doubles and fills it with the orientation matrix*/
void CoordinateSystem::calculateOrientationMatrix(double a_matrix[]) const {
	a_matrix[0] = forward.x;
	a_matrix[1] = forward.y;
	a_matrix[2] = forward.z;
	a_matrix[3] = 0.0;
	a_matrix[4] = up.x;
	a_matrix[5] = up.y;
	a_matrix[6] = up.z;
	a_matrix[7] = 0.0;
	a_matrix[8] = right.x;
	a_matrix[9] = right.y;
	a_matrix[10] = right.z;
	a_matrix[11] = 0.0;
	a_matrix[12] = 0.0;
	a_matrix[13] = 0.0;
	a_matrix[14] = 0.0;
	a_matrix[15] = 1.0;
}