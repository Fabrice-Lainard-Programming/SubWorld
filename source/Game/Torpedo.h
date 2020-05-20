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


#include "GameNode.h"



namespace SubWorld
{
	class GameLevel;

	// MKI torpedo
	class Torpedo : public GameNode
	{
	public:
		Torpedo(GameLevel* level, int id);
		virtual ~Torpedo();
		// per frame simulation update (not physic related stuff)
		virtual void update(const float currentTime, const float elapsedTime);
		// sets the wanderer for pursuit
		void setWanderer(GameNodePtr wander) { _wanderer = wander; }
	protected:
		void check_target(const float elapsedTime);
	protected:
		virtual void createNode();
		GameNodePtr _wanderer;
		bool _wanderer_attached;
		bool _exploded;
		float _impact_radius;
		float _impact_power;
		 

	};

	typedef RCPtr<Torpedo>  TorpedoPtr;

}

