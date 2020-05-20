 
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



#include "UnitGOAPPlannerAI.h"
#include "../GameLevel.h"
#include "../GameWorld.h"
#include "../GamePlay.h"
#include "../GameNode.h"
#include "../PathFinder.h"
#include "../Converter.h"
#include <UnigineApp.h>
#include <UnigineWorld.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <list>


// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;
using namespace goap;

// ----------------------------------------------------------------------------

REGISTER_COMPONENT(UnitGOAPPlannerAI);



// ----------------------------------------------------------------------------


void UnitGOAPPlannerAI::shutdown()
{
	 

}

// ----------------------------------------------------------------------------


void UnitGOAPPlannerAI::update()
{

}

// ----------------------------------------------------------------------------

void UnitGOAPPlannerAI::update_planner(const float currentTime, const float elapsedTime)
{
	sensorUpdate(initial_state, elapsedTime);
	computePlan();
}


// ----------------------------------------------------------------------------


void UnitGOAPPlannerAI::init()
{
	GOAPInterface::initialize();

}

// ----------------------------------------------------------------------------



void UnitGOAPPlannerAI::initActionsHandler(std::vector<goap::Action>& actions)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	GOAPInterface* htnInterface = dynamic_cast<GOAPInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->initActionsHandler(actions);

}

 

// ----------------------------------------------------------------------------

void UnitGOAPPlannerAI::initWorldStateHandler(goap::WorldState& wstate)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	GOAPInterface* htnInterface = dynamic_cast<GOAPInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->initWorldStateHandler(wstate);

}
 
// ----------------------------------------------------------------------------

void UnitGOAPPlannerAI::initGoalsHandler(goap::WorldState& goals)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	GOAPInterface* htnInterface = dynamic_cast<GOAPInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->initGoalsHandler(goals);

}

// ----------------------------------------------------------------------------

void UnitGOAPPlannerAI::sensorUpdate(goap::WorldState& worldStates, const float elapsedTime)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	GOAPInterface* htnInterface = dynamic_cast<GOAPInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->sensorUpdate(worldStates, elapsedTime);
}

// ----------------------------------------------------------------------------

 
void UnitGOAPPlannerAI::onUpdatePlan(std::vector<Action>& the_plan)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	GOAPInterface* htnInterface = dynamic_cast<GOAPInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->onUpdatePlan(the_plan);
}

// ----------------------------------------------------------------------------


GameNodePtr UnitGOAPPlannerAI::getGameNode()
{
	return static_cast<GameNode*>(node->getPtr());
}
