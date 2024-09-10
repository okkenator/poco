//
// SharedLibraryLoader.h
//
// Library: Foundation
// Package: SharedLibrary
// Module:  SharedLibraryLoader
//
// Definition of the SharedLibraryLoader class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Foundation_SharedLibraryLoader_INCLUDED
#define Foundation_SharedLibraryLoader_INCLUDED


#include "Poco/Foundation.h"
#include "Poco/SharedLibrary.h"
#include "Poco/Mutex.h"
#include "Poco/Exception.h"
#include <map>


namespace Poco {


class SharedLibraryLoader
	/// The SharedLibraryLoader loads C++ shared libraries
	/// at runtime. 
	///
{
public:
	typedef void (*InitializeLibraryFunc)();
	typedef void (*UninitializeLibraryFunc)();

	struct LibraryInfo
	{
		SharedLibrary* pLibrary;
		int            refCount;
	};
	typedef std::map<std::string, LibraryInfo> LibraryMap;

	SharedLibraryLoader();

	virtual ~SharedLibraryLoader();
	/// Destroys the SharedLibraryLoader.

	static SharedLibraryLoader& instance();
	/// Returns a reference to the SharedLibraryLoader singleton.
	///
	/// Throws a NullPointerException if no SharedLibraryLoader instance exists.
	/// 

	void loadLibrary(const std::string& path, const std::string& manifest);
		/// Loads a library from the given path
		/// Increments the refCount if the library is already loaded.
		/// Throws a LibraryLoadException if the library
		/// cannot be loaded.
		/// If the library exports a function named "pocoInitializeLibrary",
		/// this function is executed.
		/// If called multiple times for the same library,
		/// the number of calls to unloadLibrary() must be the same
		/// for the library to become unloaded.

	void unloadLibrary(const std::string& path);
		/// Unloads the given library.
		/// Be extremely cautious when unloading shared libraries.
		/// If objects from the library are still referenced somewhere,
		/// a total crash is very likely.
		/// If the library exports a function named "pocoUninitializeLibrary",
		/// this function is executed before it is unloaded.
		/// If loadLibrary() has been called multiple times for the same
		/// library, the number of calls to unloadLibrary() must be the same
		/// for the library to become unloaded.

	const SharedLibrary* findLibrary(const std::string& path) const;
		/// Returns a pointer to the SharedLibrary for the given
		/// library, or a null pointer if the library has not been loaded.

	bool isLibraryLoaded(const std::string& path) const;
		/// Returns true if the library with the given name
		/// has already been loaded.

private:
	LibraryMap _map;
	mutable FastMutex _mutex;
};


} // namespace Poco


#endif // Foundation_SharedLibraryLoader_INCLUDED
