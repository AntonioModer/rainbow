// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef AUDIO_AL_SOUND_H_
#define AUDIO_AL_SOUND_H_

#include "Audio/AudioFile.h"

namespace rainbow { namespace audio
{
    struct Sound
    {
        bool stream = false;
        int format = 0;
        int rate = 0;
        int loop_count = 0;
        unsigned int buffer = 0;
        std::unique_ptr<IAudioFile> file;
        const char* key;
    };
}}  // rainbow::audio

#endif
