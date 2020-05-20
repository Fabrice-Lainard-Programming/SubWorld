
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

#include <string>
#include <functional>
#include "Geometry.h"
#include "Variant.h"

namespace HTN
{
	class EvalStack;

	// base struct for values
	struct BaseValue
	{
		virtual ~BaseValue() {}
		virtual std::string toString() const { return "<unknown>"; }
	};

	// Hold a boolean value
	struct BoolValue : BaseValue
	{
		BoolValue(bool v) : Value(v) {}
		virtual ~BoolValue() {}
		std::string toString() const { return std::to_string(Value); }
		bool Value;
	};

	// Hold a long value
	struct LongValue : BaseValue
	{
		LongValue(long v) : Value(v) {}
		virtual ~LongValue() {}
		std::string toString()const { return std::to_string(Value); }

		long Value;
	};

	// Hold a float value
	struct FloatValue : BaseValue
	{
		FloatValue(float v) : Value(v) {}
		virtual ~FloatValue() {}
		std::string toString()const { return std::to_string(Value); }

		float Value;
	};


	// Hold a string value
	struct StringValue : BaseValue
	{
		StringValue(const std::string& v) : Value(v) {}
		virtual ~StringValue() {}
		std::string toString()const { return Value; }
		std::string Value;
	};



	


	// Hold a point in space 
	struct PointValue : BaseValue
	{
		PointValue(float x, float y,float z) : Value(x,y,z) {}
		PointValue(const Point& p) : Value(p) {}
		~PointValue() {}

		bool operator==(const PointValue& rhs)
		{
			return Value == rhs.Value;
		}

		std::string toString()const { return std::to_string(Value.X) + "," + std::to_string(Value.Y) + "," + std::to_string(Value.Z);}

		Point Value;
	};


	// Hold a variant 
	struct VariantValue : BaseValue
	{
	 	VariantValue(const Variant& v) : Value(v) {}
		~VariantValue() {}

		bool operator==(const VariantValue& rhs)
		{
			return Value == rhs.Value;
		}

		std::string toString()const { return Value.toString(); }

		Variant Value;
	};

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------


	// Hold a function value
	struct FctBoolValue : BaseValue
	{
		FctBoolValue(std::function<bool()> v) : Value(v) {}
		virtual ~FctBoolValue() {}
		std::string toString()const { return "FctBoolValue"; }
		std::function<bool()> Value;
	};

	// Hold a function value
	struct FctLongValue : BaseValue
	{
		FctLongValue(std::function<long()> v) : Value(v) {}
		virtual ~FctLongValue() {}
		std::string toString()const { return "FctLongValue"; }
		std::function<long()> Value;
	};

	// Hold a function value
	struct FctFloatValue : BaseValue
	{
		FctFloatValue(std::function<float()> v) : Value(v) {}
		virtual ~FctFloatValue() {}
		std::string toString()const { return "FctFloatValue"; }
		std::function<float()> Value;
	};

	// Hold a function value
	struct FctStringValue : BaseValue
	{
		FctStringValue(std::function<std::string()> v) : Value(v) {}
		virtual ~FctStringValue() {}
		std::string toString()const { return "FctStringValue"; }
		std::function<std::string()> Value;
	};


	// Hold a function value
	struct FctPointValue : BaseValue
	{
		FctPointValue(std::function<Point()> v) : Value(v) {}
		virtual ~FctPointValue() {}
		std::string toString()const { return "FctPointValue"; }
		std::function<Point()> Value;
	};

	// Hold a variant value
	struct FctVariantValue : BaseValue
	{
		FctVariantValue(std::function<Variant()> v) : Value(v) {}
		virtual ~FctVariantValue() {}
		std::string toString()const { return "FctVariantValue"; }
		std::function<Variant()> Value;
	};

	 
// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------


	// boolean variable
	struct BooleanVar : BaseValue
	{
		BooleanVar(const std::string& v) : Variable(v) {}
		virtual ~BooleanVar() {}
		std::string toString()const { return Variable; }
		bool getValue(EvalStack& evalTask) const;
		std::string Variable;
	};


	// string variable
	struct StringVar : BaseValue
	{
		StringVar(const std::string& v) : Variable(v) {}
		virtual ~StringVar() {}
		std::string toString()const { return Variable; }
		std::string getValue(EvalStack& evalTask) const;
		std::string Variable;
	};
	

	// long variable
	struct LongVar : BaseValue
	{
		LongVar(const std::string& v) : Variable(v) {}
		virtual ~LongVar() {}
		std::string toString()const { return Variable; }
		long getValue(EvalStack& evalTask) const;
		std::string  Variable;
	};

	// float variable
	struct FloatVar : BaseValue
	{
		FloatVar(const std::string& v) : Variable(v) {}
		virtual ~FloatVar() {}
		std::string toString()const { return Variable; }
		float getValue(EvalStack& evalTask) const;
		std::string  Variable;
	};

	// Point variable
	struct PointVar : BaseValue
	{
		PointVar(const std::string& v) : Variable(v) {}
		virtual ~PointVar() {}
		std::string toString()const { return Variable; }
		Point getValue(EvalStack& evalTask) const;
		std::string  Variable;
	};

	// Variant variable
	struct VariantVar : BaseValue
	{
		VariantVar(const std::string& v) : Variable(v) {}
		virtual ~VariantVar() {}
		std::string toString()const { return Variable; }
		Variant getValue(EvalStack& evalTask) const;
		std::string  Variable;
	};



}