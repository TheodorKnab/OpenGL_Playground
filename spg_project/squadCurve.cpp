#include "squadCurve.h"

squadCurve::squadCurve()
{
}


squadCurve::~squadCurve()
{
}

glm::quat squadCurve::evaluate(float t)
{
	if (mControlPoints.size() == 0)
	{
		std::cout << "Squad: No Points!!!" << std::endl;
		return glm::quat();
	}

	// handle boundary conditions
	if (t <= 0 || mControlPoints.size() == 1)
	{
		return mControlPoints[0];
	}
	if (t >= 1)
	{
		return mControlPoints[mControlPoints.size() - 1];
	}

	//find correct segment (the waypoint before it)
	float segmentFraction = 1.0 / (static_cast<float>(mControlPoints.size()) - 1);
	unsigned int activeWaypoint = static_cast<int>(t / static_cast<float>(segmentFraction));
	if (activeWaypoint == 1)
	{
		int i;
	}
	//calculate the segment specific t
	float segment_t = static_cast<float>(t - segmentFraction * activeWaypoint) / static_cast<float>(segmentFraction);

	//return glm::slerp(mControlPoints[activeWaypoint], mControlPoints[activeWaypoint + 1], segment_t);

	glm::quat q0, q1, q2, q3, s1, s2;

	//q0 = glm::length(q0 + q1) < glm::length(q0 - q1) ? -q0 : q0;
	//q2 = glm::length(q1 + q2) < glm::length(q1 - q2) ? -q2 : q2;
	//q3 = glm::length(q2 + q3) < glm::length(q2 - q3) ? -q3 : q3;

	//2 points must exist
	q1 = mControlPoints[activeWaypoint];
	q2 = mControlPoints[activeWaypoint + 1];

	//Check if the point is the first point to draw
	if (activeWaypoint == 0)
	{
		//the calculation is canged, that the previous point for the first point is assumed as the negativ vector from the first to the second point 
		q0 = mControlPoints[activeWaypoint];
	}
	else
	{
		q0 = mControlPoints[activeWaypoint - 1];
	}

	//Check for last section: -2 as we check for the point before the end point
	if (activeWaypoint >= mControlPoints.size() - 2)
	{
		q3 = mControlPoints[activeWaypoint + 1];
	}
	else
	{
		q3 = mControlPoints[activeWaypoint + 2];
	}


	s1 = intermediate(q0, q1, q2);
	s2 = intermediate(q1, q2, q3);

	return squad(q1, q2, s1, s2, segment_t);
}

glm::vec3 squadCurve::evaluateEuler(float t)
{
	return eulerAngles(evaluate(t)) * static_cast<float>(180 / M_PI);
}


void squadCurve::addPoint(glm::quat rot)
{
	std::cout << "Rotation POINTADD x: " << rot.x << " y: " << rot.y << " z: " << rot.z << " w: " << rot.w << std::endl;
	mControlPoints.push_back(rot);
}

void squadCurve::clear()
{
	mControlPoints.clear();
}

void squadCurve::addPoint(float x, float y, float z)
{
	glm::quat tempQuat;
	tempQuat = glm::quat(glm::vec3(x, y, z));
	addPoint(tempQuat);
}


void squadCurve::addDegPoint(float x, float y, float z)
{
	addPoint(x * (M_PI / 180), y * (M_PI / 180), z * (M_PI / 180));
}
