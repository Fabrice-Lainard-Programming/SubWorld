 
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



#include "UnitHTNPlannerAI.h"
#include "../GameLevel.h"
#include "../GameWorld.h"
#include "../GamePlay.h"
#include "../GameNode.h"
#include "../PathFinder.h"
#include "../Converter.h"
#include "HTNPlanner/Domain.h"
#include "HTNPlanner/Planner.h"
#include "HTNPlanner/HTNPlanner.h"
#include "HTNPlanner/HTNTrace.h"
#include <UnigineApp.h>
#include <UnigineWorld.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <list>


// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;
using namespace HTN;

// ----------------------------------------------------------------------------

REGISTER_COMPONENT(UnitHTNPlannerAI);



// ----------------------------------------------------------------------------


void UnitHTNPlannerAI::shutdown()
{
	 

}

// ----------------------------------------------------------------------------


void UnitHTNPlannerAI::update()
{

}


// ----------------------------------------------------------------------------


void UnitHTNPlannerAI::init()
{
	HTNPlanner::initialize("UnitHTNPlannerAI", true, 200);

}

// ----------------------------------------------------------------------------



void UnitHTNPlannerAI::initWorldStatesHandler(WorldStateMap& worldStates)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	HTNInterface* htnInterface = dynamic_cast<HTNInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->initWorldStatesHandler(worldStates);

}


// ----------------------------------------------------------------------------


void UnitHTNPlannerAI::initVariables(Domain& domain)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	HTNInterface* htnInterface = dynamic_cast<HTNInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->initVariables(domain);
	 
}


// ----------------------------------------------------------------------------

void UnitHTNPlannerAI::initDomainHandler(Domain& domain)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	HTNInterface* htnInterface = dynamic_cast<HTNInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->initDomainHandler(domain);

}
 

void UnitHTNPlannerAI::sensorUpdate(HTN::WorldStateProperties& worldStates, const float elapsedTime)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	HTNInterface* htnInterface = dynamic_cast<HTNInterface*>(gamenode.get());
	if (!htnInterface) return;
	// delegate to the node
	htnInterface->sensorUpdate(worldStates, elapsedTime);
}

// ----------------------------------------------------------------------------


GameNodePtr UnitHTNPlannerAI::getGameNode()
{
	return static_cast<GameNode*>(node->getPtr());
}
