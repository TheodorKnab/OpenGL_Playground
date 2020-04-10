#pragma once
#ifndef _CATMULLROMCURVE_H_
#define _CATMULLROMCURVE_H_

#include <iostream>
#include <vector>
#include <GL/glut.h>
#include <math.h>
#include <glm/glm.hpp>



class CatmullRomCurve
{
public:
	// constructor/destructor
	CatmullRomCurve();
	~CatmullRomCurve();


	// Get the position of a point in the curve at time "t"
	glm::vec3 evaluate(float t);
	void addPoint(float x, float y, float z);
	void addPoint(glm::vec3 vec);
	void clear();
	float CalculateTforLength(float length, int substeps);

	float CalculateTforLength(float t_start, float length, int substeps);

protected:


private:

	glm::vec3 CalculateCatmullRomPoint(double u, int i);

	float CalculateCurveLength(float t, int substeps);


	// List of control points / waypoints
	// example: myLinearCurve.mControlPoints.push_back(ControlPoint(10.0, 10.0, 0.0, 1.0));
	std::vector<glm::vec3> mControlPoints;
};


#endif
