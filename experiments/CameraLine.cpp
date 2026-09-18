#include"CameraLine.h"


std::vector<float> Line::directionPoint(float x, float y)
{
	float angle = atan2(x - m_origin_x, y - m_origin_y);
	float dir_x = cos(angle) * 1;
	float dir_y = cos(angle) * 1;

	return { dir_x, dir_y };
}