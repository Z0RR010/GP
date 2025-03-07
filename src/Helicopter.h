#include "Model.h"
#include "Building.h"
using namespace std;
string basePath = "res/models/";
class Helicopter : public Building
{
public:
	Helicopter()
	{
	}

	Helicopter(glm::vec3 position) : Building(basePath + "Helicopter.obj")
	{
		transform.pos = position;
		//propeller = this->addChild(Building(basePath + "Propeller.obj"));
		propellers.push_back(this->addChild(Building(Mesh::generateCube(8, 0.1, 0.2))));
		propellers.push_back(this->addChild(Building(Mesh::generateCube(0.2, 0.1, 8))));
		propellers.push_back(this->addChild(Building(Mesh::generateCube(0.2, 0.1, -8))));
		propellers.push_back(this->addChild(Building(Mesh::generateCube(-8, 0.1, 0.2))));
		for (int i = 0; i < 4; i++)
		{
			
			propellers[i]->transform.eulerRot.y = 90 * i;
			//propellers[i]->transform.eulerRot.x = 90;
			propellers[i]->transform.pos = glm::vec3(0, 1.4, 0);
			propellers[i]->meshes[0].color = glm::vec3(20, 20, 20);
			propellers[i]->reflect = true;
		}
		window = this->addChild(Building(basePath + "Window.obj"));
		window->refractionRatio = 1.52;
		Mesh tailMesh = Mesh();
		tailMesh.generateCylinder(0.2, 5, 12);
		glm::vec3 bodyColor = glm::vec3(0.51, 0.745, 1);
		tailMesh.color = bodyColor;
		tail = this->addChild(Building(tailMesh));
		glm::vec3 flat = glm::vec3(90, 0, 0);
		tail->transform.eulerRot = flat;
		tail->transform.pos = glm::vec3(0, 0, 0.5);
		Mesh l = Mesh();
		l.generateCylinder(0.05, 1, 12);
		l.color = bodyColor;
		rb = this->addChild(Building(l));
		rb->transform.pos = glm::vec3(0.4, -1.5, 0.9);
		rb->transform.eulerRot = glm::vec3(0);
		lb = this->addChild(Building(l));
		lb->transform.pos = glm::vec3(-0.4, -1.5, 0.9);
		lb->transform.eulerRot = glm::vec3(0);
		rf = this->addChild(Building(l));
		rf->transform.pos = glm::vec3(0.4, -1.5, -0.9);
		rf->transform.eulerRot = glm::vec3(0);
		lf = this->addChild(Building(l));
		lf->transform.pos = glm::vec3(-0.4, -1.5, -0.9);
		lf->transform.eulerRot = glm::vec3(0);
		Mesh m = Mesh();
		m.generateCylinder(0.05, 3, 12);
		m.color = bodyColor;
		rl = this->addChild(Building(m));
		rl->transform.pos = glm::vec3(0.4, -1.55, -1.5);
		rl->transform.eulerRot = flat;
		ll = this->addChild(Building(m));
		ll->transform.pos = glm::vec3(-0.4, -1.55, -1.5);
		ll->transform.eulerRot = flat;
		//tail->reflect = true;
		//this->reflect = true;
		//propeller->reflect = true;
		
	}

	void Draw(Shader shader)
	{
		Building::Draw(shader);
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
		for (auto& propeller : propellers)
		{
			propeller->transform.eulerRot += glm::vec3(0, propellerSpeed, 0);
			propeller->transform.eulerRot.x = 0;
			propeller->transform.eulerRot.z = 0;
		}
			
		//dummy.transform.pos = transform.pos;
		//dummy.transform.eulerRot = transform.eulerRot;
		//dummy.transform.scale = transform.scale;
		//dummy.dirty = true;
		Gravity();
		if (transform.eulerRot.x != 0.0)
		{
			transform.eulerRot.x -= transform.eulerRot.x / 20;
		}
		if (propellerSpeed > 0.0)
		{
			propellerSpeed -= propellerSpeed / 30;
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
			transform.eulerRot.y += velocity * 30;
			updateVectors();
		}
		if (dir == RIGHT && !checkGround())
		{
			transform.eulerRot.y -= velocity * 30;
			updateVectors();
		}
		if (dir == UP)
		{
			transform.pos += WorldUp * velocity;
			if (fallSpeed >= 0.0)
				fallSpeed -= 0.005;
		}
		if (dir == DOWN && !checkGround())
		{
			transform.pos -= WorldUp * velocity;
		}
		if (propellerSpeed < 5)
		{
			propellerSpeed += 0.1;
		}
	}

	void Gravity()
	{
		if (!checkGround())
		{
			if (fallSpeed < 0.03)
			{
				fallSpeed += 0.0001;
			}
			transform.pos.y -= fallSpeed;
		}
		else if (transform.pos.y > 0)
		{
			transform.pos.y = 0.41;
		}
	}

	bool checkGround()
	{
		return transform.pos.y <= 0.41 && transform.pos.y > -0.25 && transform.pos.x <= 5 && transform.pos.z <= 5 && transform.pos.x >= -5 && transform.pos.z >= -5;
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
	vector<shared_ptr<Building>> propellers;
	shared_ptr<Building> propeller;
	shared_ptr<Building> window;
	shared_ptr<Building> tail;
	shared_ptr<Building> lf;
	shared_ptr<Building> rf;
	shared_ptr<Building> lb;
	shared_ptr<Building> rb;
	shared_ptr<Building> rl;
	shared_ptr<Building> ll;
};