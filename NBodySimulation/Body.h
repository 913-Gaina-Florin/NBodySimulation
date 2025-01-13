#pragma once
#include "Vector2D.h"

class Body
{
public:
	unsigned int mass;
	Vector2D position;
	Vector2D velocity;

	Body()
	{
		this->mass = 1;
		this->position = Vector2D{};
		this->velocity = Vector2D{};
	}

	Body(unsigned int mass, Vector2D position, Vector2D velocity)
	{
		this->mass = mass;
		this->position = position;
		this->velocity = velocity;
	}

	int* serialiseData();

	static Body deserialiseData(int* buffer);

	// Output stream overload for easy printing
	friend std::ostream& operator<<(std::ostream& os, const Body& v) {
		os << "mass: " << v.mass << " position: ( " << v.position.x << " " << v.position.y << " )" << " velocity: ( " << v.velocity.x << " " << v.velocity.y << " ) \n";
		return os;
	}
};

