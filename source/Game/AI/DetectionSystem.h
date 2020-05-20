 
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
 
#include "../Opensteer/include/OpenSteer/Vec3.h"
#include "../ReffCount.h"
#include "EnumNoiseLevel.h"
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <string>
#include <vector>
#include <algorithm>
 



namespace SubWorld
{
	class GameNode;
	 
	 


	// Detected target of a detection system
	struct DetectedTarget
	{

		DetectedTarget(const OpenSteer::Vec3& bearing, float distance, long game_id)
			: _bearing(bearing), _distance(distance), _game_id(game_id)
		{
		}

		OpenSteer::Vec3 _bearing;
		float _distance;
		long _game_id;
	};

	// A list of detected targets
	struct DetectionList
	{
		void push(DetectedTarget& target)
		{
			remove(target); // remove old target if needed
			_targets.push_back(target);
		}

		void remove(DetectedTarget& target)
		{
			auto it = std::remove_if(_targets.begin(), _targets.end(),
				[this, &target](DetectedTarget& g)
			{
				return g._game_id == target._game_id;
			});
			_targets.erase(it, _targets.end());
		}

		bool empty()
		{
			return _targets.empty();
		}

		std::vector <DetectedTarget> _targets;
	};

	

	// base class of a detection system 
	class DetectionSystem
	{
	public:
		virtual ~DetectionSystem() {}
		virtual void passiveDetection(RCPtr <GameNode> gamenode, DetectionList& friends, DetectionList& enemies) {};
		virtual void activeDetection(RCPtr <GameNode> gamenode, DetectionList& friends, DetectionList& enemies) {};
	};


	// Underwater detection system 
	class UnderwaterAcousticDetectionSystem : public DetectionSystem
	{
	public:
		UnderwaterAcousticDetectionSystem(float passive_detection_range, float active_detection_range);
		virtual ~UnderwaterAcousticDetectionSystem() {}

		virtual void passiveDetection(RCPtr <GameNode> gamenode, DetectionList& friends, DetectionList& enemies);
		virtual void activeDetection(RCPtr <GameNode> gamenode, DetectionList& friends, DetectionList& enemies);
	protected:
		enumNoiseLevel _noise_level;
		float _passive_detection_range;
		float _active_detection_range;

	};




}



