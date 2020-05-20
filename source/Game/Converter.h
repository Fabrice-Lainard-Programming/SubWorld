// ----------------------------------------------------------------------------
//
//
// SubWorld -- SubMarine Game
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

#include "Opensteer/include/OpenSteer/SimpleVehicle.h"
#include "Opensteer/include/OpenSteer/Pathway.h"
#include "UnigineMathLib.h"

 
namespace SubWorld
{
	// converter used to manage unigine / opensteer data conversion
	class Converter
	{
	public:

		static Unigine::Math::vec3 toUnigine(OpenSteer::Vec3 v)
		{
			return Unigine::Math::vec3(v.x, v.z, v.y);
		}

		static OpenSteer::Vec3 toOpenSteer(Unigine::Math::vec3 v)
		{
			return OpenSteer::Vec3(v.x, v.z, v.y);
		}

		static OpenSteer::Vec3 toOpenSteerColor(Unigine::Math::vec4 v)
		{
			return OpenSteer::Vec3(v.x, v.y, v.z);
		}
	};


}