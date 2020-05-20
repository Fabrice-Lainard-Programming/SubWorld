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

 
#include "PrimitiveTask.h"
 

namespace HTN
{
	struct Domain;
	struct CompoundTask;
	class EvalStack;


	struct TaskRef
	{
		TaskRef(const std::string& name, std::initializer_list<std::string> args)
			: Name(name), Args(args) {}
		std::string Name;
		std::initializer_list<std::string> Args;
	};


	// Needed to determine which approach to accomplish a compound task.
	// Methods are comprised of a set of conditions and tasks
	struct Method : public BaseTask
	{
		
		Method(const std::string& name, Domain* d, CompoundTask* ts, float cost) : BaseTask(d, name, {},cost),Task(ts) {}
		// free memory
		void free();
		// add a pre-condition used by the planer to test if this task can be done
		Method& AddPreCondition(const DefState& condition, FCT fct, BaseValue* value);
		// add compound task as subclass 
		Method& AddCompoundTask(const std::string& name, std::initializer_list<std::string> args = {});
		// add primitive task as subclass 
		Method& AddPrimitiveTask(const std::string& name, std::initializer_list<std::string> args = {});
		// return true if preconditions are satisfied in world states
		bool isSatisfied(EvalStack& evalTask, WStates& states);
		// gets associated domain
		Domain& getDomain() { return  *domain; }
		// associated CompoundTask
		CompoundTask& back() { return *Task;  }
		// prints out
		void dump(int level);
		 

		// conditions
		std::vector <Precondition> Preconditions;
		// tasks
		std::vector<TaskRef> Tasks;
	 	// associated CompoundTask (do not delete this directly)
		CompoundTask* Task;
	
		 
	 
	};

 

}



