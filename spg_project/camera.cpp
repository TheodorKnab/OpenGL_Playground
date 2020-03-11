#include "camera.h"


camera::camera()
{
}


camera::~camera()
{
}

glm::quat camera::getRotation()
{
	return rotation;
}

glm::vec3 camera::getEulerDegRotation()
{
	return eulerAngles(rotation) * static_cast<float>(180 / M_PI);
}


void camera::setRotation(glm::quat rot)
{
	rotation = rot;
}

void camera::addRotation(glm::quat rot)
{
	rotation = rot * rotation;
	rotation = normalize(rotation);
}

void camera::addEulerDegRotation(glm::vec3 rot)
{
	addRotation((glm::quat(rot * static_cast<float>((M_PI / 180)))));
}

glm::vec3 camera::getPosition()
{
	return position;
}

void camera::setPosition(glm::vec3 pos)
{
	position = pos;
}

void camera::addToPosition(glm::vec3 pos)
{
	position += pos;
}


void camera::addToLocalPosition(glm::vec3 pos)
{
	pos = pos * rotation;
	addToPosition(pos);
}

glm::mat4 camera::getTransformationMatrix()
{
	glm::quat tempquat = getRotation();
	glm::mat4 tempmat = toMat4(normalize(tempquat));
	tempmat = translate(tempmat, position);
	return tempmat;
}
