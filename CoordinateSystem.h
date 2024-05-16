
#ifndef COORDINATE_SYSTEM_H
#define COORDINATE_SYSTEM_H

#include "ObjLibrary/Vector3.h"
#include "GetGlut.h"
using namespace ObjLibrary;

class CoordinateSystem {
private:
	Vector3 forward, up, right, position;

public:
	CoordinateSystem();
	CoordinateSystem(Vector3 pos);
	const Vector3& getPosition() const;
	const Vector3& getForward() const;
	const Vector3& getUp() const;
	const Vector3& getRight() const;
	void setupCamera() const;
	void setPosition(const Vector3& pos);
	void roll(double radians);
	void turn(double radians);
	void tilt(double radians);
	void rotateAroundArbitrary(Vector3 axis, double radians);
	void moveForward(double distance);
	void moveUp(double distance);
	void moveRight(double distance);
	void rotateToVector(const Vector3& desired_forward, double max_radians);
	void applyDrawTransformations() const;
	void calculateOrientationMatrix(double a_matrix[]) const;
	//void setOrientation(const Vector3& forward, const Vector3& up);
};

#endif