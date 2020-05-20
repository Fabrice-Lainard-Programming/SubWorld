
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



#include "DetectionSystem.h"
#include "../GameLevel.h"
#include "../GameWorld.h"
#include "../GamePlay.h"
#include "../GameNode.h"
#include "../PathFinder.h"
#include "../Converter.h"
#include <UnigineApp.h>
#include <UnigineWorld.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <list>

// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;
 

// ----------------------------------------------------------------------------




UnderwaterAcousticDetectionSystem::UnderwaterAcousticDetectionSystem(float passive_detection_range, float active_detection_range)
	: _noise_level(NoiseLevelLow), _passive_detection_range(passive_detection_range), _active_detection_range(active_detection_range)
{

}


void UnderwaterAcousticDetectionSystem::passiveDetection(RCPtr <GameNode> gamenode, DetectionList& friends, DetectionList& enemies)
{
	// check the intersection with nodes based on radar performance
	Unigine::VectorStack<Unigine::NodePtr> nodes;
	std::vector<long> nodesIds = GamePlay::Game->getCurrentevel()->getBoudingSphereIntersection(gamenode->position(), _passive_detection_range / 2);
	if (nodesIds.size() > 1) // (because the current node is also in this list)
	{
		for (int i = 0; i < nodesIds.size(); i++)
		{
			long nodeID = nodesIds[i];

			// verify the given node
			GameNodePtr threat = GamePlay::Game->getCurrentevel()->getGameNode(nodeID);
			// add to threat list if needed
			if (threat && (threat->_id != gamenode->_id))
			{
				OpenSteer::Vec3 bearing = (threat->position() - gamenode->position()).normalize();
				DetectedTarget dt(bearing, gamenode->nodeDistance(threat), threat->_id);
				if (GamePlay::Game->isEnemy(gamenode->getFaction(), threat->getFaction()))
				{
					enemies.push(dt);
				}
				else
				{
					friends.push(dt);
				}

			}
		}
	}
}




void UnderwaterAcousticDetectionSystem::activeDetection(RCPtr <GameNode> gamenode, DetectionList& friends, DetectionList& enemies)
{
	_noise_level = NoiseLevelHigh;
	passiveDetection(gamenode, friends, enemies);
	_noise_level = NoiseLevelLow;
}
