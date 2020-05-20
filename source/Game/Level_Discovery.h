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
#include "GameLevel.h"

namespace SubWorld
{
	 
	// The first level in game
	class LevelDiscovery : public  GameLevel
	{
	public:
		LevelDiscovery(GamePlay* rcs);	
		virtual ~LevelDiscovery();

		// called when a level should load its content
		virtual void loadLevel();
		// called when a level should unload its content
		virtual void closeLevel();
		// update objects
		virtual void update(const float currentTime, const float elapsedTime);
		// draw annotation
		virtual void drawAnnotations(const float elapsedTime);


		void initDemo();
		void updateDemo(const float currentTime, const float elapsedTime);

	public:
		// public accessors (get and set) 
	 	 
	};

}