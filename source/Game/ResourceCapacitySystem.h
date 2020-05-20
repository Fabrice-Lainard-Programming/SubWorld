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
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include "GameFactory.h"
#include <string>
#include <vector>
#include <functional>
#include <time.h>
#include <chrono>


namespace SubWorld
{
	class GameNode;

	// describe a capacity : available asset for a given type and the time to build it
	struct Resource_Capacity
	{
		std::string _template_name;  
		enumGameTemplateType _template_type;
		int _available_capacity;
		float _build_time_s;
		void*_parameter;
		std::function<void(void*asset,void*param)> _builder;
		Resource_Capacity();
		Resource_Capacity(const std::string& template_name, enumGameTemplateType _template_type, int available_capacity, float build_time_s, std::function<void(void*,void*)> builder);
	};

	// describe a resource which the system actually build
	struct Resource_Build_In_Progress
	{
		std::chrono::system_clock::time_point _dt_started;
		bool _started;
		Resource_Capacity* _resource_Capacity;
		Resource_Build_In_Progress();
		Resource_Build_In_Progress(Resource_Capacity* resource_Capacity);
		void start();
		bool isReady();
		long getElaspedTime_s();
	};

 


	// Manage game resources of a node
	class ResourceCapacitySystem : public ComponentBase
	{
	public:

		// methods
		COMPONENT(ResourceCapacitySystem, ComponentBase);
		COMPONENT_UPDATE(update);
		COMPONENT_INIT(init)
		COMPONENT_SHUTDOWN(shutdown);

		PROP_NAME("Resource_Capacity");
		PROP_PARAM(String, name, "DummyParam");


		// add new capacity to the game play
		void addCapacity(Resource_Capacity& capacity);
		// planify a build of an asset
		void planify_creation(const std::string& template_name, int quantity, void* parameter=nullptr);
		// search a capacity
		Resource_Capacity* find_capacity(const std::string& template_name);
		 // list of all resource capacities
		std::vector< Resource_Capacity>& getCapacities() { return  _resources_capacities; }
		// gets in progress building if any for a given resource capacity
		std::vector<Resource_Build_In_Progress*> find_build(Resource_Capacity& rc);
		// gets in progress building if any for a given resource capacity : number of build, total time in s, how many seconds for the first assets to be read
		void get_build_counters(Resource_Capacity& rc,long &nbbuild,long& total_build_time_s,long& first_time_left_s);



	protected:
		void init();
		void update();
		void shutdown();
		void check_builder();
		 
	protected:

		// list of capacities (ie game assets which can be created)
		std::vector< Resource_Capacity> _resources_capacities;
		// list of assets which are currently build 
		std::vector< Resource_Build_In_Progress> _build_in_progress;
		 
	};



}