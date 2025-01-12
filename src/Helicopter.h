#include "Model.h"
#include "Building.h"
using namespace std;


class Helicopter : public Building
{
public:
	Helicopter() : Building(pathToBody)
	{
		propeller = this->addChild(Building(pathToPropeller));
	}

	Helicopter(glm::vec3 position) : Helicopter()
	{
		transform.pos = position;
	}

	void Update(float deltaTime)
	{
		delta = deltaTime;
		propeller->transform.eulerRot += glm::vec3(0, 0.3, 0);
	}

	void Move(MovementDirection dir)
	{
		float velocity = Speed * delta;
		if (dir == FORWARD)
		{
			transform.pos += Front * velocity;
			transform.eulerRot.x += velocity / 20;
			if (transform.eulerRot.x > 15)
			{
				transform.eulerRot.x = 15;
			}
		}
			
		if (dir == BACKWARD)
		{
			transform.pos -= Front * velocity;
			transform.eulerRot.x -= velocity / 20;
			if (transform.eulerRot.x < -15)
			{
				transform.eulerRot.x = -15;
			}
		}
		if (dir == LEFT)
		{
			transform.eulerRot.y += velocity / 10;
			updateVectors();
		}
		if (dir == RIGHT)
		{
			transform.eulerRot.y -= velocity / 10;
			updateVectors();
		}
		if (dir == UP)
		{
			transform.pos += Up * velocity;
		}
		if (dir == DOWN)
		{
			transform.pos -= Up * velocity;
		}
	}
private:
	void updateVectors()
	{
		//pitch = x
		//yaw = y
		//roll = z
		glm::vec3 front;
		front.x = cos(glm::radians(transform.eulerRot.y)) * cos(glm::radians(transform.eulerRot.x));
		front.y = sin(glm::radians(transform.eulerRot.x));
		front.z = sin(glm::radians(transform.eulerRot.y)) * cos(glm::radians(transform.eulerRot.x));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
	float speed = 1.0f;
	float delta = 0.0f;
	glm::vec3 Up = glm::vec3(0,1,0);
	glm::vec3 Front = glm::vec3(1, 0, 0);
	glm::vec3 Right = glm::vec3(0, 0, 1);
	glm::vec3 WorldUp = Up;
	string pathToBody = "res/models/Helicopter.obj";
	string pathToPropeller = "res/models/Propeller.obj";
	shared_ptr<Building> propeller;
};