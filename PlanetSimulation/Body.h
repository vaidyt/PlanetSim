#pragma once
#define _USE_MATH_DEFINES

#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <fstream>
#include "Helper.h"

#include "framework.h"

#include "nlohmann.h"

using json = nlohmann::json;

class Body
{
public:
	// Constructor
	Body(std::string name, double mass, double radius,
		double vx, double vy,
		double px, double py,
		std::vector<int> color
		): 
		_name(name), _mass(mass), _radius(radius),
		_vx(vx), _vy(vy), _px(px), _py(py)
	{
		if (color.size() < 3)
		{
			throw std::invalid_argument("Invalid RGB values");
		}
		_color.push_back(color[0]);
		_color.push_back(color[1]);
		_color.push_back(color[2]);
		init();
	}

	Body()
	{
		_mass = 0;
		_radius = 0;
		_vx = 0;
		_vy = 0;
		_px = 0;
		_py = 0;
		_color.push_back(0);
		_color.push_back(0);
		_color.push_back(0);
		_id = idCounter++;
		init();
	}

	void deserialize(json &body)
	{
		body.at("name").get_to(_name);
		body.at("mass").get_to(_mass);
		body.at("radius").get_to(_radius);
		body.at("vx").get_to(_vx);
		body.at("vy").get_to(_vy);
		body.at("px").get_to(_px);
		body.at("py").get_to(_py);
		body.at("color").get_to(_color);

		init();
	}


	void init()
	{
		_radius = Helper::convertAUToMeters(_radius);
		_orbitalRadius = Helper::convertAUToMeters(_orbitalRadius);
		_px = Helper::convertAUToMeters(_px);
		_py = Helper::convertAUToMeters(_py);
		_v0 = sqrt(_vx * _vx + _vy * _vy);

		_theta = 0;
		_deltaTheta = 1;
		_scaledRadius = _radius * _scale;
		_orbitalRadius = sqrt(_px * _px + _py * _py);

		// scaled values are used to map the position/radius to screen coordinates
		_px_scaled = _px * _scale;
		_py_scaled = _py * _scale;
		_orbitalRadius_scaled = _orbitalRadius * _scale;
		
	}

	double getOribtalRadius()
	{
		return _orbitalRadius;
	}

	double getRadius()
	{
		return _radius;
	}

	double getPx()
	{
		return _px;
	}

	double getPy()
	{
		return _py;
	}

	void move()
	{
		if (_vx == 0 && _vy == 0)
			return;

		if (_theta >= 360)
			_theta = 0;
		_theta += _deltaTheta;
		auto thetaInRadians = -Helper::getRadians(_theta);
		_px = _orbitalRadius * cos(thetaInRadians);
		_py = _orbitalRadius * sin(thetaInRadians);
		_px_scaled = _orbitalRadius_scaled * cos(thetaInRadians);
		_py_scaled = _orbitalRadius_scaled * sin(thetaInRadians);
		
	}

	void erase(HDC& hdc, int originX, int originY) const
	{
		auto color = RGB(0, 0, 0);
		// auto color = RGB(255, 255, 255);
		Helper::drawCircle(hdc, color, (int) (originX + _px_scaled), (int)(originY + _py_scaled), (int)_scaledRadius);
	}

	void draw(HDC &hdc, int originX, int originY) const
	{
		auto color = RGB(_color[0], _color[1], _color[2]);
		Helper::drawCircle(hdc, color, (int)(originX + _px_scaled), (int)(originY + _py_scaled), (int)_scaledRadius);
	}

	// Update the time step in theta
	// This deltaTheta determines the spped of animation
	double updateDeltaTheta(std::vector<Body> &bodyList)
	{
		if (this->_id == 0)
		{
			// This is the sun and so it is not moving
			_deltaTheta = 0;
			return _deltaTheta;
		}

		// Calculate centripetal acceleration based on: 
		// Universal Law of Gravitation and Newton's second law
		// F = m_1 *(G*m_2 / r^2)
		// Therefore, acceleration = G*m_2^2 / r^2
		double acceleration = 0;

		for (size_t i = 0; i < bodyList.size(); i++)
		{
			if (bodyList[i]._id != this->_id)
			{
				auto dPx = -(bodyList[i]._px - this->_px);
				auto dPy = -(bodyList[i]._py - this->_py);
				auto r = sqrt(dPx * dPx + dPy * dPy);
				auto M = bodyList[i]._mass;
				auto sign = dPx/abs(dPx);
				acceleration += sign*(M / (r*r));
			}
		}
		acceleration *= G;

		auto r = this->_orbitalRadius;

		// Orbital Velocity in m / s
		auto v = _v0 + sqrt(acceleration * r);
		// Angular velocity in rad / s
		auto w = v / r;
		// Time period of the body in seconds
		auto TimePeriod_In_Seconds = 2 * M_PI / w;

		auto TimePeriod_In_Weeks = TimePeriod_In_Seconds / (3600.0*24*7);

		// time step
		_deltaTheta = (360 / TimePeriod_In_Weeks);
		return _deltaTheta;
	}

	static void setScale(double scale)
	{
		_scale = scale;
	}
		
private:

	std::string _name;
	double _mass; // in kg
	double _radius; // in m
	double _scaledRadius;
	double _vx, _vy; // in m / sec
	double _v0;
	double _px, _py; // in AU = 150 X 10^9 m
	double _px_scaled, _py_scaled;
	double _theta; // radians
	double _deltaTheta; // radians
	double _orbitalRadius; // in m
	double _orbitalRadius_scaled;
	std::vector<int> _color;
	int _id;
	static double _scale;
	static double G; // Gravitational Constant
	static int idCounter;

};

double Body::_scale = 8;
double Body::G = 6.674e-11; // in N Kg^-2 m^-2
int Body::idCounter = 0;