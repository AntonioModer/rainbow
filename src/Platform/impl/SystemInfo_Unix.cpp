// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Platform/SystemInfo.h"
#if defined(RAINBOW_OS_ANDROID) || defined(RAINBOW_OS_LINUX) || defined(RAINBOW_JS)

#include <cstring>
#include <locale.h>
#include <unistd.h>

namespace rainbow
{
    namespace system_info
    {
#ifdef RAINBOW_OS_UNIX
        bool has_accelerometer()
        {
            return false;
        }

        bool has_touchscreen()
        {
            return false;
        }

        void locales(std::vector<std::unique_ptr<char[]>>& locales)
        {
            char* lc = setlocale(LC_ALL, nullptr);
            if (!lc)
                lc = setlocale(LC_MESSAGES, nullptr);
            else
            {
                char* lc_msg = strstr(lc, "LC_MESSAGES");
                if (lc_msg)
                    lc = lc_msg + 12;
            }
            char* locale = new char[6];
            if (!lc || strcmp("C", lc) == 0)
                strcpy(locale, "en");
            else
            {
                const size_t length = strlen(lc);
                size_t i = 0;
                for (; i < length; ++i)
                {
                    if (lc[i] == ';')
                        break;
                    locale[i] = lc[i];
                }
                locale[i] = '\0';
            }
            locales.emplace_back(locale);
        }
#endif

        size_t memory()
        {
            return sysconf(_SC_PAGE_SIZE) / 1024 * sysconf(_SC_PHYS_PAGES) / 1024;
        }
    }
}

#endif
