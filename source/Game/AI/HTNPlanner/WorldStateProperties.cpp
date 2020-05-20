
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

#include "WorldStateProperties.h"
#include "HTNTrace.h"
#include "EvalStack.h"
#include "Domain.h"


// ----------------------------------------------------------------------------

using namespace HTN;

// ----------------------------------------------------------------------------
// WorldStateProperties
// ----------------------------------------------------------------------------


WorldStateProperties::~WorldStateProperties()
{
	
}
 


void WorldStateProperties::free()
{
	std::for_each(CurrentWorldState.begin(), CurrentWorldState.end(),
		[&]( std::pair<const std::string, StateGroup>  &element) {
		element.second.free();
	 
	});
	_isModified = false;
}

void WorldStateProperties::clone(WorldStateProperties& wsp)
{
	std::for_each(wsp.CurrentWorldState.begin(), wsp.CurrentWorldState.end(),
		[&](std::pair<const std::string, StateGroup>  &element) {
		StateGroup sg;
		sg.clone(element.second);
		CurrentWorldState[element.first] = sg;
	});
	_isModified = false;
}

BaseState* WorldStateProperties::wState(const std::string& group, const std::string& state)
{
	if (CurrentWorldState.find(group) != CurrentWorldState.end())
	{
		StateGroup* stgroup = &CurrentWorldState[group];
		if (!stgroup) return nullptr;
		return stgroup->getState(state);
	}
	return nullptr;
}

// ----------------------------------------------------------------------------



bool WorldStateProperties::wSetState(const std::string& group, const std::string& state, bool value)
{
	return wSetState(group, state, FCT::Equal, BoolValue(value));
}

bool WorldStateProperties::wSetState(const std::string& group, const std::string& state, long value)
{
	return wSetState(group, state, FCT::Equal, LongValue(value));
}

bool WorldStateProperties::wSetState(const std::string& group, const std::string& state, float value)
{
	return wSetState(group, state, FCT::Equal, FloatValue(value));
}

bool WorldStateProperties::wSetState(const std::string& group, const std::string& state, const std::string& value)
{
	return wSetState(group, state, FCT::Equal, StringValue(value));
}

bool WorldStateProperties::wSetState(const std::string& group, const std::string& state, const Point& value)
{
	return wSetState(group, state, FCT::Equal, PointValue(value));
}

// ----------------------------------------------------------------------------



bool WorldStateProperties::wSetState(const std::string& group, const std::string& state, FCT Fct,const BaseValue& Value)
{
	if (CurrentWorldState.find(group) != CurrentWorldState.end())
	{
		StateGroup* stgroup = &CurrentWorldState[group];
		
		BaseState* bs =  stgroup->getState(state);
		if (!bs)
		{
			 throw std::domain_error("WorldStateProperties::wSetState : state does not exist : " + group + "." + state);
		}
		const BaseValue* valueOperand = &Value;

		// indirection, we need to find the value of a state
		const StateValue* sv = dynamic_cast<const StateValue*>(valueOperand);
		if (sv)
		{
			BaseState* valueState = wState(sv->state.Group, sv->state.State);
			valueOperand = dynamic_cast<BaseValue*>(valueState);
			if (!valueOperand)
			{
				throw std::domain_error("WorldStateProperties::wSetState :  " + sv->state.Group + "." + sv->state.State + FctLib::toString(Fct) + " ? Value is null");
			}
		}

		if (!valueOperand)
		{
			throw std::domain_error("WorldStateProperties::wSetState :  "+ group+"."+ state+ FctLib::toString(Fct)+" ? Value is null");
		}
		 
		HTNTrace(_domain.Name, "\tmodify state %s.%s %s ", group.c_str(), state.c_str(), FctLib::toString(Fct).c_str());// , valueOperand->toString().c_str());

		bs->affect(_domain.EvalTask,Fct, valueOperand);

		setIsModified(true);
		return true;
	}
	else
	{
		throw std::domain_error("WorldStateProperties::wSetState : state does not exist : " + group + "." + state);
	}
  
}
 

// ----------------------------------------------------------------------------
// StateGroup
// ----------------------------------------------------------------------------

BaseState* StateGroup::getState(const std::string& state)
{
	if (States.find(state) != States.end())
	{
		return  States[state];
	}
	return nullptr;
}

 
StateGroup::~StateGroup()
{
	 
}


void StateGroup::free()
{
	std::for_each(States.begin(), States.end(),
		[&](std::pair<const std::string, BaseState*>  &element) {
		delete element.second;
		element.second = nullptr;
	});
}


void StateGroup::clone(const StateGroup& sg)
{
	std::for_each(sg.States.begin(), sg.States.end(),
		[&](const std::pair<const std::string, BaseState*>  &element) {

		States[element.first] = element.second->clone();

	});
}
 
 
 
 

// ----------------------------------------------------------------------------





bool StateBoolValue::compare(EvalStack& evalTask,FCT fct, BaseValue* value)
{
	bool v;
	BooleanVar* var = dynamic_cast<BooleanVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		FctBoolValue* fctvalue = dynamic_cast<FctBoolValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			BoolValue* bvalue = dynamic_cast<BoolValue*>(value);
			if (!bvalue)
			{

				throw std::domain_error("StateBoolValue compare : argument is not a boolean value");

			}
			v = bvalue->Value;
		}
	}
	switch (fct)
	{
	case IsEqual:  return this->Value == v;
	case IsNequal:  return this->Value != v;
	default:
	{
		throw std::domain_error("StateBoolValue compare : bad operation !");
	}
	}
}



void StateBoolValue::affect(EvalStack& evalTask, FCT fct, const BaseValue* value)
{
	bool v;
	const BooleanVar* var = dynamic_cast<const BooleanVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		const FctBoolValue* fctvalue = dynamic_cast<const FctBoolValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			const BoolValue* bvalue = dynamic_cast<const BoolValue*>(value);
			if (!bvalue)
			{

				throw std::domain_error("StateBoolValue affect : argument is not a boolean value");

			}
			v = bvalue->Value;
		}
	}
#if !defined NDEBUG
	printf("%s", v ? "true" : "false");
#endif
	switch (fct)
	{
	case Equal:   this->Value = v; break;
	default:
	{
		throw std::domain_error("StateBoolValue affect : bad operation !");
	}
	}
}

 


// ----------------------------------------------------------------------------


bool StateLongValue::compare(EvalStack& evalTask, FCT fct, BaseValue* value)
{
	long v;
	const LongVar* var = dynamic_cast<const LongVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		FctLongValue* fctvalue = dynamic_cast<FctLongValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{


			LongValue* bvalue = dynamic_cast<LongValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateLongValue compare : argument is not a Long");
			}

			v = bvalue->Value;
		}
	}
	switch (fct)
	{
	case IsEqual:  return this->Value == v;
	case IsNequal:  return this->Value != v;
	case Sup: return this->Value > v;
	case Inf:  return this->Value < v;
	default:
	{
		throw std::domain_error("StateLongValue compare : bad operation !");	 
	}
	}
}



void StateLongValue::affect(EvalStack& evalTask, FCT fct, const BaseValue* value)
{
	long v;
	const LongVar* var = dynamic_cast<const LongVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		const FctLongValue* fctvalue = dynamic_cast<const FctLongValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			const LongValue* bvalue = dynamic_cast<const LongValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateLongValue compare : argument is not a Long");
			}
			v = bvalue->Value;
		}
	}
#if !defined NDEBUG
	printf("%ld", v);
#endif
	switch (fct)
	{
	case Equal:   this->Value = v; break;
	case Incr:   this->Value += v; break;
	case Decr:   this->Value -= v; break;
	default:
	{
		throw std::domain_error("StateLongValue affect : bad operation !");
	}
	}
}
 
// ----------------------------------------------------------------------------


bool StateFloatValue::compare(EvalStack& evalTask, FCT fct,  BaseValue* value)
{
	float v;
	const FloatVar* var = dynamic_cast<const FloatVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		FctFloatValue* fctvalue = dynamic_cast<FctFloatValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			FloatValue* bvalue = dynamic_cast<FloatValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateFloatValue compare : argument is not a Float");
			}
			v = bvalue->Value;
		}
	}
	switch (fct)
	{
	case IsEqual:  return this->Value == v;
	case IsNequal:  return this->Value != v;
	case Sup: return this->Value > v;
	case Inf:  return this->Value < v;
	default:
	{
		throw std::domain_error("StateFloatValue compare : bad operation !");
	}
	}
}




void StateFloatValue::affect(EvalStack& evalTask, FCT fct, const BaseValue* value)
{
	float v;
	const FloatVar* var = dynamic_cast<const FloatVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		const FctFloatValue* fctvalue = dynamic_cast<const FctFloatValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			const FloatValue* bvalue = dynamic_cast<const FloatValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateFloatValue affect : argument is not a Float");
			}
			v = bvalue->Value;
		}
	}
#if !defined NDEBUG
	printf("%f", v);
#endif

	switch (fct)
	{
	case Equal:   this->Value = v; break;
	case Incr:   this->Value += v; break;
	case Decr:   this->Value -= v; break;
	default:
	{
		throw std::domain_error("StateFloatValue affect : bad operation !");
	}
	}
}


// ----------------------------------------------------------------------------




bool StateStringValue::compare(EvalStack& evalTask, FCT fct,  BaseValue* value)
{
	std::string v;
	StringVar* var = dynamic_cast<StringVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		FctStringValue* fctvalue = dynamic_cast<FctStringValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			StringValue* bvalue = dynamic_cast<StringValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateStringValue compare : argument is not a String");
			}
			v = bvalue->Value;
		}
	}
	switch (fct)
	{
	case IsEqual:  return !this->Value.compare(v);
	case IsNequal:  return this->Value.compare(v);
	case Sup: return this->Value > v;
	case Inf:  return this->Value < v;
	default:
	{
		throw std::domain_error("StateStringValue compare : bad operation !");
	}
	}
}


void StateStringValue::affect(EvalStack& evalTask, FCT fct, const BaseValue* value)
{
	std::string v;

	const StringVar* var = dynamic_cast<const StringVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		const FctStringValue* fctvalue = dynamic_cast<const FctStringValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			const StringValue* bvalue = dynamic_cast<const StringValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateStringValue affect : argument is not a String");
			}
			v = bvalue->Value;
		}
	}
#if !defined NDEBUG
	printf("%s", v.c_str());
#endif
	switch (fct)
	{
	case Equal:   this->Value = v; break;
	default:
	{
		throw std::domain_error("StateStringValue affect : bad operation !");
	}
	}
}


// ----------------------------------------------------------------------------


bool StatePointValue::compare(EvalStack& evalTask, FCT fct, BaseValue* value)
{
	Point v;
	const PointVar* var = dynamic_cast<const PointVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		FctPointValue* fctvalue = dynamic_cast<FctPointValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			PointValue* bvalue = dynamic_cast<PointValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StatePointValue compare : argument is not a Point");
			}
			v = bvalue->Value;
		}
	}
	switch (fct)
	{
	case IsEqual:  return (this->Value == v);
	case IsNequal:    return !(this->Value == v);
	default:
	{
		throw std::domain_error("StatePointValue compare : bad operation !");	 
	}
	}
}



void StatePointValue::affect(EvalStack& evalTask, FCT fct, const BaseValue* value)
{
	Point v;
	const PointVar* var = dynamic_cast<const PointVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		const FctPointValue* fctvalue = dynamic_cast<const FctPointValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			const PointValue* bvalue = dynamic_cast<const PointValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StatePointValue compare : argument is not a Point");
			}
			v = bvalue->Value;
		}
	}
#if !defined NDEBUG
	printf("%s", v.toString().c_str());
#endif
	switch (fct)
	{
	case Equal:  this->Value = v;  break;
	case Incr:   this->Value.X += v.X; this->Value.Y +=v.Y; this->Value.Z += v.Z; break;
	case Decr:   this->Value.X -= v.X; this->Value.Y -= v.Y; this->Value.Z -= v.Z; break;
	default:
	{
		throw std::domain_error("StatePointValue affect : bad operation !");
	}
	}
}




// ----------------------------------------------------------------------------


bool StateVariantValue::compare(EvalStack& evalTask, FCT fct, BaseValue* value)
{
	Variant v;
	const VariantVar* var = dynamic_cast<const VariantVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		FctVariantValue* fctvalue = dynamic_cast<FctVariantValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			VariantValue* bvalue = dynamic_cast<VariantValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateVariantValue compare : argument is not a Variant");
			}
			v = bvalue->Value;
		}
	}
	switch (fct)
	{
	case IsEqual:  return (this->Value == v);
	case IsNequal:    return !(this->Value == v);
	default:
	{
		throw std::domain_error("StateVariantValue compare : bad operation !");
	}
	}
}



void StateVariantValue::affect(EvalStack& evalTask, FCT fct, const BaseValue* value)
{
	Variant v;
	const VariantVar* var = dynamic_cast<const VariantVar*>(value);
	if (var)
	{
		v = var->getValue(evalTask);
	}
	else
	{
		const FctVariantValue* fctvalue = dynamic_cast<const FctVariantValue*>(value);
		if (fctvalue)
		{
			v = fctvalue->Value();
		}
		else
		{
			const VariantValue* bvalue = dynamic_cast<const VariantValue*>(value);
			if (!bvalue)
			{
				throw std::domain_error("StateVariantValue compare : argument is not a Point");
			}
			v = bvalue->Value;
		}
	}
#if !defined NDEBUG
	printf("%s", v.toString().c_str());
#endif
	switch (fct)
	{
	case Equal:  this->Value = v;  break;
 	default:
	{
		throw std::domain_error("StateVariantValue affect : bad operation !");
	}
	}
}






// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------



bool BooleanVar::getValue(EvalStack& evalTask) const
{
	bool value;
	bool hasVarValue = evalTask.varAsBoolean(Variable, value);
	if (!hasVarValue)
	{
		throw std::domain_error("BooleanVar::getValue : variable not found in stack : " + Variable);
	}
	return value;
}

std::string StringVar::getValue(EvalStack& evalTask) const 
{
	std::string value;
	bool hasVarValue = evalTask.varAsString(Variable, value);
	if (!hasVarValue)
	{
		throw std::domain_error("StringVar::getValue : variable not found in stack: " + Variable);
	}
	return value;
}

long LongVar::getValue(EvalStack& evalTask) const
{
	long value;
	bool hasVarValue = evalTask.varAsLong(Variable, value);
	if (!hasVarValue)
	{
		throw std::domain_error("LongVar::getValue : variable not found in stack: " + Variable);
	}
	return value;
}

float FloatVar::getValue(EvalStack& evalTask) const
{
	float value;
	bool hasVarValue = evalTask.varAsFloat(Variable, value);
	if (!hasVarValue)
	{
		throw std::domain_error("FloatVar::getValue : variable not found in stack: " + Variable);
	}
	return value;
}

Point PointVar::getValue(EvalStack& evalTask) const
{
	Point value;
	bool hasVarValue = evalTask.varAsPoint(Variable, value);
	if (!hasVarValue)
	{
		throw std::domain_error("PointVar::getValue : variable not found in stack: " + Variable);
	}
	return value;
}

Variant VariantVar::getValue(EvalStack& evalTask) const
{
	Variant value;
	bool hasVarValue = evalTask.varAsVariant(Variable, value);
	if (!hasVarValue)
	{
		throw std::domain_error("VariantVar::getValue : variable not found in stack: " + Variable);
	}
	return value;
}

