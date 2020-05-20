 
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
#include <queue>
 

namespace HTN
{

	struct Domain;
	struct BaseTask;
	struct Method;
	struct CompoundTask;
	
	// records the planner’s state so the planner can backtrack either
	// when a compound task cannot be decomposed or when a primitive’s conditions aren’t satisfied
	struct BacktrackPoint
	{
		//BacktrackPoint() {}
		BacktrackPoint(Domain& d,std::vector<BaseTask*>& plan, std::deque<BaseTask*>& tasksToProcess, Method* currentMethod,const  WStates& workingWS)
			: Plan(plan), TasksToProcess(tasksToProcess), CurrentMethod(currentMethod), WorkingWS(workingWS), domain(d){}

		std::vector<BaseTask*> Plan;
		std::deque<BaseTask*> TasksToProcess;
		Method* CurrentMethod;
		WStates WorkingWS;
		Domain& domain;
	};

	typedef std::deque<BacktrackPoint> backtrack_stack;

	struct CallTaskRef
	{
		CallTaskRef(BaseTask* task, std::initializer_list<std::string> args)
			: Task(task), Args(args) {}
		BaseTask* Task;
		std::initializer_list<std::string> Args;
	};

	// Finding plans in domain
	 
	class Planner
	{
	public:
		Planner(Domain& d);
		~Planner();
		// search for a plan
		std::vector<BaseTask*> findPlan();
	protected:
		// search a plan for the given compound task
		std::vector<BaseTask*> findPlan(BaseTask* task);
		// decompose CompoundTask by adding  method’s subtasks to the TaskToProcess stack.  	 
		void decompose(Method* currentMethod);
		// register this decomposition point so we can backtrack later 
		void recordDecompositionOfTask( Method* method, std::vector<BaseTask*>& finalPlan,  WStates& WorkingWS, backtrack_stack& decomposition_history);
		// backtrack either when a compound task cannot be decomposed or when a primitive’s conditions aren’t satisfied
		void restoreToLastDecomposedTask(std::vector<BaseTask*>& finalPlan, WStates& WorkingWS, backtrack_stack& decomposition_history);
		// clear stack
		void  clearStack();
		// clear backtracking stack
		void  clearBacktrackStack(backtrack_stack& stack);
	protected:
		// working set of tasks
		std::deque<BaseTask*> TasksToProcess;
	 	// associated domain
		Domain& domain;
	};



}

