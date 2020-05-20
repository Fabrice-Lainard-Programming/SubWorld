 
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


#include "WorldStateProperties.h"
#include "HTNInterface.h"
#include <thread>
#include <mutex>
#include <string>

namespace HTN
{

	struct Domain;
	class Planner;
	class Runner;

	 

	 
	//  HTN planning engine based on a total-order forward decomposition planner
	class HTNPlanner : public HTNInterface
	{ 

	public:
		HTNPlanner();
		virtual ~HTNPlanner();
		// initialize the HTN and call handlers
		void initialize(const std::string& name,bool use_thread, int wait_cycle_ms = 1000);
		 	

	protected:
		
		// gets the domain
		Domain& getDomain() { return *_domain; }
		// thread method
		void run();

	protected:
		// contains world states and tasks
		Domain* _domain;
		// Actual planner algorithm
		Planner* _planner;
		// run the plan
		Runner* _runner;
		// thread used to compute plan in background
		std::thread _thread;
		// lock the data
		std::mutex _mutex;
		// create a thread yes or no
		bool _use_thread;
		// if true, quit the thread
		bool _stop_thread = false;
		// sleep thread during cycle in ms
		int _wait_cycle_ms;
		// name of this planner
		std::string _name;
 	 };




}