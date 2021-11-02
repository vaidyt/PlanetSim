#pragma once

#include <math.h>

namespace Helper
{

	double getRadians(double theta)
	{
		return theta * M_PI / 180;
	}

	double solveQuadratic(double A, double B, double C)
	{
		auto D = sqrt(B * B - 4 * A * C);
		auto t1 = (-B + D) / (2 * A);
		auto t2 = (-B - D) / (2 * A);

		if (t1 >= 0)
			return t1;
		else
			return t2;
	}

	double convertAUToMeters(double distanceInAU)
	{
		return distanceInAU * 149597870.691 * 1e3;
	}

	void drawCircle(HDC& hdc, COLORREF& color, int originX, int originY, int radius)
	{
		SelectObject(hdc, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdc, color);
		SetDCPenColor(hdc, color);
		auto r = std::unique_ptr<tagRECT>(new tagRECT());

		auto leftX = originX - radius;
		auto rightX = originX + radius;
		auto leftY = originY - radius;
		auto rightY = originY + radius;
		Ellipse(hdc, (int)leftX, (int)leftY, (int)rightX, (int)rightY);
	}

}