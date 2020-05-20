
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

#include <vector>
#include "Operator.h"
 

namespace HTN
{
	struct Domain;
	struct BaseTask;


	enum RunnerState
	{
		
		// evaluation of the next operation
		EVAL_NEXT_OPERATION,
		// wait this operation finish
		WAIT_OPERATION_TO_FINISH,
		// failure on operation
		EVAL_FAILURE,
		// plan is finished
		PLAN_FINISHED,

	};


	// return value of the runPlan function
	enum RunningStatus
	{
		// waiting plan
		RunningWaiting,
		// plan requires more time to execute
		RunningContinue,
		// the plan is successfully completed
		RunningSuccess,
		// the plan ended with an error
		RunningFailure
	};

	class Runner
	{
	public:
		Runner(Domain& d);
		~Runner();
		// load plan
		void loadPlan(const std::vector<BaseTask*>& plan);
		// execute plan 
		RunningStatus update(const float elapsedTime);
	protected:
		void advance();
		void eval();
	protected:
		// associated domain
		Domain& _domain;
		// actual plan to execute
		std::vector<BaseTask*> _plan;
		// current task index of a task in plan 
		int _current_plan_index;
		// state of the runner
		RunnerState _state;
		// current status of the current operation
		OperationStatus _curent_status;
		
	};



}


