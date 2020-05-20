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

#include "GameFactory.h"
#include "GameLevel.h"
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <chrono>

namespace Gdiplus
{
	class Bitmap;
	class Graphics;
}

namespace SubWorld
{
	class GameNode;
	class BattleUnitUI;
	 
	enum enumAlertLevel
	{
		ALERT_LEVEL_LOW,
		ALERT_LEVEL_MEDIUM,
		ALERT_LEVEL_HIGH,
		ALERT_LEVEL_CRITICAL,
	};



	// Manage battlefield unit  
	class BattleUnit : public ComponentBase
	{
	public:

		// methods
		COMPONENT(BattleUnit, ComponentBase);
	 	COMPONENT_INIT(init)
		COMPONENT_SHUTDOWN(shutdown);
 

		PROP_NAME("BattleUnit");
		PROP_PARAM(String, name, "DummyParam");
		// update annotation of this unit
		void drawAnnotations(const float elapsedTime);
		// update battle unit status (damage, energy, etc)
		void updateUnitStatus(const float elapsedTime);
		// impact the managed unit with that power [0,1]
		void impactDamage(float power);
		// gets attached game node
		GameNodePtr getGameNode();
		void shutdown();
	protected:
		void init();
	
		bool paintUI(Gdiplus::Graphics *g, int w, int h);
		void determineCombinedstatus(const float elapsedTime);
		void updateWCSStatus(const float elapsedTime);
		void updateSteeringStatus(const float elapsedTime);
		void buildExplosion();
		
	 		 
	public:
		bool _explosion_on_destroy;
		// emitted noise [O,1]
		float _amount_of_noise_emitted;
		// energy [O,1]
		float _amount_of_energy;
		// quantity of damage [0,1] where 1 = totally destroyed
		float _amount_of_damage;
		// count of enemy in the range of the WCS system
		int _amount_of_enemy;
		// alert level
		enumAlertLevel _emergency_level;
		 // UI
		BattleUnitUI* _ui;
	protected:
	

	};



}

