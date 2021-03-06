#include "Turret.h"

Turret::Turret()
{}

Turret::Turret(float x, float z, int weaponType, Physics* physics)
	: Actor(x, z), Ranged(&this->position, &this->targetPos, &this->velocity, &this->deltaTime, weaponType)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->sightRange = 23;
	turretAngle = 90;
	this->calculateTarget(turretAngle);
	setUpActor();
	this->body.setPosition(this->position);
	this->body.setScale(this->scale);
	this->body.mesh = Game::getGraphics().getMeshPointer("Entities/Turret");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Turret1");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Turret"));
	this->body.setMaterial(Game::getGraphics().getMaterial("Entities/Turret"));
	Game::getGraphics().addToDraw(&this->body);
	Game::getGraphics().addToDraw(this);
	this->stats = VehicleStats::AITurret;

	float newHealth = stats.maxHealth;
	scaling(newHealth, 1.3);
	setMaxHealth(newHealth);
	setHealth(newHealth);
	scaling(weapon.damage, 1.1);
	this->setPoints(100 * (1 + (0.2 * Game::getGameInfo().nrOfClearedStages)));

	this->velocity = Vector3();
	createRigidbody(physics);
}

Turret::~Turret()
{
	Game::getGraphics().removeFromDraw(&this->body);
	Game::getGraphics().removeFromDraw(this);
}

void Turret::update(float dt, const Vector3& targetPos)
{
	Actor::update(dt, targetPos);
	updateBullets(dt);
	onFire();
}

void Turret::createRigidbody(Physics* physics)
{
	btRigidBody* tempo = physics->addSphere(0.5f, btVector3(position.x, position.y, position.z), 0, this);
	setRigidBody(tempo, physics);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(0, 0, 0));
}

void Turret::setForwardVector(Vector3 forward)
{
	this->vecForward = forward;
}

void Turret::setSightRange(int range)
{
	this->sightRange = range;
}

void Turret::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Turret::inRange, std::ref(*this)));

	Behavior& rotate = bt.getAction();
	rotate.addAction(std::bind(&Turret::rotateTowards, std::ref(*this)));

	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Turret::shoot, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Turret::idle, std::ref(*this)));

	Behavior& lineOf = bt.getAction();
	lineOf.addAction(std::bind(&Turret::lineOfSight, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(idle);
	sequence.addChildren(inRange);
	sequence.addChildren(lineOf);
	sequence.addChildren(rotate);
	sequence.addChildren(shoot);
}

Status Turret::rotateTowards()
{
	Vector3 targetToSelf = (targetPos - position);

	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.1f;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		this->setRotation(Vector3(0, newRot - (XM_PI / 2), 0));
	}
	return Status::SUCCESS;
}

Status Turret::inRange()
{
	Status status;
	if ((getPosition() - targetPos).Length() > this->sightRange)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Turret::lineOfSight()
{
	Status status = Status::FAILURE;
	Vector3 dir = this->position - targetPos;
	dir.Normalize();
	float value = dir.Dot(vecForward);
	float angle = std::abs(acos(value));
	if (angle <= XM_PIDIV2)
	{
		status = Status::SUCCESS;
	}
	return status;

}

void Turret::calculateTarget(float angle)
{
	float radians = angle * (3.14f / 180.f);
	float x = cos(radians) * vecForward.x + sin(radians) * vecForward.z;
	float y = cos(radians) * vecForward.x - sin(radians) * vecForward.z;
	this->idleTarget = Vector3(x, 0.0f, y);
	this->idleTarget.Normalize();
}

Status Turret::idle()
{
	/*calculate new target position if are getting close to the point*/
	if (idleTarget.Dot(vecForward) <= 0.1f)
	{
		turretAngle += 90;
		this->calculateTarget(turretAngle);
	}
	else
	{
		vecForward -= (idleTarget * deltaTime);
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		Vector3 tempRot = Vector3(0, newRot - (XM_PIDIV2), 0);
		this->setRotation(tempRot);
	}
	return Status::SUCCESS;
}