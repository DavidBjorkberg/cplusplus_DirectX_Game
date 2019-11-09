#include "../States/PlayingGameState.h"
#include "Car_Spitfire.h"
#include "../Input.h"
void Spitfire::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Spitfire::inAggroRange, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Spitfire::setIdleState, std::ref(*this)));

	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Spitfire::setChaseState, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(idle);

	sequence.addChildren(inRange);
	sequence.addChildren(chase);
}

Spitfire::Spitfire(float x, float z, Physics* physics)
	: DynamicActor(x,z)
{
	velocity = Vector2(0.0f, 0.0f);

	targetRotation = 0.0f;
	drivingMode = 1;
	this->rotateAcceleration = 0.0f;
	this->velocitySpeed = 0.0f;
	this->rotationDirection = true;
	this->vehicle = nullptr;
	this->reverseTimer = 0;
	this->reverseTimer2 = 0;
	this->direction = Vector3(1, 0, 0);
	//this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->deltaTime = 0;
	setUpActor();
	throttleInputStrength = 0;
//	car = new AIVehicle();
	init(physics);
	vehicle->setPosition(Vector3(position.x, 0 - 1.2f, position.z));
	vehicleBody1->setPosition(Vector3(position.x, 0 - 1.2f + 0.65f, position.z));
	this->stats = VehicleStats::AIAttacker;
	setHealth(this->stats.maxHealth);
	this->aggroRange = 500; //TODO: Find better aggro range 

	//Game::getGraphics().loadModel("Entities/Dummy_Player_Car1");
	//this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car1");
	//this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car1"));
	//Game::getGraphics().addToDraw(&mesh);

}

Spitfire::Spitfire()
{
}

Spitfire::~Spitfire()
{
	Game::getGraphics().removeFromDraw(vehicleBody1);
	Game::getGraphics().removeFromDraw(wheel1);
	Game::getGraphics().removeFromDraw(wheel2);
	Game::getGraphics().removeFromDraw(wheel3);
	Game::getGraphics().removeFromDraw(wheel4);
	delete vehicle;
	delete vehicleBody1;

	delete wheel1;
	delete wheel2;
	delete wheel3;
	delete wheel4;
	//delete this->car;
}

void Spitfire::updateVehicle()
{
	prevAccelForce = Vector3(vehicle->getRigidBody()->getLinearVelocity());
	vehicleMovement(deltaTime, throttleInputStrength, false, false, Vector2(-direction.x, -direction.z));
	Vector3 accelForce = Vector3(vehicle->getRigidBody()->getLinearVelocity().getX(),vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ()) - Vector3(prevAccelForce.x, prevAccelForce.y, prevAccelForce.z);
	setAccelForce(accelForce, deltaTime);
	setWheelRotation();
	this->setPosition(vehicle->getPosition());

}

void Spitfire::move()
{
	direction = this->position - destination;
	direction.Normalize();
	if ((this->position - destination).Length() > 5)
	{
		throttleInputStrength += 0.025 * deltaTime;
		if (throttleInputStrength > 1)
		{
			throttleInputStrength = 1;
		}
	}
	/*else if ((car->getVehicle()->getPosition() - destination).Length() < 5)
	{
		throttleInputStrength -= 0.5 * deltaTime;
		if (throttleInputStrength < 0)
		{
			throttleInputStrength = 0;
		}
	}*/

}

void Spitfire::update(float dt, Vector3 targetPos)
{
	DynamicActor::update(dt, targetPos);
	updateVehicle();
}

void Spitfire::followPath()
{
	if (path != nullptr)
	{

		if (path->size() > 0)
		{
			destination = path->at(path->size() - 1);
			if ((destination - this->position).Length() < 15)
			{
				path->pop_back();
			}
		}
		else
		{
			destination = targetPos;
		}
	}
}

void Spitfire::init(Physics* physics)
{
	this->physics = physics;
	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	//Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 10.0f, 0.0f));
	vehicle->setScale(Vector3(0.5f, 0.14f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));


	this->vehicleBody1 = new GameObject;
	vehicleBody1->mesh = Game::getGraphics().getMeshPointer("Entities/Player1");
	vehicleBody1->setSpotShadow(false);
	Game::getGraphics().addToDraw(vehicleBody1);
	vehicleBody1->setScale(Vector3(0.005f, 0.005f, 0.005f));
	//vehicleBody1->setPosition(Vector3(0.0f, 0.65f, 0.0f));
	//vehicleBody1->setScale(Vector3(0.5f, 0.22f, 0.9f));

	vehicleBody1->setTexture(Game::getGraphics().getMaterial("Entities/Player").diffuse);

	this->wheel1 = new GameObject;
	this->wheel2 = new GameObject;
	this->wheel3 = new GameObject;
	this->wheel4 = new GameObject;
	wheel1->mesh = Game::getGraphics().getMeshPointer("Entities/Player");
	wheel2->mesh = Game::getGraphics().getMeshPointer("Entities/Player2");
	wheel3->mesh = Game::getGraphics().getMeshPointer("Entities/Player3");
	wheel4->mesh = Game::getGraphics().getMeshPointer("Entities/Player4");
	Game::getGraphics().addToDraw(wheel1);
	Game::getGraphics().addToDraw(wheel2);
	Game::getGraphics().addToDraw(wheel3);
	Game::getGraphics().addToDraw(wheel4);
	wheel1->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel2->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel3->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel4->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel1->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel2->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel3->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel4->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel1->setTexture(Game::getGraphics().getMaterial("Entities/Player").diffuse);
	wheel2->setTexture(Game::getGraphics().getMaterial("Entities/Player").diffuse);
	wheel3->setTexture(Game::getGraphics().getMaterial("Entities/Player").diffuse);
	wheel4->setTexture(Game::getGraphics().getMaterial("Entities/Player").diffuse);


	btRigidBody* tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z), btVector3(vehicle->getScale().x, vehicle->getScale().y, vehicle->getScale().z), 10.0f);
	vehicle->setRigidBody(tempo, physics);
	vehicle->getRigidBody()->activate();
	vehicle->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	vehicle->getRigidBody()->setFriction(0);
	vehicle->getRigidBody()->setLinearFactor(btVector3(1, 0, 1));

	tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y + 0.65f, vehicle->getPosition().z), -btVector3(this->vehicleBody1->getAABB().minPos.x - this->vehicleBody1->getAABB().maxPos.x, (this->vehicleBody1->getAABB().minPos.y - this->vehicleBody1->getAABB().maxPos.y) * 0.2f, this->vehicleBody1->getAABB().minPos.z - this->vehicleBody1->getAABB().maxPos.z) * 0.5f, 1.0f);
	vehicleBody1->setRigidBody(tempo, physics);
	vehicleBody1->getRigidBody()->activate();
	vehicleBody1->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	vehicleBody1->getRigidBody()->setFriction(1);

	physics->addPointJoint(this->vehicle->getRigidBody(), this->vehicleBody1->getRigidBody());

}

void Spitfire::vehicleMovement(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput)
{

	//Quaternion Rotation to Euler
	btScalar m;
	btScalar n;
	Quaternion qt = Vector4(this->vehicle->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getW());

	qt = this->vehicle->getRotationQuaternion();

	float heading = getHeading(qt);

	this->vehicle->getRigidBody()->getWorldTransform().getRotation().getEulerZYX(n, mRotation, m);

	//Current rotation of vehicle
	float vehicleRotation = fmod((/*(getYaw(qt)*/heading * 180 / DirectX::XM_PI), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
	mRotation = vehicleRotation;

	//Rotate Chassis toward Body
	vehicleBody1->getRigidBody()->applyImpulse(btVector3(0, 80 * deltaTime, 0), btVector3(0, 10, 0));
	vehicleBody1->getRigidBody()->setDamping(0, 0.99f);
	qt = Vector4(this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getW());
	float bodyHeading = getHeading(qt);

	bodyHeading = fmod((bodyHeading * 180 / DirectX::XM_PI), 360.0f);
	heading = fmod((heading * 180 / DirectX::XM_PI), 360.0f);
	float difference = (180 - abs(abs(bodyHeading - heading) - 180)) * 0.45f; ;
	if (bodyHeading < heading) {
		if (abs(bodyHeading - heading) < 180) {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
		else {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), -difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
	}
	else {
		if (abs(bodyHeading - heading) < 180) {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), -difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
		else {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
	}

	//Direction of Vehicle in dx & dy
	float dx = sin((DirectX::XM_PI / 180) * vehicleRotation);
	float dy = -cos((DirectX::XM_PI / 180) * vehicleRotation);


	targetRotation = (atan2(-directionInput.x, -directionInput.y) * 180 / DirectX::XM_PI) + 180;
	velocitySpeed = (this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dx)) + (-this->vehicle->getRigidBody()->getLinearVelocity().getZ() * (dy));

	Vector3 steering = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
		throttleInputStrength * /*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
		vehicle->getRigidBody()->getAngularVelocity().getZ());

	Vector3 steering2 = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
		throttleInputStrength * /*deltaTime*/0.035f * 80 * max(velocitySpeed * 0.15f, -1),
		vehicle->getRigidBody()->getAngularVelocity().getZ());


	//Driving Mode: Turn towards direction, semi-realistic
	if (drivingMode == 1)
	{

		Vector3 steering3 = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
			/*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
			vehicle->getRigidBody()->getAngularVelocity().getZ());

		if (throttleInputStrength > 0 && !this->isDead()) {
			if (velocitySpeed < 0.5f) {
				reverseTimer += 10.0f * deltaTime;
			}
			else {
				reverseTimer = 0;
				reverseTimer2 = 0;
			}
			if ((reverseTimer > 10.0f) && !(reverseTimer2 > 10.0f)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-dx * deltaTime * 160.0f * stats.accelerationRate, 0, (dy * deltaTime * 160.0f * stats.accelerationRate)) * throttleInputStrength, btVector3(0, 0, 0));
				reverseTimer2 += 10.0f * deltaTime;
				if (reverseTimer2 > 10.0f) {
					reverseTimer = 0;
					reverseTimer2 = 0;
				}
			}
			else {
				reverseTimer2 = 0;
				if (velocitySpeed < (40 * stats.maxSpeed)) {
					this->vehicle->getRigidBody()->applyImpulse(btVector3(dx * deltaTime * 160.0f * stats.accelerationRate, 0, -(dy * deltaTime * 160.0f * stats.accelerationRate)) * throttleInputStrength, btVector3(0, 0, 0));
				}
			}

			float difference2 = min((180 - abs(abs(vehicleRotation - targetRotation) - 180)) * 0.05f, 1.0f);

			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering3.y * difference2, 0));
				}
				else {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, -steering3.y * difference2, 0));
				}
			}
			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, -steering3.y * difference2, 0));
				}
				else {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering3.y * difference2, 0));
				}
			}
		}
		else if (!isDead()) {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (1.8f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (1.8f * deltaTime))));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;
			//vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime*2 * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)*DirectX::XM_PI / 180) * deltaTime * 120 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}
		else {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (0.6f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (0.6f * deltaTime))));
			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;

			vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, (vehicle->getRigidBody()->getAngularVelocity().getY() / (1 + (0.8f * deltaTime)))/** min(abs(velocitySpeed), 1)*/, steering.z));
		}

		rotateAcceleration /= 1.0f + 6 * deltaTime;
	}


	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dy / hypoC) + -this->vehicle->getRigidBody()->getLinearVelocity().getZ() * -(dx / hypoC);
	Vector2 driftResistance = Vector2(-((dy / hypoC) * 4000 * deltaTime) * stats.handlingRate, -(-((dx / hypoC) * 4000 * deltaTime)) * stats.handlingRate);
	if (abs(driftForce) < 250) {
		driftResistance = driftResistance * (abs(driftForce) * 0.005f);
	}
	if (drivingMode != 2) {
		if (Input::getStrengthL() > 0) {
			if (driftForce < -0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-driftResistance.x, 0, 0), btVector3(0, 0, 0));
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
	//this->vehicle->getRigidBody()->setLinearVelocity(btVector3((velocity.x * deltaTime*2 * 0.002f) * 100.0f, this->vehicle->getRigidBody()->getLinearVelocity().getY(), -(velocity.y * deltaTime*2 * 0.002f) * 100.0f));
	vehicle->getRigidBody()->setAngularVelocity(btVector3(0, vehicle->getRigidBody()->getAngularVelocity().getY(), 0));


	if (deltaTime > 0.01f) {
		soundTimer += 100.0f * deltaTime;
	}
	else {
		soundTimer += 100.0f;
	}

}

void Spitfire::setAccelForce(Vector3 accelForce, float deltaTime)
{

	//AccelForce
	if ((max(abs(accelForce.x), abs(accelForce.z)) > 5.0f)) {
		int randomSound = rand() % 3 + 1;
		std::wstring soundEffect = L"data/sound/CarImpact" + std::to_wstring(randomSound) + L".wav";
		int randomSound2 = rand() % 3 + 1;
		std::wstring soundEffect2 = L"data/sound/MetalImpactPitched" + std::to_wstring(randomSound) + L".wav";
		if (max(abs(accelForce.x), abs(accelForce.z)) > 25.0f) {
			Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
			changeHealth(-20.0f);
			Sound::PlaySoundEffect(L"data/sound/CarCrash.wav");
			Sound::PlaySoundEffect(soundEffect2);
		}
		else if (max(abs(accelForce.x), abs(accelForce.z)) > 15.0f) {
			Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
			changeHealth(-10.0f);
			Sound::PlaySoundEffect(soundEffect);
			Sound::PlaySoundEffect(soundEffect2);
		}
		else {
			Sound::PlaySoundEffect(L"data/sound/CarImpactSoft.wav");
			Sound::PlaySoundEffect(soundEffect2);
		}
	}
	/*else {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x/(1 + 20.0f*deltaTime), vehicleBody1->getColor().y / (1 + 20.0f * deltaTime), vehicleBody1->getColor().z / (1 + 20.0f * deltaTime), 1));
	}*/
}

void Spitfire::setWheelRotation()
{
	wheel1->setPosition(vehicle->getPosition() + Vector3(0, 0.25f, 0));
	wheel2->setPosition(vehicle->getPosition() + Vector3(0, 0.25f, 0));
	wheel3->setPosition(vehicle->getPosition() + Vector3(0, 0.25f, 0));
	wheel4->setPosition(vehicle->getPosition() + Vector3(0, 0.25f, 0));

	wheel1->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel2->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel3->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel4->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
}

float Spitfire::getYaw(DirectX::XMVECTOR Quaternion)
{
	return asin(-2 * (Quaternion.m128_f32[0] * Quaternion.m128_f32[2] - Quaternion.m128_f32[3] * Quaternion.m128_f32[1]));
}

float Spitfire::getHeading(Quaternion qt)
{
	float heading;
	float attitude;
	float bank;

	float test = qt.x * qt.y + qt.z * qt.w;
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
		float sqx = qt.x * qt.x;
		float sqy = qt.y * qt.y;
		float sqz = qt.z * qt.z;
		heading = atan2(2 * qt.y * qt.w - 2 * qt.x * qt.z, 1 - 2 * sqy - 2 * sqz);
		attitude = asin(2 * test);
		bank = atan2(2 * qt.x * qt.w - 2 * qt.y * qt.z, 1 - 2 * sqx - 2 * sqz);
	}
	return heading;
}