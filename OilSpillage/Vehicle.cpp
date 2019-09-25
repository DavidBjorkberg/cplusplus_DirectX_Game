#include "Vehicle.h"
#include"game.h"
#include "Input.h"

Vehicle::Vehicle()
{
	velocity = Vector2(0.0f, 0.0f);
	accelerator = Vector3(0.0f, 0.0f, 0.0f);
	acceleratorTempX = 0;
	acceleratorTempZ = 0;
	strength = 0;

	targetRotation = 0;
	drivingMode = 0;
	topSpeed = 4700;
	this->add = 0.0f;
	this->counter = 0.0f;
	this->rotateAcceleration = 0.0f;
	this->rotationSmoother = 1.0f;
	this->tempTargetRotation = 0.0f;
	this->rotationDirection = true;
	this->vehicle = nullptr;
	this->bodyRotation = nullptr;
	this->bodyRotationPoint = nullptr;
}

Vehicle::~Vehicle()
{
	delete vehicle;
	delete bodyRotation;
	delete bodyRotationPoint;
}

void Vehicle::init()
{
	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	vehicle->setScale(Vector3(0.75f, 0.2f, 0.8f));
	Game::getGraphics().loadTexture("brickwall.tga");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("brickwall.tga"));

	this->bodyRotation = new GameObject;
	bodyRotation->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotation);
	bodyRotation->setPosition(Vector3(0.0f, 0.2f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	bodyRotation->setScale(Vector3(0.5f, 0.12f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp.tga");
	bodyRotation->setTexture(Game::getGraphics().getTexturePointer("CarTemp.tga"));

	this->bodyRotationPoint = new GameObject;
	bodyRotationPoint->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotationPoint);
	bodyRotationPoint->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	bodyRotationPoint->setScale(Vector3(0.2f, 0.2f, 0.2f));
	Game::getGraphics().loadTexture("brickwall.tga");
	bodyRotationPoint->setTexture(Game::getGraphics().getTexturePointer("brickwall.tga"));

	bodyPivot = Vector3(0.0f, 1.2f, 0.0f);
}

void Vehicle::update(float deltaTime)
{
	tempTargetRotation = targetRotation;


	//Current rotation of vehicle
	float vehicleRotation = fmod((vehicle->getRotation().y * (180 / DirectX::XM_PI)), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
		
	
	//Direction of Vehicle in dx & dy
	float dx = sin((DirectX::XM_PI / 180) * vehicleRotation);
	float dy = -cos((DirectX::XM_PI / 180)* vehicleRotation);


	targetRotation = (atan2(-Input::GetDirectionL(0).x, -Input::GetDirectionL(0).y) * 180 / DirectX::XM_PI) + 180;

	
	//Driving mode: Turn towards direction, semi-realistic
	if (drivingMode == 0) {
	
		if (Input::GetStrengthL(0) > 0) {
			this->velocity.x += dx * 0.2f *deltaTime* 4000 * Input::GetStrengthL(0);
			this->velocity.y += dy * 0.2f * deltaTime * 4000 * Input::GetStrengthL(0);

			if (strength < Input::GetStrengthL(0)) {
				strength = Input::GetStrengthL(0);
			}
			else {
				strength -= 0.01f * deltaTime;
			}

			if (velocity.x > topSpeed*strength) {
				velocity.x = topSpeed * strength;
			}
			if (velocity.x < -topSpeed * strength) {
				velocity.x = -topSpeed * strength;
			}
			if (velocity.y > topSpeed * strength) {
				velocity.y = topSpeed * strength;
			}
			if (velocity.y < -topSpeed * strength) {
				velocity.y = -topSpeed * strength;
			}
			if (velocity.x > topSpeed * strength * 0.75f && velocity.y < -topSpeed * strength * 0.75f) {
				velocity.x = topSpeed * strength * 0.75f;
				velocity.y = -topSpeed * strength * 0.75f;
			}
			if (velocity.x < -topSpeed * strength * 0.75f && velocity.y > topSpeed * strength * 0.75f) {
				velocity.x = -topSpeed * strength * 0.75f;
				velocity.y = topSpeed * strength * 0.75f;
			}
			if (velocity.y > topSpeed * strength * 0.75f && velocity.x > topSpeed * strength * 0.75f) {
				velocity.y = topSpeed * strength * 0.75f;
				velocity.x = topSpeed * strength * 0.75f;
			}
			if (velocity.y < -topSpeed * strength * 0.75f && velocity.x < -topSpeed * strength * 0.75f) {
				velocity.y = -topSpeed * strength * 0.75f;
				velocity.x = -topSpeed * strength * 0.75f;
			}


			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += 0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3) * DirectX::XM_PI / 180)*deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += -0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2 ) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}

			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += -0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += 0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}
		}
		else {
			this->velocity.x = (velocity.x / (1 + (0.001f * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001f * 3000 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 200;
			vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}

		rotateAcceleration /= 1.0f + 0.002f*deltaTime*1500;
	}
	//Driving mode: Throttle and turning, realistic
	else if (drivingMode == 1) {
		if (Input::CheckButton(Keys::R_TRIGGER, HELD, 0) || Input::IsKeyDown_DEBUG(Keyboard::W)) {
			this->velocity.x += dx * 0.2f * deltaTime * 4000 /** Input::GetStrengthL(0)*/;
			this->velocity.y += dy * 0.2f * deltaTime * 4000 /** Input::GetStrengthL(0)*/;
		}
		if (Input::GetDirectionL(0).x > 0) {
			if (rotationDirection == true) {
				rotateAcceleration = 0;
			}
			if (velocity.x * dx > 0.0f || velocity.y * dy > 0.0f) {
				rotateAcceleration += 0.01f * 200 * deltaTime * 5 * rotationSmoother * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration > 1.9f * abs(Input::GetDirectionL(0).x)) {
					rotateAcceleration = 1.9f * abs(Input::GetDirectionL(0).x);
				}
				vehicle->addRotation(Vector3(0, abs(Input::GetDirectionL(0).x)* ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = false;
			}
			else {
				rotateAcceleration -= 0.01f * 200 * deltaTime * 5 * rotationSmoother * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration < -1.9f * abs(Input::GetDirectionL(0).x)) {
					rotateAcceleration = -1.9f * abs(Input::GetDirectionL(0).x);
				}
				vehicle->addRotation(Vector3(0, abs(Input::GetDirectionL(0).x)* ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = false;
			}
		}
		else if (Input::GetDirectionL(0).x < 0) {
			if (rotationDirection == false) {
				rotateAcceleration = 0;
			}
			if (velocity.y * dy > 0.0f || velocity.x * dx > 0.0f) {
				rotateAcceleration -= 0.01f * 200 * deltaTime * 5 * rotationSmoother * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration < -1.9f * abs(Input::GetDirectionL(0).x)) {
					rotateAcceleration = -1.9f * abs(Input::GetDirectionL(0).x);
				}
				vehicle->addRotation(Vector3(0, abs(Input::GetDirectionL(0).x)* ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3) * DirectX::XM_PI / 180) * deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = true;
			}
			else {
				rotateAcceleration += 0.01f * 200 * deltaTime * 5 * rotationSmoother * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2) * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration > 1.9f * abs(Input::GetDirectionL(0).x)) {
					rotateAcceleration = 1.9f * abs(Input::GetDirectionL(0).x);
				}
				vehicle->addRotation(Vector3(0, abs(Input::GetDirectionL(0).x)* ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = true;
			}
		}
		else {
			//rotateAcceleration = 0;
			rotateAcceleration /= 1.0f + 0.05f * deltaTime * 200;
			vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 8000), 0));

		}
		if (Input::CheckButton(Keys::L_TRIGGER, HELD, 0) || Input::IsKeyDown_DEBUG(Keyboard::S)) {
			this->velocity.x -= dx * 0.2f * deltaTime * 4000 * 0.7f /*Input::GetStrengthL(0)*/;
			this->velocity.y -= dy * 0.2f * deltaTime * 4000 * 0.7f /*Input::GetStrengthL(0)*/;
		}
		if (!Input::CheckButton(Keys::R_TRIGGER, HELD, 0) && !Input::IsKeyDown_DEBUG(Keyboard::S) && !Input::CheckButton(Keys::L_TRIGGER, HELD, 0) && !Input::IsKeyDown_DEBUG(Keyboard::W)) {
		
			this->velocity.x = (velocity.x / (1 + (0.001f * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001f * 3000 * deltaTime)));
		}
	}
	
	//if ((tempTargetRotation != targetRotation) && (drivingMode == 0)) {
	//	//rotationSmoother = 1;
	//	rotateAcceleration = 0;
	//}




	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = velocity.x * (dy / hypoC) + velocity.y * -(dx / hypoC);
	if (drivingMode != 2) {
		if (Input::GetStrengthL(0) > 0) {
			if (driftForce < 0) {
				this->velocity.x -= -((dy / hypoC) * 4000 * deltaTime);
				this->velocity.y -= -(-((dx / hypoC) * 4000 * deltaTime));
			}
			else if (driftForce > 0) {
				this->velocity.x += -((dy / hypoC) * 4000 * deltaTime);
				this->velocity.y += -(-((dx / hypoC) * 4000 * deltaTime));
			}
		}
		else {
			if (driftForce < 0) {
				this->velocity.x -= -((dy / hypoC) * 4000 * deltaTime);
				this->velocity.y -= -(-((dx / hypoC) * 4000 * deltaTime));
			}
			else if (driftForce > 0) {
				this->velocity.x += -((dy / hypoC) * 4000 * deltaTime);
				this->velocity.y += -(-((dx / hypoC) * 4000 * deltaTime));
			}
		}
	}


	this->vehicle->move(Vector3((velocity.x * deltaTime *0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));




	//Body Rotation
	Vector2 curPos3 = Vector2(bodyPivot.x, bodyPivot.z);
	Vector2 position3 = Vector2(vehicle->getPosition().x, vehicle->getPosition().z);

	float dx3 = (curPos3.x - position3.x);
	float dy3 = (curPos3.y - position3.y);

	float hypoC3 = sqrt(pow(dx3, 2) + (pow(dy3, 2)));

	
	

	float springValue = 8.2f;
	Vector2 currentToTarget = Vector2(dx3 / hypoC3, dy3 / hypoC3);
	Vector2 springForce = currentToTarget * springValue;
	Vector2 dampingForce = Vector2(((-accelerator.x)) * 0.2f * sqrt(springValue), ((-accelerator.z)) * 0.2f * sqrt(springValue));
	Vector2 force = springForce + dampingForce;

	/*if (!(dx3 == 0 && hypoC3 == 0) && !(dy3 == 0 && hypoC3 == 0)) {
		this->accelerator.x += force.x * deltaTime * 2.0f;
		this->accelerator.z += force.y * deltaTime * 2.0f;
	}*/

	if ((bodyPivot.x - vehicle->getPosition().x) > 3 && vehicle->getPosition().x < bodyPivot.x) {
		this->accelerator.x += 10 * deltaTime * 2.0f;
	}
	if ((bodyPivot.x - vehicle->getPosition().x) < -3 && vehicle->getPosition().x > bodyPivot.x) {
		this->accelerator.x -= 10 * deltaTime * 2.0f;
	}
	if ((bodyPivot.z - vehicle->getPosition().z) > 3 && vehicle->getPosition().z < bodyPivot.z) {
		this->accelerator.z += 10 * deltaTime * 2.0f;
	}
	if ((bodyPivot.z - vehicle->getPosition().z) < -3 && vehicle->getPosition().z > bodyPivot.z) {
		this->accelerator.z -= 10 * deltaTime * 2.0f;
	}
	
	if (!(dx3 == 0 && hypoC3 == 0) && !(dy3 == 0 && hypoC3 == 0)) {
		this->accelerator.x += force.x * deltaTime * 2.0f;
		this->accelerator.z += force.y * deltaTime * 2.0f;
	}

	Vector3 rotationVec = vehicle->getPosition() - bodyPivot;
	//Vector3 rotationRadians = DirectX::XMVector3AngleBetweenVectors(Vector3(0,1,0), rotationVec);
	//Vector3 rotationRadians = Vector3(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(Vector3(0, 1, 0)), DirectX::XMVector3Normalize(rotationVec)));
	float angle = acos(Vector3(0, 1, 0).Dot(DirectX::XMVector3Normalize(rotationVec)));
	Vector3 axis = Vector3(0, 1, 0).Cross(DirectX::XMVector3Normalize(rotationVec));

	/*float rotx = atan2(y, z)
	float roty = atan2(x * Math.cos(rotx), z)
	float rotz = atan2(Math.cos(rotx), Math.sin(rotx) * Math.sin(roty))*/

	this->bodyRotation->setPosition(Vector3(vehicle->getPosition().x, vehicle->getPosition().y + 1, vehicle->getPosition().z));
	//this->bodyRotation->move(Vector3(-accelerator.x*deltaTime, 0.00f * 0.0000f, -accelerator.z* deltaTime));
	bodyPivot += Vector3(-accelerator.x * deltaTime, 0.00f * 0.0000f, -accelerator.z * deltaTime);
	//this->bodyRotation->setPosition(Vector3(bodyRotation->getPosition().x, bodyRotation->getPosition().y + 1, bodyRotation->getPosition().z));
	//this->bodyRotation->move(Vector3((velocity.x* deltaTime * 0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));
	add += 0.2f * deltaTime;
	//this->bodyRotation->setRotation(-Vector3(rotationVec.x *0.2, -vehicle->getRotation().y, rotationVec.z * 0.2));
	this->bodyRotation->setRotation(-Vector3(0, -vehicle->getRotation().y, 0));
	
	

	//bodyRotationPoint->setPosition(Vector3(bodyPivot.x,bodyPivot.y,bodyPivot.z));
}

float Vehicle::getAcceleratorX()
{
	return accelerator.x;
}
