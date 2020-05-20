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



#include "SteeringBehaviors.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "GameNode.h"
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

REGISTER_COMPONENT(SteeringBehaviors);



// ----------------------------------------------------------------------------


void SteeringBehaviors::shutdown()
{
	_wanderer = nullptr;

}

// ----------------------------------------------------------------------------


void SteeringBehaviors::update()
{
	 
}


// ----------------------------------------------------------------------------


void SteeringBehaviors::init()
{
	_wanderer = nullptr;
	changeBehavior(STEERING_STOP);

}


// ----------------------------------------------------------------------------

void SteeringBehaviors::changeBehavior(enumSteeringBehaviors behavior, GameNodePtr wanderer)
{
	_wanderer = wanderer;
	_steering_behavior = behavior;
}
 


// ----------------------------------------------------------------------------

OpenSteer::Vec3 SteeringBehaviors::determineCombinedSteering(const float elapsedTime)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return OpenSteer::Vec3(0, 0, 0);
	
	// move forward
	OpenSteer::Vec3 steeringForce = OpenSteer::Vec3(0, 0, 0);// gamenode->getConstantSteeringForce();

	switch (_steering_behavior)
	{
	case STEERING_STOP: break;
	case STEERING_MOVE: steeringForce+=gamenode->getConstantSteeringForce(); break;
	case STEERING_TRAVEL_WAIT_FOR_DESTINATION: 
	case STEERING_TRAVEL_WAIT_FOR_CIRCULAR_DESTINATION:
	{
		// we need to get a mouse click for the destination to follow
		if (GamePlay::Game->getMode() != enumCurrentGameMode::ACQUIRE_MOUSE_CLICK)
		{
			// change game mode to grab this click
			if (!GamePlay::Game->pushGameMode(enumCurrentGameMode::ACQUIRE_MOUSE_CLICK))
			{
				// cannot change the game mode now... maybe later...
				changeBehavior(STEERING_MOVE);
				return OpenSteer::Vec3(0, 0, 0);
			}
		}
		else
		{
			GamePlay::Game->showMessage("Select travel location...", GamePlay::Game->_selectionColor);
	 	}
	} break;
	case STEERING_TRAVEL: steeringForce +=  steerToFollowPath(gamenode,elapsedTime); break;
	case STEER_FOR_PURSUIT: steeringForce += steerForPursuit(gamenode, _wanderer, elapsedTime); break;
 
	}

	return steeringForce;
}



 
// ----------------------------------------------------------------------------

OpenSteer::Vec3 SteeringBehaviors::steerForPursuit(GameNodePtr gamenode, GameNodePtr wanderer,const float elapsedTime)
{
	const float maxTime = 20; // xxx hard-to-justify value
	if (!_validGameNode(wanderer))
	{
		_wanderer = nullptr;
		// wender is about to be destroyed so stop the pursuit
		changeBehavior(enumSteeringBehaviors::STEERING_MOVE, nullptr);
		return OpenSteer::Vec3(0, 0, 0);
	}
	return gamenode->steerForPursuit(*wanderer, maxTime);
}


// ----------------------------------------------------------------------------

OpenSteer::Vec3 SteeringBehaviors::steerToFollowPath(GameNodePtr gamenode,const float elapsedTime)
{
	if (!gamenode->getPath()) return OpenSteer::Vec3(0, 0, 0);
	return gamenode->steerToFollowPath(1, 2, *gamenode->getPath());
}

// ----------------------------------------------------------------------------

void SteeringBehaviors::change_speed(float s)
{
	GameNodePtr gamenode = getGameNode();
//	gamenode->setSpeed(s);
	gamenode->setMaxSpeed(s);
}

// ----------------------------------------------------------------------------

// TODO: affine that!
float SteeringBehaviors::getAmountOfNoisEmitted()
{
	GameNodePtr gamenode = getGameNode();
	if (gamenode->speed() > 0)
	{
		return 0.5;
	}
	return 0;
}

// ----------------------------------------------------------------------------

GameNodePtr SteeringBehaviors::getGameNode()
{
	return static_cast<GameNode*>(node->getPtr());
}


// ----------------------------------------------------------------------------

bool SteeringBehaviors::travel(const Unigine::Math::vec3& pt)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return false;
	float height = gamenode->position().y;
	OpenSteer::PolylinePathway* newpath = GamePlay::Game->_current_level->_pathFinder->plan(gamenode->radius(),
		gamenode->position().x, gamenode->position().z, height, pt.x, pt.y, height);
	 
	if (newpath)
	{
		gamenode->setPath(newpath);
		changeBehavior(STEERING_TRAVEL);
		return true;
	}
	return false;
}

// ----------------------------------------------------------------------------


bool SteeringBehaviors::circular_travel(const Unigine::Math::vec3& pt, float radius)
 
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return false;
	float height = gamenode->position().y;
	OpenSteer::PolylinePathway* newpath = GamePlay::Game->_current_level->_pathFinder->circular_plan(gamenode->radius(),
		gamenode->position().x, gamenode->position().z, gamenode->position().y,pt.x, pt.y, height, radius, height);

	if (newpath)
	{
		gamenode->setPath(newpath);
		changeBehavior(STEERING_TRAVEL);
		return true;
	}
	return false;
}



// ----------------------------------------------------------------------------

void SteeringBehaviors::mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt)
{
	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;


	if (GamePlay::Game->getMode() == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK &&
		_steering_behavior == STEERING_MOVE)
	{
		GamePlay::Game->popGameMode(); 
	}
	else
	if (GamePlay::Game->getMode() == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK &&
		_steering_behavior == STEERING_TRAVEL_WAIT_FOR_DESTINATION)
	{
		GamePlay::Game->popGameMode();
		travel(pt);
	  
	}
	else
	if (GamePlay::Game->getMode() == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK &&
		_steering_behavior == STEERING_TRAVEL_WAIT_FOR_CIRCULAR_DESTINATION)
	{
		GamePlay::Game->popGameMode();
		circular_travel(pt,60);

	}
}




// ----------------------------------------------------------------------------
 
std::string SteeringBehaviors::toString(enumSteeringBehaviors behaviors)
{
	 
	switch (behaviors)
	{	
	case	STEERING_STOP: return("STEERING_STOP");
	case	STEERING_MOVE: return("STEERING_MOVE");
	case	STEERING_TRAVEL_WAIT_FOR_DESTINATION: return("STEERING_TRAVEL_WAIT_FOR_DESTINATION");
	case	STEERING_TRAVEL: return("STEERING_TRAVEL");
	case	STEER_FOR_PURSUIT: return("STEER_FOR_PURSUIT");
	default: return "unknown";
	}
}


// ----------------------------------------------------------------------------


std::string SteeringBehaviors::toHUDString()
{
	 GameNodePtr gamenode = getGameNode();
	if (!gamenode) return "";
	char buff[500];
	snprintf(buff, sizeof(buff), " Steering: %s", toString(_steering_behavior).c_str());
	return buff;
}
