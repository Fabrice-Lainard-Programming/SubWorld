
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



#include "ShieldEffect.h"
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

REGISTER_COMPONENT(ShieldEffect);



// ----------------------------------------------------------------------------


void ShieldEffect::shutdown()
{


}

// ----------------------------------------------------------------------------


void ShieldEffect::update()
{
	GameNodePtr gamenode = getGameNode();
	float time = Game::getTime();
	if (_mesh)
	{
		vec4 pos = Converter::toUnigine(gamenode->position());
		_mesh->setWorldTransform(Mat4(translate(pos) * rotateX(time * 8.0f) * rotateY(time * 12.0f) * rotateZ(time * 16.0f)));
	}
}


// ----------------------------------------------------------------------------


void ShieldEffect::init()
{
	_mesh = ObjectMeshDynamic::create();
	_mesh->setMaterial("mesh_sonar", "*");
	_mesh->setSurfaceProperty("surface_base", "*");

	for (int i = 0; i < 360; ++i) {
		float angle = 180.0f * i / 10;
		create_circle(_mesh, rotateY(90.0f) * rotateX(angle), 1024, 4.0f, 4.0f, 0.3f, GamePlay::Game->_selectionLightColor);
	}
	_mesh->updateBounds();
}

// ----------------------------------------------------------------------------


void ShieldEffect::create_circle(ObjectMeshDynamicPtr mesh, mat4 transform, int num_vertex, float radius, float size, float period, Vec4 color)
{

	int num = 0;
	int enable = 1;
	int offset = 0;
	float distance = 0.0f;

	vec3 previous = transform * vec3(radius, 0.0f, 0.0f);
	for (int i = 0; i < num_vertex; ++i) {

		float angle = 360.0f * i / (num_vertex - 1);
		vec3 vertex = transform * rotateZ(angle) * vec3(radius, 0.0f, 0.0f);

		distance += length(previous - vertex);
		if (distance > period) {
			enable = !enable;
			distance = 0.0f;
		}

		if (enable) {
			if (num++ == 0) offset = _mesh->getNumVertex();
			_mesh->addVertex(vertex);
			_mesh->addTangent(quat(vec4(previous, size)));
			_mesh->addColor(color);
		}
		else if (num) {
			for (int j = offset; j < offset + num - 2; ++j) {
				_mesh->addIndex(j + 0);
				_mesh->addIndex(j + 1);
				_mesh->addIndex(j + 2);
			}
			offset += num;
			num = 0;
		}

		previous = vertex;
	}

	if (num) {
		for (int i = offset; i < offset + num - 2; ++i) {
			_mesh->addIndex(i + 0);
			_mesh->addIndex(i + 1);
			_mesh->addIndex(i + 2);
		}
	}
}


// ----------------------------------------------------------------------------

GameNodePtr ShieldEffect::getGameNode()
{
	return static_cast<GameNode*>(node->getPtr());
}
