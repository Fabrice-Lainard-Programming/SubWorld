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

#include "Level_Discovery.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "GameLevel.h"
#include "PathFinder.h"
#include "GameNode.h"
#include "GameFactory.h"

// ----------------------------------------------------------------------------

#include "BasicStation.h"
#include "SubClassA.h"
#include "DroneClassA.h"

// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;


// ----------------------------------------------------------------------------

LevelDiscovery::LevelDiscovery(GamePlay* rcs)
	: GameLevel(rcs, "E:\\code17\\SubWorld\\Levels\\LevelDiscovery\\height.png")
{

}

// ----------------------------------------------------------------------------

LevelDiscovery::~LevelDiscovery() 
{
	
}



// ----------------------------------------------------------------------------

void LevelDiscovery::loadLevel()
{
	
	_pathFinder->init(_heightMap.c_str(), 1000,false);

	initDemo();
}


// ----------------------------------------------------------------------------

 
void LevelDiscovery::closeLevel()
{

}

// ----------------------------------------------------------------------------

 
void LevelDiscovery::update(const float currentTime, const float elapsedTime)
{
	GameLevel::update(currentTime, elapsedTime);
	updateDemo(currentTime,elapsedTime);
}

// ----------------------------------------------------------------------------

 
void LevelDiscovery::drawAnnotations(const float elapsedTime)
{
	GameLevel::drawAnnotations(elapsedTime);
	 
}



// ************************************************************************
 
 

 
void LevelDiscovery::initDemo()
{
	// ---------- RED FACTION

	// -- Place Stations
	BasicStation*	station = reinterpret_cast<BasicStation*>(GamePlay::Game->factory()->buildAsset("STATION::BASIC"));
	addGameNode(station);
	station->init();
	station->setWorldPosition(OpenSteer::Vec3(90,0,400));
	station->setWorldRotation(OpenSteer::Vec3(0, 180, 0));
	station->setFaction(enumFaction::BLUE_FACTION);
	


	// ---------- RED FACTION

	// -- Place Drones
	DroneClassA*	drone1 = reinterpret_cast<DroneClassA*>(GamePlay::Game->factory()->buildAsset("DRONE::DroneClassA"));
	addGameNode(drone1);
	drone1->init();
	drone1->setWorldPosition(OpenSteer::Vec3(390, 0, 64));
	drone1->setConstantSteeringForce(OpenSteer::Vec3::zero);
	drone1->setFaction(enumFaction::RED_FACTION);
	//station->setWorldRotation(OpenSteer::Vec3(0, 180, 0));
	
	DroneClassA*	drone2 = reinterpret_cast<DroneClassA*>(GamePlay::Game->factory()->buildAsset("DRONE::DroneClassA"));
	addGameNode(drone2);
	drone2->init();
	drone2->setWorldPosition(OpenSteer::Vec3(560, 0, 115));
	drone2->setConstantSteeringForce(OpenSteer::Vec3::zero);
	drone2->setFaction(enumFaction::RED_FACTION);

}


 

 

void LevelDiscovery::updateDemo(const float currentTime, const float elapsedTime)
{
 
	 
	  
	 
	 
}
