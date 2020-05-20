
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

#include "HTNTest.h"
#include "Domain.h"
#include "Planner.h"
#include "HTNPlanner.h"
#include "HTNTrace.h"

// ----------------------------------------------------------------------------

using namespace HTN;

// ----------------------------------------------------------------------------





/*
int main(int argc,char *argv[])
{
	{
		std::vector< HTNDemo*> demos;
		for (int i = 0; i < 60; i++)
		{
			HTNDemo* demo = new HTNDemo("HTN::" + std::to_string(i));
			demos.push_back(demo);
		}

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(5));
			 break;
		}
		for (HTNDemo* d : demos)
		{
			delete d;
		}
	}
	return(-1);
}*/

// ----------------------------------------------------------------------------
// ------ Simple travel Sample
// ----------------------------------------------------------------------------



struct WsMe : public StateGroup
{
	WsMe()
	{
		States["loc"] = new StateStringValue("home");
		States["dest"] = new StateStringValue("park");
		States["cash"] = new StateFloatValue(2000.0f);
		States["owe"] = new StateFloatValue(0);
		States["max-walk-distance"] = new StateLongValue(2);
		States["temp"] = new StateStringValue("temp");
	}
};

struct WsTaxi : public StateGroup
{
	WsTaxi()
	{
		States["loc"] = new StateStringValue("unknown");
		 
	}
};

struct WsRoad : public StateGroup
{
	WsRoad()
	{
		States["home-park"] = new StateLongValue(8);
		States["park-home"] = new StateLongValue(8);
	 }
};
 


void simple_travel::initWorldStatesHandler(WorldStateMap& worldStates)
{
	worldStates["WsRoad"] = WsRoad();
	worldStates["WsMe"] = WsMe();
	worldStates["WsTaxi"] = WsTaxi();
	 
}
 
void simple_travel::initVariables(Domain& domain)
{
	domain.EvalTask.push("start-loc", "home");
	domain.EvalTask.push("arrive-loc", "park");
	domain.EvalTask.push("distance", 8L);
	 
	
}



void simple_travel::initDomainHandler(Domain& domain)
{
	

	domain
		.AddCompoundTask("travel")
			.AddMethod("travel_by_taxi", 0.0f)
				.AddPreCondition(DefState("WsMe", "cash"), FCT::Sup, new FctFloatValue([&domain]() { long d;  domain.EvalTask.varAsLong("distance", d); return 1.5f + 0.5f * (float)d; }))
					.AddPrimitiveTask("call_taxi")
					.AddPrimitiveTask("ride_taxi")
					.AddPrimitiveTask("pay_driver").back()
		 	.AddMethod("travel_by_foot", 0.2f)
				.AddPreCondition(DefState("WsMe", "max-walk-distance"), FCT::Sup, new LongVar("distance"))
					.AddPrimitiveTask("walk").back().back()
	    // Primitives
		.AddPrimitiveTask("walk")
			 .AddOperator(Operator([this](OperationStatus status, WStates& states) 	{ return  WalkToPark(status,states); }))
			.AddEffect(DefState("WsMe", "temp"), FCT::Equal, new StateValue("WsMe", "dest"))
			.AddEffect(DefState("WsMe", "dest"), FCT::Equal, new StateValue("WsMe", "loc"))
			.AddEffect(DefState("WsMe", "loc"), FCT::Equal, new StateValue("WsMe","temp")).back()
		.AddPrimitiveTask("call_taxi")
			.AddOperator(Operator([this](OperationStatus status, WStates& states) { HTNTrace(_domain->Name, "\t  Taxi called"); return OperationStatus::Success; }))
			.AddEffect(DefState("WsTaxi", "loc"), FCT::Equal, new StateValue("WsMe","loc")).back()
		.AddPrimitiveTask("ride_taxi")
			.AddPreCondition(DefState("WsTaxi", "loc"), FCT::IsEqual, new StateValue("WsMe", "loc"))
			.AddOperator(Operator([this](OperationStatus status, WStates& states) {  return  RideToPark(status, states); }))
			.AddEffect(DefState("WsMe", "temp"), FCT::Equal, new StateValue("WsMe", "loc"))
			.AddEffect(DefState("WsTaxi", "loc"), FCT::Equal, new StateValue("WsMe", "dest"))
			.AddEffect(DefState("WsMe", "loc"), FCT::Equal, new StateValue("WsMe", "dest"))
			.AddEffect(DefState("WsMe", "dest"), FCT::Equal, new StateValue("WsMe","temp"))
			.AddEffect(DefState("WsMe", "owe"), FCT::Incr, new FctFloatValue([&domain]() { long d;  domain.EvalTask.varAsLong("distance", d); return 1.5f + 0.5f * (float)d; })).back()
		.AddPrimitiveTask("pay_driver")
			.AddPreCondition(DefState("WsMe", "cash"), FCT::Sup, new StateValue("WsMe", "owe"))
			.AddOperator(Operator([this](OperationStatus status, WStates& states) { HTNTrace(_domain->Name, "\t  pay_driver"); return OperationStatus::Success; }))
			.AddEffect(DefState("WsMe", "cash"), FCT::Decr, new StateValue("WsMe", "owe")).back();

	  	 
	 
}
 

OperationStatus simple_travel::WalkToPark(OperationStatus status, WStates& states)
{
	static bool walking = false;
	// long operation
	if (status == OperationStatus::FirstEvaluation)
	{
		HTNUsrTrace(_domain->Name, "\t  Walk : simulate long operation from %s to %s",states.wStateAsString("WsMe","loc").c_str(), states.wStateAsString("WsMe", "dest").c_str());
		walking = true;
		std::thread t1([&]() {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			walking = false;
			 
		});
		t1.detach();
	}
	//HTNTrace(_domain->Name, "\t  WalkToPark : waiting to finish...");

	if (!walking) // if unit has navigated to the bridge
	{
		return OperationStatus::Success; // task is finished
	}
	return OperationStatus::Continue; // task is not finished

}





OperationStatus simple_travel::RideToPark(OperationStatus status, WStates& states)
{
	static bool riding = false;
	// long operation
	if (status == OperationStatus::FirstEvaluation)
	{
		HTNUsrTrace(_domain->Name, "\t  Ride taxi : simulate long operation from %s to %s", states.wStateAsString("WsMe", "loc").c_str(), states.wStateAsString("WsMe", "dest").c_str());
	 	riding = true;
		std::thread t1([&]() {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			riding = false;

		});
		t1.detach();
	}
	//HTNTrace(_domain->Name, "\t  RideToPark : waiting to finish...");

	if (!riding) // if unit has navigated to the bridge
	{
		return OperationStatus::Success; // task is finished
	}
	return OperationStatus::Continue; // task is not finished

}

// ----------------------------------------------------------------------------
// ------ BeTrunkThumper Sample
// ----------------------------------------------------------------------------




struct WsGlobal : public StateGroup
{
	WsGlobal()
	{
		States["BooleanTrue"] = new StateBoolValue(true);
		States["BooleanFalse"] = new StateBoolValue(false);
		States["NextBridgeLocRef"] = new StatePointValue(54.0f, 0.0f, 22.0f);


	}
};


struct WsHealth : public StateGroup
{
	WsHealth()
	{
		States["WsHasEnemy"] = new StateLongValue(0);
		States["Speed_Fast"] = new StateLongValue(0);
		States["WsIsTired"] = new StateBoolValue(false);
		States["WsLocation"] = new StatePointValue(0.0f, 0.0f,0.0f);
		States["WsHasNavigatedToBridge"] = new StateBoolValue(false);
		

	}

};

struct EEnemyRange : public StateGroup
{
	EEnemyRange()
	{
		States["MeleeRange"] = new StateLongValue(0);
		States["ViewRange"] = new StateFloatValue(0.0f);
		States["OutOfRange"] = new StateLongValue(0);
		States["WsCanSeeEnemy"] = new StateBoolValue(true);
		States["EnemyLoc"] = new StatePointValue(10.0f, 0.0f, 10.0f);

	}

};

 



void HTNTest::initWorldStatesHandler(WorldStateMap& worldStates)
{
	worldStates["WsGlobal"] = WsGlobal();
	worldStates["EEnemyRange"] = EEnemyRange();
	worldStates["WsHealth"] = WsHealth();
}


void HTNTest::initDomainHandler(Domain& domain)
{
	/*
	domain.AddCompoundTask("BeTrunkThumper", {})
		.AddMethod("M1",0)
			.AddPreCondition(DefState("EEnemyRange", "WsCanSeeEnemy"), FCT::IsEqual, new BoolValue(true))
			.AddPrimitiveTask("NavigateToEnemy")
			.AddPrimitiveTask("DoTrunkSlam").back()
		.AddMethod("M2", 0.5)
			.AddPreCondition(DefState("EEnemyRange", "WsCanSeeEnemy"), FCT::IsEqual, new BoolValue(true))
			.AddPrimitiveTask("NavigateToEnemy")
			.AddPrimitiveTask("DoTrunkSlam").back()
		.AddMethod("M3",0)
			.AddPreCondition(DefState("WsGlobal", "BooleanTrue"), FCT::IsEqual, new BoolValue(true))
			.AddPrimitiveTask("ChooseBridgeToCheck")
			.AddPrimitiveTask("NavigateToBridge")
			.AddPrimitiveTask("CheckBridge").back().back()
		.AddPrimitiveTask("DoTrunkSlam", {})
		.AddOperator(Operator([](OperationStatus,WStates& states) {  states.wSetState("EEnemyRange", "WsCanSeeEnemy",false); return  OperationStatus::Success; })).back()
		.AddPrimitiveTask("NavigateToEnemy", {})
			.AddOperator(Operator([](OperationStatus, WStates& ) { return  OperationStatus::Success; }))
			.AddEffect(DefState("WsHealth", "WsLocation"), FCT::Equal, new StateValue("EEnemyRange", "EnemyLoc")).back()
		.AddPrimitiveTask("ChooseBridgeToCheck", {})
			.AddOperator(Operator([](OperationStatus, WStates& ) { return OperationStatus::Success; })).back()
		.AddPrimitiveTask("NavigateToBridge", {})
			.AddOperator(Operator([this](OperationStatus status, WStates& states)
			{ 	return NavigateToBridge(status, states); }))
			.AddEffect(DefState("WsHealth", "WsLocation"), FCT::Equal, new StateValue("WsGlobal", "NextBridgeLocRef")).back()
		.AddPrimitiveTask("CheckBridge", {})
		.AddOperator(Operator([](OperationStatus, WStates& ) { return OperationStatus::Success; }))
		;
		*/
}


 
		 
OperationStatus HTNTest::NavigateToBridge(OperationStatus status, WStates& states)
{
	// long operation
	if (status == OperationStatus::FirstEvaluation)
	{
		HTNTrace(_domain->Name, "\t  NavigateToBridge : create simulate long operation");

		std::thread t1([&]() {
			std::this_thread::sleep_for(std::chrono::seconds(10));
			states.wSetState("WsHealth", "WsHasNavigatedToBridge", true);
		}); 
		t1.detach();
	}
	HTNTrace(_domain->Name, "\t  NavigateToBridge : waiting to finish...");

	if (states.wStateAsBool("WsHealth", "WsHasNavigatedToBridge")) // if unit has navigated to the bridge
	{
		states.wSetState("WsHealth", "WsHasNavigatedToBridge",false);
		return OperationStatus::Success; // task is finished
	}
	return OperationStatus::Continue; // task is not finished

}


/*
Compound Task[BeTrunkThumper]
	Method[WsCanSeeEnemy == true]
		Subtasks[NavigateToEnemy(), DoTrunkSlam()]
	Method[true]
		Subtasks[ChooseBridgeToCheck(), NavigateToBridge(), CheckBridge()]
	Primitive Task[DoTrunkSlam]
		Operator[AnimatedAttackOperator(TrunkSlamAnimName)]
	Primitive Task[NavigateToEnemy]
		Operator[NavigateToOperator(EnemyLocRef)]
		Effects[WsLocation = EnemyLocRef]
	Primitive Task[ChooseBridgeToCheck]
		Operator[ChooseBridgeToCheckOperator]
	Primitive Task[NavigateToBridge]
		Operator[NavigateToOperator(NextBridgeLocRef)]
		Effects[WsLocation = NextBridgeLocRef]
	Primitive Task[CheckBridge]
		Operator[CheckBridgeOperator(SearchAnimName)]
*/
