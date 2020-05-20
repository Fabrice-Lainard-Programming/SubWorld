

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

#include "Runner.h"
#include "Domain.h"
#include "HTNTrace.h"

// ----------------------------------------------------------------------------

using namespace HTN;


// ----------------------------------------------------------------------------


Runner::Runner(Domain& d)
	: _domain(d), _current_plan_index(0), _state(RunnerState::EVAL_NEXT_OPERATION)
{

}

// ----------------------------------------------------------------------------

Runner::~Runner()
{

}

// ----------------------------------------------------------------------------




void Runner::loadPlan(const std::vector<BaseTask*>& plan)
{
 
	HTNTrace(_domain.Name,"-------------------------------------------------");
	HTNTrace(_domain.Name, "RUN PLAN ");
	HTNTrace(_domain.Name, "-------------------------------------------------");

	if (plan.empty())
	{
		HTNTrace(_domain.Name, "Plan is empty");
		return;
	}
	 
	_plan = plan;
	_current_plan_index = 0;
	_state = RunnerState::EVAL_NEXT_OPERATION;
}

 
RunningStatus Runner::update(const float elapsedTime)
{ 


	if(_state== RunnerState::EVAL_NEXT_OPERATION) 
		advance(); 
	if (_state == RunnerState::WAIT_OPERATION_TO_FINISH)
		eval(); 


	switch (_state)
	{
	case RunnerState::EVAL_NEXT_OPERATION:  return RunningStatus::RunningContinue;
	case RunnerState::WAIT_OPERATION_TO_FINISH: return RunningStatus::RunningContinue;
	case RunnerState::EVAL_FAILURE: return RunningStatus::RunningFailure;
	case RunnerState::PLAN_FINISHED: return RunningStatus::RunningSuccess;
	}

	return  RunningStatus::RunningContinue;
}


void Runner::advance()
{
	
	if (_current_plan_index >= _plan.size() )
	{
		_plan.clear();
		_current_plan_index = -1;
		_state = RunnerState::PLAN_FINISHED;
		return;
	}

	BaseTask* bt = _plan.at(_current_plan_index);
	PrimitiveTask* pt = static_cast<PrimitiveTask*>(bt);
	if (!pt)
	{
		throw std::domain_error("Runner::advance : task is null");
	}
	if (!pt->Operation.Action)//.isValid())
	{
		HTNTrace(_domain.Name, "Operation has no action %s", pt->Name.c_str());
		_state = RunnerState::EVAL_NEXT_OPERATION;
		return;
	}
	HTNTrace(_domain.Name, "Eval operation for task '%s'", pt->Name.c_str());

	if (!pt->isSatisfied(_domain.EvalTask,_domain.WorldStates))
	{
		HTNTrace(_domain.Name, "\t conditions are not satisfied for task '%s'", pt->Name.c_str());
		_state = RunnerState::EVAL_FAILURE;
		return;
	}
	_state = RunnerState::WAIT_OPERATION_TO_FINISH;
	_curent_status = OperationStatus::FirstEvaluation;
}



void Runner::eval()
{
	BaseTask* bt = _plan.at(_current_plan_index);
	PrimitiveTask* pt = static_cast<PrimitiveTask*>(bt);

	_curent_status = pt->Operation.Action(_curent_status,_domain.WorldStates);
	if (_curent_status == OperationStatus::Failure)
	{ 
		HTNTrace(_domain.Name, "\t  task '%s' action failure", pt->Name.c_str());
		_state = RunnerState::EVAL_FAILURE;
		return;
		
	}
	if (_curent_status == OperationStatus::Success)
	{
		HTNTrace(_domain.Name, "\t  task '%s' action finished", pt->Name.c_str());
		_state = RunnerState::EVAL_NEXT_OPERATION;
		pt->applyEffects(_domain.EvalTask,_domain.WorldStates);
		_current_plan_index++;
		return;
	}
	if (_curent_status == OperationStatus::Continue)
	{
		_state = RunnerState::WAIT_OPERATION_TO_FINISH;
		return;
	}
}