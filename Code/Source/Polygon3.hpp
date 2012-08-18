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
//=================================== Original header ========================
//
// COTD Entry submitted by John W. Ratcliff [jratcliff@verant.com]
//
// ** THIS IS A CODE SNIPPET WHICH WILL EFFICIEINTLY TRIANGULATE ANY
// ** POLYGON/CONTOUR (without holes) AS A STATIC CLASS.  THIS SNIPPET
// ** IS COMPRISED OF 3 FILES, TRIANGULATE.H, THE HEADER FILE FOR THE
// ** TRIANGULATE BASE CLASS, TRIANGULATE.CPP, THE IMPLEMENTATION OF
// ** THE TRIANGULATE BASE CLASS, AND TEST.CPP, A SMALL TEST PROGRAM
// ** DEMONSTRATING THE USAGE OF THE TRIANGULATOR.  THE TRIANGULATE
// ** BASE CLASS ALSO PROVIDES TWO USEFUL HELPER METHODS, ONE WHICH
// ** COMPUTES THE AREA OF A POLYGON, AND ANOTHER WHICH DOES AN EFFICENT
// ** POINT IN A TRIANGLE TEST.
// ** SUBMITTED BY JOHN W. RATCLIFF (jratcliff@verant.com) July 22, 2000
//
// Static class to triangulate any contour/polygon efficiently
// You should replace Vector2d with whatever your own Vector
// class might be.  Does not support polygons with holes.
// Uses STL vectors to represent a dynamic array of vertices.
// This code snippet was submitted to FlipCode.com by
// John W. Ratcliff (jratcliff@verant.com) on July 22, 2000
// I did not write the original code/algorithm for this
// this triangulator, in fact, I can't even remember where I
// found it in the first place.  However, I did rework it into
// the following black-box static class so you can make easy
// use of it in your own code.  Simply replace Vector2d with
// whatever your own Vector implementation might be.
//
//============================================================================

#ifndef __Thea_Algorithms_Polygon3_hpp__
#define __Thea_Algorithms_Polygon3_hpp__

#include "Common.hpp"
#include "Array.hpp"
#include "AxisAlignedBox3.hpp"
#include "Vector3.hpp"

namespace Thea {

/** A polygon in 3-space. Original code due to John W. Ratcliff. */
class THEA_API Polygon3
{
  public:
    THEA_DEF_POINTER_TYPES(Polygon3, shared_ptr, weak_ptr)

    /** A vertex plus an index. */
    struct THEA_API IndexedVertex
    {
      /** Default constructor. */
      IndexedVertex() {}

      /** Initializing constructor. */
      IndexedVertex(Vector3 const & position_, long index_) : position(position_), index(index_) {}

      Vector3 position;  ///< The position of the vertex.
      long index;  ///< The index of the vertex.
    };

    /** Construct an empty polygon. */
    Polygon3();

    /**
     * Add a vertex to the polygon. The vertex is inserted at the end of the current sequence of vertices, and by default is
     * assigned an index that is one more than the maximum index in the polygon so far (or zero if this is the first vertex).
     * For efficiency the polygon <b>is not checked for planarity</b>, the caller should ensure that all vertices are coplanar.
     */
    void addVertex(Vector3 const & p);

    /**
     * Add an indexed vertex to the polygon. The vertex is inserted at the end of the current sequence of vertices. For
     * efficiency the polygon <b>is not checked for planarity</b>, the caller should ensure that all vertices are coplanar.
     */
    void addVertex(Vector3 const & p, long index);

    /** Get the number of vertices in the polygon. */
    long numVertices() const;

    /**
     * Get the vertex at position \a poly_index in the sequence of vertices around the polygon boundary.
     *
     * @note \a poly_index is determined by the sequence of addVertex() calls, <b>NOT</b> by the index supplied in
     *   addVertex(Vector2 const &, long)!
     */
    IndexedVertex const & getVertex(long poly_index) const;

    /** Delete all vertices from the polygon. */
    void clear();

    /**
     * Triangulate the polygon and return the set of triangle indices (in successive groups of 3). All prior data in the
     * supplied array are cleared.
     *
     * @return The number of triangles created.
     */
    long triangulate(TheaArray<long> & tri_indices) const;

    /** Compute the area of the polygon. */
    Real area() const;

    /** Get the bounding box of the polygon. */
    AxisAlignedBox3 const & getBounds();

  private:
    /** Signed area of projection onto primary coordinate plane. */
    Real projArea() const;

    /** Check if a triangle can be removed. */
    bool snip(array_size_t u, array_size_t v, array_size_t w, array_size_t n, TheaArray<array_size_t> const & indices) const;

    TheaArray<IndexedVertex> vertices;
    long max_index;
    AxisAlignedBox3 bounds;
    mutable TheaArray<Vector2> proj_vertices;

    friend class Polygon2;

}; // class Polygon3

} // namespace Thea

#endif