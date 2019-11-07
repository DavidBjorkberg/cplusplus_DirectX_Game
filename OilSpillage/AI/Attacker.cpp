#include "../States/PlayingGameState.h"
#include "Attacker.h"

Attacker::Attacker()
{
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->defaultStats = VehicleStats::AIAttacker;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;
}

Attacker::Attacker(float x, float z, int weaponType)
	:Actor(x, z, weaponType)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->defaultStats = VehicleStats::AIAttacker;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;
	Game::getGraphics().loadModel("Entities/Roller_Melee");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Roller_Melee"));

	boidOffset = 9;
}

Attacker::~Attacker()
{
	Game::getGraphics().removeFromDraw(this);
}

void Attacker::update(float dt, Vector3 targetPos)
{
	this->targetPos = targetPos;
	deltaTime = dt;
	updateWeapon(dt);
	this->root->func();
	followPath();
}

void Attacker::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector = bt.getSelector();
	Sequence& seq2 = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Attacker::inAttackRange, std::ref(*this)));
	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Attacker::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Attacker::setIdleState, std::ref(*this)));
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Attacker::shoot, std::ref(*this)));
	Behavior& enemyNear = bt.getAction();
	enemyNear.addAction(std::bind(&Attacker::enemyNear, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);

	sequence.addChildren(enemyNear);
	sequence.addChildren(selector);

	selector.addChildren(seq2);
	selector.addChildren(chase);

	seq2.addChildren(inRange);
	seq2.addChildren(shoot);
}