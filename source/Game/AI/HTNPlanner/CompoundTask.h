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
#include "Method.h"

namespace HTN
{
	struct Domain;
	class EvalStack;

	// high level task that has multiple ways of being accomplished
	struct CompoundTask : public BaseTask
	{ 
		// build new compound task
		CompoundTask(Domain* d,const std::string& name,float cost, std::initializer_list<std::string> args);
		// destructor
		virtual ~CompoundTask() {}
		// free memory
		void free();
		// add a method
		Method& AddMethod(const std::string& name, float cost);
		// search valid method 
		Method* FindSatisfiedMethod(EvalStack& evalTask, WStates& states);


		// gets associated domain (used as fluent back operator)
		Domain& back() { return *domain; }
		// print the task
		virtual void dump(int level);



		// Needed to determine which approach to accomplish a compound task.
		// Methods are comprised of a set of conditions and tasks
		std::vector<Method> Methods;
		
		 

		
	};


}

