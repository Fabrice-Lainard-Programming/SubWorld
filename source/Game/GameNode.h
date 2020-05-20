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

#include "Opensteer/include/OpenSteer/SimpleVehicle.h"
#include "Opensteer/include/OpenSteer/Pathway.h"
#include "Opensteer/include/OpenSteer/Proximity.h"
#include "UnigineNode.h"
#include "UniginePhysics.h"
#include "UnigineMathLib.h"
#include <UnigineSounds.h>
#include <memory>
#include "ReffCount.h"
#include "GamePlay.h"
#include "AI/EnumNoiseLevel.h"
 

namespace SubWorld
{
	class GameLevel;
	struct GameAssetTemplate;


	#define _validGameNode(node) (node && !node->_deleting)


	// Proximimity data base
	typedef OpenSteer::AbstractProximityDatabase<OpenSteer::AbstractVehicle*> ProximityDatabase;
	typedef OpenSteer::AbstractTokenForProximityDatabase<OpenSteer::AbstractVehicle*> ProximityToken;

 


	// A moveable entity in game
	class GameNode : public OpenSteer::SimpleVehicle, public RCBase
	{
	public:
		GameNode(GameLevel* level, int id,const std::string& model);
		
		 
		// init the node
		void init(void);
		// per frame simulation update (not physic related stuff)
		virtual void update(const float currentTime, const float elapsedTime);
		//  callback fo update called each seconds
		virtual void update_on_400ms(const float currentTime, const float elapsedTime);
		// per frame physical body simulation update
		virtual void updatePhysic(const float currentTime, const float elapsedTime);
		// draw this character/vehicle into the scene
		virtual void drawAnnotations(const float elapsedTime);
		// called when unigine nodes should be created 
		virtual void createNode();
		// called on mouse click over terrain, node, ...
		virtual void mouseClick(enumGameZone zone, const Unigine::Math::vec3& pt);
		// a callback function that will be run when a contact with a given body emerges
		virtual void contact_callback(Unigine::BodyPtr body, int num);

		// change world position and update proximity database
		void setWorldPosition(const OpenSteer::Vec3& position);
		// change world rotation
		void setWorldRotation(const OpenSteer::Vec3& rotation);
		// retrieve the current path to follow
		OpenSteer::PolylinePathway* getPath() { return _path; }
		// change the current path to follow
		void setPath(OpenSteer::PolylinePathway* path);
		// gets the selected flags
		bool getSelected() const { return _isSelected; }
		// sets the selected flags
		virtual void setSelected(bool isSelected);
		// return the constant steering force
		OpenSteer::Vec3 getConstantSteeringForce(void) const { return _steeringForce; }
		// sets the constant steering force
		void setConstantSteeringForce(OpenSteer::Vec3 sf) { _steeringForce = sf; }
		// add a sound localized on the node
		Unigine::SoundSourcePtr addSound(const std::string& sound, long autoDeleteInMs=0);
		// gets a sound
		Unigine::SoundSourcePtr getSound(const std::string& sound);
		// distance of that node to the camera
		float cameraDistance(void);
		// distance of that node to other
		float nodeDistance(RCPtr < GameNode> gamenode);
		// gets the game play group of that node
		enumFaction getFaction() const { return _faction; }
		// sets the game play group of that node
		void setFaction(enumFaction faction) { _faction = faction; }
		// returns the associated level
		GameLevel* getLevel() { return _level; }
		// returns visiblility state
		bool isVisible() const { return _visible;	}
		// sets visiblility state
		void setVisibility(bool visible);
		// distance of that node to the camera
		float getCameraDistance() const { return _camera_distance; }
	protected:
		virtual ~GameNode();
		virtual void buildUI();
		void updateNode(const float currentTime, const float elapsedTime);
		void updateBody(const float currentTime, const float elapsedTime);
		virtual OpenSteer::Vec3 determineCombinedSteering(const float elapsedTime);
		void drawSelectedState();
	protected:
		void annotateNodeUILink();
		void annotateLabels(void);
		void annotateSelected(void);
		void annotatePath(void);
		virtual void annotatePathFollowing(const OpenSteer::Vec3& future, const OpenSteer::Vec3& onPath, const OpenSteer::Vec3& target, const float outside);
		void annotateAvoidCloseNeighbor(const  OpenSteer::AbstractVehicle& other, const float /*additionalDistance*/);
		void annotateAvoidNeighbor(const  OpenSteer::AbstractVehicle& threat, const float /*steer*/, const  OpenSteer::Vec3& ourFuture, const  OpenSteer::Vec3& threatFuture);
		void annotateAvoidObstacle(const float minDistanceToCollision);
		void show3DTo2DLine(int x, int y, const Unigine::Math::Vec4& vcolor);
	public:
		// id's generation 
		static int current_vehicule_unique_id;
		static bool gWanderSwitch;
		static bool gUseDirectedPathFollowing;
		// public accessors  
		GameLevel* _level;
		// unique identifier of this vehicule
		int _id;
		// a pointer to this interface object for the proximity database
		ProximityToken* _proximityToken;
		// unigine node  
		Unigine::NodePtr _node;	
		// unigine node type
		std::string _node_model;
		// model of the node
		GameAssetTemplate* _template;
		// actual transform in unigine coordinates
		Unigine::Math::mat4 _transform;
		// true if this node is about to be deleted
		bool _deleting;
		// coef of he smoothed direction accumulator
		float _smoothedDirectionFactor;
		// if we do not use accumulator the direction change abruptly
		bool _useSmoothedDirectionAccumulator;
		// level of noise emitted by this node
		enumNoiseLevel _noiseLevel;
	protected:
		// dummy node which contains the BodyRigid object
		Unigine::NodePtr _dummyBody;
		// unigine physical body  
		Unigine::BodyRigidPtr _body;
		// path to follow
		OpenSteer::PolylinePathway* _path;
		// is it selected ?
		bool _isSelected;
		// sets when the selected state is about to be changed
		bool _selectionStateChanged;
		// smooth direction along the forward vector
		OpenSteer::Vec3 _smoothedDirection;
		// constant steering force
		OpenSteer::Vec3 _steeringForce;
		// rotation of the node (used unikely by unigine node)
		OpenSteer::Vec3 _rotation;
		// distance of that node to the camera
		float _camera_distance;
		// distance at which annotations are visible
		const float _annotation_lod = 150;
		// distance at which annotations are visible
		const float _annotation_lod2 = 60;
		// ennemy or friend ?
		enumFaction _faction;
		// is node visible ?
		bool _visible;
		



		friend GameLevel;
	};

	typedef RCPtr<GameNode>  GameNodePtr;

}

