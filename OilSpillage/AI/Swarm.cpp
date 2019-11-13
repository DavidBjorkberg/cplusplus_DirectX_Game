#include "../States/PlayingGameState.h"
#include "Swarm.h"

Swarm::Swarm()
{

}

Swarm::Swarm(float x, float z, int weaponType)
	: DynamicActor(x,z),Melee()
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AISwarm;
	setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Drone");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Drone");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Drone"));
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMelee);

	this->boidOffset = 2.5;
	this->aggroRange = 40;
}

void Swarm::update(float dt, const Vector3& targetPos)
{
	DynamicActor::update(dt, targetPos);
}

Swarm::~Swarm()
{
	Game::getGraphics().removeFromDraw(this);
}

void Swarm::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector = bt.getSelector();
	Sequence& seq2 = bt.getSequence();


	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Swarm::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Swarm::setIdleState, std::ref(*this)));
	Behavior& inAggroRange = bt.getAction();
	inAggroRange.addAction(std::bind(&Swarm::inAggroRange, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);

	sequence.addChildren(inAggroRange);
	sequence.addChildren(selector);

	selector.addChildren(seq2);
	selector.addChildren(chase);
}

Vector3 Swarm::seek()
{
	Vector3 desiredDirection;
	Vector3 offsetVec;
	Vector3 eliminatingVec = Vector3(0.0f, -1.0f, 0.0f) - Vector3(0.0f, 1.0f, 0.0f);
	//standard group movement
	if (!vActive)
	{
		desiredDirection -= position - destination;
		//desired *= maxSpeed;
		if (this->stats.maxSpeed != 7.0)
		{
			this->stats.maxSpeed = 7.0;
		}
	}

	//strafe to the left of player
	else if (vActive && duty == 1)
	{
		if (this->stats.maxSpeed == 7.0)
		{
			this->stats.maxSpeed = 9.0;
		}
		Vector3 crossVector = Vector3(position.x - destination.x, 0.0f, position.z - destination.z);
		offsetVec = crossVector.Cross(eliminatingVec);
		offsetVec.Normalize();
		offsetVec *= 8;
		desiredDirection -= position - (destination - offsetVec);
	}

	//strafe to the right of player
	else if (vActive && duty == 2)
	{
		if (this->stats.maxSpeed == 7.0)
		{
			this->stats.maxSpeed = 9.0;
		}
		Vector3 crossVector = Vector3(position.x - destination.x, 0.0f, position.z - destination.z);
		offsetVec = crossVector.Cross(eliminatingVec);
		offsetVec.Normalize();
		offsetVec *= 8;
		desiredDirection -= position - (destination + offsetVec);
	}

	acceleration = desiredDirection - velocity;
	if (acceleration.Length() > maxForce)
	{
		acceleration /= acceleration.Length();
	}
	vActive = false;
	return acceleration;
}

