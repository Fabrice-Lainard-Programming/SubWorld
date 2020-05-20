
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

#include "Effect.h"
#include "HTNTrace.h"

// ----------------------------------------------------------------------------

using namespace HTN;

// ----------------------------------------------------------------------------
 

Effect::Effect(const DefState& state, FCT fct, BaseValue* value)
	: State(state), Fct(fct), Value(value)
{

}


// ----------------------------------------------------------------------------


Effect::~Effect()
{
	 
}

// ----------------------------------------------------------------------------

void Effect::apply(EvalStack&, WStates& state)
{

	state.wSetState(State.Group, State.State, Fct, *Value);
 
}


// ----------------------------------------------------------------------------


void Effect::dump(int level)
{
	printf("\n");
	for (int c = 0; c < level; ++c) printf("\t");
	printf(" Effect[%s %s %s]", State.toString().c_str(), FctLib::toString(Fct).c_str(), Value->toString().c_str());

}

// ----------------------------------------------------------------------------
