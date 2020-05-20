 
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



#include "ResourceCapacitySystem.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include <UnigineApp.h>
#include <UnigineUserInterface.h>
#include <list>

// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;


// ----------------------------------------------------------------------------

REGISTER_COMPONENT(ResourceCapacitySystem);



// ----------------------------------------------------------------------------


void ResourceCapacitySystem::shutdown()
{
	 

}

// ----------------------------------------------------------------------------


void ResourceCapacitySystem::update()
{
	check_builder();
}


// ----------------------------------------------------------------------------
 

void ResourceCapacitySystem::init()
{
	
	
 
}


// ----------------------------------------------------------------------------

void ResourceCapacitySystem::check_builder()
{
	std::vector< Resource_Build_In_Progress*> builder_finished;

	// schedule starts
	for (Resource_Build_In_Progress& build : _build_in_progress)
	{
		// if a build is not started
		if (!build._started) 
		{
			// search if another build of the same resource caapcity type is started
			bool other_started = false;
			for (Resource_Build_In_Progress& other_build : _build_in_progress)
			{
				if (build._resource_Capacity == other_build._resource_Capacity && &build != &other_build && other_build._started)
				{
					other_started = true;
					break;
				}
			}
			// if there is no other build started for that capacity, schedule it
			if(!other_started) 
				build.start();
		}
	}

	// search if we have finished build
	for (Resource_Build_In_Progress& build : _build_in_progress)
	{
		// if the build is finished
		if (build.isReady())
		{
			//  build game asset with game factory
			void* asset = GamePlay::Game->factory()->buildAsset(build._resource_Capacity->_template_name);
			// call game function which need it 
			build._resource_Capacity->_builder(asset, build._resource_Capacity->_parameter);
			// memorize this build to delete later
			builder_finished.push_back(&build);
		}
	}
	// remove finished builds
	for (Resource_Build_In_Progress* build : builder_finished)
	{
		for (std::vector<Resource_Build_In_Progress>::iterator it = _build_in_progress.begin(); it != _build_in_progress.end(); ++it)
		{
			if(build == &(*it))
			{
				_build_in_progress.erase(it);
					break;
			}
		}
	}

}

// ----------------------------------------------------------------------------


void ResourceCapacitySystem::addCapacity(Resource_Capacity& capacity)
{
	_resources_capacities.push_back(capacity);
}


// ----------------------------------------------------------------------------


void ResourceCapacitySystem::planify_creation(const std::string& template_name, int quantity,void* parameter)
{
	Resource_Capacity* capacity = find_capacity(template_name);
	if (!capacity)
	{
		printf("ResourceCapacitySystem::planify_creation %s does not exists ! ", template_name.c_str());
		return;
	}

	if (quantity > capacity->_available_capacity)
	{
		return; 
	}
	// add builds
	for (int i = 0; i < quantity; ++i)
	{
		capacity->_parameter = parameter;
		_build_in_progress.push_back(Resource_Build_In_Progress(capacity));
	}
	// substract the quantity 
	capacity->_available_capacity -= quantity;

}


// ----------------------------------------------------------------------------


Resource_Capacity* ResourceCapacitySystem::find_capacity(const std::string& template_name)
{
	for (Resource_Capacity& def : _resources_capacities)
	{
		//if (!strcmpi(def._template_name.c_str(),template_name.c_str()))
		if (def._template_name == template_name)
		{
			return &def;
		}
	} 
	return nullptr;
}

// ----------------------------------------------------------------------------


std::vector<Resource_Build_In_Progress*> ResourceCapacitySystem::find_build(Resource_Capacity& rc)
{
	std::vector<Resource_Build_In_Progress*> builds;
	for (Resource_Build_In_Progress& build : _build_in_progress)
	{
		if (build._resource_Capacity->_template_name == rc._template_name)
		{
			builds.push_back(&build);
		}
	}
	return builds;
}

// ----------------------------------------------------------------------------



void ResourceCapacitySystem::get_build_counters(Resource_Capacity& rc, long &nbbuild, long& total_build_time_s, long& first_time_left_s)
{
	nbbuild = 0;
	total_build_time_s = 0;
	first_time_left_s = 0;
	long maxs = 100000;
	bool onStarted = false;
	for (Resource_Build_In_Progress& build : _build_in_progress)
	{
		if (build._resource_Capacity->_template_name != rc._template_name)
		{
			continue;
		}
		total_build_time_s += (long)build._resource_Capacity->_build_time_s;
		nbbuild++;
		
		if (build._started)
		{
			onStarted = true;
			long elapsed_s = (long)(build._resource_Capacity->_build_time_s - build.getElaspedTime_s());
			if (elapsed_s < maxs)
				maxs = elapsed_s;
		}
	}
	first_time_left_s = onStarted ? maxs : -1;
}


// ----------------------------------------------------------------------------
// Resource_Capacity
// ----------------------------------------------------------------------------


Resource_Capacity::Resource_Capacity()
	: _parameter(nullptr)
{

}


Resource_Capacity::Resource_Capacity(const std::string& template_name, enumGameTemplateType template_type, int available_capacity, float build_time_s, std::function<void(void*,void*)> builder)
	: _template_name(template_name), _template_type(template_type), _available_capacity(available_capacity), _build_time_s(build_time_s), _builder(builder), _parameter(nullptr)
{

}

// ----------------------------------------------------------------------------
// Resource_Build_In_Progress
// ----------------------------------------------------------------------------


Resource_Build_In_Progress::Resource_Build_In_Progress() 
	: _resource_Capacity(nullptr), _started(false)
{
}

// ----------------------------------------------------------------------------


Resource_Build_In_Progress::Resource_Build_In_Progress(Resource_Capacity* resource_Capacity)
	: _resource_Capacity(resource_Capacity),  _started(false)
{

}

// ----------------------------------------------------------------------------


void Resource_Build_In_Progress::start()
{
	_dt_started = std::chrono::system_clock::now();
	_started = true;
}
 
// ----------------------------------------------------------------------------


bool Resource_Build_In_Progress::isReady()
{
	if (!_started) return false;
	auto elapsed_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _dt_started).count();
	return elapsed_s > _resource_Capacity->_build_time_s;

}

// ----------------------------------------------------------------------------



long Resource_Build_In_Progress::getElaspedTime_s()
{
	auto elapsed_s = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - _dt_started);
	return (long)elapsed_s.count();

}
 
 