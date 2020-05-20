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


#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "GameLevel.h"
#include "PathFinder.h"
#include "GameNode.h"
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UnigineInput.h>
#include <UnigineVisualizer.h>
#include <sstream>
#include <iomanip>
#include "Opensteer/include/OpenSteer/Draw.h"
#include "Converter.h"

// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;

// ----------------------------------------------------------------------------

GameLevel::GameLevel(GamePlay* gameplay, const std::string& heightMap)
	: _gameplay(gameplay), _heightMap(heightMap), _pathFinder(new PathFinder(this))
{
	initProximityDatabase();
}

// ----------------------------------------------------------------------------

GameLevel::~GameLevel()
{
	safe_delete(_pathFinder);
}

// ----------------------------------------------------------------------------

void GameLevel::addGameNode(GameNodePtr v)
{
	_nodes.push_back(v);
}

// ----------------------------------------------------------------------------


void GameLevel::removeGameNode(GameNodePtr v)
{
	 
	printf("\n removeGameNode: %ld", v->_id);
	// remove node
	auto it = std::remove_if(_nodes.begin(), _nodes.end(),
		[this, v](GameNodePtr g)
	{
		return g.get() == v.get();
	});
	_nodes.erase(it, _nodes.end());
}


// ----------------------------------------------------------------------------

GameNodePtr GameLevel::getGameNode(int id)
{
	for (GameNodePtr v : _nodes)
	{
		if (v->_id == id) return v;
	}
	return nullptr;
}



// ----------------------------------------------------------------------------



void GameLevel::update(const float currentTime, const float elapsedTime)
{
	for (GameNodePtr v : _nodes)
	{
		v->update(currentTime, elapsedTime);
	}

	trySelectNode();

}

// ----------------------------------------------------------------------------

void GameLevel::update_on_400ms(const float currentTime, const float elapsedTime)
{
	for (GameNodePtr v : _nodes)
	{
		v->update_on_400ms(currentTime, elapsedTime);
	}

}


// ----------------------------------------------------------------------------



void GameLevel::updatePhysic(const float currentTime, const float elapsedTime)
{
	for (GameNodePtr v : _nodes)
	{
		v->updatePhysic(currentTime, elapsedTime);
	}
}

// ----------------------------------------------------------------------------

void GameLevel::trySelectNode()
{
	if (!Unigine::Input::isMouseButtonDown(Unigine::Input::MOUSE_BUTTON_LEFT)) return;

	Unigine::PlayerPtr player = Unigine::Game::getPlayer();
	// initializing points of the ray from player's position in the direction pointed by the mouse cursor 
	Unigine::Math::ivec2 mouse = Unigine::Input::getMouseCoord();
	Unigine::Math::Vec3 p0 = player->getWorldPosition();
	Unigine::Math::Vec3 p1 = p0 + Unigine::Math::Vec3(player->getDirectionFromScreen(mouse.x, mouse.y)) * 10000;
	// memorize screen position of the click
	_last_mouse_click_coordinates = mouse;
	//int getScreenPosition(int & x, int & y, const Math::Vec3 & point, int width = -1, int height = -1)
	//creating a WorldIntersection object to store the information about the intersection
	Unigine::WorldIntersectionPtr intersection = Unigine::WorldIntersection::create();

	// casting a ray from p0 to p1 to find the first intersected object
	Unigine::ObjectPtr obj = Unigine::World::getIntersection(p0, p1, 1, intersection);


	// test if the location is on the terrain
	if (obj && !_strcmpi(obj->getName(), "ObjectLandscapeTerrain"))
	{
		if (GamePlay::Game->getMode() == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK)
		{
			Unigine::Math::vec3 terrain_pt = intersection->getPoint();
			// node should be aware of that click
			GamePlay::Game->mouseClick(enumGameZone::TERRAIN, terrain_pt);
		}

	}
	else
	{
		// is node selected ?
		if (obj && (!_strcmpi(obj->getName(), "SUBWORLD_NODE") || (obj->getRootNode() && !_strcmpi(obj->getRootNode()->getName(), "SUBWORLD_NODE"))))
		{
			GameNodePtr v = static_cast<GameNode*>(obj->getPtr());
			 

			// selected node i not the root node of the game object..
			// we need to find the root in hierarchy
			if (v == nullptr)
			{
				v = static_cast<GameNode*>(obj->getRootNode()->getPtr());
			}
			//if (!v) return;
			// select node if needed
			for (GameNodePtr gnode : _nodes)
			{
				if (gnode.get() != v.get())
					gnode->setSelected(false);
			}

			if (!v) return;
			v->setSelected(!v->getSelected());
			//Unigine::Math::Vec3 p = intersection->getPoint();
			//Unigine::Log::message("The first object intersected by the ray at point (%f, %f, %f) is: %s \n ", p.x, p.y, p.z, obj->getName());
		}
		else
		{
			// unselect node
		/*	for (GameNodePtr gnode : _nodes)
			{
				gnode->setSelected(false);
			}*/
		}
	}
}


// ----------------------------------------------------------------------------


void GameLevel::mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt)
{
	for (GameNodePtr v : _nodes)
	{
		v->mouseClick(zone, pt);
	}
}

// ----------------------------------------------------------------------------


void GameLevel::drawAnnotations(const float elapsedTime)
{
	for (GameNodePtr v : _nodes)
	{
		v->drawAnnotations(elapsedTime);
	}
}


// ----------------------------------------------------------------------------


std::vector<long> GameLevel::getBoudingSphereIntersection(const OpenSteer::Vec3& center, float radius)
{
	std::vector<long> nodes;
	for (GameNodePtr v : _nodes)
	{

		Unigine::Math::Vec3 d(Converter::toUnigine(center) - Converter::toUnigine(v->position()));
		float distance = d.length();
		if (distance < radius)
		{
			nodes.push_back(v->_id);
		}
	}
	return nodes;
}

// ----------------------------------------------------------------------------


void GameLevel::initProximityDatabase(void)
{
	const Vec3 center;
	const float div = 20.0f;
	const Vec3 divisions(div, 1.0f, div);
	const float diameter = 80; //XXX need better way to get this
	const Vec3 dimensions(diameter, diameter, diameter);
	typedef LQProximityDatabase<AbstractVehicle*> LQPDAV;
	_pd = new LQPDAV(center, dimensions, divisions);
}