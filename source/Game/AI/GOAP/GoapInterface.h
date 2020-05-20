
// ----------------------------------------------------------------------------
//
//
//	Hierarchical Task Networks - HTN Implementation in C++
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


 
#include <string>
#include <vector>
#include <thread>
#include "Action.h"
#include "WorldState.h"

namespace goap
{

	struct Domain;
	 
	// Goal Oriented Planning interface
	struct  GOAPInterface
	{
		virtual ~GOAPInterface() {}
		// called when the user needs to initialize the list of action states with default values
		virtual void initActionsHandler(std::vector<Action>& actions) = 0;
		// called when the user needs to world state the domain
		virtual void initWorldStateHandler(WorldState& wstate) = 0;
		// called when the user needs to initialize goals
		virtual void initGoalsHandler(WorldState& goals) = 0;
		// called at regular interval to let sensor to modify the states
		virtual void sensorUpdate(WorldState& worldStates, const float elapsedTime) = 0;
		// called when the plan is update
		virtual void onUpdatePlan(std::vector<Action>& the_plan) = 0;
		// called when the planer 
		void computePlan();
		// initialize the Goap planner
		void initialize()
		{
			initActionsHandler(all_actions);
			initWorldStateHandler(initial_state);
			initGoalsHandler(goal_win);
		}
		WorldState initial_state;
		WorldState goal_win;
		std::vector<Action> all_actions;
		
	};
	



}