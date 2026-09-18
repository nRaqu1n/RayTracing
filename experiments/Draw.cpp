#include<iostream>
#include"sphere.h"
#include"CameraLine.h"


uint32_t draw_function(Sphere sphere, Line camera, int32_t width, int32_t height, uint32_t current)
{
	int32_t x_coo = current / width;
	int32_t y_coo = current / height;

	std::vector<float> u_dir = camera.directionPoint(x_coo, y_coo);

	float b = (float)(2 * u_dir[0]*(camera.origin_x() - sphere.ccenter_x()) + 2 * u_dir[1]*(camera.origin_y() - sphere.ccenter_y()));
	float a = (u_dir[0] * u_dir[0] + u_dir[1] * u_dir[1]);
	float c = pow((camera.origin_x() - sphere.ccenter_x()), 2) + pow((camera.origin_y() - sphere.ccenter_y()), 2) - pow(sphere.cradius(), 2);

	float discrete = b - 4 * a * c;

	if (discrete < 0)
	{
		return 0xffff00ff;
	}
	else
	{
		return 0xff0000ff;
	}
}