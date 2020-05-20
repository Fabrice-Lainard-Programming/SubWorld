 
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



#include "Operator.h"
#include "PreCondition.h"
#include "Effect.h"
#include <vector>


namespace HTN
{

	struct Domain;
	class EvalStack;
	
	// base class representing either a primitive task or a compound task
	struct BaseTask
	{
		BaseTask(Domain* d, const std::string& name,std::initializer_list<std::string>,float cost=0) : Name(name), domain(d), Cost(cost){}
		virtual ~BaseTask() {}
		virtual void dump(int) {}
		virtual void free() {}

		// name of the task (like 'SprintToEnemy')
		std::string Name;
		// associated domain
		Domain* domain;
		// arguments
		std::initializer_list<std::string> Args;
		// cost of the task [0..1]
		float Cost;
	
	};
 
	// Primitive tasks represent a single step that can be performed by NPC
	// A set of primitive tasks is the plan that we are ultimately getting out of
	// the HTN.
	// Primitive tasks are comprised of an operator and sets of effects and conditions.
	struct PrimitiveTask  : public BaseTask
	{
		
	
		// create a task 
		PrimitiveTask(Domain* d, const std::string& name,   std::initializer_list<std::string> args) : BaseTask(d,name, args){}
		// destructor
		virtual ~PrimitiveTask() {}
		// free memory
		virtual void free();
		// add a pre-condition used by the planer to test if this task can be done
		PrimitiveTask& AddPreCondition(const DefState& condition, FCT fct, BaseValue* value);
		// add a basic operation
		PrimitiveTask& AddOperator(const Operator& op);
		//a primitive task’s effects describe how the success of the task will affect the NPC’s	world state
		PrimitiveTask& AddEffect(const DefState& state, FCT fct, BaseValue* value);
		// test if the preconditions are satisfied by the state of the world
		bool isSatisfied(EvalStack& evalTask, WStates& states);
		// apply effects to the working world state
		void applyEffects(EvalStack& evalTask, WStates& states);
		// gets associated domain (used as fluent back operator)
		Domain& back() { return *domain; }
		// print the task
		virtual void dump(int level);


		// precondition used to check if the planner can use this task
		std::vector<Precondition> Preconditions;
		// describe how the success of the task will affect the NPC’s world state
		std::vector<Effect> Effects;
		// basic action 
		Operator Operation;
		 
	};

	 
}

