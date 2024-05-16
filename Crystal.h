#ifndef CRYSTAL_H
#define CRYSTAL_H

#include "ObjLibrary\Vector3.h"
#include "CoordinateSystem.h"
#include "Entity.h"
#include <math.h>

using namespace ObjLibrary;
using namespace std;

class Crystal : public Entity {
private:
	double radius;
public:
	Crystal();
	Crystal(Vector3 pos, Vector3 vel);
	void update(double d_time);
	double getRadius() const;
};

#endif