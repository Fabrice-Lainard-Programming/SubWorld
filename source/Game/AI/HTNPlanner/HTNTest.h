
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

#include "HTNPlanner.h"
#include "Operator.h"
#include "Variant.h"

namespace HTN
{
	 
	// Demonstration & Test of this HTN 
	class HTNTest : public HTNPlanner
	{
	public:
	 

		virtual void initWorldStatesHandler(WorldStateMap& worldStates);
		virtual void initDomainHandler(Domain& domain);
		virtual void initVariables(Domain& domain) {}
		virtual void sensorUpdate(WorldStateProperties& worldStates, const float elapsedTime) {}
		OperationStatus NavigateToBridge(OperationStatus status, WStates& states);
	};




	class simple_travel : public HTNPlanner
	{
	public:


		virtual void initWorldStatesHandler(WorldStateMap& worldStates);
		virtual void initDomainHandler(Domain& domain);
		virtual void initVariables(Domain& domain);
		virtual void sensorUpdate(WorldStateProperties& worldStates,const float elapsedTime) {}
		OperationStatus WalkToPark(OperationStatus status, WStates& states);
		OperationStatus RideToPark(OperationStatus status, WStates& states);
	};
	


	struct HTNDemo
	{
		HTNDemo(const std::string& name)
		{
			test.initialize(name,true,200);
		
		}
		simple_travel test;
		//HTNTest test;
	};
	 
}

