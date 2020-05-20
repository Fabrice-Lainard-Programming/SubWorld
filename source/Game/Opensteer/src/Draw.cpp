// ----------------------------------------------------------------------------
//
//
// OpenSteer -- Steering Behaviors for Autonomous Characters
//
// Copyright (c) 2002-2003, Sony Computer Entertainment America
// Original author: Craig Reynolds <craig_reynolds@playstation.sony.com>
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
//
//
// Draw
//
// This is a first stab at a graphics module for OpenSteerDemo.  It is intended
// to encapsulate all functionality related to 3d graphics as well as windows
// and graphics input devices such as the mouse.
//
// However this is purely an OpenGL-based implementation.  No special effort
// has been made to keep the "OpenGL way" from leaking through.  Attempting to
// port this to another graphics substrate may run into modularity problems.
//
// In any case, all calls to the underlying graphics substrate should be made
// from this module only.
//
// 10-04-04 bk:  put everything into the OpenSteer namespace
// 06-25-02 cwr: created 
//
//
// ----------------------------------------------------------------------------


#include <iomanip>
#include <sstream>

 
// To include OpenSteer::round.
#include "../include/OpenSteer/Draw.h"
#include "../include/OpenSteer/Utilities.h"

#include <UnigineGame.h>
#include <UnigineWorld.h>
#include <UnigineVisualizer.h>
 

// ----------------------------------------------------------------------------

	  
// ----------------------------------------------------------------------------
// do all initialization related to graphics


void 
OpenSteer::initializeGraphics (int argc, char **argv)
{
    
}


// ----------------------------------------------------------------------------
// run graphics event loop


void 
OpenSteer::runGraphics (void)
{
    
}



 

 



// ----------------------------------------------------------------------------
// warn when draw functions are called during OpenSteerDemo's update phase


void 
OpenSteer::warnIfInUpdatePhase2(const char* name)
{
	std::ostringstream message;
	message << "use annotation (during simulation update, do not call ";
	message << name;
	message << ")";
	message << std::ends;
}
	 

void 
OpenSteer::drawLine (const Vec3& a,
                     const Vec3& b,
                     const Vec3& color)
{
	warnIfInUpdatePhase("drawLine");

	Unigine::Math::Vec3 va(a.x, a.z, a.y);
	Unigine::Math::Vec3 vb(b.x, b.z, b.y);

	Unigine::Math::Vec4 vcolor(color.x, color.y, color.z, 1.0);
	Unigine::Visualizer::renderLine3D(va, vb, vcolor);

}


// ----------------------------------------------------------------------------
// draw a line with alpha blending

// see also glAlphaFunc
// glBlendFunc (GL_SRC_ALPHA)
// glEnable (GL_BLEND)


void 
OpenSteer::drawLineAlpha (const Vec3& a,
                          const Vec3& b,
                          const Vec3& color,
                          const float alpha)
{
    warnIfInUpdatePhase ("drawLineAlpha");
	Unigine::Math::Vec3 va(a.x, a.z, a.y);
	Unigine::Math::Vec3 vb(b.x, b.z, b.y);
	 
	Unigine::Math::Vec4 vcolor(color.x, color.y, color.z, alpha);
	Unigine::Visualizer::renderLine3D(va, vb,  vcolor);
}




 

void 
OpenSteer::drawTriangle (const Vec3& a,
                         const Vec3& b,
                         const Vec3& c,
                         const Vec3& color)
{
	OpenSteer::warnIfInUpdatePhase("iDrawTriangle");
	Unigine::Math::Vec3 va(a.x, a.z, a.y);
	Unigine::Math::Vec3 vb(b.x, b.z, b.y);
	Unigine::Math::Vec3 vc(c.x, c.z, c.y);
	Unigine::Math::Vec4 vcolor(color.x, color.y, color.z, 1.0);
	Unigine::Visualizer::renderTriangle3D(va,vb,vc, vcolor);
	
}
 
 
    
void 
OpenSteer::drawQuadrangle (const Vec3& a,
                           const Vec3& b,
                           const Vec3& c,
                           const Vec3& d,
                           const Vec3& color)
{
	OpenSteer::warnIfInUpdatePhase("iDrawQuadrangle");
}


// ------------------------------------------------------------------------
// draws a "wide line segment": a rectangle of the given width and color
// whose mid-line connects two given endpoints


void 
OpenSteer::drawXZWideLine (const Vec3& startPoint,
                           const Vec3& endPoint,
                           const Vec3& color,
                           float width)
{
    warnIfInUpdatePhase ("drawXZWideLine");
 
}



 

// ------------------------------------------------------------------------
// General purpose circle/disk drawing routine.  Draws circles or disks (as
// specified by "filled" argument) and handles both special case 2d circles
// on the XZ plane or arbitrary circles in 3d space (as specified by "in3d"
// argument)


void 
OpenSteer::drawCircleOrDisk (const float radius,
                             const Vec3& axis,
                             const Vec3& center,
                             const Vec3& color,
                             const int segments,
                             const bool filled,
                             const bool in3d)
{

	 
	Unigine::Math::Mat4 mat;
	mat.setTranslate(Unigine::Math::Vec3(center.x, center.z, center.y));
	Unigine::Math::Vec4 vcolor(color.x, color.y, color.z,1.0);
	Unigine::Visualizer::renderCircle(radius, mat, vcolor);

}


// ------------------------------------------------------------------------


void 
OpenSteer::draw3dCircleOrDisk (const float radius,
                               const Vec3& center,
                               const Vec3& axis,
                               const Vec3& color,
                               const int segments,
                               const bool filled)
{
    // draw a circle-or-disk in the given local space
    drawCircleOrDisk (radius, axis, center, color, segments, filled, true);
}


// ------------------------------------------------------------------------
// drawing utility used by both drawXZCircle and drawXZDisk


void 
OpenSteer::drawXZCircleOrDisk (const float radius,
                               const Vec3& center,
                               const Vec3& color,
                               const int segments,
                               const bool filled)
{
    // draw a circle-or-disk on the XZ plane
    drawCircleOrDisk (radius, Vec3::zero, center, color, segments, filled, false);
}


// ------------------------------------------------------------------------
// draw a circular arc on the XZ plane, from a start point, around a center,
// for a given arc length, in a given number of segments and color.  The
// sign of arcLength determines the direction in which the arc is drawn.
//
// XXX maybe this should alow allow "in3d" cricles about an given axis
// XXX maybe there should be a "filled" version of this
// XXX maybe this should be merged in with drawCircleOrDisk


void 
OpenSteer::drawXZArc (const Vec3& start,
                      const Vec3& center,
                      const float arcLength,
                      const int segments,
                      const Vec3& color)
{
    warnIfInUpdatePhase ("drawXZArc");

   
}


// ------------------------------------------------------------------------
// a simple 2d vehicle on the XZ plane


void 
OpenSteer::drawBasic2dCircularVehicle (const AbstractVehicle& vehicle,
                                       const Vec3& color)
{
	// "aspect ratio" of body (as seen from above)
	const float x = 0.5f;
	const float y = sqrtXXX(1 - (x * x));

	// radius and position of vehicle
	 float r = vehicle.radius();
	const Vec3& p = vehicle.position();

	// shape of triangular body
	 
	const Vec3 u = r * 0.7f * Vec3(0, 1, 0); // slightly up
	//r = 0.5;
	const Vec3 f = r * vehicle.forward();
	const Vec3 s = r * vehicle.side() * x;
	const Vec3 b = r * vehicle.forward() * -y;

	/* drawTriangle(p + f + u,
		p + b - s + u,
		p + b + s + u,
		color); 
	 */

	// draw the circular collision boundary
	drawXZCircle(r, p + u, gWhite, 20);
}


// ------------------------------------------------------------------------
// a simple 3d vehicle


void 
OpenSteer::drawBasic3dSphericalVehicle (const AbstractVehicle& vehicle,
                                        const Vec3& color)
{
   
}



// ------------------------------------------------------------------------
// draw a (filled-in, polygon-based) square checkerboard grid on the XZ
// (horizontal) plane.
//
// ("size" is the length of a side of the overall grid, "subsquares" is the
// number of subsquares along each edge (for example a standard checkboard
// has eight), "center" is the 3d position of the center of the grid,
// color1 and color2 are used for alternating subsquares.)


void 
OpenSteer::drawXZCheckerboardGrid (const float size,
                                   const int subsquares,
                                   const Vec3& center,
                                   const Vec3& color1,
                                   const Vec3& color2)
{
   
}


// ------------------------------------------------------------------------
// draw a square grid of lines on the XZ (horizontal) plane.
//
// ("size" is the length of a side of the overall grid, "subsquares" is the
// number of subsquares along each edge (for example a standard checkboard
// has eight), "center" is the 3d position of the center of the grid, lines
// are drawn in the specified "color".)


void 
OpenSteer::drawXZLineGrid (const float size,
                           const int subsquares,
                           const Vec3& center,
                           const Vec3& color)
{
    warnIfInUpdatePhase ("drawXZLineGrid");
 
}


// ------------------------------------------------------------------------
// draw the three axes of a LocalSpace: three lines parallel to the
// basis vectors of the space, centered at its origin, of lengths
// given by the coordinates of "size".


void 
OpenSteer::drawAxes  (const AbstractLocalSpace& ls,
                      const Vec3& size,
                      const Vec3& color)
{
   
}


// ------------------------------------------------------------------------
// draw the edges of a box with a given position, orientation, size
// and color.  The box edges are aligned with the axes of the given
// LocalSpace, and it is centered at the origin of that LocalSpace.
// "size" is the main diagonal of the box.
//
// use gGlobalSpace to draw a box aligned with global space


void 
OpenSteer::drawBoxOutline  (const AbstractLocalSpace& localSpace,
                            const Vec3& size,
                            const Vec3& color)
{
     
}
 
// ------------------------------------------------------------------------
// Define scene's camera (viewing transformation) in terms of the camera's
// position, the point to look at (an "aim point" in the scene which will
// end up at the center of the camera's view), and an "up" vector defining
// the camera's "roll" around the "view axis" between cameraPosition and
// pointToLookAt (the image of the up vector will be vertical in the
// camera's view).


void 
OpenSteer::drawCameraLookAt (const Vec3& cameraPosition,
                             const Vec3& pointToLookAt,
                             const Vec3& up)
{
   
     
}


// ------------------------------------------------------------------------
// draw a reticle at the center of the window.  Currently it is small
// crosshair with a gap at the center, drawn in white with black borders


void 
OpenSteer::drawReticle (void)
{
   
}
 
// ------------------------------------------------------------------------
// check for errors during redraw, report any and then exit


void 
OpenSteer::checkForDrawError (const char * locationDescription)
{
    
}





// ----------------------------------------------------------------------------
// accessors for GLUT's window dimensions


float 
OpenSteer::drawGetWindowHeight (void) 
{
	return 1;
}


float 
OpenSteer::drawGetWindowWidth  (void) 
{
	return 1;
}

    
// ----------------------------------------------------------------------------
// return a normalized direction vector pointing from the camera towards a
// given point on the screen: the ray that would be traced for that pixel


OpenSteer::Vec3 
OpenSteer::directionFromCameraToScreenPosition (int x, int y)
{ 
    const Vec3 direction;
    return direction;
}

 
void 
OpenSteer::deferredDrawLine (const Vec3& startPoint,
                             const Vec3& endPoint,
                             const Vec3& color)
{
    
}


void 
OpenSteer::drawAllDeferredLines (void)
{
     
}

 

void 
OpenSteer::deferredDrawCircleOrDisk (const float radius,
                                     const Vec3& axis,
                                     const Vec3& center,
                                     const Vec3& color,
                                     const int segments,
                                     const bool filled,
                                     const bool in3d)
{
    
}


void 
OpenSteer::drawAllDeferredCirclesOrDisks (void)
{
   
}
  


void 
OpenSteer::draw2dTextAt3dLocation (const char& text,
                                   const Vec3& location,
                                   const Vec3& color)
{
 
	Unigine::Math::Vec3 pos(location.x, location.z, location.y);
	Unigine::Math::Vec3 center(-1,-1,0);
	Unigine::Math::Vec4 vcolor(color.x, color.y, color.z, 1.0);
	Unigine::Visualizer::renderMessage3D(pos,center, &text,vcolor,1);
}

//static void renderMessage3D(const Math::Vec3 & position, const Math::vec3 &center, const char *str, const Math::vec4 &color, int outline);
void 
OpenSteer::draw2dTextAt3dLocation (const std::ostringstream& text,
                                   const Vec3& location,
                                   const Vec3& color)
{
    draw2dTextAt3dLocation (*text.str().c_str(), location, color);
}


void 
OpenSteer::draw2dTextAt2dLocation (const char& text,
                                   const Vec3 location,
                                   const Vec3 color)
{
    
}


void 
OpenSteer::draw2dTextAt2dLocation (const std::ostringstream& text,
                                   const Vec3 location,
                                   const Vec3 color)
{
    draw2dTextAt2dLocation (*text.str().c_str(), location, color);
}


// ------------------------------------------------------------------------
// draw 2d lines in screen space: x and y are the relevant coordinates


void 
OpenSteer::draw2dLine (const Vec3& startPoint,
                       const Vec3& endPoint,
                       const Vec3& color)
{
   
}


// ----------------------------------------------------------------------------
    
