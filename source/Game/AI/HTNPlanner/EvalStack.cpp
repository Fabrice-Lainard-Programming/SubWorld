

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

#include "EvalStack.h"
// ----------------------------------------------------------------------------

using namespace HTN;

// ----------------------------------------------------------------------------

 
void EvalStack::push(const std::string& variable, long value)
{
	_stack.push_front(Variant(variable, value));
}


void EvalStack::push(const std::string& variable, float value)
{
	_stack.push_front(Variant(variable, value));
}


void EvalStack::push(const std::string& variable, const std::string& value)
{
	_stack.push_front(Variant(variable, value));
}


void EvalStack::push(const std::string& variable, const Point& value)
{
	_stack.push_front(Variant(variable, value));
}

Variant EvalStack::pop()
{
	Variant v = _stack.front();
	_stack.pop_front();
	return v;
}

Variant EvalStack::varAsVariant(const std::string& variable)
{
	for (const Variant& variant : _stack)
	{
		if (variant.variable == variable)
			return variant;
	}
	return Variant();
}



bool EvalStack::varAsBoolean(const std::string& variable, bool& value)
{
	Variant variant = varAsVariant(variable);
	if (variant.isEmpty() || variant.type != VAR_BOOL) return false;
	value = variant.bvalue;
	return true;
}

 bool EvalStack::varAsString(const std::string& variable, std::string& value)
{
	Variant variant = varAsVariant(variable);
	if (variant.isEmpty() || variant.type != VAR_STRING) return false;
	value = variant.svalue;
	return true;
}

bool EvalStack::varAsLong(const std::string& variable, long& value)
{
	Variant variant = varAsVariant(variable);
	if (variant.isEmpty() || variant.type != VAR_LONG) return false;
	value = variant.lvalue;
	return true;
}

bool EvalStack::varAsFloat(const std::string& variable, float& value)
{
	Variant variant = varAsVariant(variable);
	if (variant.isEmpty() || variant.type != VAR_FLOAT) return false;
	value = variant.fvalue;
	return true;
}


bool EvalStack::varAsPoint(const std::string& variable, Point& value)
{
	Variant variant = varAsVariant(variable);
	if (variant.isEmpty() || variant.type != VAR_POINT) return false;
	value = variant.pvalue;
	return true;
}


bool EvalStack::varAsVariant(const std::string& variable, Variant& value)
{

	throw "not implemented";
	return true;
}
