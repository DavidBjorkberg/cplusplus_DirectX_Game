#ifndef VEHICLE_STATS_H
#define VEHICLE_STATS_H

struct Stats
{
	float durability = 1.0F;
	float armour = 1.0F;
	float accelerationRate = 1.0F;
	float maxSpeed = 1.0F;
	float handlingRate = 1.0F;
};

class VehicleStats
{
public:
	static constexpr Stats defaultStats = Stats();

	                               //{ durability, armour, accelerationRate, maxSpeed, handlingRate };
	static constexpr Stats fastCar = {       1.0F,   1.0F,             1.2F,     2.0F,         1.2F };
};

#endif // !VEHICLE_STATS_H