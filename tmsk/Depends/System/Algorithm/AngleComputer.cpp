#include "AngleComputer.h"

#define  _USE_MATH_DEFINES
#include <math.h>

namespace System
{
namespace Algorithm
{

	float AngleComputer::GetFaceAngle( float fromx, float fromy, float tox, float toy )
	{
		float dx = tox - fromx;
		float dy = toy - fromy;
		if (dx == 0.0f && dy == 0.0f)
		{
			return 0.0f;
		}
		float angle = 0.0f;
		if (dx == 0.0f)
		{
			if (dy < 0.0f)
			{
				angle = -(float)M_PI_2;
			}
			else
			{
				angle = (float)M_PI_2;
			}
		}
		else
		{
			angle = atan(dy / dx);
		}
		if (dx < 0.0f)
		{
			angle += (float)M_PI;
		}
		if (angle < 0) angle += (float)M_PI * 2;
		return angle;
	}

	float AngleComputer::AngleDiff( float a, float b )
	{
		float result = fabs(NormalizeAngle(a) - NormalizeAngle(b));
		if (result > (float)M_PI)
		{
			result = (float)M_PI * 2.0f - result;
		}
		return result;
	}

	float AngleComputer::NormalizeAngle( float a )
	{
		if (a < 0)
		{
			int toAdd = (int)(-a / (float)M_PI / 2.0f) + 1;
			return a + (float)toAdd * (float)M_PI * 2.0f;
		}
		else if (a < (float)M_PI * 2)
		{
			return a;
		}
		else
		{
			return a - (float)(int)(a / (float)M_PI / 2.0f) * (float)M_PI * 2.0f;
		}
	}

}
}