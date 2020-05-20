
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

#include "PreCondition.h"
#include "Domain.h"
#include "HTNTrace.h"

// ----------------------------------------------------------------------------

using namespace HTN;

// ----------------------------------------------------------------------------



Precondition::Precondition(const DefState& condition, FCT fct, BaseValue* value)
	: Condition(condition), Fct(fct), Value(value)
{

}

Precondition::~Precondition()
{
	 
}

// ----------------------------------------------------------------------------

bool Precondition::isSatisfied(EvalStack& evalTask,WStates& states)
{
	// gets a state of a given group
	BaseState*state = states.wState(Condition.Group, Condition.State);
	if (!state) {
		throw std::domain_error("Precondition::isSatisfied : state does not exists : %s" + Condition.toString());
	}

	 BaseValue* valueOperand = Value;
	const StateValue* sv = dynamic_cast<const StateValue*>(Value);
	if (sv)
	{
		BaseState* valueState = states.wState(sv->state.Group, sv->state.State);
		valueOperand = dynamic_cast<BaseValue*>(valueState);
		if (!valueOperand)
		{
			throw std::domain_error("Precondition::isSatisfied :  " + sv->state.Group + "." + sv->state.State + FctLib::toString(Fct) + " ? Value is null");
		}
	}


	bool sb = state->compare(evalTask,Fct, valueOperand);

	HTNTrace(states._domain.Name," isSatisfied %s = %s ",  Condition.toString().c_str(), sb ? "true" : "false");
	return sb;
}



// ----------------------------------------------------------------------------


void Precondition::dump(int level)
{
	printf("\n");
	for (int c = 0; c < level; ++c) printf("\t");
	printf(" Precondition[%s %s %s]", Condition.toString().c_str(), FctLib::toString(Fct).c_str(), Value->toString().c_str());

}
