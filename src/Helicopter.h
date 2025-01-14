#include "Model.h"
#include "Building.h"
using namespace std;
string basePath = "res/models/";
class Helicopter : public Building
{
public:
	Helicopter()// : Building(pathToBody)
	{
	}

	Helicopter(glm::vec3 position) : Building(basePath + "Helicopter.obj")
	{
		transform.pos = position;
		propeller = this->addChild(Building(basePath + "Propeller.obj"));
		window = this->addChild(Building(basePath + "Window.obj"));
	}

	void Draw(Shader shader)
	{
		Model::Draw(shader);
		for (auto& c : children)
		{
			c->Draw(shader);
		}
	}

	void fixGraph()
	{
		for (auto& c : children)
		{
			c->parent = this;
		}
	}

	void Update(float deltaTime)
	{
		//cout << deltaTime << endl;
		delta = deltaTime;
		dirty = true;
		propeller->transform.eulerRot += glm::vec3(0, propellerSpeed, 0);
		//dummy.transform.pos = transform.pos;
		//dummy.transform.eulerRot = transform.eulerRot;
		//dummy.transform.scale = transform.scale;
		//dummy.dirty = true;
		Gravity();
		if (transform.eulerRot.x != 0.0)
		{
			transform.eulerRot.x -= transform.eulerRot.x / 20;
		}
		if (propellerSpeed != 0.0)
		{
			propellerSpeed -= propellerSpeed / 20;
		}
		/*for (auto& c : children)
		{
			cout << (c->parent == &dummy) << endl;
		}*/
		updateSelfAndChild();
		//dummy.updateSelfAndChild();
	}

	void Move(MovementDirection dir)
	{
		float velocity = speed * delta;
		if (dir == FORWARD && !checkGround())
		{
			transform.pos -= Front * velocity;
			transform.eulerRot.x -= velocity * 2;
			if (transform.eulerRot.x < -15)
			{
				transform.eulerRot.x = -15;
			}
		}
			
		if (dir == BACKWARD && !checkGround())
		{
			transform.pos += Front * velocity;
			transform.eulerRot.x += velocity * 2;
			if (transform.eulerRot.x > 15)
			{
				transform.eulerRot.x = 15;
			}
		}
		if (dir == LEFT && !checkGround())
		{
			transform.eulerRot.y += velocity * 20;
			updateVectors();
		}
		if (dir == RIGHT && !checkGround())
		{
			transform.eulerRot.y -= velocity * 20;
			updateVectors();
		}
		if (dir == UP)
		{
			transform.pos += WorldUp * velocity;
			fallSpeed = 0.001;
		}
		if (dir == DOWN && !checkGround())
		{
			transform.pos -= WorldUp * velocity;
		}
		if (propellerSpeed < 1)
		{
			propellerSpeed += 0.05;
		}
	}

	void Gravity()
	{
		if (!checkGround())
		{
			if (fallSpeed < 0.01)
			{
				fallSpeed += 0.0001;
			}
			transform.pos.y -= fallSpeed;
		}
	}

	bool checkGround()
	{
		return transform.pos.y <= 0.18 && transform.pos.y > 0.17 && transform.pos.x <= 5 && transform.pos.z <= 5 && transform.pos.x >= -5 && transform.pos.z >= -5;
	}
	glm::vec3 Front = glm::vec3(0, 0, 1);
private:
	void updateVectors()
	{
		//pitch = x
		//yaw = y
		//roll = z
		glm::vec3 front;
		/*front.z = cos(glm::radians(transform.eulerRot.y)) * cos(glm::radians(transform.eulerRot.x));
		front.y = sin(glm::radians(transform.eulerRot.x));
		front.x = sin(glm::radians(transform.eulerRot.y)) * cos(glm::radians(transform.eulerRot.x));*/
		front.z = cos(glm::radians(transform.eulerRot.y));
		front.y = 0;
		front.x = sin(glm::radians(transform.eulerRot.y));
		Front = glm::normalize(front);
		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}
	float fallSpeed = 0.0001;
	float propellerSpeed = 0;
	float speed = 1.0f;
	float delta = 0.0f;
	glm::vec3 Up = glm::vec3(0,1,0);
	glm::vec3 Right = glm::vec3(1, 0, 0);
	glm::vec3 WorldUp = Up;
	shared_ptr<Building> propeller;
	shared_ptr<Building> window;
};