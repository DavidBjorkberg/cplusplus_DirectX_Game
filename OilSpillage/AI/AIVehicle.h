#pragma once
#include"..//VehicleStats.h"
#include "..//GameObject.h"
class AIVehicle
{
private:
	GameObject* vehicle;
	btScalar mRotation;
	GameObject* wheel1;
	GameObject* wheel2;
	GameObject* wheel3;
	GameObject* wheel4;

	GameObject* vehicleBody1;

	Stats defaultStats;
	Stats updatedStats;

	DirectX::XMFLOAT2 velocity;

	float targetRotation;
	int drivingMode;
	float rotateAcceleration = 0.0f;
	bool rotationDirection;
	float reverseTimer;
	float reverseTimer2;
	float soundTimer;

	
	float velocitySpeed;
	class Physics* physics;

	int health;
public:
	AIVehicle();
	virtual ~AIVehicle();

	void init(Physics *physics);
	void update(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput);
	
	GameObject* getVehicle() { return this->vehicle; }
	GameObject* getVehicleBody1() { return this->vehicleBody1; }

	void setAccelForce(Vector3 accelForce, float deltaTime);
	void setWheelRotation();
	//void onCollision(Vector2 direction);


	float getYaw(DirectX::XMVECTOR Quaternion);
	float getHeading(Quaternion qt);

	const int& getHealthRef() const;
	int getHealth() const;
	int getMaxHealth() const;
	void setHealth(int health);
	void setMaxHealth(int maxHealth);
	void resetHealth();
	void changeHealth(int amount);
};