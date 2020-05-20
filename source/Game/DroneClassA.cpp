
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

#include "DroneClassA.h"
#include "GameNode.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "Opensteer/include/OpenSteer/Draw.h"
#include "Opensteer/include/OpenSteer/Vec3.h"
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UniginePhysics.h>
#include <UnigineSounds.h>
#include <UnigineVisualizer.h>
#include <UniginePlayers.h>
#include <UnigineObjects.h>
#include <sstream>
#include <iomanip>
#include "Converter.h"
#include "AutoActionTimer.h"


// ----------------------------------------------------------------------------


#include "NodeUI.h"
#include "WeaponControlSystem.h"
#include "BattleUnit.h"
#include "BattleUnitUI.h"
#include "ResourceCapacitySystem.h"
#include "SteeringBehaviors.h"



// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;


// ----------------------------------------------------------------------------


DroneClassA::DroneClassA(GameLevel* level, int id)
	: GameNode(level, id, "SubWorld\\Assets\\Turrets\\OrbialDefenseCannonBase.node")
{
	


}

// ----------------------------------------------------------------------------

DroneClassA::~DroneClassA()
{
	WeaponControlSystem* wcs = ComponentSystem::get()->getComponent<WeaponControlSystem>(_node);
	if (wcs)
	{
		wcs->setResourceCapacity(nullptr);
	}
}



// ----------------------------------------------------------------------------



void DroneClassA::createNode()
{
	GameNode::createNode();
	// move forward
	setConstantSteeringForce(forward());
	// max speed and max steering force (maneuverability) 
	setMaxSpeed(6.0);
	setMaxForce(8.0);
	// initially stopped
	setSpeed(0);
	// randomize 2D heading
	randomizeHeadingOnXZPlane();
	// trail parameters: 3 seconds with 60 points along the trail
	setTrailParameters(4, 40);
	 
	// add basic Ui component
	ComponentSystem::get()->addComponent<NodeUI>(_node);
	// add basic WCS component
	ComponentSystem::get()->addComponent<WeaponControlSystem>(_node);
	// add battle unit component
	ComponentSystem::get()->addComponent<BattleUnit>(_node);
	 // add resource component
	ComponentSystem::get()->addComponent<ResourceCapacitySystem>(_node);

	// init capacities
	initResourcesCapacities();
	// init WCS
	initWeaponControlSystem();

}


// ----------------------------------------------------------------------------


void DroneClassA::initResourcesCapacities()
{
	ResourceCapacitySystem* rcs = ComponentSystem::get()->getComponent<ResourceCapacitySystem>(_node);

	// ---------------------
	auto resource = Resource_Capacity("TORPEDO::MKI", TEMPLATE_TORPEDO, 35, 10,
		[this](void*asset, void *parameter)
	{

		TorpedoPtr	missile = static_cast<Torpedo*>(asset);
#pragma warning( push )
#pragma warning( disable : 4311 4302)
		long threat_gamenode_id = (long)parameter;
#pragma warning( pop ) 
		launchTorpedo(missile, threat_gamenode_id);

	});

	rcs->addCapacity(resource);


}

// ----------------------------------------------------------------------------


void DroneClassA::launchTorpedo(TorpedoPtr	missile, long threat_gamenode_id)
{
	missile->init();
	missile->setConstantSteeringForce(OpenSteer::Vec3::forward);
	_level->addGameNode((GameNodePtr)missile);
#ifdef PLAY_SOUNDS
	// play a sound with autodelete
	Unigine::SoundSourcePtr sound = missile->addSound("SubWorld\\Assets\\Sounds\\LaunchingSubFromStation.wav", 1000 * 45);
	sound->setGain(0.5f);
	sound->play();
#endif

	// wanderer
	GameNodePtr wanderer = GamePlay::Game->getCurrentevel()->getGameNode(threat_gamenode_id);
	missile->setWanderer(wanderer);
	missile->setWorldPosition(OpenSteer::Vec3(position().x + 1, wanderer ? wanderer->position().y : position().y, position().z + 1));
	missile->setFaction(getFaction());
	// missile auto-destruction
	new AutoActionTimer<TorpedoPtr>(missile, 1000 * 30,
		[](AutoActionTimer<TorpedoPtr>& update_delegate, const float currentTime, const float elapsedTime)	{},
		[](AutoActionTimer<TorpedoPtr>& delete_delegate)
	{// destroy the missile
		delete_delegate._ptr->getLevel()->removeGameNode((GameNodePtr)delete_delegate._ptr);
	});
}

// ----------------------------------------------------------------------------



void DroneClassA::initWeaponControlSystem()
{
	ResourceCapacitySystem* rcs = ComponentSystem::get()->getComponent<ResourceCapacitySystem>(_node);
	WeaponControlSystem* wcs = ComponentSystem::get()->getComponent<WeaponControlSystem>(_node);
	std::vector< Weapon> weapons;

	std::vector <weapon_missile_def> missile_definition;
	missile_definition.push_back(weapon_missile_def("TORPEDO::MKI", 1000));

	auto weapon = Weapon(enumWeaponType::ANTI_SURFACE_MISSILE_BASED,
		Unigine::Math::vec3(0, 0, 0), 300, "OrbitalDefenseCannonBody", "", "OrbitalDefenseCannonBarrelAnimated1",
		Unigine::Math::vec3(1, 0, 1), missile_definition,4000);
	weapons.push_back(weapon);

	Radar radar(300);
	wcs->initWCS(radar, weapons, rcs);
	wcs->setHudVisibility(true);
}



// ----------------------------------------------------------------------------

void DroneClassA::update(const float currentTime, const float elapsedTime)
{
	GameNode::update(currentTime, elapsedTime);
	
	annotateNodeUILink();
}




