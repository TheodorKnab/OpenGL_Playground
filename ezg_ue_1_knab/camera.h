#pragma once
#include <iostream>
#include <vector>
#include <GL/glut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>


class camera
{
public:
	camera();
	~camera();
	
	glm::quat getRotation();
	glm::vec3 getEulerDegRotation();
	void setRotation(glm::quat rot);
	void addRotation(glm::quat rot);
	void addEulerDegRotation(glm::vec3 rot);

	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	void addToPosition(glm::vec3 pos);
	void addToLocalPosition(glm::vec3 pos);
	glm::mat4 getTransformationMatrix();
private:
	glm::quat rotation;
	glm::vec3 position;
};

