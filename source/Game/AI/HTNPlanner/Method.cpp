
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


#include "Method.h"
#include "CompoundTask.h"

// ----------------------------------------------------------------------------


using namespace HTN;


// ----------------------------------------------------------------------------

void Method::free()
{
	for (Precondition& pre : Preconditions)
	{
		pre.free();
	}
	Preconditions.clear();
 
}

// ----------------------------------------------------------------------------



Method& Method::AddPreCondition(const DefState& condition, FCT fct, BaseValue* value)
{
	Precondition  cond(condition, fct, value);
	Preconditions.push_back(cond);
	return *this;
}

// ----------------------------------------------------------------------------


Method& Method::AddCompoundTask(const std::string& name,   std::initializer_list<std::string> args)
{
 	Tasks.push_back(TaskRef(name, args));
	return *this;
}
 

// ----------------------------------------------------------------------------


Method& Method::AddPrimitiveTask(const std::string& name, std::initializer_list<std::string> args)
{
	Tasks.push_back(TaskRef(name,args));
	return *this;
}

// ----------------------------------------------------------------------------

bool Method::isSatisfied(EvalStack& evalTask,WStates& states)
{
	for (Precondition& pre : Preconditions)
	{
		if (!pre.isSatisfied(evalTask,states))
			return false;
	}
	return true;
}


// ----------------------------------------------------------------------------

void Method::dump(int level)
{
	printf("\n");
	for (int c = 0; c < level; ++c) printf("\t");
	printf(" Method %s:", Name.c_str());
	for (Precondition& pre : Preconditions)
	{
		pre.dump(level+1);
	}
	printf("\n");
	
	for (int c = 0; c < level+1; ++c) printf("\t");
	printf(" Subtask:");
	for (const TaskRef& task : Tasks)
	{
		printf(" [%s] ", task.Name.c_str());
	}

}