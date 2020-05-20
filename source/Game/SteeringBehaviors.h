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


#pragma once

#include "../ComponentSystem/ComponentSystem.h"
#include "Opensteer/include/OpenSteer/SimpleVehicle.h"
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include "GameFactory.h"
#include "GameLevel.h"
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <chrono>


namespace SubWorld
{
	class GameNode;
	 
	enum enumSteeringBehaviors
	{
		STEERING_STOP,
		STEERING_MOVE,
		STEERING_TRAVEL_WAIT_FOR_DESTINATION,
		STEERING_TRAVEL_WAIT_FOR_CIRCULAR_DESTINATION,
		STEERING_TRAVEL,
		STEER_FOR_PURSUIT,
		 
	};

	// Manage steering of nodes
	class SteeringBehaviors : public ComponentBase
	{
	public:

		// methods
		COMPONENT(SteeringBehaviors, ComponentBase);
		COMPONENT_UPDATE(update);
		COMPONENT_INIT(init)
		COMPONENT_SHUTDOWN(shutdown);

		PROP_NAME("Steering");
		PROP_PARAM(String, name, "DummyParam");

		// change the current behavior
		void changeBehavior(enumSteeringBehaviors behavior, GameNodePtr _wanderer=nullptr);
		// update steering of node
		OpenSteer::Vec3 determineCombinedSteering(const float elapsedTime);
		// travel towards this direction using pathfinder
		bool travel(const Unigine::Math::vec3& pt);
		// 	revolves around the circle centered in pt
		bool circular_travel(const Unigine::Math::vec3& pt,float radius);
		// called when a click is requested 
		void mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt);
		// change the speed of attached node
		void change_speed(float s);
		// retrieve the amount of noise emitted by the engine (in the range of [0,1])
		float getAmountOfNoisEmitted();
		// gets attached game node
		GameNodePtr getGameNode();
		// returns the current behaviors
		enumSteeringBehaviors getBehaviors() { return _steering_behavior; }
		// build string which is visible in the hud
		std::string toHUDString();
	protected:
		void init();
		void update();
		void shutdown();
		OpenSteer::Vec3 steerToFollowPath(GameNodePtr gamenode, const float elapsedTime);
		OpenSteer::Vec3 steerForPursuit(GameNodePtr gamenode, GameNodePtr wanderer, const float elapsedTime);
		std::string toString(enumSteeringBehaviors behaviors);
		 
	protected:
		enumSteeringBehaviors _steering_behavior;
		GameNodePtr _wanderer;
		 

	};



}

