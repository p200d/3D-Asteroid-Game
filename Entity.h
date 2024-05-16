#ifndef ENTITY_H
#define ENTITY_H

#include "CoordinateSystem.h"
#include "ObjLibrary/Vector3.h"

#include <iostream>
using namespace std;

const double G = 6.67408e-11;	//Gravitational constant (Nm^2/kg^2)
const double m = 5.0e16;	//Mass of black hole (kg)

class Entity {
	protected:
		Vector3 velocity;
		Vector3 position;
		double mass, delta_time;
	public:
		Entity();
		CoordinateSystem local_coords;
		virtual void update(double d_time);
		virtual void display();
		virtual void default_update(double d_time);
		double getOrbitSpeed();
		Vector3 getTangentVector();
		Vector3 getPosition() const;
		void multiplyDeltaTime(double multiplier);
		void setVelocity(Vector3 vel);
		double getMass() const;
		Vector3 getVelocity() const;
		void setPosition(Vector3 pos);
};

#endif