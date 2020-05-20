
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


#include "CompoundTask.h"
#include "Domain.h"
#include<algorithm>
#include "HTNTrace.h"

// ----------------------------------------------------------------------------


using namespace HTN;

 

// ----------------------------------------------------------------------------

void CompoundTask::free()
{
	for (Method& m : Methods)
	{
		m.free();
	}
	 
	Methods.clear();
}

// ----------------------------------------------------------------------------

CompoundTask::CompoundTask(Domain* d,const std::string& name,   float cost ,std::initializer_list<std::string> args)
	: BaseTask(d, name,args,cost)
{

}


// ----------------------------------------------------------------------------


Method& CompoundTask::AddMethod(const std::string& name,float cost)
{
	Methods.push_back(Method(name,domain,this,cost));
	return Methods.back();
}

// ----------------------------------------------------------------------------


Method* CompoundTask::FindSatisfiedMethod(EvalStack& evalTask, WStates& states)
{
	// sort by cost	 
	std::sort(Methods.begin(), Methods.end(),
		[](const Method & a, const Method & b) -> bool
	{
		return a.Cost > b.Cost;
	});
	 

	// test method pre-conditions
	for (Method& method : Methods)
	{
		HTNTrace(domain->Name, "Trying method named : '%s'", method.Name.c_str());
		bool satisfied = method.isSatisfied(evalTask,states);
		if (satisfied)
			return &method;
	}
	return nullptr;
}

// ----------------------------------------------------------------------------


void CompoundTask::dump(int level)
{
	printf("\n");
	for (int c = 0; c < level; ++c) printf("\t");
	printf(" CompoundTask[%s]", Name.c_str());
  	for (Method& method : Methods)
	{
		method.dump(level+1);
	}

}

