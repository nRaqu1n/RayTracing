#pragma once
#include"sphere.h"
#include<vector>
#include<cmath>


#ifndef CAMERALINE_H_
#define CAMERALINE_H_

class Line {
	float m_origin_x = 0, m_origin_y = 0;

public:
	Line(float x, float y)
	{
		m_origin_x = x;
		m_origin_y = y;
	}

	float origin_x()
	{
		return m_origin_x;
	}

	float origin_y()
	{
		return m_origin_y;
	}

	std::vector<float> directionPoint(float x, float y);
};

#endif