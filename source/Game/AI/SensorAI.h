 
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

#include "../../ComponentSystem/ComponentSystem.h"
#include "../Opensteer/include/OpenSteer/SimpleVehicle.h"
#include "DetectionSystem.h"
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include "../GameFactory.h"
#include "../GameLevel.h"
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <chrono>


namespace SubWorld
{
	class GameNode;



	// capacities of the sensor
	enum enumSensorAICapacities
	{
		DETECT_ENEMIES = 1,
		DETECT_FRIENDS = 2,

	};

 
	// Environement Assessement
	class SensorAI : public ComponentBase
	{
	public:

		// methods
		COMPONENT(SensorAI, ComponentBase);
		COMPONENT_UPDATE(update);
		COMPONENT_INIT(init)
		COMPONENT_SHUTDOWN(shutdown);

		PROP_NAME("SensorAI");
		PROP_PARAM(String, name, "DummyParam");

		// add a detection system
		void addDetectionSystem(DetectionSystem* ds);
		// refresh detected data with passive acquisition
		void passive_update(const float elapsedTime);
		// returns friends informations
		DetectionList& getDetectedFriends() { return _friends; }
		// returns threats informations
		DetectionList& getDetectedThreats() { return _enemies; }

	protected:
		void init();
		void update();
		void shutdown();
		GameNodePtr getGameNode();
	protected:
	 	GameNodePtr _wanderer;
		enumSensorAICapacities _capacities;
		std::vector<DetectionSystem*> _detection_systems;
		DetectionList _friends;
		DetectionList _enemies;

	};



}



