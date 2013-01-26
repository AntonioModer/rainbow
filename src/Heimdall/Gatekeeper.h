#ifndef HEIMDALL_GATEKEEPER_H_
#define HEIMDALL_GATEKEEPER_H_

#ifdef USE_HEIMDALL

#include "Director.h"
#include "Heimdall/DebugInfo.h"
#include "Heimdall/Overlay.h"
#include "Input/Touch.h"
#include "Input/Touchable.h"

namespace Heimdall
{
	/// Overlay for debugging options.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Gatekeeper : public Touchable, private NonCopyable<Gatekeeper>
	{
	public:
		Gatekeeper();
		~Gatekeeper();

		inline void draw();

		void init(const Data &);
		void set_video(const int width, const int height);

		void update(const unsigned long t = 0);

		inline void on_memory_warning();

		virtual void touch_began(const Touch *const touches, const size_t count);
		virtual void touch_canceled();
		virtual void touch_ended(const Touch *const touches, const size_t count);
		virtual void touch_moved(const Touch *const touches, const size_t count);

	private:
		int width;
		int height;
		unsigned int touch_count;
		unsigned long touch_held;
		Touch touches[2];

		Rainbow::Director *director;
		SharedPtr<FontAtlas> console_font;
		SharedPtr<FontAtlas> ui_font;

		SceneGraph::Node *overlay_node;
		SceneGraph::Node scenegraph;

		Overlay overlay;
		DebugInfo info;

		inline void toggle_overlay();
	};

	void Gatekeeper::draw()
	{
		this->director->draw();
		this->scenegraph.draw();
	}

	void Gatekeeper::toggle_overlay()
	{
		this->overlay_node->enabled = !this->overlay_node->enabled;
	}

	void Gatekeeper::on_memory_warning()
	{
		this->director->on_memory_warning();
	}
}

typedef Heimdall::Gatekeeper Director;

#endif  // USE_HEIMDALL
#endif  // HEIMDALL_GATEKEEPER_H_