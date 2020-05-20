 
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



#include "StrategicAI.h"
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

extern "C"
{
#include "../AI/CLIPS/clips.h"
}


// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;
#include "Reasoner.h"
 
// ----------------------------------------------------------------------------

REGISTER_COMPONENT(StrategicAI);

// ----------------------------------------------------------------------------


void StrategicAI::init()
{
	_reasoner = new Reasoner();
}

// ----------------------------------------------------------------------------


void StrategicAI::shutdown()
{
	delete _reasoner;
	_reasoner = nullptr;

}

// ----------------------------------------------------------------------------


void StrategicAI::update()
{
	
}


// ----------------------------------------------------------------------------


/*Que doit faire le strategicAI ?

 - Objectif Militaire ?
 - Zone ?
 - Situation Assessement ?

	Objectif : Protection Zone
	Zone : x,y,w,h
	Situation :
		Enemy location : base , subs, etc.

 SensorAI = create facts about situation : enemi, base, resources

			enemy : x,y,z , steering, speed, etc.
			resources : x,y,z

 strategicAI : 
		policy = Zone Protection


			Rule: si enemy detected et zone protection et unit dispo
				=> demande au TeamUnit de proteger la zone devant l'enemy
			'
				
				



*/

		

