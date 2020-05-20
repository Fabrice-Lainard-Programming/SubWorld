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


#include "GameNode.h"
#include "GameLevel.h"
#include "GameWorld.h"
#include "GamePlay.h"
#include "Opensteer/include/OpenSteer/Draw.h"
#include "Opensteer/include/OpenSteer/Vec3.h"
#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UniginePhysics.h>
#include <UnigineSounds.h>
#include <UnigineVisualizer.h>
#include <UniginePlayers.h>
#include <UnigineObjects.h>
#include <UnigineApp.h>
#include <sstream>
#include <iomanip>

#include "AutoActionTimer.h"
#include "Converter.h"
#include "NodeUI.h"
#include "BattleUnit.h"
#include "SteeringBehaviors.h"
#include "WeaponControlSystem.h"
#include "BattleUnitUI.h"

// ----------------------------------------------------------------------------

using namespace SubWorld;
using namespace OpenSteer;

// ----------------------------------------------------------------------------

int GameNode::current_vehicule_unique_id = 0;
bool GameNode::gWanderSwitch = false;
bool GameNode::gUseDirectedPathFollowing = true;

// ----------------------------------------------------------------------------

GameNode::GameNode(GameLevel* level, int id, const std::string& model)
	: _level(level), _id(id), _path(nullptr), _node_model(model), _isSelected(false), _selectionStateChanged(false),_deleting(false),
	_faction(::enumFaction::RED_FACTION), _visible(true),_smoothedDirectionFactor(30.0f), _useSmoothedDirectionAccumulator(true),
	_noiseLevel(enumNoiseLevel::NoiseLevelNormal)

{
	 	// allocate a token for this boid in the proximity database
	_proximityToken = level->_pd->allocateToken(this);

}

// ----------------------------------------------------------------------------

GameNode::~GameNode()
{

	_deleting = true;



	printf("\n DELETE node : %ld", _id);

	BattleUnit* bu = ComponentSystem::get()->getComponent<BattleUnit>(_node);
	if (bu)
	{
		bu->shutdown();
	}

	_dummyBody = nullptr;
	_body = nullptr;
	if (_node)
	{
		_node->setPtr(nullptr);
		_node. deleteLater();
	}
	safe_delete(_proximityToken);
	safe_delete(_path);
	_template = nullptr;
	_level = nullptr;

}

// ----------------------------------------------------------------------------



void GameNode::init(void)
{
	SimpleVehicle::reset(); // reset the vehicle 
	_smoothedDirection = Vec3::zero;

	setSpeed(0);         // speed along Forward direction.
	setMaxForce(0.3f);      // steering force is clipped to this magnitude
	setMaxSpeed(5);         // velocity is clipped to this magnitude
	clearTrailHistory();    // prevent long streaks due to teleportation 
	createNode(); // create unigine node
	buildUI(); // create user interface associated with this node

}



// ----------------------------------------------------------------------------


void GameNode::update(const float currentTime, const float elapsedTime)
{
 
	_camera_distance = cameraDistance();
	applySteeringForce(determineCombinedSteering(elapsedTime), elapsedTime);
	updateNode(currentTime, elapsedTime);
	
	// update WCS alignement if needed
	WeaponControlSystem* wcs = ComponentSystem::get()->getComponent<WeaponControlSystem>(_node);
	if (wcs)
	{
		wcs->align_weapons(elapsedTime);
	}
}

// ----------------------------------------------------------------------------




void GameNode::update_on_400ms(const float currentTime, const float elapsedTime)
{
	 
	NodeUI* ui = ComponentSystem::get()->getComponent<NodeUI>(_node);
	if (ui)
	{
		// refresh activated UI to reflect changes in real-time
		ui->refresh_ui();
	}
	// refresh battle unit informations if needed
	BattleUnit* bu = ComponentSystem::get()->getComponent<BattleUnit>(_node);
	if (bu)
	{
		bu->updateUnitStatus(elapsedTime);
	}
	WeaponControlSystem* wcs = ComponentSystem::get()->getComponent<WeaponControlSystem>(_node);
	if (wcs)
	{
		wcs->compute_weapons(elapsedTime);
	}
 
}


// ----------------------------------------------------------------------------

void GameNode::updatePhysic(const float currentTime, const float elapsedTime)
{
	 
	updateBody(currentTime, elapsedTime);
}

// ----------------------------------------------------------------------------


void GameNode::drawAnnotations(const float elapsedTime)
{
	 
	BattleUnit* bu = ComponentSystem::get()->getComponent<BattleUnit>(_node);
	if (bu)
	{
		bu->drawAnnotations(elapsedTime);
	}

	/*if (_camera_distance < _annotation_lod)
	{
		//	annotationVelocityAcceleration();
		recordTrailVertex(elapsedTime, position());
		//	drawBasic2dCircularVehicle(*this, gGray50);
		drawTrail();

		annotatePath();
	}
	if (_camera_distance < _annotation_lod2)
	{
		annotateLabels();
	}
	annotateSelected();*/

}

// ----------------------------------------------------------------------------

void GameNode::createNode()
{
	if (_node)
	{
		// update steer radius
		Unigine::BoundSphere sphere = _node->getHierarchyWorldBoundSphere();
		float radius = sphere.getRadius();
		setRadius(radius);

		return; // already created
	}
	if (_node_model.empty())
	{
		printf("\n GameNode::createNode: empty model");
		return;
	}
	_node = Unigine::World::loadNode(_node_model.c_str(), 0);
	if (!_node)
	{
		printf("\n GameNode::createNode: loadNode failed");
		return;
	}
	_node->setName("SUBWORLD_NODE");
	_node->setID(_id);
	//_node->rotate(Unigine::Math::vec3(0, 0, 0));
	_node->setPosition(Converter::toUnigine(position()));
	_node->setPtr(this);
	// update steer radius
	Unigine::BoundSphere sphere = _node->getHierarchyWorldBoundSphere();
	float radius = sphere.getRadius();
	setRadius(radius);
	
	// gets physical body
	_dummyBody = _node->findNode("body", 1);
	if (!_dummyBody) return;
	_body = _dummyBody->getObjectBodyRigid();
	if (!_body)
	{
		printf("\nWarning: GameNode::createNode: this is not a physical node !");
	}
	else
	{
		 
		_body->addContactCallback(MakeCallback(this, &GameNode::contact_callback));
	}
	_dummyBody->setPtr(this);
	_dummyBody->setName("SUBWORLD_NODE");

	//ComponentSystem::get()->addComponent<SteeringComponent>(node);

}

// ----------------------------------------------------------------------------

void GameNode::contact_callback(Unigine::BodyPtr body, int num)
{
	 
}


// ----------------------------------------------------------------------------

void GameNode::buildUI()
{

}

// ----------------------------------------------------------------------------


void GameNode::updateNode(const float currentTime, const float elapsedTime)
{
	if (!_node) return;
	if (!_body) return;

	if (_selectionStateChanged)
	{
		_selectionStateChanged = false;
		drawSelectedState();
	}
}


// ----------------------------------------------------------------------------



void GameNode::updateBody(const float currentTime, const float elapsedTime)
{
	if (!_node) return;

	if (_useSmoothedDirectionAccumulator)
		// running average of recent direction
	{
		OpenSteer::blendIntoAccumulator(elapsedTime * _smoothedDirectionFactor, // QQQ
			forward(),
			_smoothedDirection);
	}

	Unigine::Math::vec3 pos = Converter::toUnigine(position());


	/*if (!_body)
	{
		// if there is no body, we move and rotate manually
		_transform = Unigine::Math::mat4(Unigine::Math::rotateZ(_rotation.x) * Unigine::Math::rotateZ(_rotation.y) * Unigine::Math::rotateZ(_rotation.z));
		_node->setWorldRotation(_transform.getRotate());
		_node->setWorldPosition(pos);
		 
	}
	else*/
	{
		Unigine::Math::vec3 dir = Converter::toUnigine(_useSmoothedDirectionAccumulator ? _smoothedDirection : forward());
		_transform = Unigine::Math::setTo(pos, pos + (dir), Unigine::Math::vec3_up, Unigine::Math::AXIS_Y);
		_node->setWorldRotation(_transform.getRotate());
		_node->setWorldPosition(pos);
		if (_body)
			_body->setVelocityTransform(_transform);
		 
	}
}


// ----------------------------------------------------------------------------

Vec3 GameNode::determineCombinedSteering(const float elapsedTime)
{
	SteeringBehaviors* steering = ComponentSystem::get()->getComponent<SteeringBehaviors>(_node);
	if (steering)
	{
		 Vec3 steeringForce = steering->determineCombinedSteering(elapsedTime);
		return steeringForce.setYtoZero();
	}
	return Vec3(0, 0, 0);
}

// ----------------------------------------------------------------------------

void GameNode::setPath(PolylinePathway* path)
{
	safe_delete(_path);
	_path = path;
}


// ----------------------------------------------------------------------------


void GameNode::setSelected(bool isSelected)
{
	_isSelected = isSelected;
	_selectionStateChanged = true;

	NodeUI* ui = ComponentSystem::get()->getComponent<NodeUI>(_node);

	if (ui && isSelected)
	{
		ui->show_ui();
	}
}
// ----------------------------------------------------------------------------


void GameNode::setVisibility(bool visible) 
{
	_node->setEnabled(visible);
	_visible = visible;
}
 

// ----------------------------------------------------------------------------

void GameNode::mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt)
{

}


// ----------------------------------------------------------------------------

//post_filter_selection

void GameNode::drawSelectedState()
{
	if (!_dummyBody) return;
	Unigine::ObjectMeshStaticPtr mesh = Unigine::static_ptr_cast<Unigine::ObjectMeshStatic>(_dummyBody);
	if (!mesh) return;
	/*
https://developer.unigine.com/en/docs/2.11/code/uusl/create_post?words=post%2Cselection%2Cselect#highlight
	if (!_isSelected)
		{
			mesh->setSurfaceProperty("surface_base", "*");
			mesh->setMaterialState("auxiliary", 0, 0);
			mesh->setMaterialParameterFloat4("auxiliary_color", Unigine::Math::vec4_one, 0);

		}
		else
		{
			mesh->setSurfaceProperty("surface_base", "*");
			mesh->setMaterialState("auxiliary", 1, 0);
			mesh->setMaterialParameterFloat4("auxiliary_color", Unigine::Math::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0);
		}*/

}
// ----------------------------------------------------------------------------


void GameNode::setWorldPosition(const OpenSteer::Vec3& position)
{
	setPosition(position);
	// notify proximity database that our position has changed
	_proximityToken->updateForNewPosition(position);

}


// ----------------------------------------------------------------------------


void GameNode::setWorldRotation(const OpenSteer::Vec3& rotation)
{
	_rotation = rotation;
}


// ----------------------------------------------------------------------------

Unigine::SoundSourcePtr GameNode::getSound(const std::string& sound)
{
	int idx = _node->findChild(sound.c_str());
	if (idx >= 0)
	{
		Unigine::SoundSourcePtr s = Unigine::checked_ptr_cast<Unigine::SoundSource>(_node->getChild(idx));
		return s;
	}
	return nullptr;
}


// ----------------------------------------------------------------------------



Unigine::SoundSourcePtr GameNode::addSound(const std::string& sound, long autoDeleteInMs)
{
	Unigine::SoundSourcePtr s = getSound(sound);
	if (s) return s;
	s = Unigine::SoundSource::create(sound.c_str());
	// specify necessary settings for the sound source
	s->setOcclusion(0);
	s->setMinDistance(0);
	s->setMaxDistance(240.0f);
	s->setGain(0.5f);
	s->setWorldTransform(_transform);
	s->setName(sound.c_str());

	s->setLoop(100);
	_node->addChild(s);

	if (autoDeleteInMs > 0)
	{
		new AutoActionTimer<Unigine::SoundSourcePtr>(s, autoDeleteInMs,
			[this](AutoActionTimer<Unigine::SoundSourcePtr>& update_delegate, const float currentTime, const float elapsedTime)
		{
			update_delegate._ptr->setWorldTransform(this->_transform);
		},
			[this](AutoActionTimer<Unigine::SoundSourcePtr>& delete_delegate)
		{
			delete_delegate._ptr->deleteLater();
		}
		);
	}
	 

	return s;
}

// ----------------------------------------------------------------------------

float GameNode::cameraDistance(void)
{
	Unigine::PlayerPtr player = Unigine::Game::getPlayer();
	return(player->getWorldPosition() - Converter::toUnigine(position())).length();
}


// ----------------------------------------------------------------------------


float GameNode::nodeDistance(RCPtr < GameNode> gamenode)
{
	 	Unigine::Math::Vec3 d(Converter::toUnigine(gamenode->position()) - Converter::toUnigine(position()));
		return d.length();
}

// ----------------------------------------------------------------------------
// Annotations
// ----------------------------------------------------------------------------



// ----------------------------------------------------------------------------

void GameNode::annotateNodeUILink()
{
	if (GamePlay::Game->getMode() == enumCurrentGameMode::ACQUIRE_MOUSE_CLICK) return; 
	if (_node)
	{
		NodeUI* ui = ComponentSystem::get()->getComponent<NodeUI>(_node);
		if (ui)
		{
			if (ui->_uiVisible && ui->_window)
			{
				show3DTo2DLine(ui->_window->getScreenPositionX() + ui->_window->getWidth() / 2, ui->_window->getScreenPositionY() + ui->_window->getHeight() *0.3f, GamePlay::Game->_selectionLightColor);
			}
		}
	}
}


// ----------------------------------------------------------------------------


void GameNode::annotateLabels(void)
{


	// display speed near agent's screen position
	const OpenSteer::Vec3 textColor(0.8f, 0.8f, 1.0f);
	const  OpenSteer::Vec3 textOffset(0, 0, 0.50f);
	const  OpenSteer::Vec3 pos(position());// .x, position().y, position().z);

	OpenSteer::Vec3 textPosition = pos;

	if (_camera_distance < _annotation_lod)
	{
		std::ostringstream annote;
		annote
			<< "["
			<< std::setprecision(2)
			<< std::setiosflags(std::ios::fixed)
			<< pos.x << "," << pos.y << "," << pos.z << "] " << speed() << " kts" << std::ends;
		OpenSteer::draw2dTextAt3dLocation(annote, textPosition, textColor);
	}

}



void GameNode::annotateSelected(void)
{

	const OpenSteer::Vec3 textColor(Converter::toOpenSteerColor(GamePlay::Game->_selectionColor));
	const  OpenSteer::Vec3 textOffset(0, 0.30f, 0);
	const  OpenSteer::Vec3 pos(position().x, position().y, position().z);

	OpenSteer::Vec3 textPosition = pos;
	std::ostringstream annote;
	textPosition = pos + textOffset * 3;
	std::ostringstream annote2;
	annote2 << std::setprecision(2) << std::setiosflags(std::ios::fixed);
	annote2 << _template->_user_name << std::ends;
	OpenSteer::draw2dTextAt3dLocation(annote2, textPosition, textColor);

}


// ----------------------------------------------------------------------------

void GameNode::annotatePath(void)
{
	if (_path)
	{
		// draw a line along each segment of path
		for (int i = 0; i < _path->pointCount; i++)
		{
			if (i > 0)
			{

				Unigine::Math::Vec3 va(Converter::toUnigine(_path->points[i]));
				Unigine::Math::Vec3 vb(Converter::toUnigine(_path->points[i - 1]));

				Unigine::Math::Vec4 vcolor(1.0, 0.5, 0, 1.0);
				Unigine::Math::Vec4 vcolor2(0.0, 0.5, 1, 1.0);
				//	printf("\n line: %f,%f,%f  ->   %f,%f,%f", va.x, va.y, va.z,  vb.x, vb.y, vb.z);
				Unigine::Visualizer::renderLine3D(va, vb, i%2 ? vcolor: vcolor2);

			}
		}
	}

}


// ----------------------------------------------------------------------------



// called when steerToFollowPath decides steering is required
void GameNode::annotatePathFollowing(const Vec3& future, const Vec3& onPath, const Vec3& target, const float outside)
{
	const Vec3 yellow(1, 1, 0);
	const Vec3 lightOrange(1.0f, 0.5f, 0.0f);
	const Vec3 darkOrange(0.6f, 0.3f, 0.0f);
	const Vec3 yellowOrange(1.0f, 0.75f, 0.0f);

	// draw line from our position to our predicted future position
	annotationLine(position(), future, yellow);

	// draw line from our position to our steering target on the path
	annotationLine(position(), target, yellowOrange);

	// draw a two-toned line between the future test point and its
	// projection onto the path, the change from dark to light color
	// indicates the boundary of the tube.
	const Vec3 boundaryOffset = (onPath - future).normalize() * outside;
	const Vec3 onPathBoundary = future + boundaryOffset;
	annotationLine(onPath, onPathBoundary, darkOrange);
	annotationLine(onPathBoundary, future, lightOrange);
}

// ----------------------------------------------------------------------------



// called when steerToAvoidCloseNeighbors decides steering is required
   // (parameter names commented out to prevent compiler warning from "-W")
void GameNode::annotateAvoidCloseNeighbor(const AbstractVehicle& other,
	const float /*additionalDistance*/)
{
	// draw the word "Ouch!" above colliding vehicles
	const float headOn = forward().dot(other.forward()) < 0;
	const Vec3 green(0.4f, 0.8f, 0.1f);
	const Vec3 red(1, 0.1f, 0);
	const Vec3 color = headOn ? red : green;
	const char* string = headOn ? "OUCH!" : "pardon me";
	const Vec3 location = position() + Vec3(0, 0.5f, 0);

	draw2dTextAt3dLocation(*string, location, color);
}

// ----------------------------------------------------------------------------


// (parameter names commented out to prevent compiler warning from "-W")
void GameNode::annotateAvoidNeighbor(const AbstractVehicle& threat,
	const float /*steer*/,
	const Vec3& ourFuture,
	const Vec3& threatFuture)
{
	const Vec3 green(0.15f, 0.6f, 0.0f);

	annotationLine(position(), ourFuture, green);
	annotationLine(threat.position(), threatFuture, green);
	annotationLine(ourFuture, threatFuture, gRed);
	annotationXZCircle(radius(), ourFuture, green, 12);
	annotationXZCircle(radius(), threatFuture, green, 12);
}

// ----------------------------------------------------------------------------


// xxx perhaps this should be a call to a general purpose annotation for
// xxx "local xxx axis aligned box in XZ plane" -- same code in in
// xxx CaptureTheFlag.cpp
void GameNode::annotateAvoidObstacle(const float minDistanceToCollision)
{
	const Vec3 boxSide = side() * radius();
	const Vec3 boxFront = forward() * minDistanceToCollision;
	const Vec3 FR = position() + boxFront - boxSide;
	const Vec3 FL = position() + boxFront + boxSide;
	const Vec3 BR = position() - boxSide;
	const Vec3 BL = position() + boxSide;
	const Vec3 white(1, 1, 1);
	annotationLine(FR, FL, white);
	annotationLine(FL, BL, white);
	annotationLine(BL, BR, white);
	annotationLine(BR, FR, white);
}

// ----------------------------------------------------------------------------

void GameNode::show3DTo2DLine(int x, int y, const Unigine::Math::Vec4& vcolor)
{
	Unigine::PlayerPtr player = Unigine::Game::getPlayer();
	Unigine::Math::Vec3 p0, p1;
	Unigine::Math::Vec3 wpos = Converter::toUnigine(position());
	player->getDirectionFromScreen(p0, p1, x - 50, y);
	//	Unigine::Visualizer::MODE mode = Unigine::Visualizer::getMode();

	Unigine::Visualizer::renderLine3D(p1, wpos, vcolor);


	//Unigine::Visualizer::renderLine2D(Unigine::Math::Vec3(x/Unigine::App::getWidth() -50,y/ Unigine::App::getHeight(),0), Unigine::Math::Vec3(x / Unigine::App::getWidth() , y / Unigine::App::getHeight(),0),  vcolor);

//	Unigine::Visualizer::setMode(mode);

}