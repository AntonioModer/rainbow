#ifndef CONFUOCO_OGGVORBISAUDIOFILE_H_
#define CONFUOCO_OGGVORBISAUDIOFILE_H_

#include "Platform/Definitions.h"
#if !defined(RAINBOW_JS) && defined(RAINBOW_SDL)

#ifdef RAINBOW_MAC  // Seriously, Apple?
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wunused-variable"
#endif
#include <vorbis/vorbisfile.h>
#ifdef RAINBOW_MAC
#	pragma GCC diagnostic pop
#endif

#include "ConFuoco/AudioFile.h"

namespace ConFuoco
{
	/// Ogg Vorbis implementation of AudioFile.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	class OggVorbisAudioFile : public AudioFile
	{
	public:
		OggVorbisAudioFile(const char *const file);
		virtual ~OggVorbisAudioFile();

	private:
		OggVorbis_File vf;
		vorbis_info *vi;

		/* Implement AudioFile */

		virtual int get_channels_impl() const override;
		virtual int get_rate_impl() const override;
		virtual size_t read_impl(char **dst) override;
		virtual size_t read_impl(char *dst, const size_t size) override;
		virtual void rewind_impl() override;
	};
}

#endif  // !RAINBOW_JS && RAINBOW_SDL
#endif  // CONFUOCO_OGGVORBISAUDIOFILE_H_