#pragma once

#include <algorithm>
#include "Body.h"

class Bodies
{
private:
	std::vector<Body> _bodies;
	std::string _jsonFileName;

	void clear()
	{
		_bodies.clear();
	}


public:
	// Constructor
	Bodies(std::string jsonFileName)
	{
		Initialize(jsonFileName);
	}

	Bodies()
	{
		_jsonFileName = "";
	}

	void Initialize(std::string jsonFileName)
	{
		_jsonFileName = jsonFileName;
		std::ifstream inputFile(jsonFileName);
		if (inputFile.fail()) 
		{
			//File does not exist
			throw std::invalid_argument(jsonFileName + " not found");
		}
		json jsonObject = json::parse(inputFile);
		auto bodies = jsonObject["bodies"];
		for (auto& body : bodies)
		{
			Body b;
			b.deserialize(body);
			_bodies.push_back(b);
		}

		for (unsigned int i = 0; i < count(); i++)
		{
			_bodies[i].updateDeltaTheta(_bodies);
		}

		inputFile.close();
	}

	// This scale maps the physical coordinates to screen coordinates
	void setScale(int X1, int Y1, int X2, int Y2)
	{
		double maxDimension = -1;

		// Take only 70 % of screen for our display
		double fraction = 0.7;
		int width = (int)(fraction * abs(X2 - X1));
		int height = (int)(fraction * abs(Y1 - Y1));
		double maxWindowDimension = std::max<double>(width, height);

		for (auto& body : _bodies)
		{
			double thisMaxDimension = 2*ceil(body.getOribtalRadius() + body.getRadius());
			if (maxDimension < thisMaxDimension)
			{
				maxDimension = thisMaxDimension;
			}
		}

		double scale = maxWindowDimension / maxDimension;
		Body::setScale(scale);
	}

	size_t count() const
	{
		return _bodies.size();
	}

	void reset()
	{
		clear();
		Initialize(_jsonFileName);
	}
	
	
	const Body* operator[](unsigned int index)
	{
		Body b;
		if (index < 0 || index > count() - 1)
		{
			return nullptr;
		}

		return &_bodies[index];
	}

	void move()
	{
	
		for (unsigned int i = 0; i < count(); i++)
		{
			_bodies[i].move();
		}
	}

	void erase(HDC& hdc, int originX, int originY) const
	{
		for (unsigned int i = 0; i < count(); i++)
		{
			_bodies[i].erase(hdc, originX, originY);
		}
	}

	void draw(HDC &hdc, int originX, int originY) const
	{
		auto c = count();
		for (unsigned int i = 0; i < c; i++)
		{
			_bodies[i].draw(hdc, originX, originY);
		}

	}

};
