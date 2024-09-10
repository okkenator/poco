//
// SharedLibraryLoader.cpp
//
// Library: Foundation
// Package: SharedLibrary
// Module:  SharedLibraryLoader
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#include "Poco/SharedLibraryLoader.h"
#include "Poco/Exception.h"


namespace Poco {

SharedLibraryLoader::SharedLibraryLoader()
{
}

SharedLibraryLoader::~SharedLibraryLoader()
/// Destroys the SharedLibraryLoader.
{
	for (auto& p : _map)
	{
		delete p.second.pLibrary;
	}
}

SharedLibraryLoader& SharedLibraryLoader::instance()
{
	static SharedLibraryLoader p;
	return p;
}

void SharedLibraryLoader::loadLibrary(const std::string& path, const std::string& manifest)
{
	FastMutex::ScopedLock lock(_mutex);

	typename LibraryMap::iterator it = _map.find(path);
	if (it == _map.end())
	{
		LibraryInfo li;
		li.pLibrary = 0;
		li.refCount = 1;
		try
		{
			li.pLibrary = new SharedLibrary(path);
			if (li.pLibrary->hasSymbol("pocoInitializeLibrary"))
			{
				InitializeLibraryFunc initializeLibrary = (InitializeLibraryFunc)li.pLibrary->getSymbol("pocoInitializeLibrary");
				initializeLibrary();
			}
		}
		catch (...)
		{
			delete li.pLibrary;
			throw;
		}
	}
	else
	{
		++it->second.refCount;
	}
}

void SharedLibraryLoader::unloadLibrary(const std::string& path)
{
	FastMutex::ScopedLock lock(_mutex);

	typename LibraryMap::iterator it = _map.find(path);
	if (it != _map.end())
	{
		if (--it->second.refCount == 0)
		{
			if (it->second.pLibrary->hasSymbol("pocoUninitializeLibrary"))
			{
				UninitializeLibraryFunc uninitializeLibrary = (UninitializeLibraryFunc)it->second.pLibrary->getSymbol("pocoUninitializeLibrary");
				uninitializeLibrary();
			}
			it->second.pLibrary->unload();
			delete it->second.pLibrary;
			_map.erase(it);
		}
	}
	else throw NotFoundException(path);
}

const SharedLibrary* SharedLibraryLoader::findLibrary(const std::string& path) const
{
	FastMutex::ScopedLock lock(_mutex);

	typename LibraryMap::const_iterator it = _map.find(path);
	if (it != _map.end())
		return it->second.pLibrary;
	else
		return 0;
}

bool SharedLibraryLoader::isLibraryLoaded(const std::string& path) const
{
	return findLibrary(path) != 0;
}


} // namespace Poco
