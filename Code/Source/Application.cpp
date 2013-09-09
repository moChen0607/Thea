//============================================================================
//
// This file is part of the Thea project.
//
// This software is covered by the following BSD license, except for portions
// derived from other works which are covered by their respective licenses.
// For full licensing information including reproduction of these external
// licenses, see the file LICENSE.txt provided in the documentation.
//
// Copyright (C) 2013, Siddhartha Chaudhuri/Stanford University
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holders nor the names of contributors
// to this software may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//============================================================================

#include "Application.hpp"
#include "FilePath.hpp"
#include "FileSystem.hpp"
#include "System.hpp"

#if defined(THEA_WINDOWS)

#  include <windows.h>

#elif defined(THEA_LINUX)

#  include <unistd.h>

#elif defined(THEA_OSX)

#  include <stdio.h>
#  include <unistd.h>

#endif

namespace Thea {

std::string
Application::programPath()
{
  char path[2048];

#ifdef THEA_WINDOWS
  {
    GetModuleFileNameA(NULL, path, sizeof(path));
  }
#elif defined(THEA_OSX)
  {
    // Run the 'ps' program to extract the program name from the process ID.
    int pid;
    FILE * fd;
    char cmd[80];
    pid = getpid();
    sprintf(cmd, "ps -p %d -o comm=\"\"", pid);
    fd = popen(cmd, "r");
    int s = fread(path, 1, sizeof(path), fd);

    // Path will contain a newline: overwrite it
    path[s - 1] = '\0';
  }
#else
  {
    int ret = readlink("/proc/self/exe", path, sizeof(path));

    // In case of an error, leave the handling up to the caller
    if (ret == -1)
      return "";

    debugAssertM((int)sizeof(path) > ret, "System: String too short to store current program path");
    // Ensure proper NULL termination
    path[ret] = 0;
  }
#endif

  return path;
}

void
Application::setResourceArchive(std::string const & path)
{
  if (!path.empty())
  {
    if (!FileSystem::directoryExists(path))
      throw Error("Resource archive '" + path + "' does not exist or is not a valid directory");

    _resourceArchive() = FileSystem::resolve(path);

    THEA_DEBUG << "Resource archive set to '" << _resourceArchive() << '\'';
  }
}

std::string
Application::getFullResourcePath(std::string const & resource_name)
{
  return FilePath::concat(_resourceArchive(), resource_name);
}

std::string &
Application::_resourceArchive()
{
  static std::string resource_archive = FilePath::parent(programPath());
  return resource_archive;
}

} // namespace Thea