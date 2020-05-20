 
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

#include "../ComponentSystem/ComponentSystem.h"
#include "Opensteer/include/OpenSteer/SimpleVehicle.h"
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UnigineMathLib.h>
#include "GameFactory.h"
#include "GameLevel.h"
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <chrono>


namespace SubWorld
{
	class GameNode;
 

	// Manage steering of nodes
	class SonarEffect : public ComponentBase
	{
	public:

		// methods
		COMPONENT(SonarEffect, ComponentBase);
		COMPONENT_UPDATE(update);
		COMPONENT_INIT(init)
		COMPONENT_SHUTDOWN(shutdown);

		PROP_NAME("SonarEffect");
		PROP_PARAM(String, name, "DummyParam");

		void ping(float radius);
		 
	protected:
		void init();
		void update();
		void shutdown();
		void setAuxiliaryRecursive(Unigine::NodePtr gamenode, bool resetAux);
 		GameNodePtr getGameNode();

	protected:
		float _radius;
		float _propagation_radius;
		float _interval;

	};



}

