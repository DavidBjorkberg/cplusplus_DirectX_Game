#include "ItemWeapon.h"
#include <sstream>
#include "../game.h"
std::string ItemWeapon::generateDescription(Weapon weapon)
{
	std::stringstream stream;
	if (weapon.type == WeaponType::MachineGun) {
		stream << "Damage/S: " << weapon.damage / weapon.fireRate << "\n";
		stream << "Fire Rate: " << weapon.fireRate << "\n";
		stream << "Bullet Speed: " << weapon.bulletSpeed << "\n";
		stream << "Max Spread: " << weapon.maxSpread << "\n";
	}
	else if(weapon.type == WeaponType::Laser){
		stream << "Damage/S: " << weapon.damage / weapon.fireRate << "\n";
		stream << "Overheat : " << weapon.maxSpread << "\n";

	}
	else if(weapon.type == WeaponType::Flamethrower) {
		stream << "Damage/S: " << weapon.damage / weapon.fireRate << "\n";
		stream << "Fire Rate: " << weapon.fireRate << "\n";
		stream << "Bullet Speed: " << weapon.bulletSpeed << "\n";
		stream << "Max Spread: " << weapon.maxSpread << "\n";
	}
	else if(weapon.type == WeaponType::Spikes){
		stream << "Damage/S: " << weapon.damage / weapon.fireRate << "\n";
	}
	else {
		stream << "Damage/S: " << weapon.damage / weapon.fireRate << "\n";
		stream << "Fire Rate: " << weapon.fireRate << "\n";
		stream << "Bullet Speed: " << weapon.bulletSpeed << "\n";
		stream << "Max Spread: " << weapon.maxSpread << "\n";
	}
	
	return stream.str();
}

ItemWeapon::ItemWeapon(std::string name, Weapon weapon, GameObject * object) : Item(name, generateDescription(weapon), ItemType::WEAPON, object), weapon(weapon)
{

}

ItemWeapon::~ItemWeapon()
{
}

ItemWeapon::ItemWeapon(const ItemWeapon& obj) : Item(obj)
{
	this->weapon = obj.weapon;
}

Item* ItemWeapon::clone() const
{
	return new ItemWeapon(*this);
}

void ItemWeapon::randomize()
{
	//damage
	//fireRate
	//bulletSpeed
	//bulletLifetime
	//spreadRadians
	//maxSpread
	//spreadIncreasePerSecond
	//spreadDecreasePerSecond
	if (weapon.type == WeaponType::MachineGun) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 400) *0.01f) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians + ((rand() % 101) *0.01f)*0.52f;//30 degree = 0.52 radians
		this->weapon.maxSpread = this->weapon.maxSpread + ((rand() % 101) * 0.01f+1.0f) * this->weapon.spreadRadians;
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
	}
	else if (weapon.type == WeaponType::Laser) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 101) * 0.01f) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed * 1;
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * 1;
		this->weapon.spreadRadians = this->weapon.spreadRadians * (((rand() % 101) * 0.01f) + (1 * Game::getLocalScale()));
		this->weapon.maxSpread = this->weapon.maxSpread * (((rand() % 101) * 0.01f) + (1 * Game::getLocalScale()));
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * 1/*(((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()))*/;
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond *1 /*(((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()))*/;
	}
	else if (weapon.type == WeaponType::Flamethrower) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 50) * 0.01f) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed;// *(((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 50)*0.01f) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians + ((rand() % 101) * 0.01f) * 0.52f;//30 degree = 0.52 radians
		this->weapon.maxSpread = this->weapon.maxSpread + ((rand() % 101) * 0.01f + 1.0f) * this->weapon.spreadRadians;
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
	}
	else if (weapon.type == WeaponType::Spikes) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.maxSpread = this->weapon.maxSpread * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
	}
	else 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.maxSpread = this->weapon.maxSpread * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
	}
	

	this->description = generateDescription(this->weapon);
	if (this->weapon.type == WeaponType::Laser)
	{
		this->weapon.lightColor = Vector3(rand(), rand(), rand());
		this->weapon.lightColor /= max(max(this->weapon.lightColor.x, this->weapon.lightColor.y), this->weapon.lightColor.z);
	}
	Item::randomize();
}

Weapon& ItemWeapon::getWeapon()
{
	return this->weapon;
}
