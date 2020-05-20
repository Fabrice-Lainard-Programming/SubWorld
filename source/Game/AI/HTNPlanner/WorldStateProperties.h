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

#include "Value.h"
#include "Fct.h"
#include <vector>
#include <map>
#include <algorithm>

namespace HTN
{
	class EvalStack;
	struct Domain;

	// basic state
	struct BaseState
	{
		virtual ~BaseState() {}
		virtual bool compare(EvalStack&, FCT, BaseValue*) { return false; }
		virtual void affect(EvalStack&, FCT,const  BaseValue*) {}
		virtual BaseState* clone() const { return new BaseState(); }
	};


	// State which can hold a boolean value
	struct StateBoolValue  : BaseState,BoolValue
	{
		StateBoolValue(bool v) : BoolValue(v) {}
		virtual ~StateBoolValue() {}
		virtual bool compare(EvalStack& evalTask, FCT fct, BaseValue* value);
		virtual void affect(EvalStack& evalTask, FCT fct, const BaseValue* value);
		virtual StateBoolValue* clone() const { return new StateBoolValue(Value); }
	};

	// State which can hold a long value
	struct StateLongValue : BaseState, LongValue
	{
		StateLongValue(long v) : LongValue(v) {}
		virtual ~StateLongValue() {}
		virtual bool compare(EvalStack& evalTask, FCT fct, BaseValue* value);
		virtual void affect(EvalStack& evalTask, FCT fct, const BaseValue* value);
		virtual StateLongValue* clone() const { return new StateLongValue(Value); }
	};

	// State which can hold a float value
	struct StateFloatValue : BaseState, FloatValue
	{
		StateFloatValue(float v) : FloatValue(v) {}
		virtual ~StateFloatValue() {}
		virtual bool compare(EvalStack& evalTask, FCT fct, BaseValue* value);
		virtual void affect(EvalStack& evalTask, FCT fct, const  BaseValue* value);
		virtual StateFloatValue* clone() const { return new StateFloatValue(Value); }
	};
	 
	// State which can hold a string value
	struct StateStringValue : BaseState, StringValue
	{
		explicit StateStringValue(const std::string& v) : StringValue(v) {}
		virtual ~StateStringValue() {}
		virtual bool compare(EvalStack& evalTask, FCT fct, BaseValue* value);
		virtual void affect(EvalStack& evalTask, FCT fct, const BaseValue* value);
		virtual StateStringValue* clone() const { return new StateStringValue(Value); }
	};


	// State which can hold a location value
	struct StatePointValue :  PointValue, BaseState
	{
		StatePointValue(float x,float y,float z) : PointValue(x,y,z) {}
		virtual ~StatePointValue() {}
		virtual bool compare(EvalStack& evalTask, FCT fct, BaseValue* value);
		virtual void affect(EvalStack& evalTask, FCT fct, const BaseValue* value);
		virtual StatePointValue* clone() const { return new StatePointValue(Value.X,Value.Y,Value.Z); }
	};

	// State which can hold a Variant value
	struct StateVariantValue : VariantValue, BaseState
	{
		StateVariantValue(const Variant&v ) : VariantValue(v) {}
		virtual ~StateVariantValue() {}
		virtual bool compare(EvalStack& evalTask, FCT fct, BaseValue* value);
		virtual void affect(EvalStack& evalTask, FCT fct, const BaseValue* value);
		virtual StateVariantValue* clone() const { return new StateVariantValue(Value); }
	};


	// a set of states
	struct StateGroup
	{
		StateGroup() {}
		~StateGroup();
		// gets a state from its name
		BaseState* getState(const std::string& state);
		// clone
		void clone(const StateGroup& sg);
		//  free all memory allocated in hashmap by a call to 'new'
		void free();
		std::map<std::string, BaseState*> States;
 
	 
	};

	 
	// state definition
	struct DefState
	{
		DefState(const std::string& group, const std::string& state)
			: Group(group),State(state)
		{}
		std::string toString() { return "ds[" + Group + "," + State + "]"; }

		std::string Group;
		std::string State;
	};

	// Hold a state value
	struct StateValue : BaseValue
	{
		StateValue(const std::string& group, const std::string& state) : state(group, state) {}
		virtual ~StateValue() {}
		std::string toString() { return state.toString(); }

		DefState state;
	};


	typedef std::map<std::string, StateGroup> WorldStateMap;

	// The world state is essentially a vector of properties that describe what our
	// HTN is going to reason about
	struct WorldStateProperties
	{
		WorldStateProperties(Domain& domain ) : _domain(domain),_isModified(false) {}
	 	~WorldStateProperties();
		// free memory
		void free();
		// free memory
		void clone(WorldStateProperties& wsp);

		// sets an existing state with a given value
		bool wSetState(const std::string& group, const std::string& state, FCT Fct, const BaseValue& Value);
		// helper mehtod which set an existing state with a given boolean value
		bool wSetState(const std::string& group, const std::string& state, bool value);
		// helper mehtod which set an existing state with a given long value
		bool wSetState(const std::string& group, const std::string& state, long value);
		// helper mehtod which set an existing state with a given float value
		bool wSetState(const std::string& group, const std::string& state, float value);
		// helper mehtod which set an existing state with a given string value
		bool wSetState(const std::string& group, const std::string& state, const std::string& value);
		// helper mehtod which set an existing state with a given point value
		bool wSetState(const std::string& group, const std::string& state, const Point& value);

		// gets a state of a given group
		BaseState* wState(const std::string& group, const std::string& state);
	
			 
		
		// template version of wstate which returns the value of state directly
		template <typename T>
		T wStateAs(const std::string& group, const std::string& state)
		{
			BaseState* bs = wState(group, state);
			if (!bs)
			{
				throw std::domain_error("wStateAs : state does not exist : " + group + "." + state);
			}
			const T* sv = dynamic_cast<const T*>(bs);
			if (!sv)
			{
				throw std::domain_error("wStateAs :  " + group + "." + state + " bad type");
			}
			return sv->Value;
		}

		// returns the value of boolean state 
		bool wStateAsBool(const std::string& group, const std::string& state)
		{
			return wStateAs<BoolValue>(group, state).Value;
		}
		// returns the float value of state 
		float wStateAsFloat(const std::string& group, const std::string& state)
		{
			return wStateAs<FloatValue>(group, state).Value;
		}
		// returns the Long value of state 
		long wStateAsLong(const std::string& group, const std::string& state)
		{
			return wStateAs<LongValue>(group, state).Value;
		}
		// returns the Long value of state 
		std::string wStateAsString(const std::string& group, const std::string& state)
		{
			return wStateAs<StringValue>(group, state).Value;
		}
		// returns the Point value of state 
		Point wStateAsPoint(const std::string& group, const std::string& state)
		{
			return wStateAs<PointValue>(group, state).Value;
		}
		// modification of the flag which indicates if the state is changed
		void setIsModified(bool modified) { _isModified = modified; }
		// test if the state is modified
		bool isModified() const { return _isModified;  }

	 		 

		// all states of the world
		WorldStateMap CurrentWorldState;
		// indicates if state is changed
		bool _isModified;
		// domain associated with states
		Domain& _domain;

	};

	
	typedef WorldStateProperties WStates;

}

