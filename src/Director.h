#ifndef DIRECTOR_H_
#define DIRECTOR_H_

#include "LuaMachine.h"
#include "Common/ShutdownSequence.h"
#include "Graphics/SceneGraph.h"

/// Simple game loop for Lua-scripted games.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Director
{
public:
	Director();

	inline void draw();

	/// Load and initialise main script.
	void init(const Data &);

	/// Set screen resolution.
	void set_video(const int width, const int height);

	/// Update world.
	/// \param t  Current time or milliseconds since last frame (platform-dependent).
	void update(const unsigned long t = 0);

	/// Called when a low memory warning has been issued.
	void on_memory_warning();

private:
	ShutdownSequence shutdown;
	LuaMachine lua;
	SceneGraph::Node scenegraph;

	/// Intentionally left undefined.
	Director(const Director &);

	/// Intentionally left undefined.
	Director& operator=(const Director &);
};

void Director::draw()
{
	this->scenegraph.draw();
}

#endif
