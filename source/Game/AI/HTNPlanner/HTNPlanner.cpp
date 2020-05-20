
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

#include "HTNPlanner.h"
#include "Domain.h"
#include "CompoundTask.h"
#include "Planner.h"
#include "Runner.h"
#include "HTNTrace.h"
#include <iostream>


// ----------------------------------------------------------------------------


using namespace HTN;


// ----------------------------------------------------------------------------
 

HTNPlanner::HTNPlanner()
	: _domain(new Domain()), _planner(new Planner(*_domain)), _use_thread(true),
	_runner(new Runner(*_domain)), _stop_thread(false), _wait_cycle_ms(1000)
{
	 
}
 

// ----------------------------------------------------------------------------

HTNPlanner::~HTNPlanner()
{
	_stop_thread = true;
	
	if (_use_thread)
	{
		if (_thread.joinable())
		{
			_thread.join();
		}
	}
	
	_domain->free();
	delete _domain;
	_domain = nullptr;
	delete _planner;
	_planner = nullptr;
	delete _runner;
	_runner = nullptr;
	
}



// ----------------------------------------------------------------------------

void HTNPlanner::initialize(const std::string& name, bool use_thread, int wait_cycle_ms)
{
	_name = name;
	_use_thread = use_thread;
	_wait_cycle_ms = wait_cycle_ms;
	_domain->Name = name;
	initWorldStatesHandler(_domain->WorldStates.CurrentWorldState);
	_domain->WorldStates.setIsModified(false);
	initDomainHandler(*_domain);
	initVariables(*_domain);

	if (!_use_thread)
	{
		HTNPlanner::run();
	}
	else
	{
		_thread = std::thread(&HTNPlanner::run, this);
	}
}
 

// ----------------------------------------------------------------------------

void HTNPlanner::run()
{
	 
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::chrono::system_clock::time_point dt_started;

	RunningStatus state = RunningStatus::RunningWaiting;

	while (!_stop_thread)
	{
			auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - dt_started).count();	
			try
			{
				if (state == RunningStatus::RunningWaiting)
				{
					std::vector<BaseTask*> plan = _planner->findPlan();
					//std::this_thread::sleep_for(std::chrono::seconds(44));

					_runner->loadPlan(plan);
				}
				state = _runner->update((float)elapsed_ms);
				if (state == RunningStatus::RunningFailure || state == RunningStatus::RunningSuccess)
				{
					state = RunningStatus::RunningWaiting;
					//_stop_thread = true;
				}
				sensorUpdate(_domain->WorldStates, (float)elapsed_ms);

				
			}
			catch (const std::domain_error& e)
			{
				HTNTrace(_domain->Name, "Catch domain_error : '%s'", e.what());

			}
			dt_started = std::chrono::system_clock::now();
			std::this_thread::sleep_for(std::chrono::milliseconds(_wait_cycle_ms));
	}
}