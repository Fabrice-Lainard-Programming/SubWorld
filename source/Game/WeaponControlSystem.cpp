
// ----------------------------------------------------------------------------
//
//
// SubWorld -- SubMarine Game
//
// Copyright (c) 2020, F.Lainard
// Original author: F.Lainard
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//
// ----------------------------------------------------------------------------



#include "WeaponControlSystem.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "GameNode.h"
#include "ResourceCapacitySystem.h"
#include "PathFinder.h"
#include "Converter.h"
#include <UnigineApp.h>
#include <UnigineWorld.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <list>


// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;


// ----------------------------------------------------------------------------

REGISTER_COMPONENT(WeaponControlSystem);



// ----------------------------------------------------------------------------


void WeaponControlSystem::shutdown()
{
	_showHUD = false;
	_resourceCapacitySystem = nullptr;
}

// ----------------------------------------------------------------------------


void WeaponControlSystem::update()
{
	 
}


// ----------------------------------------------------------------------------


void WeaponControlSystem::init()
{

}

// ----------------------------------------------------------------------------



void WeaponControlSystem::initWCS(Radar radar, std::vector< Weapon>& weapons, ResourceCapacitySystem* resourceCapacitySystem)
{
	_radar = radar;
	_weapons = weapons;
	_resourceCapacitySystem = resourceCapacitySystem;
}


// ----------------------------------------------------------------------------


void WeaponControlSystem::compute_weapons(const float elapsedTime)
{
	GameNodePtr gamenode = getGameNode();
//	printf("\nWCS: Compute Weapons for %ld", gamenode->_id);
	detect_targets();
	track_targets(elapsedTime);
	weapons_fire(elapsedTime);

}


// ----------------------------------------------------------------------------



void WeaponControlSystem::weapons_fire(const float elapsedTime)
{
	for (Threat& threat : _threats)
	{
		for (int weapon_index : threat._attached_weapon_index)
		{

			try_fire_weapon(elapsedTime, threat, _weapons.at(weapon_index));

		//	!!! supprimer  l'association threat/weapon!!!'

		}
	}
	
}


// ----------------------------------------------------------------------------

void WeaponControlSystem::detect_targets()
{
	GameNodePtr gamenode = getGameNode();
	// check the intersection with nodes based on radar performance
	Unigine::VectorStack<NodePtr> nodes;
	std::vector<long> nodesIds = GamePlay::Game->getCurrentevel()->getBoudingSphereIntersection(gamenode->position(), _radar._radar_detection_range / 2);
	if (nodesIds.size() > 1) // (because the current node is also in this list)
	{
		for (int i = 0; i < nodesIds.size(); i++)
		{
			long nodeID = nodesIds[i];

			//printf("\nWCS: IN BOUNDS of %d : %d", gamenode->_id, nodeID);
			// verify the given node
			GameNodePtr threat = GamePlay::Game->getCurrentevel()->getGameNode(nodeID);
			// add to threat list if needed
			if (threat && (threat->_id != gamenode->_id) && GamePlay::Game->isEnemy(gamenode->getFaction(), threat->getFaction()))
			{
				add_threat(threat->_id);
			}
		}
	}
}



// ----------------------------------------------------------------------------

void WeaponControlSystem::track_targets(const float elapsedTime)
{
	// remove threat which are out of radar range
	remove_threat_out_of_range();

	// attach targets to weapons
	attach_threats_to_weapons();

	// adjust missile guidance
	adjust_weapons_guidance(elapsedTime);
}

// ----------------------------------------------------------------------------
// Threats
// ----------------------------------------------------------------------------


void WeaponControlSystem::attach_threats_to_weapons()
{
	for (Threat& threat : _threats)
	{
		bool acquired = try_attach_threat_to_weapon(threat);
		if (!acquired)
		{
			//printf("\nWCS: warning threat %ld is not acquired !", threat._gamenode_id);
		}
	}
}


// ----------------------------------------------------------------------------


bool WeaponControlSystem::try_attach_threat_to_weapon(Threat& threat)
{
	if (threat._attached_weapon_index.size() == 0)
	{
		// threat is not managed by a weapon
		int weapon_idx = 0;
		// search a weapon 
		for (Weapon& weapon : _weapons)
		{
			// if weapon has no targets and target is in range
			if (!weapon._target_acquired && is_threat_manageable_by_weapon(threat, weapon))
			{
				// attach the weapon to the threat
				threat._attached_weapon_index.push_back(weapon_idx);
				weapon._target_acquired = true;
				return true;
			}
			weapon_idx++;
		}
		// target cannot be managed by weapon
		return false;
	}
	// threat already managed by a weapon
	return true;
}

// ----------------------------------------------------------------------------


void WeaponControlSystem::remove_threat_out_of_range()
{
	// remove threat which are out of radar range
	auto it = std::remove_if(_threats.begin(), _threats.end(),
		[this](const Threat& threat)
	{
		return !is_threat_in_radar_range(threat);
	});
	_threats.erase(it, _threats.end());
}



// ----------------------------------------------------------------------------


bool WeaponControlSystem::is_threat_in_radar_range(const Threat& threat)
{
	NodePtr threatnode = Unigine::World::getNodeById(threat._gamenode_id);
	if (!threatnode) return false;
	GameNodePtr gnode = static_cast<GameNode*>(node->getPtr());
	GameNodePtr tnode = static_cast<GameNode*>(threatnode->getPtr());
	if (!tnode || !gnode) return false;
	Unigine::Math::Vec3 d(Converter::toUnigine(gnode->position()) - Converter::toUnigine(tnode->position()));
	float distance = d.length();
	if (distance < _radar._radar_detection_range)
		return true;
	return false;
}

// ----------------------------------------------------------------------------

const Threat* WeaponControlSystem::find_threat(long gamenode_id)
{
	for (const Threat& threat : _threats)
	{
		if (threat._gamenode_id == gamenode_id)
		{
			return &threat;
		}
	}
	return nullptr;
}



// ----------------------------------------------------------------------------

void WeaponControlSystem::add_threat(long gamenode_id)
{
	if (!find_threat(gamenode_id))
	{
		_threats.push_back(Threat(gamenode_id));
		//printf("\nWCS: new threat %ld acquired", gamenode_id);
	}
}


// ----------------------------------------------------------------------------
// Weapons
// ----------------------------------------------------------------------------


 
bool WeaponControlSystem::try_fire_weapon(const float elapsedTime, Threat& threat, Weapon& weapon)
{ 
	// determine if we can fire now
	auto now = std::chrono::system_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - weapon._lastFireTime   ).count();
	if (elapsed_ms < weapon._fire_interval_ms)
		return false;

	//TODO : select weapon in weapon list
	if (weapon._missile_definition.size() == 0) return false;
	std::string missile_type = weapon._missile_definition.at(0)._missile_resource_name;
	// create the resource with the RCS system (passing the gamenode id of the threat)
	_resourceCapacitySystem->planify_creation(missile_type, 1, (void*)threat._gamenode_id);
	weapon._lastFireTime = now;
	threat._fire_count++;
	return true;
}


void WeaponControlSystem::adjust_weapons_guidance(const float elapsedTime)
{
	for (Threat& threat : _threats)
	{
		for (int weapon_index : threat._attached_weapon_index)
		{
			adjust_weapon_guidance(elapsedTime, threat, _weapons.at(weapon_index));
		}
	}
}

// ----------------------------------------------------------------------------


void WeaponControlSystem::adjust_weapon_guidance(const float elapsedTime, Threat& threat, Weapon& weapon)
{
	GameNodePtr gamenode = getGameNode();
	// compute the position of the weapon in world coordinates
	Unigine::Math::vec3 position = Converter::toUnigine(gamenode->position()) + weapon._local_position;
	// compute the position of the target in world coordinates
	NodePtr threatnode = Unigine::World::getNodeById(threat._gamenode_id);
	if (!threatnode) return;
	// compute the needed rotation of weapon's axis
	Unigine::Math::mat4 transform;
	transform = Unigine::Math::setTo(position, threatnode->getWorldPosition(), Unigine::Math::vec3_up, Unigine::Math::AXIS_Y);
	float rotX = transform.getRotate().getAngle(Unigine::Math::vec3(1, 0, 0));
	float rotY = transform.getRotate().getAngle(Unigine::Math::vec3(0, 1, 0));
	float rotZ = transform.getRotate().getAngle(Unigine::Math::vec3(0, 0, 1));

	weapon._rotation_angles = Unigine::Math::vec3(rotX, rotY, rotZ);
}

// ----------------------------------------------------------------------------


void WeaponControlSystem::align_weapon(const float elapsedTime, Threat& threat, Weapon& weapon)
{
	// running average of recent orientation
	OpenSteer::blendIntoAccumulator(elapsedTime *8.0, // QQQ
		weapon._rotation_angles,
		weapon._smoothed_orientation);


	// rotate angles 

	if (weapon._authorized_rotation_angles.x > 0 && !weapon._x_axis_rotation_node.empty())
	{
		Unigine::NodePtr  mesh = node->findNode(weapon._x_axis_rotation_node.c_str(), 1);
		Unigine::Math::quat r(weapon._smoothed_orientation.x, 0, 0);
		mesh->setWorldRotation(r);
	}

	if (weapon._authorized_rotation_angles.y > 0 && !weapon._y_axis_rotation_node.empty())
	{
		Unigine::NodePtr  mesh = node->findNode(weapon._y_axis_rotation_node.c_str(), 1);
		Unigine::Math::quat r(0, weapon._smoothed_orientation.y, 0);
		mesh->setWorldRotation(r);
	}


	if (weapon._authorized_rotation_angles.z > 0 && !weapon._z_axis_rotation_node.empty())
	{

		Unigine::NodePtr  mesh = node->findNode(weapon._z_axis_rotation_node.c_str(), 1);
		Unigine::Math::quat r(0, 0, weapon._smoothed_orientation.z);
		mesh->setWorldRotation(r);
	}
}

// ----------------------------------------------------------------------------


bool WeaponControlSystem::is_threat_manageable_by_weapon(const Threat& threat, const Weapon& weapon)
{
	// first test distance range
	bool manageable = is_threat_in_weapon_range(threat, weapon);
	// then air-air / submarine - air / etc..
	// TODO
	return manageable;
}

// ----------------------------------------------------------------------------


bool WeaponControlSystem::is_threat_in_weapon_range(const Threat& threat, const Weapon& weapon)
{
	NodePtr threatnode = Unigine::World::getNodeById(threat._gamenode_id);
	GameNodePtr gthreat = static_cast<GameNode*>(threatnode->getPtr());
	GameNodePtr gnode = getGameNode();
	if (!gthreat || !gnode) return false;
	Math::Vec3 d(Converter::toUnigine(gthreat->position()) - Converter::toUnigine(gnode->position()));
	float distance = d.length();
	if (distance < weapon._range)
		return true;
	return false;
}

// ----------------------------------------------------------------------------

GameNodePtr WeaponControlSystem::getGameNode()
{
	return static_cast<GameNode*>(node->getPtr());
}


 
// ----------------------------------------------------------------------------


void WeaponControlSystem::align_weapons(const float elapsedTime)
{

	for (Threat& threat : _threats)
	{
		for (int weapon_index : threat._attached_weapon_index)
		{
			align_weapon(elapsedTime, threat, _weapons.at(weapon_index));
		}
	}
}



// ----------------------------------------------------------------------------


std::vector<std::string> WeaponControlSystem::toHUDString()
{
	 
	std::vector<std::string> str;
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return str;
	Unigine::Math::Vec3 pos(gamenode->position().x, gamenode->position().z, gamenode->position().y + 5);
	Unigine::Math::Vec3 center(-1, -1, 0);

	int threadIdx = 0;

	
	char buff[500];

	for (Threat& threat : _threats)
	{
		GameNodePtr threatNode = GamePlay::Game->getCurrentevel()->getGameNode(threat._gamenode_id);
		if (!threatNode) continue;

		
		std::string weaponsList;
		for (int weapon_index : threat._attached_weapon_index)
		{
			Weapon& weapon = _weapons.at(weapon_index);
			snprintf(buff, sizeof(buff), " Weapon N°%d  #Fire: %d  Target Acquired = %d", weapon_index, threat._fire_count, weapon._target_acquired);
			weaponsList += buff;

		}

		snprintf(buff, sizeof(buff), "Detected Threat N°%d Position: %d,%d,%d %s", threadIdx,
			(int)threatNode->position().x, (int)threatNode->position().z, (int)threatNode->position().y, weaponsList.c_str());

		str.push_back(buff);

	 	threadIdx++;
	}

	return str;


}


// ----------------------------------------------------------------------------
