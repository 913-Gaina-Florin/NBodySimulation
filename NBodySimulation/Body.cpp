#include "Body.h"

int* Body::serialiseData()
{
	int* buffer = new int[5];
	buffer[0] = (int)this->mass;
	buffer[1] = (int)this->position.x;
	buffer[2] = (int)this->position.y;
	buffer[3] = (int)this->velocity.x;
	buffer[4] = (int)this->velocity.y;

	return buffer;
}

Body Body::deserialiseData(int* buffer)
{
	Body newBody{};
	newBody.mass = (unsigned int)buffer[0];
	newBody.position.x = (float)buffer[1];
	newBody.position.y = (float)buffer[2];
	newBody.velocity.x = (float)buffer[3];
	newBody.velocity.y = (float)buffer[4];
	return newBody;
}
