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




#include "BattleUnitUI.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")


// ----------------------------------------------------------------------------

using namespace Unigine;
using namespace Math;
using namespace Gdiplus;
using namespace SubWorld;

// ----------------------------------------------------------------------------
ULONG_PTR  BattleUnitUI::gdiplusToken = 0;
Unigine::BlobPtr BattleUnitUI::_blob;
Gdiplus::Graphics *BattleUnitUI::_g = nullptr;
Gdiplus::Bitmap* BattleUnitUI::_bitmap = nullptr;
Gdiplus::Font* BattleUnitUI::_font = nullptr;
Gdiplus::SolidBrush*  BattleUnitUI::_withBrush;
Gdiplus::SolidBrush*  BattleUnitUI::_blackBrush;
Gdiplus::Pen* BattleUnitUI::_blackpen;
Gdiplus::Pen* BattleUnitUI::_whitepen;
Gdiplus::Pen* BattleUnitUI::_graypen;
// ----------------------------------------------------------------------------


void BattleUnitUI::globalShutdown()
{
	_blob->clear();
	delete _bitmap;
	_bitmap = nullptr;
	delete _g;
	_g = nullptr;
	delete _font;
	delete _blackBrush;
	delete _withBrush;
	delete _blackpen;
	delete _whitepen;
	delete _graypen;
	GdiplusShutdown(gdiplusToken);

}

// ----------------------------------------------------------------------------


void BattleUnitUI::globalInit()
{

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);


	_blob = Blob::create((_guiScreenWidth * 4)*_guiScreenHeight);
	_bitmap = new Gdiplus::Bitmap(_guiScreenWidth, _guiScreenHeight, PixelFormat32bppARGB);


	_g = new Gdiplus::Graphics(_bitmap);
	_g->SetInterpolationMode(InterpolationMode::InterpolationModeHighQualityBilinear);
	_g->SetTextRenderingHint(TextRenderingHint::TextRenderingHintAntiAlias);

	_font = Gdiplus::Font(&Gdiplus::FontFamily(L"Courier New"), 14).Clone();


	_blackBrush = new Gdiplus::SolidBrush(InvColor(0, 0, 0));

	_withBrush = new Gdiplus::SolidBrush(InvColor(255, 255, 255));
	_blackpen = new Gdiplus::Pen(InvColor(0, 0, 0));
	_whitepen = new Gdiplus::Pen(InvColor(255, 255, 255));
	_graypen = new Gdiplus::Pen(InvColor(128, 128, 128));
}



// ----------------------------------------------------------------------------


BattleUnitUI::BattleUnitUI(GameNodePtr gamenode, std::function<bool(Gdiplus::Graphics *g, int w, int h)> painter)
	: _initialized(false)
{
	_painter = painter;
	_object_gui = ObjectGui::create(4, 2);
	_object_gui->setBillboard(true);
	_object_gui->setBackground(0);
	_object_gui->setPhysicalSize(_guiPhysicalWidth, _guiPhysicalHeight);
	_object_gui->setScreenSize(_guiScreenWidth, _guiScreenHeight);
	_object_gui->setPosition(vec3(0, 0, 1));
	gamenode->_node->addChild(_object_gui);

	GuiPtr gui = _object_gui->getGui();

	_sprite = WidgetSprite::create(gui);// , "SubWorld/textures/Color_helice.png");

	_uiImage = Unigine::Image::create();
	_uiImage->create2D(_guiScreenWidth, _guiScreenHeight, Unigine::Image::FORMAT_RGBA8);

	gui->addChild(_sprite, Gui::ALIGN_OVERLAP | Gui::ALIGN_BACKGROUND);

	_initialized = true;

}

// ----------------------------------------------------------------------------


BattleUnitUI::~BattleUnitUI()
{
	if (_initialized && _sprite) 
	{
		GuiPtr gui = _object_gui->getGui();
		gui->setHidden(1);
	}
	_initialized = false;
	_uiImage->clear();
	_uiImage = nullptr;
	_sprite->deleteForce();
	_sprite = nullptr;
	_object_gui->deleteForce();
	_object_gui = nullptr;

}

// ----------------------------------------------------------------------------


void BattleUnitUI::draw(const float elapsedTime)
{
	if (!_initialized || !_sprite) return;
	GuiPtr gui = _object_gui->getGui();
	// painter
	bool painted = _painter(_g, _guiScreenWidth, _guiScreenHeight);

	if (!painted)
	{
		gui->setHidden(1);
		return;
	}
	if (gui->isHidden()) gui->setHidden(0);
	// swap image buffer of the sprite
	copyBitmap(_uiImage);
}

// ----------------------------------------------------------------------------


void BattleUnitUI::copyBitmap(Unigine::ImagePtr uiImage)
{

	// copy Gdi bitmap to Unigine image
	Gdiplus::Rect rect(0, 0, _bitmap->GetWidth(), _bitmap->GetHeight());
	Gdiplus::BitmapData data;
	_bitmap->LockBits(&rect, Gdiplus::ImageLockMode::ImageLockModeRead, PixelFormat32bppRGB, &data);
	size_t nBytes = (size_t)(::abs(data.Stride) * _bitmap->GetHeight());
	_blob->seekSet(0);
	_blob->write(data.Scan0, nBytes);
	uiImage->setPixels(_blob);
	_bitmap->UnlockBits(&data);

	// Refresh the gui sprite (TODO: NEED TO BYPASS THAT TRICKS!)
	GuiPtr gui = _object_gui->getGui();
	WidgetSpritePtr old = _sprite;
	_sprite = WidgetSprite::create(gui);
	_sprite->setImage(_uiImage);
	gui->replaceChild(_sprite, old);
	old->deleteForce();

}

// ----------------------------------------------------------------------------
