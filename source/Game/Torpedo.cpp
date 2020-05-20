

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

#include "Torpedo.h"
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
#include <UnigineWorld.h>
#include <sstream>
#include <iomanip>
#include "Converter.h"
#include "AutoActionTimer.h"

// ----------------------------------------------------------------------------


#include "NodeUI.h"
#include "WeaponControlSystem.h"
#include "BattleUnit.h"
#include "SteeringBehaviors.h"

// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;



Torpedo::Torpedo(GameLevel* level, int id)
	: GameNode(level, id, "SubWorld\\Assets\\Torpedo\\TorpedoMKI_0.node"),
	_wanderer(nullptr), _wanderer_attached(false), _impact_radius(2), _exploded(false), _impact_power(0.4) 
{



}

// ----------------------------------------------------------------------------

Torpedo::~Torpedo()
{

}



// ----------------------------------------------------------------------------



void Torpedo::createNode()
{
	GameNode::createNode();

	_useSmoothedDirectionAccumulator = false;
	// move forward
	setConstantSteeringForce(OpenSteer::Vec3::zero);// forward());
	// max speed and max steering force (maneuverability) 
	setMaxSpeed(5);
	setMaxForce(3);
	setMass(1);
	// initially stopped
	setSpeed(0);
	// randomize 2D heading
	randomizeHeadingOnXZPlane();

	setTrailParameters(8, 40);

	// add basic steering component
	ComponentSystem::get()->addComponent<SteeringBehaviors>(_node);
	
	SteeringBehaviors* sb = ComponentSystem::get()->getComponent<SteeringBehaviors>(_node);
	sb->changeBehavior(enumSteeringBehaviors::STEERING_MOVE, _wanderer);

}
 


// ----------------------------------------------------------------------------

void Torpedo::update(const float currentTime, const float elapsedTime)
{
	GameNode::update(currentTime, elapsedTime);
	if (_node)
		_node->renderVisualizer();
	annotateNodeUILink();

	// manage the pursuit
	if (_validGameNode(_wanderer))
	{

		SteeringBehaviors* missileSteeringBehaviors = ComponentSystem::get()->getComponent<SteeringBehaviors>(_node);
		// we need to attach the target and change the steering behavior to STEER_FOR_PURSUIT
		if (!_wanderer_attached)
		{
			if (missileSteeringBehaviors->isInitialized())
			{
				missileSteeringBehaviors->changeBehavior(enumSteeringBehaviors::STEER_FOR_PURSUIT, _wanderer);
				_wanderer_attached = true;
			}
		}

		check_target(elapsedTime);
	}
	else
		
	{
		SteeringBehaviors* missileSteeringBehaviors = ComponentSystem::get()->getComponent<SteeringBehaviors>(_node);
		if (missileSteeringBehaviors->getBehaviors() != enumSteeringBehaviors::STEERING_MOVE)
		{
			missileSteeringBehaviors->changeBehavior(enumSteeringBehaviors::STEERING_MOVE, nullptr);
		}

	}

}


// ----------------------------------------------------------------------------


void Torpedo::check_target(const float elapsedTime)
{
	float distance = nodeDistance(_wanderer);
	//printf("\n distance : %f", distance);
	if (!_exploded && distance < _impact_radius)
	{
		_exploded = true;

		SteeringBehaviors* missileSteeringBehaviors = ComponentSystem::get()->getComponent<SteeringBehaviors>(_node);
		missileSteeringBehaviors->changeBehavior(enumSteeringBehaviors::STEERING_MOVE, nullptr);
		setVisibility(false);
		_level->removeGameNode(this);
	
		
		Unigine::Ptr<Unigine::Node> explosion = Unigine::World::loadNode("vfx\\nodes\\vfx_explosions_4.node", 0);
		OpenSteer::Vec3 wanderPos = _wanderer->position();
		OpenSteer::Vec3 pos(wanderPos.x, wanderPos.y , wanderPos.z);//GamePlay::Game->_see_level +1
		 
	
	 
		explosion->setWorldPosition(Converter::toUnigine(pos));
		new AutoActionTimer<Unigine::NodePtr>(explosion, 1000 * 2,
			[](AutoActionTimer<Unigine::NodePtr>& update_delegate, const float currentTime, const float elapsedTime) {},
			[](AutoActionTimer<Unigine::NodePtr>& delete_delegate)
			{
		 	// delete explosion
			delete_delegate._ptr->deleteLater();
			 	
			});

	 
		// compute the status of the wanderer units
		BattleUnit* threat_battle_unit = ComponentSystem::get()->getComponent<BattleUnit>(_wanderer->_node);
		threat_battle_unit->impactDamage(_impact_power);

	}
}


// ----------------------------------------------------------------------------

