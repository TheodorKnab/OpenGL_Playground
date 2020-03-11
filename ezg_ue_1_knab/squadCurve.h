#pragma once
#include <iostream>
#include <vector>
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace std;


class squadCurve
{
public:
	squadCurve();
	~squadCurve();
	
	// Get the position of a point in the curve at time "t"
	glm::quat evaluate(float t);
	glm::vec3 evaluateEuler(float t);
	void addPoint(float x, float y, float z);	
	void addDegPoint(float x, float y, float z);
	void addPoint(glm::quat rot);
	void clear();
private:
	vector<glm::quat> mControlPoints;
};

