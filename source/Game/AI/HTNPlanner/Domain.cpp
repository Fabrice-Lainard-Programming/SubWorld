
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


#include "Domain.h"

// ----------------------------------------------------------------------------


using namespace HTN;

 
// ----------------------------------------------------------------------------

void Domain::free()
{
	WorldStates.free();
	for (BaseTask* task : Tasks)
	{
		task->free();
		delete task;
	}
	Tasks.clear();
}

// ----------------------------------------------------------------------------


PrimitiveTask& Domain::AddPrimitiveTask(const std::string& name, std::initializer_list<std::string> args)
{
	PrimitiveTask* task = new PrimitiveTask(this,name,args);
	Tasks.push_back(task);
	return *task;
}


// ----------------------------------------------------------------------------
 

CompoundTask& Domain::AddCompoundTask(const std::string& name, std::initializer_list<std::string> args,float cost)
{
	CompoundTask* task = new CompoundTask(this,name, cost,args);
	Tasks.push_back(task);
	return *task;
}

// ----------------------------------------------------------------------------

BaseState* Domain::WorldState(const std::string& group, const std::string& state)
{
	return WorldStates.wState(group, state);
}

// ----------------------------------------------------------------------------


BaseTask* Domain::findTask(const std::string& name)
{
	for (BaseTask* task : Tasks)
	{
	 	if (task->Name == name) return task;
	}
	return nullptr;
}

// ----------------------------------------------------------------------------

void Domain::dump()
{
	int level = 0;
	for (BaseTask* task : Tasks)
		task->dump(level);
}