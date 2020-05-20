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

#include <vector>
#include "Opensteer/include/OpenSteer/AbstractVehicle.h"
#include "Opensteer/include/OpenSteer/Obstacle.h"
#include <UnigineMathLib.h>
#include "GameNode.h"

namespace SubWorld
{
	class GamePlay;
 
	class PathFinder;

	
	// A level in game
	class GameLevel
	{
	public:
		GameLevel(GamePlay* gameplay, const std::string& heightMap);
		~GameLevel();

		// called when a level should load its content
		virtual void loadLevel() = 0;
		// called when a level should unload its content
		virtual void closeLevel() = 0;
		// update objects
		virtual void update(const float currentTime, const float elapsedTime);
		//  callback fo update called each seconds
		virtual void update_on_400ms(const float currentTime, const float elapsedTime);
		// update physic bodies
		virtual void updatePhysic(const float currentTime, const float elapsedTime);	
		// called on mouse click over terrain, node, ...
		virtual void mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt);
		// draw annotations
		virtual void drawAnnotations(const float elapsedTime);
		// return all nodes ids which are in the given sphere
		std::vector<long> getBoudingSphereIntersection(const OpenSteer::Vec3& center, float radius);
		// try to select the node under the mouse pointer
		void trySelectNode();
		// add a node to the list
		void addGameNode(GameNodePtr v);
		// remove a node
		void removeGameNode(GameNodePtr v);
		// retrieves all nodes of this level
		std::vector<GameNodePtr>& getNodes() { return _nodes;	}
		// search and return a vehicule by its id
		GameNodePtr getGameNode(int id);
		
	private:
		void initProximityDatabase(void);

	public:
		// public accessors (get and set) 
		GamePlay* _gameplay;
		// height map path
		std::string _heightMap;
		// list of nodes actually in the level
		std::vector<GameNodePtr> _nodes;
		// a pointer to the proximity database	 
		ProximityDatabase* _pd;	 
		// grouped vehicule used in collision database
		OpenSteer::AVGroup _neighbors;
		// obstacles
		OpenSteer::ObstacleGroup _obstacles;
		// path finder
		PathFinder* _pathFinder;
		// last click location in screen coordinate
		Unigine::Math::ivec2 _last_mouse_click_coordinates;
	};

}