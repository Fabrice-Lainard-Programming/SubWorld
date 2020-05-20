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


#define NOMINMAX
#include "PathFinder.h"


#include "GameLevel.h"
#include "GameWorld.h"
#include "GameLevel.h"
#include "GameNode.h"
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UnigineVisualizer.h>
#include <sstream>
#include <iomanip>
#include <ompl/base/spaces/RealVectorStateSpace.h>
#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/planners/rrt/RRT.h>
#include <ompl/geometric/planners/rrt/RRTstar.h>
#include <ompl/geometric/planners/rrt/RRTConnect.h>
#include <ompl/geometric/planners/rrt/LazyRRT.h>
#include <ompl/base/objectives/PathLengthOptimizationObjective.h>
#include <ompl/base/objectives/StateCostIntegralObjective.h>
#include <ompl/base/objectives/MaximizeMinClearanceObjective.h>
#include <ompl/util/PPM.h>
#include <string>
#include <ompl/config.h>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/math/interpolators/catmull_rom.hpp>
#include <iostream>
#include <functional>
#include <algorithm>
#include <array>
#include "Converter.h"


// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace Unigine;
using namespace boost;
namespace ob = ompl::base;
namespace og = ompl::geometric;



// ----------------------------------------------------------------------------

PathFinder::PathFinder(GameLevel* level)
	: _level(level),_debug(false)
{
}


// ----------------------------------------------------------------------------

PathFinder::~PathFinder()
{
}


// ----------------------------------------------------------------------------



void PathFinder::init(const char * path, int terrainSize,bool debug)
{
	_debug = debug;
	_mapImage = Image::create();
	bool loaded = _mapImage->load(path);
	if (!loaded)
	{
		Log::message("\nPathFinder : Map does not exists: %s ", path);
		return;
	}
	_heightmap_path = path;
	_terrainSize = terrainSize;

	_maxWidth = _mapImage->getWidth()-1;
	_maxHeight = _mapImage->getHeight()-1;
	_scale = (float)_maxWidth / (float)_terrainSize;
	buildDebugImage();

	auto space(std::make_shared<ob::RealVectorStateSpace>());
	space->addDimension(0.0, _maxWidth);
	space->addDimension(0.0, _maxHeight);
//	space->addDimension(0.0, _maxAltitude);

	ss_ = std::make_shared<og::SimpleSetup>(space);
	// set state validity checking for this space
	ss_->setStateValidityChecker([this](const ob::State *state) { return isStateValid(state); });
	space->setup();

	//og::RRTConnect* planner = new og::RRTConnect(ss_->getSpaceInformation());

	//og::RRTstar* planner = new og::RRTstar(ss_->getSpaceInformation());
	og::LazyRRT* planner = new og::LazyRRT(ss_->getSpaceInformation());



	ss_->setPlanner(ob::PlannerPtr(planner));

	if (_debug && !_pathImage)
	{
		OMPL_INFORM("_pathImage is null !!!!!!!");
	}


}



// ----------------------------------------------------------------------------


OpenSteer::PolylinePathway* PathFinder::circular_plan(float radius, int nodex, int nodey, int nodez, int centerx, int centery, int centerz, int circle_radius,int circle_z)
{
	std::vector< OpenSteer::PolylinePathway*> segments;
	bool error = false;
	int nbItems = 0;
	 
	int old_goal_x = (float)( centerx + Unigine::Math::cos((float)0) * circle_radius);
	int old_goal_y = (float)(centery+ Unigine::Math::sin((float)0) * circle_radius);

	// move to the border
	OpenSteer::PolylinePathway* p = plan(radius, nodex, nodey, nodez, old_goal_x, old_goal_y, circle_z);
	if (!p) return nullptr;
	segments.push_back(p);
	nbItems += p->pointCount;

	// travel along the border
	 for (float i = 0; i < 6.28318530718; i += 0.2)
	{
		int  goal_x = centerx + Unigine::Math::cos(i) * circle_radius;
		int  goal_y = centery + Unigine::Math::sin(i) * circle_radius;
		//printf("\n plan: %d %d -> %d %d\n", old_goal_x, old_goal_y, goal_x, goal_y);

	 	p = plan(radius, old_goal_x, old_goal_y, centerz, goal_x, goal_y, circle_z);
		if (p)
		{ 
			segments.push_back(p);
			nbItems += p->pointCount;
		}
		else
		{
			error = true;
			break;
		}
		 old_goal_x = goal_x;
		 old_goal_y = goal_y;

 
	} 

	OpenSteer::Vec3* arr = new OpenSteer::Vec3[nbItems];
	int ip = 0;
	for (OpenSteer::PolylinePathway* ps : segments)
	{
		memcpy(&arr[ip], ps->points, ps->pointCount*sizeof(OpenSteer::Vec3));
		ip += ps->pointCount;
		delete ps;
	}
	
	OpenSteer::PolylinePathway* path = new OpenSteer::PolylinePathway((const int)nbItems, arr, _pathRadius, false);
	return path;

}

// ----------------------------------------------------------------------------


OpenSteer::PolylinePathway* PathFinder::plan(float radius, int x, int y, int z, int goal_x, int goal_y, int goal_z)
{
	if (!ss_)
	{
		OMPL_INFORM("PathFinder bad config");
		return nullptr;
	}

	_sampling_radius = radius * _scale;
	_goal_z = goal_z;
	ob::ScopedState<> start(ss_->getStateSpace());
	WorldToMap(x, y);
	start[0] = x;
	start[1] = y;
	//start[2] = z;
	ob::ScopedState<> goal(ss_->getStateSpace());
	WorldToMap(goal_x, goal_y);
	goal[0] = goal_x;
	goal[1] = goal_y;
	//goal[2] = goal_z;
	ss_->setStartAndGoalStates(start, goal);
	if (ss_->getPlanner())
			ss_->getPlanner()->clear();

	if (_debug && !_pathImage)
	{
		OMPL_INFORM("_pathImage is null !!!!!!!");
	}

	ss_->solve();
	std::size_t ns = ss_->getProblemDefinition()->getSolutionCount();
	if (_debug)
	{
		OMPL_INFORM("Found %d solutions", (int)ns);
	}
	if (ss_->haveSolutionPath())
	{
		return recordSolution();
	}
	saveDebugImage();
	return nullptr;
}

// ----------------------------------------------------------------------------


void setPixel(Image::Pixel& p, int r, int g, int b)
{
	p.i.r = r; p.i.g = g;  p.i.b = b;

}

// ----------------------------------------------------------------------------


bool PathFinder::isStateValid(const ob::State *state) const
{
	int w = std::min((int)state->as<ob::RealVectorStateSpace::StateType>()->values[0], _maxWidth);
	int h = std::min((int)state->as<ob::RealVectorStateSpace::StateType>()->values[1], _maxHeight);

	if (w < 0) return false;
	if (h < 0) return false;
	Image::Pixel pixel = _mapImage->get2D(w, h);

	bool valid = pixel.i.r <= 0;

	// the location is valid, but with want to test the zone around
	if (valid)
	{
		const float  PI_F = 3.14159265358979f;

		float pconv = PI_F / 180.0f;
		for (float i = 0.0; i < 360.0; i += 15.0)
		{
			float angle = i * pconv;
			int x = std::min((int)(w + _sampling_radius * std::cos(angle)), _maxWidth);
			int y = std::min((int)(h + _sampling_radius * std::sin(angle)), _maxHeight);	
			Image::Pixel pixelExtern = _mapImage->get2D(x, y);
			valid = pixelExtern.i.r <= 0;
			if (!valid) break;
		}

	}


	if (_debug)
	{
		if (_debug && !_pathImage)
		{
			OMPL_INFORM("_pathImage is null !!!!!!!");
		}
		if (!valid)
		{
			//	printf("\npt invalid %d %d = %d",h,w, pixel.i.r);
			setPixel(pixel, 255, 80, 10);
			_pathImage->set2D(w, h, pixel);
		}
		else
		{
			setPixel(pixel, 255, 0, 100);
			_pathImage->set2D(w, h, pixel);
		}
	}

	return valid;
}


// ----------------------------------------------------------------------------



OpenSteer::PolylinePathway*  PathFinder::recordSolution()
{
	if (!ss_ || !ss_->haveSolutionPath())
		return nullptr;

	ss_->simplifySolution();
	og::PathGeometric &p = ss_->getSolutionPath();
	ss_->getPathSimplifier()->simplifyMax(p);
	ss_->getPathSimplifier()->smoothBSpline(p);
	p.interpolate();
	if (_debug)
	{
		OMPL_INFORM("Path %d points", (int)p.getStateCount());
	}
	std::vector< OpenSteer::Vec3> pointList;
	for (std::size_t i = 0; i < p.getStateCount(); ++i)
	{
		int w = std::min(_maxWidth, (int)p.getState(i)->as<ob::RealVectorStateSpace::StateType>()->values[0]);
		int h = std::min(_maxHeight, (int)p.getState(i)->as<ob::RealVectorStateSpace::StateType>()->values[1]);
		//int z = std::min(_maxHeight, (int)p.getState(i)->as<ob::RealVectorStateSpace::StateType>()->values[2]);
		int x = w;
		int y = h;
		MapToWorld(x, y);
		pointList.push_back(OpenSteer::Vec3((float)x, (float)y, (float)_goal_z));
		// origin of the map is left bottom 
		h -= _maxHeight;

		if (_debug)
		{
			//	OMPL_INFORM("Path %d %d ", (int)w, h);

			Image::Pixel pixel;
			setPixel(pixel, 0, 254, 0);
			_pathImage->set2D(w, h, pixel);
		}
	}


	// save debug image
	saveDebugImage();

	// catmull rom smoother
	//Smooth(pointList);
	// build 	PolylinePathway
	OpenSteer::Vec3* arr = new OpenSteer::Vec3[pointList.size()];
	std::copy(pointList.begin(), pointList.end(), arr);
	OpenSteer::PolylinePathway* path = new OpenSteer::PolylinePathway((const int)pointList.size(), arr, _pathRadius, false);
	// convert to openSteer coordinate reference
	for (int i = 0; i < path->pointCount; i++)
	{
		std::swap(path->points[i].y, path->points[i].z);
	}

	return path;
}

// ----------------------------------------------------------------------------


void PathFinder::Smooth(std::vector< OpenSteer::Vec3>& pointList)
{
	std::vector< OpenSteer::Vec3> smoothedPoints;
	if (pointList.size() < 4) return;
	std::vector<std::array<float, 3>> points;
	// remove points wich are to close 
	for (int i = 0; i < pointList.size(); ++i)
	{
		OpenSteer::Vec3 n1 = pointList.at(i);
		std::array<float, 3> c = { n1.x, n1.y, n1.z };

		if (i > 0 && i < pointList.size() - 1)
		{
			std::array<float, 3> back = points.back();
			OpenSteer::Vec3  fd = pointList.at(i);
			OpenSteer::Vec3 vn1(fd.x, fd.y, fd.z);
			OpenSteer::Vec3 vn2(back[0], back[1], back[2]);
			float dd = (vn2 - vn1).length();
			if (dd < 2)
				continue;

		}
		points.push_back(c);
	}
	if (points.size() < 4) return;
	try
	{

		 
		boost::math::catmull_rom<std::array<float, 3>> cr(std::move(points), false, 1);
		OpenSteer::Vec3 firstPt = pointList.front();
		OpenSteer::Vec3 lastPt = pointList.back();
		pointList.clear();
		//pointList.push_back(firstPt);
		for (float p = 0; p < cr.max_parameter(); p += 1)
		{
			std::array<float, 3> pt = cr(p);
			pointList.push_back(OpenSteer::Vec3(pt[0], pt[1], pt[2]));
		}
		pointList.push_back(lastPt);

	}
	catch (std::domain_error& e)
	{
		printf("PathFinder::Smooth domain_error! %s", e.what());
	}

}


// ----------------------------------------------------------------------------



void PathFinder::WorldToMap(int& x, int& y)
{
	x = (int)(x * _scale);
	y = (int)(y * _scale);
	y = _maxHeight - y;
}

// ----------------------------------------------------------------------------



void PathFinder::MapToWorld(int& x, int& y)
{
	x = (int)(x / _scale);
	y = (int)+(_maxHeight - y);
	y = (int)(y / _scale);
}


// ----------------------------------------------------------------------------


void PathFinder::saveDebugImage()
{
	if (_debug)
	{
		std::string path = _heightmap_path + "debug.png";
		_pathImage->save(path.c_str());
	}
}



void PathFinder::buildDebugImage()
{
	if (_debug && !_pathImage)
	{
		_pathImage = Image::create();
		_pathImage->create2D(_maxWidth + 1, _maxHeight + 1, Image::FORMAT_RGBA8);

		if (_debug && !_pathImage)
		{
			OMPL_INFORM("_pathImage is null !!!!!!!");
		}


		for (int x = 0; x < _maxWidth; x++)
			for (int y = 0; y < _maxHeight; y++)
			{
				Image::Pixel pixel;
				pixel = _mapImage->get2D(x, y);
				int height = pixel.i.r;

				if (height > 10)
				{
					setPixel(pixel, 255, 255, 255);
				}
				else
				{
					setPixel(pixel, 0, 0, 100);
				}
				_pathImage->set2D(x, y, pixel);
			}
	}
	
}


