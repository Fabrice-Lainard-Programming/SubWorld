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

#include "SubClassA.h"
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
#include "AI/HTNPlanner/Domain.h"
#include "AI/HTNPlanner/Planner.h"
#include "AI/HTNPlanner/HTNPlanner.h"
#include "AI/HTNPlanner/HTNTrace.h"

// ----------------------------------------------------------------------------


#include "NodeUI.h"
#include "SteeringBehaviors.h"
#include "BattleUnit.h"
#include "BattleUnitUI.h"
#include "WeaponControlSystem.h"
#include "SonarEffect.h"
#include "AI/UnitGOAPPlannerAI.h"
#include "AI/SensorAI.h"


// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;
using namespace goap;

// ----------------------------------------------------------------------------

SubClassA::SubClassA(GameLevel* level, int id)
	: GameNode(level,id,"SubWorld\\Assets\\Subs\\SubClassA\\subclassA.node")
{
	


}

// ----------------------------------------------------------------------------

SubClassA::~SubClassA()
{
	 
}



// ----------------------------------------------------------------------------



void SubClassA::createNode()
{
	GameNode::createNode();
	// move forward
	setConstantSteeringForce(forward());
	// max speed and max steering force (maneuverability) 
	setMaxSpeed(5.0);
	setMaxForce(30);
	setMass(1.0);
	// initially stopped
	setSpeed(0);
	// randomize 2D heading
	randomizeHeadingOnXZPlane();
	// trail parameters: 3 seconds with 60 points along the trail
	setTrailParameters(4, 40);

	// add basic Ui component
	ComponentSystem::get()->addComponent<NodeUI>(_node);
	// add basic steering component
	ComponentSystem::get()->addComponent<SteeringBehaviors>(_node);
	// add battle unit component
	ComponentSystem::get()->addComponent<BattleUnit>(_node);
	// add basic WCS component
	ComponentSystem::get()->addComponent<WeaponControlSystem>(_node);
	initWeaponControlSystem();
	// add sonar effect component
	ComponentSystem::get()->addComponent<SonarEffect>(_node);
	// add unit AI component
	ComponentSystem::get()->addComponent<UnitGOAPPlannerAI>(_node);
	// add sensor AI component
	ComponentSystem::get()->addComponent<SensorAI>(_node);
	initSensorAI();
	

}

// ----------------------------------------------------------------------------
 

void SubClassA::update(const float currentTime, const float elapsedTime)
{
	GameNode::update(currentTime, elapsedTime);
	annotateNodeUILink();
}

// ----------------------------------------------------------------------------

//  callback fo update called each seconds
void SubClassA::update_on_400ms(const float currentTime, const float elapsedTime)
{
	UnitGOAPPlannerAI* planner = ComponentSystem::get()->getComponent<UnitGOAPPlannerAI>(_node);
	if (!planner) return;
	planner->update_planner(currentTime, elapsedTime);

}

 

// ----------------------------------------------------------------------------

void SubClassA::initWeaponControlSystem()
{
	WeaponControlSystem* wcs = ComponentSystem::get()->getComponent<WeaponControlSystem>(_node);
	std::vector< Weapon> weapons;

	std::vector <weapon_missile_def> missile_definition;
	 
	auto weapon = Weapon(enumWeaponType::ANTI_SURFACE_MISSILE_BASED,
		Unigine::Math::vec3(0, 0, 0), 300, "", "", "",
		Unigine::Math::vec3(0, 0, 0), missile_definition,4000);
	weapons.push_back(weapon);

	Radar radar(300);
	wcs->initWCS(radar, weapons,nullptr);
}

 

// ----------------------------------------------------------------------------


void SubClassA::mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt)
{
	switch (zone)
	{
	case enumGameZone::TERRAIN:
	{
		if (GamePlay::Game->getMode() == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK)
		{
			SteeringBehaviors* steering = ComponentSystem::get()->getComponent<SteeringBehaviors>(_node);
			steering->mouseClick(zone, pt);
		}

	} break;
	}
}
 
// ----------------------------------------------------------------------------
 

void SubClassA::setSelected(bool isSelected)
{
	GameNode::setSelected(isSelected);
	SonarEffect* se = ComponentSystem::get()->getComponent<SonarEffect>(_node);

	if (se)
	{
		se->ping(1000);
	}

}







// ----------------------------------------------------------------------------
// -- AI (HTN/GOALP/CLIPS)
// ----------------------------------------------------------------------------



void SubClassA::initSensorAI()
{
	SensorAI* sensor = ComponentSystem::get()->getComponent<SensorAI>(_node);
	if (!sensor) return;
	 
	sensor->addDetectionSystem(new UnderwaterAcousticDetectionSystem(200, 400));
}
 

const int enemy_sighted = 1;
const int enemy_dead = 2;
const int enemy_in_range = 3;
const int enemy_in_close_range = 4;
const int inventory_knife = 5;
const int inventory_gun = 6;
const int gun_drawn = 7;;
const int gun_loaded = 8;
const int have_ammo = 9;
const int knife_drawn = 10;
const int weapon_in_hand = 11;
const int me_dead = 12;


void SubClassA::initActionsHandler(std::vector<goap::Action>& actions)
{
	Action scout("scoutStealthily", 5);
	scout.setPrecondition(enemy_sighted, false);
	scout.setPrecondition(weapon_in_hand, true);
	scout.setEffect(enemy_sighted, true);
	actions.push_back(scout);

	Action run("scoutRunning", 15);
	run.setPrecondition(enemy_sighted, false);
	scout.setPrecondition(weapon_in_hand, true);
	run.setEffect(enemy_sighted, true);
	actions.push_back(run);

	Action approach("closeToGunRange", 2);
	approach.setPrecondition(enemy_sighted, true);
	approach.setPrecondition(enemy_dead, false);
	approach.setPrecondition(enemy_in_range, false);
	approach.setPrecondition(gun_loaded, true);
	approach.setEffect(enemy_in_range, true);
	actions.push_back(approach);

	Action approachClose("closeToKnifeRange", 4);
	approachClose.setPrecondition(enemy_sighted, true);
	approachClose.setPrecondition(enemy_dead, false);
	approachClose.setPrecondition(enemy_in_close_range, false);
	approachClose.setEffect(enemy_in_close_range, true);
	actions.push_back(approachClose);

	Action load("loadGun", 2);
	load.setPrecondition(have_ammo, true);
	load.setPrecondition(gun_loaded, false);
	load.setPrecondition(gun_drawn, true);
	load.setEffect(gun_loaded, true);
	load.setEffect(have_ammo, false);
	actions.push_back(load);

	Action draw("drawGun", 1);
	draw.setPrecondition(inventory_gun, true);
	draw.setPrecondition(weapon_in_hand, false);
	draw.setPrecondition(gun_drawn, false);
	draw.setEffect(gun_drawn, true);
	draw.setEffect(weapon_in_hand, true);
	actions.push_back(draw);

	Action holster("holsterGun", 1);
	holster.setPrecondition(weapon_in_hand, true);
	holster.setPrecondition(gun_drawn, true);
	holster.setEffect(gun_drawn, false);
	holster.setEffect(weapon_in_hand, false);
	actions.push_back(holster);

	Action drawKnife("drawKnife", 1);
	drawKnife.setPrecondition(inventory_knife, true);
	drawKnife.setPrecondition(weapon_in_hand, false);
	drawKnife.setPrecondition(knife_drawn, false);
	drawKnife.setEffect(knife_drawn, true);
	drawKnife.setEffect(weapon_in_hand, true);
	actions.push_back(drawKnife);

	Action sheath("sheathKnife", 1);
	sheath.setPrecondition(weapon_in_hand, true);
	sheath.setPrecondition(knife_drawn, true);
	sheath.setEffect(knife_drawn, false);
	sheath.setEffect(weapon_in_hand, false);
	actions.push_back(sheath);

	Action shoot("shootEnemy", 3);
	shoot.setPrecondition(enemy_sighted, true);
	shoot.setPrecondition(enemy_dead, false);
	shoot.setPrecondition(gun_drawn, true);
	shoot.setPrecondition(gun_loaded, true);
	shoot.setPrecondition(enemy_in_range, true);
	shoot.setEffect(enemy_dead, true);
	actions.push_back(shoot);

	Action knife("knifeEnemy", 3);
	knife.setPrecondition(enemy_sighted, true);
	knife.setPrecondition(enemy_dead, false);
	knife.setPrecondition(knife_drawn, true);
	knife.setPrecondition(enemy_in_close_range, true);
	knife.setEffect(enemy_dead, true);
	actions.push_back(knife);

	Action destruct("selfDestruct", 30);
	destruct.setPrecondition(enemy_sighted, true);
	destruct.setPrecondition(enemy_dead, false);
	destruct.setPrecondition(enemy_in_range, true);
	destruct.setEffect(enemy_dead, true);
	destruct.setEffect(me_dead, true);
	actions.push_back(destruct);

}



// ----------------------------------------------------------------------------

void SubClassA::initWorldStateHandler(goap::WorldState& wstate)
{
 
	wstate.setVariable(enemy_dead, false);
	wstate.setVariable(enemy_sighted, false);
	wstate.setVariable(enemy_in_range, false);
	wstate.setVariable(enemy_in_close_range, false);
	wstate.setVariable(gun_loaded, false);
	wstate.setVariable(gun_drawn, false);
	wstate.setVariable(knife_drawn, false);
	wstate.setVariable(weapon_in_hand, false);
	wstate.setVariable(me_dead, false);
	wstate.setVariable(have_ammo, true);
	wstate.setVariable(inventory_knife, true);
	wstate.setVariable(inventory_gun, true);
}

// ----------------------------------------------------------------------------

void SubClassA::initGoalsHandler(goap::WorldState& goal_win)
{
	goal_win.setVariable(enemy_dead, true);
	goal_win.setVariable(me_dead, false);
	goal_win.setVariable(weapon_in_hand, true);
	goal_win.priority_ = 100;

}

// ----------------------------------------------------------------------------

void SubClassA::sensorUpdate(goap::WorldState& worldStates, const float elapsedTime)
{
	SensorAI* sensor = ComponentSystem::get()->getComponent<SensorAI>(_node);
	if (!sensor) return;
	sensor->passive_update(elapsedTime);

	worldStates.setVariable(enemy_in_range, true);

	/*if (sensor->getDetectedThreats().empty())
		worldStates.wSetState("WsSonar", "enemy-detected", false);
	else
		worldStates.wSetState("WsSonar", "enemy-detected", true);*/

}

// ----------------------------------------------------------------------------

void SubClassA::onUpdatePlan(std::vector<goap::Action>& the_plan)
{
	
}


/*

struct WsSub : public StateGroup
{
	WsSub()
	{
		States["manoeuvre"] = new StateStringValue("");
	
	}
};
 
 

struct WsSonar : public StateGroup
{
	WsSonar()
	{
		States["enemy-detected"] = new StateBoolValue(false);

	}
};


void SubClassA::initWorldStatesHandler(WorldStateMap& worldStates)
{
	worldStates["WsSonar"] = WsSonar();
	worldStates["WsSub"] = WsSub();
	
	 

}

 
void SubClassA::sensorUpdate(HTN::WorldStateProperties& worldStates, const float elapsedTime)
{
	SensorAI* sensor = ComponentSystem::get()->getComponent<SensorAI>(_node);
	if (!sensor) return;
	sensor->passive_update(elapsedTime);
	
	if (sensor->getDetectedThreats().empty())
		worldStates.wSetState("WsSonar", "enemy-detected", false);
	else
		worldStates.wSetState("WsSonar", "enemy-detected", true);

}

 
Escape Manoeuvre : 

	Si detection range < 50
		methode : speed escape : pathfind in opposite direction of the threats (zigzag) , fullspeed , emit conter-mesure
	Si detection range > 50 
		 

 

void SubClassA::initDomainHandler(Domain& domain)
{
 
	domain
		.AddCompoundTask("escape")
		.AddMethod("escape", 0.0f)
		.AddPreCondition(DefState("WsSonar", "enemy-detected"), FCT::IsEqual, new BoolValue(true))
		.AddPrimitiveTask("escape_manoeuvre").back().back()

		// Primitives
		.AddPrimitiveTask("escape_manoeuvre")
			.AddPreCondition(DefState("WsSonar", "enemy-detected"), FCT::IsEqual, new BoolValue(true))
			.AddPreCondition(DefState("WsSub", "manoeuvre"), FCT::IsNequal, new StringValue("escape"))
			.AddOperator(Operator([this](OperationStatus status, WStates& states) { return  EscapeManoeuvre(status, states); }))
			.AddEffect(DefState("WsSub", "manoeuvre"), FCT::Equal, new StringValue("escape"));
		 

}


OperationStatus SubClassA::EscapeManoeuvre(OperationStatus status, WStates& states)
{
	static bool walking = false;
	// long operation
	if (status == OperationStatus::FirstEvaluation)
	{
		printf("\n  EscapeManoeuvre : simulate long operation");
		walking = true;
		std::thread t1([&]() {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			walking = false;

		});
		t1.detach();
	}
	//HTNTrace(_domain->Name, "\t  WalkToPark : waiting to finish...");

	if (!walking) // if unit has navigated to the bridge
	{
		return OperationStatus::Success; // task is finished
	}
	return OperationStatus::Continue; // task is not finished

}



 */