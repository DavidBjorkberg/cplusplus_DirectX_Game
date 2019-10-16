#ifndef PHYSICS_H
#define PHYSICS_H

#include "BulletSDK/include/btBulletDynamicsCommon.h"
#include <vector>
#include <iostream>
#include <string>
#include<DirectXMath.h>
#include<d3d11.h>
#include<SimpleMath.h>
#include <algorithm>

using namespace DirectX::SimpleMath;
class Physics
{
private:
	btDynamicsWorld* world;
	btDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btConstraintSolver* solver;
	btCollisionConfiguration* collisionConfig;
	std::vector<btRigidBody*> bodies;

	std::vector<btCollisionShape*> shapes;

	btStaticPlaneShape* plane;
public:
	Physics();
	~Physics();
	void update(float deltaTime);
	btRigidBody* addSphere(float radius, btVector3 Origin, float mass);
	btRigidBody* addBox(btVector3 Origin, btVector3 size, float mass);
	/*btRigidBody* addPlayer(btVector3 Origin, btVector3 size, float mass,Player *player);*/

	bool deleteRigidBody(btRigidBody * rb);
	//static bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);
	//void renderSphere(btRigidBody* sphere);
	//void renderPlane(btRigidBody* plane);
	//void renderBox(btRigidBody* box);
	btStaticPlaneShape* getPlaneRigidBody();
};

#endif // !PHYSICS_H