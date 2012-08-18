//============================================================================
//
// This file is part of the Thea project.
//
// This software is covered by the following BSD license, except for portions
// derived from other works which are covered by their respective licenses.
// For full licensing information including reproduction of these external
// licenses, see the file LICENSE.txt provided in the documentation.
//
// Copyright (c) 2009, Stanford University
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

#ifndef __Thea_Noncopyable_hpp__
#define __Thea_Noncopyable_hpp__

#include "Platform.hpp"

namespace Thea {

/**
 * A base class for objects that should never be copied. This is achieved by declaring the copy constructor and assignment
 * operator as private members. <b>Never ever</b> try to refer to an object of a derived class using a Noncopyable pointer or
 * reference (in any case this seems semantically weird) -- to ensure this class has zero runtime overhead, the destructor is
 * <b>not virtual</b>.
 */
class THEA_API Noncopyable
{
  protected:
    /** Constructor. */
    Noncopyable() {}

    /** Destructor. */
    ~Noncopyable() {}

  private:
    /**
     * Hidden copy constructor. No body provided since this should never be accessible -- if a linker error occurs then
     * something is seriously wrong.
     */
    THEA_DLL_LOCAL Noncopyable(const Noncopyable &);

    /**
     * Hidden assignment operator. No body provided since this should never be accessible -- if a linker error occurs then
     * something is seriously wrong.
     */
    THEA_DLL_LOCAL Noncopyable const & operator=(Noncopyable const &);

}; // class Noncopyable

} // namespace Thea

#endif