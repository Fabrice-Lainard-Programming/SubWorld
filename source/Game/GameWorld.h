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
 
#pragma warning( push )
#pragma warning( disable: 4100 )

#include <vector>
#include "Opensteer/include/OpenSteer/Clock.h"
#include <chrono>

namespace SubWorld
{
	class GamePlay;

	// helpers used to delete object properly
	#define safe_delete(ptr) if(ptr) { delete ptr; ptr = nullptr; }
	// translation helpers
	#define _tr(txt) (txt)

	// The world which is a game container
	class GameWorld
	{
	public:
		GameWorld();
		
		// start the game
		void start();
		// shutdown the game
		void close();
		// update objects
		void update() ;
		// update physic body
		void updatePhysic();
		// draw annotations
		void drawAnnotations(const float elapsedTime);

	public:
		// game play logic
		GamePlay* _gameplay;
		//keeps track of both "real time" and "simulation time"
		OpenSteer::Clock _clock;
		// used to emit signal each seconds
		std::chrono::system_clock::time_point _dt_sec;
	
	};

}