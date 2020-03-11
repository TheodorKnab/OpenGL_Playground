#include "CatmullRomCurve.h"

CatmullRomCurve::CatmullRomCurve()
{ 
}

CatmullRomCurve::~CatmullRomCurve()
{ }

 

//t can range from 0 to 1
glm::vec3 CatmullRomCurve::evaluate(float t)
{
	if (mControlPoints.size() == 0)
	{
		cout << "CatmullRomCurve: No Points!!!" << endl;
		return glm::vec3(0, 0, 0);
	}
	// handle boundary conditions
	if (t <= 0 || mControlPoints.size() == 1) {
		return mControlPoints[0];
	}
	else if (t >= 1){
		return mControlPoints[mControlPoints.size() - 1];
	}

	//find correct segment (the waypoint before it)
	float segmentFraction = 1.0 / ((float)mControlPoints.size() - 1);
	unsigned int activeWaypoint = (int) (t / (float) segmentFraction);

	//calculate the segment specific t
	float segment_t = (float)(t - segmentFraction * activeWaypoint) / (float)(segmentFraction);
	
	return CalculateCatmullRomPoint(segment_t, activeWaypoint);
}


glm::vec3 CatmullRomCurve::CalculateCatmullRomPoint(double t, int i) {

	glm::vec3 CatRomPoints[4];


	//2 points must exist
	CatRomPoints[1] = mControlPoints[i];
	CatRomPoints[2] = mControlPoints[i + 1];

	//Check if the point is the first point to draw
	if (i == 0) {
		//the calculation is canged, that the previous point for the first point is assumed as the negativ vector from the first to the second point 
		CatRomPoints[0] = mControlPoints[i];
	}
	else
	{
		CatRomPoints[0] = mControlPoints[i - 1];
	}

	//Check for last section: -2 as we check for the point before the end point
	if (i >= mControlPoints.size() - 2)
	{
		CatRomPoints[3] = mControlPoints[i + 1];
	}
	else
	{
		CatRomPoints[3] = mControlPoints[i + 2];
	}


	glm::vec3 returnVec;
	returnVec = 0.5f * ((-CatRomPoints[0] + 3.f * CatRomPoints[1] - 3.f * CatRomPoints[2] + CatRomPoints[3])*(float)t*(float)t*(float)t
				+ (2.f * CatRomPoints[0] - 5.f * CatRomPoints[1] + 4.f * CatRomPoints[2] - CatRomPoints[3])*(float)t*(float)t
				+ (-CatRomPoints[0] + CatRomPoints[2])*(float)t
				+ 2.f * CatRomPoints[1]);
	return returnVec;
}

//from 0 to t, substeps is the precision 
float CatmullRomCurve::CalculateCurveLength(float t, int substeps) {
	float refLength = 0;
	glm::vec3 laststep = evaluate(0);
	int i;
	for (i = 1; i < substeps; i++)
	{ 
		refLength += (laststep - evaluate((i / (float)substeps)*(float)t)).length();
	}

	return (refLength);
}

float CatmullRomCurve::CalculateTforLength(float length, int substeps)
{
	return CalculateTforLength(0, length, substeps);
}

float CatmullRomCurve::CalculateTforLength(float t_start, float length, int substeps)
{
		float refLength = 0;
		glm::vec3 laststep = evaluate(t_start);
		int i;
		for (i = 1; i < substeps; i++)
		{
			glm::vec3 step = evaluate(t_start + (i / (substeps * (1 - t_start))));
			refLength += glm::length(laststep - step);
			if (length < refLength)
			{
				break;
			}
		}

		return (t_start + (i / (substeps * (1 - t_start))));
}

void CatmullRomCurve::addPoint(float x, float y, float z) {
	//The third section of the CatmullRom Formula, using points
	glm::vec3 tempVec(x,y,z);
	addPoint(tempVec);
}

void CatmullRomCurve::addPoint(glm::vec3 vec) {
	//The third section of the CatmullRom Formula, using points

	cout << "Position POINTADD x: " << vec.x << " y: " << vec.y << " z: " << vec.z << endl;

	mControlPoints.push_back(vec);
}

void CatmullRomCurve::clear()
{
	mControlPoints.clear();
}
