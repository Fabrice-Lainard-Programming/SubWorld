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



#include "SonarEffect.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "GameNode.h"
#include "PathFinder.h"
#include "Converter.h"
#include <UnigineApp.h>
#include <UnigineWorld.h>
#include <UnigineGame.h>
#include <UnigineVisualizer.h>
#include <UniginePrimitives.h>
#include <list>

// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace SubWorld;


// ----------------------------------------------------------------------------

REGISTER_COMPONENT(SonarEffect);

// DOC Shader : https://developer.unigine.com/en/docs/2.11/code/formats/materials_formats/base_material_format?rlang=cpp
//https://developer.unigine.com/en/docs/2.7.3/content/materials/library/postprocess/post_sensor/?rlang=cpp
// ----------------------------------------------------------------------------


void SonarEffect::shutdown()
{


}
// ----------------------------------------------------------------------------


void SonarEffect::init()
{

	_interval = 0.0f;
	_radius = 0;
	_propagation_radius = 0;
 
}

// ----------------------------------------------------------------------------



void SonarEffect::ping(float radius)
{
	_interval = 0.0f;
	_radius = radius;
	_propagation_radius = 0;
	
	//Unigine::Render::setPostMaterials("post_sonar_filter");
}


// ----------------------------------------------------------------------------

 
void SonarEffect::update()
{
	if (_radius < 1) return;
	float time = Game::getIFps();
	_interval -= time;
	if (_interval > 0.0f)
		return;
	_interval += 0.25f;

	GameNodePtr gamenode = getGameNode();
	if (!gamenode) return;
	
	
	if (_propagation_radius > _radius)
	{
		_propagation_radius = 0;
		_radius = 0;
		for (GameNodePtr v : GamePlay::Game->getCurrentevel()->getNodes())
		{
			setAuxiliaryRecursive(v->_node, false);
		}
	}

	for (GameNodePtr v : GamePlay::Game->getCurrentevel()->getNodes())
	{
		float d = gamenode->nodeDistance(v);
		if (v->_node && ( d > _propagation_radius-10 && d < _propagation_radius+10))
		{
			setAuxiliaryRecursive(v->_node, true);
		 }
		else
		{
			setAuxiliaryRecursive(v->_node, false);
		}
	}
	_propagation_radius += 20;
}


// ----------------------------------------------------------------------------

void SonarEffect::setAuxiliaryRecursive(Unigine::NodePtr n,bool resetAux)
{
	Unigine::ObjectMeshStaticPtr mesh = Unigine::dynamic_ptr_cast<Unigine::ObjectMeshStatic>(n);
 	if (mesh)
	{
		if (!resetAux)
		{
			  
			for (int i = 0; i < mesh->getNumSurfaces(); ++i)
			{
				Ptr<Material> material = mesh->getMaterial(i);
				//if (!material->getState("auxiliary")) continue;
				mesh->setMaterialState("auxiliary", 0, i);
				mesh->setMaterialParameterFloat4("auxiliary_color", vec4_one, i);
			}
		}
		else
		{
			for (int i = 0; i < mesh->getNumSurfaces(); ++i)
			{
				Ptr<Material> material = mesh->getMaterial(i);
				//if (!material->getState("auxiliary")) continue;
			 	mesh->setMaterialState("auxiliary", 1, 0);
				mesh->setMaterialParameterFloat4("auxiliary_color", vec4(1.0f, 0.0f, 1.0f, 0.0f),i);
			}
		}
	}

	for (int i = 0; i < n->getNumChildren(); ++i)
		setAuxiliaryRecursive(n->getChild(i),resetAux);
}

// ----------------------------------------------------------------------------

GameNodePtr SonarEffect::getGameNode()
{
	return static_cast<GameNode*>(node->getPtr());
}
