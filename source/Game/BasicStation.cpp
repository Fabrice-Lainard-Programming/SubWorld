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

 
#include "BasicStation.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "PathFinder.h"
#include "Opensteer/include/OpenSteer/Draw.h"
#include "Opensteer/include/OpenSteer/Vec3.h"
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UniginePhysics.h>
#include <UnigineVisualizer.h>
#include <UniginePlayers.h>
#include <UnigineObjects.h>
#include <UnigineSounds.h>
#include <sstream>
#include <iomanip>
#include "Converter.h"


// ----------------------------------------------------------------------------

#include "ResourceCapacitySystem.h"
#include "NodeUI.h"
#include "BattleUnit.h"
#include "AI/StrategicAI.h"
 
// ----------------------------------------------------------------------------

#include "Level_Discovery.h"
#include "SubClassA.h"

// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;


// ----------------------------------------------------------------------------

BasicStation::BasicStation(GameLevel* level, int id)
	: GameNode(level, id, "SubWorld\\Assets\\Bases\\factory\\base_factory.node")
{


}

// ----------------------------------------------------------------------------

BasicStation::~BasicStation()
{

}



// ----------------------------------------------------------------------------



void BasicStation::createNode()
{
	GameNode::createNode();
	// add basic Ui component
	ComponentSystem::get()->addComponent<NodeUI>(_node);
	// add game resource component
	ComponentSystem::get()->addComponent<ResourceCapacitySystem>(_node);
	// add battle unit component
	ComponentSystem::get()->addComponent<BattleUnit>(_node);
	// Add Strategic AI for this army
//	ComponentSystem::get()->addComponent<StrategicAI>(_node);
	
	
	// init capacities
	initResourcesCapacities();
}





// ----------------------------------------------------------------------------


void BasicStation::initResourcesCapacities()
{
	ResourceCapacitySystem* rcs = ComponentSystem::get()->getComponent<ResourceCapacitySystem>(_node);
	 
	// ---------------------
	auto resource = Resource_Capacity("SUB::CLASSA", TEMPLATE_SUBMARINE, 20, 2,
		[this](void*asset,void *parameter)
	{
	 	GameNodePtr	v1 = static_cast<GameNode*>(asset);
		v1->init();
		
		v1->setConstantSteeringForce(OpenSteer::Vec3::forward);
		_level->addGameNode(v1);
#ifdef PLAY_SOUNDS
		// play a sound with autodelete
		Unigine::SoundSourcePtr sound = v1->addSound("SubWorld\\Assets\\Sounds\\LaunchingSubFromStation.wav"  , 1000 * 45);
		sound->setGain(0.5f);
		 sound->play();
#endif
		 
			 v1->setWorldPosition(OpenSteer::Vec3(330, v1->radius(), 117));
		 //v1->setWorldPosition(OpenSteer::Vec3(position().x + radius() + 2,4, position().z + radius()+2));
		 v1->setFaction(getFaction());
		 v1 -> setSpeed(0.1);

		

	});

	rcs->addCapacity(resource);
	
	//todelete
	rcs->planify_creation("SUB::CLASSA", 1);
}

 


// ----------------------------------------------------------------------------


void BasicStation::update(const float currentTime, const float elapsedTime)
{
	GameNode::update(currentTime, elapsedTime);
	annotateNodeUILink();
 	

}
 
 
 
 


// ----------------------------------------------------------------------------


void BasicStation::drawAnnotations(const float elapsedTime)
{
	annotateSelected();

}
