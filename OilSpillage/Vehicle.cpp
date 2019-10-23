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

	targetRotation = 0.0f;
	drivingMode = 1;
	topSpeed = 4700;
	this->add = 0.0f;
	this->counter = 0.0f;
	this->rotateAcceleration = 0.0f;
	this->rotationSmoother = 1.0f;
	this->tempTargetRotation = 0.0f;
	this->velocitySimple = 0.0f;
	this->velocitySpeed = 0.0f;
	this->rotationDirection = true;
	this->vehicle = nullptr;
	this->bodyRotation = nullptr;
	this->bodyRotationPoint = nullptr;
	this->cameraDistance = 0.0f;

	this->leftoverTime = 0.0f;
	this->weapon = VehicleWeapon::missileLauncher;
	this->defaultStats = VehicleStats::fastCar;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Vehicle::~Vehicle()
{
	for (int i = 0; i < Vehicle::bulletCount; i++)
	{
		delete this->bullets[i].obj;
	}

	delete vehicle;
	delete bodyRotation;
	delete bodyRotationPoint;
	delete vehicleBody1;

	delete wheel1;
	delete wheel2;
	delete wheel3;
	delete wheel4;
	delete spring1;
	delete pointJoint;
}

void Vehicle::init(Physics *physics)
{
	this->physics = physics;

	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	vehicle->setScale(Vector3(0.5f, 0.4f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));

	/*this->vehicleBody1 = new GameObject;
	vehicleBody1->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(vehicleBody1);
	vehicleBody1->setPosition(Vector3(0.0f, 0.6f, 0.0f));
	vehicleBody1->setScale(Vector3(0.5f, 0.22f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	vehicleBody1->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));*/

	/*this->bodyRotation = new GameObject;
	bodyRotation->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotation);
	bodyRotation->setPosition(Vector3(0.0f, 0.2f, 0.0f));
	bodyRotation->setScale(Vector3(0.5f, 0.12f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	bodyRotation->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));*/

	this->bodyRotationPoint = new GameObject;
	bodyRotationPoint->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotationPoint);
	bodyRotationPoint->setPosition(Vector3(0.0f, 2.0f, 0.0f));
	bodyRotationPoint->setScale(Vector3(0.2f, 0.2f, 0.2f));
	Game::getGraphics().loadTexture("brickwall");
	bodyRotationPoint->setTexture(Game::getGraphics().getTexturePointer("brickwall"));

	this->wheel1 = new GameObject;
	this->wheel2 = new GameObject;
	this->wheel3 = new GameObject;
	this->wheel4 = new GameObject;
	wheel1->mesh = Game::getGraphics().getMeshPointer("Cube");
	wheel2->mesh = Game::getGraphics().getMeshPointer("Cube");
	wheel3->mesh = Game::getGraphics().getMeshPointer("Cube");
	wheel4->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(wheel1);
	Game::getGraphics().addToDraw(wheel2);
	Game::getGraphics().addToDraw(wheel3);
	Game::getGraphics().addToDraw(wheel4);
	wheel1->setPosition(Vector3(1.0f, 2.0f, 1.0f));
	wheel2->setPosition(Vector3(-1.0f, 2.0f, 1.0f));
	wheel3->setPosition(Vector3(-1.0f, 2.0f, -1.0f));
	wheel4->setPosition(Vector3(1.0f, 2.0f, -1.0f));
	wheel1->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel2->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel3->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel4->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel1->setColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f)); //Top Right
	wheel2->setColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f)); //Top Left
	wheel3->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f)); //Bottom Left
	wheel4->setColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f)); //Bottom Right

	btRigidBody* tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z), btVector3(this->getVehicle()->getScale().x, this->getVehicle()->getScale().y, this->getVehicle()->getScale().z), 10.0f);
	this->getVehicle()->setRigidBody(tempo, physics);
	this->getVehicle()->getRigidBody()->activate();
	this->getVehicle()->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->getVehicle()->getRigidBody()->setFriction(0);

	/*tempo = physics->addBox(btVector3(vehicleBody1->getPosition().x, vehicleBody1->getPosition().y, vehicleBody1->getPosition().z), btVector3(vehicleBody1->getScale().x, vehicleBody1->getScale().y, vehicleBody1->getScale().z), 1.0f);
	vehicleBody1->setRigidBody(tempo, physics);
	vehicleBody1->getRigidBody()->activate();
	vehicleBody1->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	vehicleBody1->getRigidBody()->setFriction(1);*/

	tempo = physics->addSphere(0.2f, btVector3(wheel1->getPosition().x, wheel1->getPosition().y, wheel1->getPosition().z),10.0f);
	wheel1->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel2->getPosition().x, wheel2->getPosition().y, wheel2->getPosition().z), 10.0f);
	wheel2->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel3->getPosition().x, wheel3->getPosition().y, wheel3->getPosition().z), 10.0f);
	wheel3->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel4->getPosition().x, wheel4->getPosition().y, wheel4->getPosition().z), 10.0f);
	wheel4->setRigidBody(tempo, physics);

	/*spring1 = physics->addSpring(this->vehicle->getRigidBody(),this->wheel1->getRigidBody());
	spring1->setLinearLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
	spring1->setLinearUpperLimit(btVector3(0.0f, 0.56f, 0.0f));
	spring1->enableSpring(1, true);
	spring1->setStiffness(1, 65.0f);
	spring1->setDamping(1, 0.5f);
	spring1->setEquilibriumPoint();*/
	
	//pointJoint = physics->addPointJoint(this->vehicle->getRigidBody(), this->vehicleBody1->getRigidBody());
	//vehicleBody1->getRigidBody()->setDamping(btScalar(0),3);

	bodyPivot = Vector3(0.0f, 1.2f, 0.0f);

	for (int i = 0; i < 16; i++)
	{
		this->bullets[i].obj = new GameObject;
		this->bullets[i].obj->mesh = Game::getGraphics().getMeshPointer("Cube");
		this->bullets[i].obj->setScale(Vector3(0.25f, 0.25f, 0.25f));
		this->bullets[i].obj->setColor(Vector4(1, 1, 0, 1));
	}

}

void Vehicle::update(float deltaTime)
{
	deltaTime *= 2;
	if (Input::CheckButton(CONFIRM, HELD, 0))
	{
		float tempDelta = deltaTime + this->leftoverTime;

		if (tempDelta <= this->weapon.fireSpeed)
		{
			this->leftoverTime = tempDelta;
		}

		while (tempDelta > this->weapon.fireSpeed)
		{
			tempDelta -= this->weapon.fireSpeed;
			int freeToUse = 0;

			while (freeToUse < Vehicle::bulletCount && this->bullets[freeToUse].timeLeft > 0.0f)
			{
				freeToUse++;
			}

			if (freeToUse < Vehicle::bulletCount)
			{
				Vector2 dir = Input::GetDirectionR(0);
				this->bullets[freeToUse].dir = Vector3(dir.x, 0, dir.y);
				this->bullets[freeToUse].dir.Normalize();
				this->bullets[freeToUse].timeLeft = this->weapon.bulletLifetime;
				this->bullets[freeToUse].speed = this->weapon.bulletSpeed + max(abs(velocity.x) , abs(velocity.y));
				this->bullets[freeToUse].obj->setPosition(this->vehicle->getPosition() + Vector3(0, 2, 0));
				this->bullets[freeToUse].obj->setRotation(Vector3(XMVector3AngleBetweenVectors(Vector3(0, 0, 1), this->bullets[freeToUse].dir)) * Vector3(0, 1, 0));
			}
			else
			{
				this->leftoverTime = 0.0f;
				break;
			}

			if (tempDelta <= this->weapon.fireSpeed)
			{
				this->leftoverTime = tempDelta;
			}
		}
	}
	else
	{
		this->leftoverTime = 0.0f;
	}

	for (int i = 0; i < Vehicle::bulletCount; i++)
	{
		Game::getGraphics().removeFromDraw(this->bullets[i].obj);

		if (this->bullets[i].timeLeft > 0.0f)
		{
			this->bullets[i].timeLeft -= deltaTime;
			this->bullets[i].obj->move(this->bullets[i].dir * this->bullets[i].speed * deltaTime);
			Game::getGraphics().addToDraw(this->bullets[i].obj);
		}
	}

	tempTargetRotation = targetRotation;

	//Quaternion Rotation to Euler
	btScalar m;
	btScalar n;
	Quaternion qt = Vector4(this->vehicle->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getW());

	qt = this->vehicle->getRotationQuaternion();

	float heading = getHeading(qt);

	this->vehicle->getRigidBody()->getWorldTransform().getRotation().getEulerZYX(n,mRotation,m);
	//if (mRotation < 0) mRotation += 180;

	//Current rotation of vehicle
	float vehicleRotation = fmod((/*(getYaw(qt)*/heading * 180 / DirectX::XM_PI), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
	mRotation = vehicleRotation;

	//Rotate Chassis toward Body
	Quaternion rotationToBt = DirectX::XMQuaternionRotationRollPitchYaw(0, heading /** 180 / DirectX::XM_PI*/, 0);
	btQuaternion bt = btQuaternion(rotationToBt.x, rotationToBt.y, rotationToBt.z, rotationToBt.w);
//	vehicleBody1->getRigidBody()->getWorldTransform().setRotation(bt);
	//vehicleBody1->getRigidBody()->applyImpulse(btVector3(0,40*deltaTime,0),btVector3(0,10,0));
//	vehicleBody1->getRigidBody()->setDamping(0,0.99f);
	/*qt = Vector4(this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getW());
	float bodyHeading = getHeading(qt);
	if (bodyHeading < heading) {
		this->vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getLinearVelocity().getX(),120*deltaTime, vehicleBody1->getRigidBody()->getLinearVelocity().getZ()));
	}
	else if (bodyHeading > heading) {
		this->vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getLinearVelocity().getX(), -120 * deltaTime, vehicleBody1->getRigidBody()->getLinearVelocity().getZ()));
	}*/

	
	//Direction of Vehicle in dx & dy
	float dx = sin((DirectX::XM_PI / 180) * vehicleRotation);
	float dy = -cos((DirectX::XM_PI / 180)* vehicleRotation);


	targetRotation = (atan2(-Input::GetDirectionL(0).x, -Input::GetDirectionL(0).y) * 180 / DirectX::XM_PI) + 180;
	velocitySpeed = (this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dx)) + (-this->vehicle->getRigidBody()->getLinearVelocity().getZ() * (dy));
	
	//Driving mode: Turn towards direction, semi-realistic
	if (drivingMode == 0) {	
		if (Input::GetStrengthL(0) > 0) {
			if (velocitySpeed < (5000 * updatedStats.maxSpeed)) {
				this->velocity.x += dx *deltaTime* 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
				this->velocity.y += dy *deltaTime* 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
			}

			if (strength < Input::GetStrengthL(0)) {
				strength = Input::GetStrengthL(0);
			}
			else {
				strength -= 0.01f * deltaTime;
			}

			Vector2 targetDir(Input::GetDirectionL(0));

			if ((targetDir - this->currentDir).Length() > 0.01f)
			{
				this->currentDir = Vector2::Lerp(this->currentDir, targetDir, deltaTime * min(4* min(velocitySpeed*0.0003f,1),2));
				this->currentDir.Normalize();
			}
			else
			{
				this->currentDir = targetDir;
			}

			float newRot = atan2(this->currentDir.x, this->currentDir.y);
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, newRot, 0));

		}
		else {
			this->velocity.x = (velocity.x / (1 + (0.001f * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001f * 3000 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 20;
			vehicle->getRigidBody()->setAngularVelocity(btVector3(vehicle->getRigidBody()->getAngularVelocity().getX(), ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), vehicle->getRigidBody()->getAngularVelocity().getZ()));
		}

		rotateAcceleration /= 1.0f + 0.002f*deltaTime*1500;
		/*if (abs(rotateAcceleration) < 0.5) {
			rotateAcceleration = 0;
		}*/
	}
	//Driving mode: Throttle and turning, realistic
	else if (drivingMode == 1) {

		Vector3 steering = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
								   Input::GetStrengthL(0) * /*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
								   vehicle->getRigidBody()->getAngularVelocity().getZ());

		Vector3 steering2 = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
									Input::GetStrengthL(0) * /*deltaTime*/0.035f * 80 * max(velocitySpeed * 0.15f, -1),
									vehicle->getRigidBody()->getAngularVelocity().getZ());

		if (Input::CheckButton(Keys::R_TRIGGER, HELD, 0) || Input::IsKeyDown_DEBUG(Keyboard::W)) {
			if (velocitySpeed < (5000 * updatedStats.maxSpeed)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(dx* deltaTime * 80.0f * updatedStats.accelerationRate, 0, -(dy * deltaTime * 80.0f * updatedStats.accelerationRate)), btVector3(0, 0, 0));
			}
		}
		if (Input::GetDirectionL(0).x > 0) {
			if (velocitySpeed > 0.0f) {
				if (rotationDirection == true) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 5 * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0)) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, steering.y * min(rotateAcceleration, 1), steering.z));
				rotationDirection = false;
			}
			else {
				if (rotationDirection == false) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 5 * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0)) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, -steering2.y/* * min(rotateAcceleration, 1)*/, steering.z));
				rotationDirection = true;
			}
		}
		else if (Input::GetDirectionL(0).x < 0) {
			if (velocitySpeed > 0.0f) {
				if (rotationDirection == true) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 5 * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0)) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, -steering.y * min(rotateAcceleration, 1), steering.z));
				rotationDirection = false;
			}
			else {
				if (rotationDirection == false) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 5 * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0)) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, steering2.y/* min(rotateAcceleration,1)*/, steering.z));
				rotationDirection = true;
			}
		}
		else {
			rotateAcceleration = 0;
			rotateAcceleration /= 1.0f + 10 * deltaTime;
			vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, vehicle->getRigidBody()->getAngularVelocity().getY()/ (1 + (3.5f * deltaTime)), steering.z));

		}
		if (Input::CheckButton(Keys::L_TRIGGER, HELD, 0) || Input::IsKeyDown_DEBUG(Keyboard::S)) {
			this->vehicle->getRigidBody()->applyImpulse(btVector3(-(dx* deltaTime * 80.0f * 0.7f * updatedStats.accelerationRate), 0, (dy * deltaTime * 80.0f * updatedStats.accelerationRate)), btVector3(0, 0, 0));

			if (velocitySpeed > 0.0f) {
				vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (0.9f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (0.9f * deltaTime))));
			}
		}
		if (!Input::CheckButton(Keys::R_TRIGGER, HELD, 0) && !Input::IsKeyDown_DEBUG(Keyboard::S) && !Input::CheckButton(Keys::L_TRIGGER, HELD, 0) && !Input::IsKeyDown_DEBUG(Keyboard::W)) {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (0.3f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (0.3f * deltaTime))));
		}
	}
	//Driving Mode: Directional Smooth
	else if (drivingMode == 2)
	{
		if (Input::GetDirectionL(0) != Vector2())
		{
			Vector2 targetDir(Input::GetDirectionL(0));

			if ((targetDir - this->currentDir).Length() > 0.01f)
			{
				this->currentDir = Vector2::Lerp(this->currentDir, targetDir, deltaTime*2);
				this->currentDir.Normalize();
			}
			else
			{
				this->currentDir = targetDir;
			}

			float newRot = atan2(this->currentDir.x, this->currentDir.y);
			this->vehicle->setRotation(Vector3(0, newRot, 0));

			//v = v0 + a * t
			this->velocitySimple = this->velocitySimple + 1.2f/*accelerationSimple*/ * deltaTime;
		}
		else
		{
			if (this->velocitySimple < 0.05f)
			{
				this->velocitySimple = 0;
			}
			else
			{
				this->velocitySimple /= 1 + (1.0f * deltaTime);
			}
		}
		//OutputDebugStringA((std::to_string(this->velocitySimple) + "\n").c_str());
		Vector3 direction(this->currentDir.x, 0, this->currentDir.y);
		//s = s0 + v * t
		this->vehicle->move(direction* this->velocitySimple* deltaTime);
	}
	//Driving Mode: Old Turn towards direction, semi-realistic
	else if (drivingMode == 3) {
		if (Input::GetStrengthL(0) > 0) {
			if (velocitySpeed < (5000* updatedStats.maxSpeed)) {
				this->velocity.x += dx * deltaTime * 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
				this->velocity.y += dy * deltaTime * 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
			}

			if (strength < Input::GetStrengthL(0)) {
				strength = Input::GetStrengthL(0);
			}
			else {
				strength -= 0.01f * deltaTime;
			}



			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += deltaTime*5;
					if (rotateAcceleration > 30.4f) {
						rotateAcceleration = 30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((0.076f) + rotateAcceleration * 1) / 2) * DirectX::XM_PI / 180)*deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += deltaTime*-5;
					if (rotateAcceleration < -30.4f) {
						rotateAcceleration = -30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((-0.076f) + rotateAcceleration * 1 ) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}

			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += deltaTime*-5;
					if (rotateAcceleration < -30.4f) {
						rotateAcceleration = -30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((-0.076f) + rotateAcceleration * 1) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += deltaTime*5;
					if (rotateAcceleration > 30.4f) {
						rotateAcceleration = 30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((0.076f) + rotateAcceleration * 1) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}
		}
		else {
			this->velocity.x = (velocity.x / (1 + (3 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (3 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 20;
			//vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}

		rotateAcceleration /= 1.0f + 3*deltaTime;
		/*if (abs(rotateAcceleration) < 0.5) {
			rotateAcceleration = 0;
		}*/
	}


	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dy / hypoC) + -this->vehicle->getRigidBody()->getLinearVelocity().getZ() * -(dx / hypoC);
	Vector2 driftResistance = Vector2(-((dy / hypoC) * 2000 * deltaTime) * updatedStats.handlingRate, -(-((dx / hypoC) * 2000 * deltaTime)) * updatedStats.handlingRate);
	if (abs(driftForce) < 250) {
		driftResistance = driftResistance * (abs(driftForce)*0.005f);
	}
	if (drivingMode != 2) {
		if (Input::GetStrengthL(0) > 0) {
			if (driftForce < -0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-driftResistance.x,0,0),btVector3(0,0,0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, driftResistance.y), btVector3(0, 0, 0));
			}
			else if (driftForce > 0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(driftResistance.x, 0, 0), btVector3(0, 0, 0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, -driftResistance.y), btVector3(0, 0, 0));
			}
		}
		else {
			if (driftForce < -0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-driftResistance.x, 0, 0), btVector3(0, 0, 0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, driftResistance.y), btVector3(0, 0, 0));
			}
			else if (driftForce > 0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(driftResistance.x, 0, 0), btVector3(0, 0, 0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, -driftResistance.y), btVector3(0, 0, 0));
			}
		}
	}
	//this->vehicle->move(Vector3((velocity.x * deltaTime *0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));
	//this->vehicle->getRigidBody()->setLinearVelocity(btVector3((velocity.x * deltaTime * 0.002f) * 100.0f, this->vehicle->getRigidBody()->getLinearVelocity().getY(), -(velocity.y * deltaTime * 0.002f) * 100.0f));

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
	
	float angle = acos(Vector3(0, 1, 0).Dot(DirectX::XMVector3Normalize(rotationVec)));
	Vector3 axis = Vector3(0, 1, 0).Cross(DirectX::XMVector3Normalize(rotationVec));

	bodyPivot += Vector3(-accelerator.x * deltaTime, 0.00f * 0.0000f, -accelerator.z * deltaTime);

	add += 0.2f * deltaTime;

}

float Vehicle::getAcceleratorX()
{
	return accelerator.x;
}

void Vehicle::setDrivingMode(int i)
{
	this->drivingMode = i;
}

bool Vehicle::getDrivingMode()
{
	return this->drivingMode;
}

Vector3 Vehicle::getVelocity()
{
	return Vector3(velocity.x,0,velocity.y);
}

float Vehicle::getVelocitySpeed()
{
	return velocitySpeed;
}

float Vehicle::getRotator()
{
	return this->mRotation /** (180 / DirectX::XM_PI)*/;
}

const int& Vehicle::getHealthRef() const
{
	return this->health;
}

int Vehicle::getHealth() const
{
	return this->health;
}

int Vehicle::getMaxHealth() const
{
	return this->updatedStats.maxHealth;
}

void Vehicle::setHealth(int health)
{
	this->health = std::clamp(health, 0, this->updatedStats.maxHealth);
}

void Vehicle::setMaxHealth(int maxHealth)
{
	this->updatedStats.maxHealth = max(maxHealth, 1);
}

void Vehicle::resetHealth()
{
	this->health = this->updatedStats.maxHealth;
}

void Vehicle::changeHealth(int amount)
{
	this->health = std::clamp(this->health + amount, 0, this->updatedStats.maxHealth);
	Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
}

bool Vehicle::isDead() const
{
	return this->health <= 0;
}

float Vehicle::getCameraDistance(float deltaTime)
{
	float vehicleDistance = 0;
	Vector3 vehicleCameraDistance = Vector3(this->vehicle->getRigidBody()->getLinearVelocity().getX(), this->vehicle->getRigidBody()->getLinearVelocity().getY(), this->vehicle->getRigidBody()->getLinearVelocity().getZ());
	vehicleDistance = max(abs(vehicleCameraDistance.x), abs(vehicleCameraDistance.z)) *0.7f;

	if (vehicleDistance < 10.0f) {
		vehicleDistance = 0.0f;
	}
	vehicleDistance = min(vehicleDistance, 20.0f);

	cameraDistance = (vehicleDistance - cameraDistance) * deltaTime*1.2f + cameraDistance;

	return cameraDistance;
}

float Vehicle::getPitch(DirectX::XMVECTOR Quaternion)
{
	return atan2(2 * (Quaternion.m128_f32[1] * Quaternion.m128_f32[2] + Quaternion.m128_f32[3] * Quaternion.m128_f32[0]), Quaternion.m128_f32[3] * Quaternion.m128_f32[3] - Quaternion.m128_f32[0] * Quaternion.m128_f32[0] - Quaternion.m128_f32[1] * Quaternion.m128_f32[1] + Quaternion.m128_f32[2] * Quaternion.m128_f32[2]);
}

float Vehicle::getYaw(DirectX::XMVECTOR Quaternion)
{
	return asin(-2 * (Quaternion.m128_f32[0] * Quaternion.m128_f32[2] - Quaternion.m128_f32[3] * Quaternion.m128_f32[1]));
}

float Vehicle::getRoll(DirectX::XMVECTOR Quaternion)
{
	return atan2(2 * (Quaternion.m128_f32[0] * Quaternion.m128_f32[1] + Quaternion.m128_f32[3] * Quaternion.m128_f32[2]), Quaternion.m128_f32[3] * Quaternion.m128_f32[3] + Quaternion.m128_f32[0] * Quaternion.m128_f32[0] - Quaternion.m128_f32[1] * Quaternion.m128_f32[1] - Quaternion.m128_f32[2] * Quaternion.m128_f32[2]);
}

float Vehicle::getHeading(Quaternion qt)
{
	float heading;
	float attitude;
	float bank;

	double test = qt.x * qt.y + qt.z * qt.w;
	if (test > 0.499) { // singularity at north pole
		heading = 2 * atan2(qt.x, qt.w);
		attitude = XM_PI / 2;
		bank = 0;
	}
	else if (test < -0.499) { // singularity at south pole
		heading = -2 * atan2(qt.x, qt.w);
		attitude = -XM_PI / 2;
		bank = 0;
	}
	else {
		double sqx = qt.x * qt.x;
		double sqy = qt.y * qt.y;
		double sqz = qt.z * qt.z;
		heading = atan2(2 * qt.y * qt.w - 2 * qt.x * qt.z, 1 - 2 * sqy - 2 * sqz);
		attitude = asin(2 * test);
		bank = atan2(2 * qt.x * qt.w - 2 * qt.y * qt.z, 1 - 2 * sqx - 2 * sqz);
	}
	return heading;
}
