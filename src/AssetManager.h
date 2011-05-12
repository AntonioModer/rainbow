#ifndef ASSETMANAGER_H_
#define ASSETMANAGER_H_

#include "Platform.h"

#if defined(RAINBOW_IOS)

typedef NSBundle* Assets;

#elif defined(RAINBOW_ZIP)
#	include <zip.h>
#	define RAINBOW_DATA_PATH "assets/"

typedef zip* Assets;

#else
#	include <cassert>
#	include <cstdio>
#	include <cstring>
#	define RAINBOW_DATA_PATH "Data/"

typedef char* Assets;

#endif

/// Native assets loading. Supports zip archives.
///
/// \see http://www.nih.at/libzip/
///
/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class AssetManager
{
public:
	static AssetManager& Instance()
	{
		static AssetManager am;
		return am;
	}

	~AssetManager() { this->close(); }

	/// Close any open archive streams.
	void close();

	/// Return the full path to a resource or asset.
	const char* get_full_path(const char *const filename = 0);

	/// Load file into provided buffer.
	/// \param buffer    The buffer to fill
	/// \param filename  The file to load
	unsigned int load(unsigned char *&buffer, const char *filename);

	/// Set the source to load resources from.
	void set_source(const char *const src = 0);

private:
	unsigned int size;  ///< Size of temporary string
	char *path;         ///< Temporary string
	Assets archive;     ///< Source of assets

	AssetManager() : size(0), path(0), archive(0)  { }
	AssetManager(const AssetManager &);
	AssetManager& operator=(const AssetManager &);
};

inline void AssetManager::set_source(const char *const src)
{
#if defined(RAINBOW_IOS)

	this->archive = [NSBundle mainBundle];

#elif defined(RAINBOW_ZIP)

	int errorp = 0;
	this->archive = zip_open(src, 0, &errorp);
	assert(errorp == 0);

#endif
}

#endif