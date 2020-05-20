
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

 
#include "GameNode.h"
 
#include "AI/GOAP/GoapInterface.h"

namespace SubWorld
{
	class GameLevel;

	// A submarine 
	class SubClassA : public GameNode, public goap::GOAPInterface
	{
	public:
		SubClassA(GameLevel* level, int id);
		virtual ~SubClassA();
		// per frame simulation update (not physic related stuff)
		virtual void update(const float currentTime, const float elapsedTime);
		//  callback fo update called each n ms
		void update_on_400ms(const float currentTime, const float elapsedTime);
	 	// called on mouse click over terrain, node, ...
		virtual void mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt);
		// on selection callback
		virtual void setSelected(bool isSelected);
	protected:
		virtual void createNode();
		void initWeaponControlSystem();
		void initSensorAI();
	protected: //GOAP Planner
		// called when the user needs to initialize the list of action states with default values
		virtual void initActionsHandler(std::vector<goap::Action>& actions);
		// called when the user needs to world state the domain
		virtual void initWorldStateHandler(goap::WorldState& wstate);
		// called when the user needs to initialize goals
		virtual void initGoalsHandler(goap::WorldState& goal_win);
		// called at regular interval to let sensor to modify the states
		virtual void sensorUpdate(goap::WorldState& worldStates, const float elapsedTime);
		// called when the plan is update
		virtual void onUpdatePlan(std::vector<goap::Action>& the_plan);

	private:
		 
	};



}

