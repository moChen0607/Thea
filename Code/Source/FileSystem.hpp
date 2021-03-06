//============================================================================
//
// This file is part of the Thea toolkit.
//
// This software is distributed under the BSD license, as detailed in the
// accompanying LICENSE.txt file. Portions are derived from other works:
// their respective licenses and copyright information are reproduced in
// LICENSE.txt and/or in the relevant source files.
//
// Author: Siddhartha Chaudhuri
// First version: 2013
//
//============================================================================

#ifndef __Thea_FileSystem_hpp__
#define __Thea_FileSystem_hpp__

#include "Common.hpp"

namespace Thea {

/**
 * Filesystem operations. Unlike FilePath, these functions do actually access the filesystem.
 *
 * @note Returned paths are in native OS format (e.g. backslashes on Windows, forward slashes on Unix).
 */
class THEA_API FileSystem
{
  public:
    /** Types of objects in the filesystem (enum class). */
    struct ObjectType
    {
      /** Supported values. */
      enum Value
      {
        FILE            =  0x0001,  ///< Ordinary file.
        DIRECTORY       =  0x0002,  ///< Directory.
        SYMLINK         =  0x0004,  ///< Symbolic link.
        ALL             =  0xFFFF,  ///< Catch-all value for all types of filesystem objects.
      };

      THEA_ENUM_CLASS_BODY(ObjectType)

    }; // struct ObjectType

    /** Miscellaneous flags (enum class). */
    struct Flags
    {
      /** Supported values. */
      enum Value
      {
        RECURSIVE          =  0x0001,  ///< Recurse into subdirectories.
        CASE_INSENSITIVE   =  0x0002,  ///< Case is ignored when matching file and directory names.
        SORTED             =  0x0004,  ///< Return sorted results.
      };

      THEA_ENUM_CLASS_BODY(Flags)

    }; // struct Flags

    /** Check if a file or directory exists. */
    static bool exists(std::string const & path);

    /** Check if a file exists, and is indeed a regular file (and not for instance a directory). */
    static bool fileExists(std::string const & path);

    /** Check if a directory exists, and is indeed a directory (and not for instance a file). */
    static bool directoryExists(std::string const & path);

    /** Get the length of a file in bytes. Returns a negative number on failure. */
    static int64 fileSize(std::string const & path);

    /** Resolve a relative path. */
    static std::string resolve(std::string const & path);

    /**
     * Create a directory, including all necessary parents (equivalent to "mkdir -p").
     *
     * @return True if the directory already exists or was successfully created, false on error.
     */
    static bool createDirectory(std::string const & path);

    /** Get the entire contents of a file as a string. */
    static std::string readWholeFile(std::string const & path)
    {
      std::string s;
      if (!readWholeFile(path, s))
        throw Error("FileSystem: Could not read '" + path + '\'');

      return s;
    }

    /**
     * Get the entire contents of a file as a string.
     *
     * @return True on success, false on error.
     */
    static bool readWholeFile(std::string const & path, std::string & ret);

    /**
     * Get the objects (files, subdirectories etc) in a directory, optionally filtering objects by type and name. Symbolic links
     * are returned as symbolic links and are not dereferenced to the files or directories they point to.
     *
     * @note Subdirectories are <i>not</i> traversed recursively unless Flags::RECURSIVE is added to the \a flags argument.
     *
     * @param dir The path to the directory.
     * @param objects Used to return the full paths of all retrieved objects in the directory. Prior contents are discarded.
     * @param types A bitwise-OR of ObjectType flags (pass zero, negative or ObjectType::ALL to allow all types of objects).
     * @param patterns If this string is non-empty, it is split into fields separated by spaces and each field interpreted as a
     *   shell wildcard pattern specified in the format of patternMatch(). The function will return only objects whose names
     *   (without path) match at least one of the patterns. E.g. \a patterns = "*.txt *.png" will return only files with names
     *   ending in .txt or .png.
     * @param flags A bitwise OR of Flags values specifying additional options, e.g. recursive subdirectory traversal,
     *   case-insensitive pattern matching, or sorted output.
     *
     * @return The number of objects found, equal to the size of \a objects. If the supplied path is not a directory, returns a
     *   negative value.
     */
    static intx getDirectoryContents(std::string const & dir,
                                     Array<std::string> & objects,
                                     int types = -1,
                                     std::string const & patterns = "",
                                     int flags = 0);

    /**
     * Remove a file, a symbolic link, or a directory. If the path is a directory, the operation succeeds only if the directory
     * is empty or if \a recursive is true.
     *
     * @param path The path to remove.
     * @param recursive If true, and if the path points to a directory, contents of the directory are recursively removed before
     *   the directory itself.
     *
     * @return True if the object was successfully removed or did not exist in the first place, false on error (e.g. directory
     *   not empty (with \a recursive == false) or operation not permitted).
     */
    static bool remove(std::string const & path, bool recursive = false);

    /**
     * Make a copy of a file. Fails, returning false, if the destination already exists.
     *
     * @param from The source path.
     * @param to The destination path.
     *
     * @return True if the file was successfully copied, else false.
     *
     * @todo Document the behaviour properly for files/symlinks/directories, on different platforms.
     */
    static bool copyFile(std::string const & from, std::string const & to);

}; // class FileSystem

} // namespace Thea

#endif
