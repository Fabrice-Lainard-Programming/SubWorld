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


#include <UnigineGame.h>
#include "Opensteer/include/OpenSteer/Pathway.h"
#include <vector>

// OMPL library forward declaration
namespace ompl
{
	namespace base
	{
		class State;
	}
	namespace geometric
	{
		class SimpleSetup;

		typedef std::shared_ptr<SimpleSetup> SimpleSetupPtr;
	}
}

 


namespace SubWorld
{

	class GameLevel;

	// Search a path in a level using OMPL library
	class PathFinder
	{
	public:
		PathFinder(GameLevel* level);
		~PathFinder();

		// init the pathfinder with its height map
		void init(const char *path, int terrainSize, bool debug);
		// search for a path.. coordinates are in world coordinates
		OpenSteer::PolylinePathway* plan(float radius, int x, int y, int z,int goal_x, int goal_y,int goal_z);
		// search for a circular path.. coordinates are in world coordinates
		OpenSteer::PolylinePathway* circular_plan(float radius,int nodex,int nodey,int nodez, int centerx, int centery, int centerz, int circle_radius, int circle_z);
	private:
		bool isStateValid(const ompl::base::State *state) const;
		OpenSteer::PolylinePathway*  recordSolution();
		void Smooth(std::vector< OpenSteer::Vec3>& pointList);
		void WorldToMap(int& x, int& y);
		void MapToWorld(int& x, int& y);
		void buildDebugImage();
		void saveDebugImage();
	public:
		// public accessors (get and set) 
		GameLevel* _level;
		// heightfield image 
		Unigine::ImagePtr _mapImage, _pathImage;
		// OMPL pathfinder
		ompl::geometric::SimpleSetupPtr ss_;
		// width of the height map in pixel
		int _maxWidth;
		// height of the height map in pixel
		int _maxHeight;
		// max altitude 
		const int _maxAltitude = 1000;
		// altitude of the path
		float _goal_z;
		// if true, create an image with the searched path
		bool _debug;
		// size of the terrain in game's world unit
		int _terrainSize;
		// scale between world and map
		float _scale;
		// size of the sampler
		float _sampling_radius = 40;
		// tube around the path
		const float _pathRadius = 1;
		// path to the heightmap file
		std::string _heightmap_path;
	};

}

