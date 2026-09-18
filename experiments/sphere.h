#pragma once

#ifndef SHPERE_H_
#define SPHERE_H_

class Sphere {
	float r = 0.0;
	float x = 0.0, y = 0.0;

public:
	Sphere(float radius, float x, float y) :r(radius), x(x), y(y) {}

	float ccenter_x()
	{
		return x;
	}

	float ccenter_y() {
		return y;
	}

	float cradius() {
		return r;
	}
};



#endif 