
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

#pragma warning( push )
#pragma warning( disable : 4458)
#pragma once
#define NOMINMAX

#include "../ComponentSystem/ComponentSystem.h"
#include "Opensteer/include/OpenSteer/SimpleVehicle.h"
#include <UnigineMathLib.h>
#include <UnigineObjects.h>
#include <UnigineGui.h>
#include <UnigineWidgets.h>
#include <UnigineImage.h>
#include <functional>

#include "GameNode.h"

namespace Gdiplus
{
	class Bitmap;
	class Graphics;
 
		using std::min;
		using std::max;
	 
}


 


 
#include <gdiplus.h>

#define InvColor(r,g,b) Gdiplus::Color(255, b , g,r)

namespace SubWorld
{

	// Unit detailed UI in 3D world
	class BattleUnitUI 
	{
	public:
		BattleUnitUI(GameNodePtr gameNode, std::function<bool(Gdiplus::Graphics *g,int w,int h)> painter);
		~BattleUnitUI();
		void draw(const float elapsedTime);

		static void globalInit();
		static void globalShutdown();

		static Gdiplus::Font* _font;
		static Gdiplus::SolidBrush*  _withBrush;
		static Gdiplus::SolidBrush*  _blackBrush;
		static Gdiplus::Pen* _blackpen;
		static Gdiplus::Pen* _whitepen;
		static Gdiplus::Pen* _graypen;

	protected:
	  
		void copyBitmap(Unigine::ImagePtr uiImage);
	 

	protected:
		static ULONG_PTR  gdiplusToken;
		static Unigine::BlobPtr _blob;
		static Gdiplus::Graphics *_g;
		static Gdiplus::Bitmap* _bitmap;



		static const int _guiPhysicalWidth = 3;
		static const int _guiPhysicalHeight = 1;
		static const int _guiScreenWidth = 600;
		static const int _guiScreenHeight = 480;

		std::function<bool(Gdiplus::Graphics *g, int w, int h)> _painter;
		Unigine::ObjectGuiPtr _object_gui;
		Unigine::ImagePtr _uiImage;
		Unigine::WidgetSpritePtr _sprite;
		bool _initialized;

	};



}

#pragma warning( pop ) 