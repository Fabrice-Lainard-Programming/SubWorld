

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

#include "Planner.h"
#include "Domain.h"
#include "HTNTrace.h"

// ----------------------------------------------------------------------------

using namespace HTN;
 

// ----------------------------------------------------------------------------


Planner::Planner(Domain& d)
	: domain(d)
{

}

// ----------------------------------------------------------------------------

Planner::~Planner()
{

}

// ----------------------------------------------------------------------------
 

std::vector<BaseTask*> Planner::findPlan()
{
	std::vector<BaseTask*> finalPlan;
	std::queue<BacktrackPoint> decomposition_history;


	// return empty plan if there is no tasks
	if (domain.Tasks.empty())
		return finalPlan;

	// search for the best task

	// sort tasks by cost	 
	std::sort(domain.Tasks.begin(), domain.Tasks.end(),
		[](const BaseTask* a, const BaseTask*  b) -> bool
	{
		return a->Cost < b->Cost;
	});

	for (BaseTask* task : domain.Tasks)
	{
		CompoundTask* compoundTask = dynamic_cast<CompoundTask*>(task);
		if (compoundTask)
		{
			
			finalPlan = findPlan(task);
			if (!finalPlan.empty())
				return finalPlan;
		}
	}
	return finalPlan;
}



std::vector<BaseTask*> Planner::findPlan(BaseTask* task)
{
	std::vector<BaseTask*> finalPlan;
	backtrack_stack decomposition_history;
	// copy world state
	WStates WorkingWS(domain);
	WorkingWS.clone(domain.WorldStates);
	EvalStack WorkingEvalTask = domain.EvalTask;
	try
	{
		// clear stack
		clearStack();
		TasksToProcess.push_front(task);
		 
		HTNTrace(domain.Name,"-------------------------------------------------");
		HTNTrace(domain.Name, "FIND PLAN for Compound : %s ",task->Name.c_str());
		HTNTrace(domain.Name, "-------------------------------------------------");

		// process
		while (!TasksToProcess.empty())
		{
			BaseTask* currentTask = TasksToProcess.front();
			TasksToProcess.pop_front();
			CompoundTask* compoundTask = dynamic_cast<CompoundTask*>(currentTask);
			if (compoundTask)
			{
					// searching through its methods looking for the first set of conditions that are valid.

					// sort Methods by cost	 
				std::sort(compoundTask->Methods.begin(), compoundTask->Methods.end(),
					[](const Method & a, const Method & b) -> bool
				{
					return a.Cost < b.Cost;
				});

			 
				for (Method& m : compoundTask->Methods)
				{
					// add to task stack
					TasksToProcess.push_front(&m);
				}
			}
			else
			{
				Method* methodTask = dynamic_cast<Method*>(currentTask);
				
				

				if (methodTask)
				{
					if (!finalPlan.empty())
					{
						// we have already a plan computed for the next méthod
						// return it :
						goto tagOut; // back to 1980! lol
					}

					// register this decomposition point so we can backtrack later 
					// The planner can backtrack either when a compound task cannot be decomposed or when a primitive’s conditions aren’t satisfied
					recordDecompositionOfTask(methodTask, finalPlan, WorkingWS, decomposition_history);
					HTNTrace(domain.Name, "-------------------------------------------------");
					HTNTrace(domain.Name, "Trying method named : '%s'", methodTask->Name.c_str());
					bool satisfied = methodTask->isSatisfied(WorkingEvalTask, WorkingWS);
					if (satisfied)
					{
						HTNTrace(domain.Name, "\t'%s' is satisfied ", methodTask->Name.c_str());

							// If a method is found, that method’s subtasks are added on to the TaskToProcess stack.
						decompose(methodTask);

					}
					else
					{
						HTNTrace(domain.Name, "\t'%s' is not satisfied ", currentTask->Name.c_str());

						// If a valid method is not found, the planner’s state is rolled back to the last compound 
						// task that was decomposed.
						restoreToLastDecomposedTask(finalPlan, WorkingWS, decomposition_history);
					}
				}

				else
				{
					PrimitiveTask* pt = static_cast<PrimitiveTask*>(currentTask);
					HTNTrace(domain.Name, "Processing of PrimitiveTask '%s'", pt->Name.c_str());
					//pt->Arg
					if (pt->isSatisfied(WorkingEvalTask, WorkingWS))
					{
						HTNTrace(domain.Name, "'%s' is satisfied ", pt->Name.c_str());

						// the task is added to the final plan and its effects are
						//	applied to the working world state
						finalPlan.push_back(pt);
						HTNTrace(domain.Name, "'%s' apply effects ", pt->Name.c_str());

						pt->applyEffects(WorkingEvalTask, WorkingWS);
					}
					else
					{
						HTNTrace(domain.Name, "'%s' is not satisfied ", pt->Name.c_str());

						
						// If a valid method is not found, the planner’s state is rolled back to the last compound 
						// task that was decomposed.
						restoreToLastDecomposedTask(finalPlan, WorkingWS, decomposition_history);

					}
				}
			}
		}
	}
	catch (const std::domain_error& e)
	{
		WorkingWS.free();
		clearBacktrackStack(decomposition_history);
		HTNTrace(domain.Name, "Catch domain_error : '%s'", e.what());
		throw e;
	}
	tagOut:
	WorkingWS.free();
	clearBacktrackStack(decomposition_history);
	return finalPlan;
}

// ----------------------------------------------------------------------------
 


void Planner::decompose(Method* currentMethod)
{
	// adding  method’s subtasks to the TaskToProcess stack
	std::vector<TaskRef> cptasks = currentMethod->Tasks;
	std::reverse(cptasks.begin(), cptasks.end());
	for (const TaskRef& task : cptasks)
	{
		BaseTask* basetask = domain.findTask(task.Name);
		if (basetask)
		{
			// add to task stack
			TasksToProcess.push_front(basetask);
		}
		else
		{
			throw std::domain_error("Planner::decompose : subtask does not exist in the domain : " + task.Name);
		}
	}
}

// ----------------------------------------------------------------------------


void Planner::recordDecompositionOfTask(Method* method, std::vector<BaseTask*>& finalPlan,  WStates& WorkingWS, backtrack_stack& decomposition_history)
{
		WStates CopyWorkingWS(*method->domain);
		CopyWorkingWS.clone(WorkingWS);
		decomposition_history.push_front(BacktrackPoint(domain,finalPlan, TasksToProcess, method, CopyWorkingWS));
}

// ----------------------------------------------------------------------------

void Planner::restoreToLastDecomposedTask(std::vector<BaseTask*>& finalPlan, WStates& WorkingWS, backtrack_stack& decomposition_history)
{
	if (decomposition_history.empty()) return;
	BacktrackPoint& bp = decomposition_history.front();
	finalPlan = bp.Plan;
	WorkingWS.free();
	WorkingWS.CurrentWorldState = bp.WorkingWS.CurrentWorldState;
	TasksToProcess = bp.TasksToProcess;
	decomposition_history.pop_front();



}
	
 

// ----------------------------------------------------------------------------


void  Planner::clearStack()
{
	// clear stack
	while (!TasksToProcess.empty())
		TasksToProcess.pop_front();

}

// ----------------------------------------------------------------------------


void  Planner::clearBacktrackStack(backtrack_stack& stack)
{
	while (!stack.empty())
	{
		BacktrackPoint& bp = stack.front();
		
		bp.WorkingWS.free();
		stack.pop_front();
	}
}