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

#include "../ComponentSystem/ComponentSystem.h"
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UnigineWidgets.h>
#include <UnigineUserInterface.h>
#include <string>
#include <vector>
#include <functional>
 
namespace SubWorld
{
	class GameNode;

	enum enumNodePanel
	{
		WELCOME_MANAGER,
		RESOURCE_MANAGER,
		STEERING_MANAGER
	 };

	

	class NodeUI : public ComponentBase
	{
	public:

		// methods
		COMPONENT(NodeUI, ComponentBase);
		COMPONENT_UPDATE(update);
		COMPONENT_INIT(init)
		COMPONENT_SHUTDOWN(shutdown);

		PROP_NAME("NodeUI");
		PROP_PARAM(String, name, "DummyParam");


		// show or hide the user interface associated with the node
		void show_ui();
		void refresh_ui();


	protected:

	protected:
		// called when the component is instancied
		void init();
		// init callback
		void initCallBack();
		// on update handler
		void update();
		// called on destroy
		void shutdown();
		void showPanel(enumNodePanel panel);
	protected:
		void close_window_cb();
		void showMessage(const std::string& msg);
		void closeMessageDialog();
		void back_window_cb();
	protected:
		// Resource Manager handlers & method
		void initResourceManager();
		void refreshResourceManager();
		void manageresource_cb();
		void refresh_listbox_resource();
		void select_listbox_resource();
		void create_resources_cb();
	protected:
		// Steering behaviors
		void managesteering_cb();
		void initSteeringManager();
		void refreshSteeringManager();
		void travel_bt();
		void travel_circular_bt();
		void travel_speed_bt();
	public:
		// UI used to manage this factory
		Unigine::WidgetPtr _window;
		// flags which indicate that th ui is actualy shown
		bool _uiVisible;
		Unigine::UserInterfacePtr _ui;
		enumNodePanel _activePanel;
		Unigine:: WidgetDialogMessagePtr _messageDialog;
		 
	};

}