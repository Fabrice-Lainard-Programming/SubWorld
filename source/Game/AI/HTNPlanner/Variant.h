 
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
#include "Geometry.h"


namespace HTN
{


	enum VariantType
	{
		VAR_EMPTY,
		VAR_BOOL,
		VAR_LONG,
		VAR_FLOAT,
		VAR_STRING,
		VAR_POINT
	};

	// An instance of Variant at any given time either holds a 
	// value of one of its alternative types
	struct Variant 
	{
		Variant() : type(VariantType::VAR_EMPTY),bvalue(false),lvalue(0),fvalue(0){}
		Variant(const std::string& var, bool v) : variable(var), bvalue(v), type(VariantType::VAR_BOOL) {}
		Variant(const std::string& var, long v) : variable(var), lvalue(v), type(VariantType::VAR_LONG) {}
		Variant(const std::string& var, float v) : variable(var), fvalue(v), type(VariantType::VAR_FLOAT) {}
		Variant(const std::string& var, const std::string& v) : variable(var), svalue(v), type(VariantType::VAR_STRING) {}
		Variant(const std::string& var, const Point& v) :variable(var), pvalue(v), type(VariantType::VAR_POINT) {}
		Variant(bool v) : variable("?"), bvalue(v), type(VariantType::VAR_BOOL) {}
		Variant(int v) : variable("?"), lvalue(v), type(VariantType::VAR_LONG) {}
		Variant(long v) : variable("?"), lvalue(v), type(VariantType::VAR_LONG) {}
		Variant(float v) : variable("?"), fvalue(v), type(VariantType::VAR_FLOAT) {}
		Variant(const std::string& v) : variable("?"), svalue(v), type(VariantType::VAR_STRING) {}
		Variant(const Point& v) :variable("?"), pvalue(v), type(VariantType::VAR_POINT) {}
		bool isEmpty() const { return type == VAR_EMPTY; }


		bool operator==(const Variant& rhs)
		{
			if (type != rhs.type) return false;
			switch (type)
			{
			default:
			case VAR_EMPTY: return true;
			case VAR_BOOL: return bvalue == rhs.bvalue;
			case VAR_LONG:  return lvalue == rhs.lvalue;
			case VAR_FLOAT:  return fvalue == rhs.fvalue;
			case VAR_STRING:  return svalue == rhs.svalue;
			case VAR_POINT:  return pvalue == rhs.pvalue;
			}
			 
		}

		std::string toString() const
		{
			switch (type)
			{
			default:
			case VAR_EMPTY: return "?";
			case VAR_BOOL: return bvalue ? "true" : "false";
			case VAR_LONG: return std::to_string(lvalue);
			case VAR_FLOAT: return std::to_string(fvalue);
			case VAR_STRING: return svalue;
			case VAR_POINT: return pvalue.toString();
			}
		}

		bool AsBoolean()
		{
		 	if (isEmpty() || type != VAR_BOOL) 
				throw std::domain_error("Variant::AsBoolean : cannot convert to boolean");
			return bvalue;
		}

		std::string asString()
		{
			if (isEmpty() || type != VAR_STRING)
				throw std::domain_error("Variant::asString : cannot convert to string");
			return svalue;
		}

		long asLong()
		{
			if (isEmpty() || type != VAR_LONG)
				throw std::domain_error("Variant::asString : cannot convert to long");
			return lvalue;
		}

		float asFloat()
		{
			if (isEmpty() || type != VAR_FLOAT)
				throw std::domain_error("Variant::asString : cannot convert to float");
			return fvalue;
		}


		Point asPoint()
		{
			if (isEmpty() || type != VAR_POINT)
				throw std::domain_error("Variant::asString : cannot convert to Point");
			return pvalue;
		}

  



		VariantType type;
		bool bvalue;
		long lvalue;
		float fvalue;
		std::string svalue;
		Point pvalue;
		std::string variable;
	};
}