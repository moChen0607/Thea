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
// First version: 2009
//
//============================================================================

#include "DisplayMesh.hpp"
#include "../Polygon3.hpp"
#include "../UnorderedSet.hpp"

namespace Thea {
namespace Graphics {

DisplayMesh::DisplayMesh(std::string const & name)
: NamedObject(name),
  valid_bounds(true),
  wireframe_enabled(false),
  changed_buffers(BufferID::ALL),
  var_area(nullptr),
  vertices_var(nullptr),
  tris_var(nullptr),
  quads_var(nullptr),
  normals_var(nullptr),
  colors_var(nullptr),
  texcoords_var(nullptr),
  vertex_matrix(nullptr, 3, 0),
  tri_matrix(nullptr, 3, 0),
  quad_matrix(nullptr, 4, 0),
  vertex_wrapper(&vertex_matrix),
  tri_wrapper(&tri_matrix),
  quad_wrapper(&quad_matrix)
{}

DisplayMesh::DisplayMesh(DisplayMesh const & src)
: NamedObject(src),
  vertices(src.vertices),
  normals(src.normals),
  colors(src.colors),
  texcoords(src.texcoords),
  tris(src.tris),
  quads(src.quads),
  valid_bounds(src.valid_bounds),
  bounds(src.bounds),
  wireframe_enabled(src.wireframe_enabled),
  changed_buffers(BufferID::ALL),
  var_area(nullptr),
  vertices_var(nullptr),
  tris_var(nullptr),
  quads_var(nullptr),
  normals_var(nullptr),
  colors_var(nullptr),
  texcoords_var(nullptr),
  vertex_matrix(nullptr, 3, 0),
  tri_matrix(nullptr, 3, 0),
  quad_matrix(nullptr, 4, 0),
  vertex_wrapper(&vertex_matrix),
  tri_wrapper(&tri_matrix),
  quad_wrapper(&quad_matrix)
{}

void
DisplayMesh::clear()
{
  vertices.clear();
  normals.clear();
  colors.clear();
  texcoords.clear();
  tris.clear();
  quads.clear();
  edges.clear();

  vertex_source_indices.clear();
  tri_source_face_indices.clear();
  quad_source_face_indices.clear();

  face_vertex_indices.clear();
  triangulated_indices.clear();

  valid_bounds = true;
  bounds = AxisAlignedBox3();

  invalidateGPUBuffers();
}

AbstractDenseMatrix<Real> const *
DisplayMesh::getVertexMatrix() const
{
  // Assume Vector3 is tightly packed and has no padding
  Vector3 const * buf = (vertices.empty() ? nullptr : &vertices[0]);
  new (&vertex_matrix) VertexMatrix(reinterpret_cast<Real *>(const_cast<Vector3 *>(buf)), 3, numVertices());
  return &vertex_wrapper;
}

AbstractDenseMatrix<uint32> const *
DisplayMesh::getTriangleMatrix() const
{
  uint32 const * buf = (tris.empty() ? nullptr : &tris[0]);
  new (&tri_matrix) TriangleMatrix(const_cast<uint32 *>(buf), 3, numTriangles());
  return &tri_wrapper;
}

AbstractDenseMatrix<uint32> const *
DisplayMesh::getQuadMatrix() const
{
  uint32 const * buf = (quads.empty() ? nullptr : &quads[0]);
  new (&quad_matrix) QuadMatrix(const_cast<uint32 *>(buf), 4, numQuads());
  return &quad_wrapper;
}

DisplayMesh::Vertex
DisplayMesh::getVertex(intx i)
{
  debugAssertM(i >= 0 && i < (intx)vertices.size(), getNameStr() + ": Vertex index out of bounds");

  size_t si = (size_t)i;
  return Vertex(this, vertices[si],
                hasNormals()    ?  &normals[si]    :  nullptr,
                hasColors()     ?  &colors[si]     :  nullptr,
                hasTexCoords()  ?  &texcoords[si]  :  nullptr);
}

DisplayMesh::IndexTriple
DisplayMesh::getTriangle(intx tri_index) const
{
  debugAssertM(tri_index >= 0 && 3 * tri_index < (intx)tris.size(), getNameStr() + ": Triangle index out of bounds");

  size_t base_index = (size_t)(3 * tri_index);
  IndexTriple tri;
  tri[0] = (intx)tris[base_index];
  tri[1] = (intx)tris[base_index + 1];
  tri[2] = (intx)tris[base_index + 2];

  return tri;
}

DisplayMesh::IndexQuad
DisplayMesh::getQuad(intx quad_index) const
{
  debugAssertM(quad_index >= 0 && 4 * quad_index < (intx)quads.size(), getNameStr() + ": Quad index out of bounds");

  size_t base_index = (size_t)(4 * quad_index);
  IndexQuad quad;
  quad[0] = (intx)quads[base_index];
  quad[1] = (intx)quads[base_index + 1];
  quad[2] = (intx)quads[base_index + 2];
  quad[3] = (intx)quads[base_index + 3];

  return quad;
}

void
DisplayMesh::addColors()
{
  if (colors.size() < vertices.size())
  {
    colors.resize(vertices.size(), ColorRGBA(0, 0, 0, 0));
    invalidateGPUBuffers();
  }
}

void
DisplayMesh::addNormals()
{
  if (normals.size() < vertices.size())
  {
    normals.resize(vertices.size(), Vector3::Zero());
    invalidateGPUBuffers();
  }
}

void
DisplayMesh::addTexCoords()
{
  if (texcoords.size() < vertices.size())
  {
    texcoords.resize(vertices.size(), Vector2::Zero());
    invalidateGPUBuffers();
  }
}

intx
DisplayMesh::addVertex(Vector3 const & point, intx source_index, Vector3 const * normal, ColorRGBA const * color,
                       Vector2 const * texcoord)
{
  alwaysAssertM((source_index >= 0 && vertex_source_indices.size() == vertices.size())
             || (source_index < 0 && vertex_source_indices.empty()),
                getNameStr() + ": Mesh must have all or no vertex source indices");
  alwaysAssertM((normal && normals.size() == vertices.size()) || (!normal && normals.empty()),
                getNameStr() + ": Mesh must have all or no normals");
  alwaysAssertM((color && colors.size() == vertices.size()) || (!color && colors.empty()),
                getNameStr() + ": Mesh must have all or no vertex colors");
  alwaysAssertM((texcoord && texcoords.size() == vertices.size()) || (!texcoord && texcoords.empty()),
                getNameStr() + ": Mesh must have all or no texture coordinates");

  intx index = (intx)vertices.size();

  if (valid_bounds)
    bounds.merge(point);

  vertices.push_back(point);
  if (source_index >= 0)  vertex_source_indices.push_back(source_index);
  if (normal)             normals.push_back(*normal);
  if (color)              colors.push_back(*color);
  if (texcoord)           texcoords.push_back(*texcoord);

  invalidateGPUBuffers();

  return index;
}

intx
DisplayMesh::addTriangle(intx vi0, intx vi1, intx vi2, intx source_face_index)
{
  debugAssertM(vi0 >= 0 && vi1 >= 0 && vi2 >= 0
            && vi0 < (intx)vertices.size()
            && vi1 < (intx)vertices.size()
            && vi2 < (intx)vertices.size(), getNameStr() + ": Vertex index out of bounds");

  alwaysAssertM((source_face_index >= 0 && 3 * tri_source_face_indices.size() == tris.size())
             || (source_face_index < 0 && tri_source_face_indices.empty()),
                getNameStr() + ": Mesh must have all or no triangle face source indices");

  intx index = (intx)(tris.size() / 3);

  tris.push_back((uint32)vi0);
  tris.push_back((uint32)vi1);
  tris.push_back((uint32)vi2);

  if (source_face_index >= 0)
    tri_source_face_indices.push_back(source_face_index);

  invalidateGPUBuffers();

  return index;
}

intx
DisplayMesh::addQuad(intx vi0, intx vi1, intx vi2, intx vi3, intx source_face_index)
{
  debugAssertM(vi0 >= 0 && vi1 >= 0 && vi2 >= 0 && vi3 >= 0
            && vi0 < (intx)vertices.size()
            && vi1 < (intx)vertices.size()
            && vi2 < (intx)vertices.size()
            && vi3 < (intx)vertices.size(), getNameStr() + ": Vertex index out of bounds");

  alwaysAssertM((source_face_index >= 0 && 4 * quad_source_face_indices.size() == quads.size())
             || (source_face_index < 0 && quad_source_face_indices.empty()),
                getNameStr() + ": Mesh must have all or no quad face source indices");

  intx index = (intx)(quads.size() / 4);

  quads.push_back((uint32)vi0);
  quads.push_back((uint32)vi1);
  quads.push_back((uint32)vi2);
  quads.push_back((uint32)vi3);

  if (source_face_index >= 0)
    quad_source_face_indices.push_back(source_face_index);

  invalidateGPUBuffers();

  return index;
}

DisplayMesh::Face
DisplayMesh::addFace(int num_vertices, intx const * face_vertex_indices_, intx source_face_index)
{
  if (num_vertices < 3)
  {
    THEA_DEBUG << getName() << ": Skipping face -- too few vertices (" << num_vertices << ')';
    return Face();
  }

  if (num_vertices == 3)
    return Face(this, 3, true,
                addTriangle(face_vertex_indices_[0], face_vertex_indices_[1], face_vertex_indices_[2], source_face_index), 1);

  if (num_vertices == 4)
    return Face(this, 4, false,
                addQuad(face_vertex_indices_[0], face_vertex_indices_[1], face_vertex_indices_[2], face_vertex_indices_[3],
                        source_face_index),
                1);

  Polygon3 poly;
  for (int i = 0; i < num_vertices; ++i)
  {
    intx vi = face_vertex_indices_[i];
    debugAssertM(vi >= 0 && vi < (intx)vertices.size(), getName() + format(": Vertex index %ld out of bounds", vi));

    poly.addVertex(vertices[vi], vi);
  }

  intx num_tris = poly.triangulate(triangulated_indices);
  if (num_tris <= 0)
    return Face();

  // debugAssertM(num_tris == 3 * (num_vertices - 2),
  //              getName() + format(": Triangulation of polygonal face yielded %l triangles, whereas %l were expected",
  //                                 num_tris, num_vertices - 2));

  alwaysAssertM((source_face_index >= 0 && 3 * tri_source_face_indices.size() == tris.size())
             || (source_face_index < 0 && tri_source_face_indices.empty()),
                getNameStr() + ": Mesh must have all or no triangle face source indices");

  intx starting_index = numTriangles();
  size_t num_tri_verts = (size_t)(3 * num_tris);
  for (size_t i = 0; i < num_tri_verts; i += 3)
  {
    tris.push_back((uint32)triangulated_indices[i    ]);
    tris.push_back((uint32)triangulated_indices[i + 1]);
    tris.push_back((uint32)triangulated_indices[i + 2]);
    if (source_face_index >= 0) tri_source_face_indices.push_back(source_face_index);
  }

  invalidateGPUBuffers();

  return Face(this, num_vertices, true, starting_index, num_tris);
}

void
DisplayMesh::removeTriangle(intx tri_index)
{
  debugAssertM(tri_index >= 0 && 3 * tri_index < (intx)tris.size(), getNameStr() + ": Triangle index out of bounds");

  IndexArray::iterator ii = tris.begin() + 3 * tri_index;
  tris.erase(ii, ii + 3);

  invalidateGPUBuffers();
}

void
DisplayMesh::removeTriangles(intx begin, intx num_triangles)
{
  debugAssertM(begin >= 0 && 3 * begin < (intx)tris.size(), getNameStr() + ": Triangle index out of bounds");

  IndexArray::iterator ii = tris.begin() + 3 * begin;
  tris.erase(ii, ii + 3 * num_triangles);

  invalidateGPUBuffers();
}

void
DisplayMesh::removeQuad(intx quad_index)
{
  debugAssertM(quad_index >= 0 && 4 * quad_index < (intx)quads.size(), getNameStr() + ": Quad index out of bounds");

  IndexArray::iterator ii = quads.begin() + 4 * quad_index;
  quads.erase(ii, ii + 4);

  invalidateGPUBuffers();
}

void
DisplayMesh::removeQuads(intx begin, intx num_quads)
{
  debugAssertM(begin >= 0 && 4 * begin < (intx)quads.size(), getNameStr() + ": Quad index out of bounds");

  IndexArray::iterator ii = quads.begin() + 4 * begin;
  quads.erase(ii, ii + 4 * num_quads);

  invalidateGPUBuffers();
}

void
DisplayMesh::removeFace(Face const & face)
{
  if (!face)
    return;

  alwaysAssertM(face.getMesh() == this, getNameStr() + ": Face belongs to a different mesh");

  if (face.hasTriangles())
    removeTriangles(face.getFirstTriangle(), face.numTriangles());

  if (face.hasQuads())
    removeQuads(face.getFirstQuad(), face.numQuads());
}

void
DisplayMesh::computeAveragedVertexNormals()
{
  bool topo_change = (normals.size() != vertices.size());

  normals.resize(vertices.size());
  for (size_t i = 0; i < normals.size(); ++i)
    normals[i] = Vector3::Zero();

  // TODO: weight normals by face area?
  Vector3 n;
  uint32 i0, i1, i2, i3;
  for (size_t i = 0; i < tris.size(); i += 3)
  {
    i0 = tris[i], i1 = tris[i + 1], i2 = tris[i + 2];
    Vector3 const & v0 = vertices[i0];
    Vector3 const & v1 = vertices[i1];
    Vector3 const & v2 = vertices[i2];

    n = (v2 - v1).cross(v0 - v1).normalized();
    normals[i0] += n;
    normals[i1] += n;
    normals[i2] += n;
  }

  for (size_t i = 0; i < quads.size(); i += 4)
  {
    i0 = quads[i], i1 = quads[i + 1], i2 = quads[i + 2], i3 = quads[i + 3];
    Vector3 const & v0 = vertices[i0];
    Vector3 const & v1 = vertices[i1];
    Vector3 const & v2 = vertices[i2];

    n = (v2 - v1).cross(v0 - v1).normalized();
    normals[i0] += n;
    normals[i1] += n;
    normals[i2] += n;
    normals[i3] += n;
  }

  for (size_t i = 0; i < normals.size(); ++i)
    normals[i] = normals[i].normalized();

  invalidateGPUBuffers(topo_change ? BufferID::ALL : BufferID::NORMAL);
}

void
DisplayMesh::flipNormals()
{
  for (size_t i = 0; i < normals.size(); ++i)
    normals[i] = -normals[i];

  invalidateGPUBuffers(BufferID::NORMAL);
}

void
DisplayMesh::updateEdges()
{
  edges.clear();

  if (wireframe_enabled)
  {
    typedef std::pair<uint32, uint32> Edge;
    typedef UnorderedSet<Edge> EdgeSet;

    EdgeSet added_edges;
    Edge edge;

    for (size_t i = 0; i < tris.size(); i += 3)
      for (size_t j = 0; j < 3; ++j)
      {
        uint32 ei0 = tris[i + j];
        uint32 ei1 = tris[j == 2 ? i : (i + j + 1)];

        // Order so lower index is first, since we're considering edges to be undirected
        if (ei0 < ei1)
          edge = Edge(ei0, ei1);
        else
          edge = Edge(ei1, ei0);

        EdgeSet::iterator existing = added_edges.find(edge);
        if (existing == added_edges.end())
        {
          edges.push_back(ei0);
          edges.push_back(ei1);
          added_edges.insert(edge);
        }
      }

    for (size_t i = 0; i < quads.size(); i += 4)
      for (size_t j = 0; j < 4; ++j)
      {
        uint32 ei0 = quads[i + j];
        uint32 ei1 = quads[j == 3 ? i : (i + j + 1)];

        // Order so lower index is first, since we're considering edges to be undirected
        if (ei0 < ei1)
          edge = Edge(ei0, ei1);
        else
          edge = Edge(ei1, ei0);

        EdgeSet::iterator existing = added_edges.find(edge);
        if (existing == added_edges.end())
        {
          edges.push_back(ei0);
          edges.push_back(ei1);
          added_edges.insert(edge);
        }
      }
  }

  THEA_DEBUG << getName() << ": Mesh has " << edges.size() / 2 << " edges";
}

void
DisplayMesh::isolateFaces()
{
  VertexArray    new_vertices;
  NormalArray    new_normals;
  ColorArray     new_colors;
  TexCoordArray  new_texcoords;

  for (size_t i = 0; i < tris.size(); i += 3)
  {
    size_t i0 = (size_t)tris[i    ];
    size_t i1 = (size_t)tris[i + 1];
    size_t i2 = (size_t)tris[i + 2];

    uint32 new_vindex = (uint32)new_vertices.size();

    new_vertices.push_back(vertices[i0]);
    new_vertices.push_back(vertices[i1]);
    new_vertices.push_back(vertices[i2]);

    if (!normals.empty())
    {
      new_normals.push_back(normals[i0]);
      new_normals.push_back(normals[i1]);
      new_normals.push_back(normals[i2]);
    }

    if (!colors.empty())
    {
      new_colors.push_back(colors[i0]);
      new_colors.push_back(colors[i1]);
      new_colors.push_back(colors[i2]);
    }

    if (!texcoords.empty())
    {
      new_texcoords.push_back(texcoords[i0]);
      new_texcoords.push_back(texcoords[i1]);
      new_texcoords.push_back(texcoords[i2]);
    }

    tris[i    ]  =  new_vindex;
    tris[i + 1]  =  new_vindex + 1;
    tris[i + 2]  =  new_vindex + 2;
  }

  for (size_t i = 0; i < quads.size(); i += 4)
  {
    size_t i0 = (size_t)quads[i    ];
    size_t i1 = (size_t)quads[i + 1];
    size_t i2 = (size_t)quads[i + 2];
    size_t i3 = (size_t)quads[i + 3];

    uint32 new_vindex = (uint32)new_vertices.size();

    new_vertices.push_back(vertices[i0]);
    new_vertices.push_back(vertices[i1]);
    new_vertices.push_back(vertices[i2]);
    new_vertices.push_back(vertices[i3]);

    if (!normals.empty())
    {
      new_normals.push_back(normals[i0]);
      new_normals.push_back(normals[i1]);
      new_normals.push_back(normals[i2]);
      new_normals.push_back(normals[i3]);
    }

    if (!colors.empty())
    {
      new_colors.push_back(colors[i0]);
      new_colors.push_back(colors[i1]);
      new_colors.push_back(colors[i2]);
      new_colors.push_back(colors[i3]);
    }

    if (!texcoords.empty())
    {
      new_texcoords.push_back(texcoords[i0]);
      new_texcoords.push_back(texcoords[i1]);
      new_texcoords.push_back(texcoords[i2]);
      new_texcoords.push_back(texcoords[i3]);
    }

    quads[i    ]  =  new_vindex;
    quads[i + 1]  =  new_vindex + 1;
    quads[i + 2]  =  new_vindex + 2;
    quads[i + 3]  =  new_vindex + 3;
  }

  vertices   =  new_vertices;
  normals    =  new_normals;
  colors     =  new_colors;
  texcoords  =  new_texcoords;

  invalidateGPUBuffers(BufferID::ALL);
}

void
DisplayMesh::updateBounds()
{
  if (valid_bounds) return;

  bounds = AxisAlignedBox3();
  for (size_t i = 0; i < vertices.size(); ++i)
    bounds.merge(vertices[i]);

  valid_bounds = true;
}

void
DisplayMesh::uploadToGraphicsSystem(RenderSystem & render_system)
{
  if (changed_buffers == 0) return;

  if (changed_buffers == BufferID::ALL)
  {
    if (var_area) var_area->reset();
    vertices_var = normals_var = colors_var = texcoords_var = tris_var = quads_var = edges_var = nullptr;

    if (vertices.empty() || (tris.empty() && quads.empty()))
    {
      if (var_area)
      {
        render_system.destroyVARArea(var_area);
        var_area = nullptr;
      }

      allGPUBuffersAreValid();
      return;
    }

    static int const PADDING = 32;
    intx vertex_bytes    =  !vertices.empty()  ?  3 * 4 * (intx)vertices.size()   +  PADDING : 0;  // 3 * float
    intx normal_bytes    =  hasNormals()       ?  3 * 4 * (intx)normals.size()    +  PADDING : 0;  // 3 * float
    intx color_bytes     =  hasColors()        ?  4 * 4 * (intx)colors.size()     +  PADDING : 0;  // 4 * float
    intx texcoord_bytes  =  hasTexCoords()     ?  2 * 4 * (intx)texcoords.size()  +  PADDING : 0;  // 2 * float

    updateEdges();

#ifdef THEA_DISPLAY_MESH_NO_INDEX_ARRAY
    intx num_bytes = vertex_bytes + normal_bytes + color_bytes + texcoord_bytes + PADDING;
#else
    intx tri_bytes   =  !tris.empty()   ?  4 * (intx)tris.size()   +  PADDING : 0;  // uint32
    intx quad_bytes  =  !quads.empty()  ?  4 * (intx)quads.size()  +  PADDING : 0;  // uint32
    intx edge_bytes  =  !edges.empty()  ?  4 * (intx)edges.size()  +  PADDING : 0;  // uint32

    intx num_bytes = vertex_bytes + normal_bytes + color_bytes + texcoord_bytes + tri_bytes + quad_bytes + edge_bytes + PADDING;
#endif

    if (var_area)
    {
      if (var_area->getCapacity() <= num_bytes || var_area->getCapacity() > (intx)(1.5 * num_bytes))
      {
        render_system.destroyVARArea(var_area);

        std::string vararea_name = getNameStr() + " VAR area";
        var_area = render_system.createVARArea(vararea_name.c_str(), num_bytes, VARArea::Usage::WRITE_OCCASIONALLY, true);
        if (!var_area) throw Error(getNameStr() + ": Couldn't create VAR area");
      }
      // Else no need to reset var_area, we've done it above
    }
    else
    {
      std::string vararea_name = getNameStr() + " VAR area";
      var_area = render_system.createVARArea(vararea_name.c_str(), num_bytes, VARArea::Usage::WRITE_OCCASIONALLY, true);
      if (!var_area) throw Error(getNameStr() + ": Couldn't create VAR area");
    }

    if (!vertices.empty())
    {
      vertices_var = var_area->createArray(vertex_bytes);
      if (!vertices_var) throw Error(getNameStr() + ": Couldn't create vertices VAR");
    }

    if (hasNormals())
    {
      normals_var = var_area->createArray(normal_bytes);
      if (!normals_var) throw Error(getNameStr() + ": Couldn't create normals VAR");
    }

    if (hasColors())
    {
      colors_var = var_area->createArray(color_bytes);
      if (!colors_var) throw Error(getNameStr() + ": Couldn't create colors VAR");
    }

    if (hasTexCoords())
    {
      texcoords_var = var_area->createArray(texcoord_bytes);
      if (!texcoords_var) throw Error(getNameStr() + ": Couldn't create texcoords VAR");
    }

#ifndef THEA_DISPLAY_MESH_NO_INDEX_ARRAY
    if (!tris.empty())
    {
      tris_var = var_area->createArray(tri_bytes);
      if (!tris_var) throw Error(getNameStr() + ": Couldn't create triangle indices VAR");
    }

    if (!quads.empty())
    {
      quads_var = var_area->createArray(quad_bytes);
      if (!quads_var) throw Error(getNameStr() + ": Couldn't create quad indices VAR");
    }

    if (!edges.empty())
    {
      edges_var = var_area->createArray(edge_bytes);
      if (!edges_var) throw Error(getNameStr() + ": Couldn't create edge indices VAR");
    }

    if (!tris.empty())  tris_var->updateIndices (0, (intx)tris.size(),  &tris[0]);
    if (!quads.empty()) quads_var->updateIndices(0, (intx)quads.size(), &quads[0]);
    if (!edges.empty()) edges_var->updateIndices(0, (intx)edges.size(), &edges[0]);
#endif

    if (!vertices.empty()) vertices_var->updateVectors (0, (intx)vertices.size(),  &vertices[0]);
    if (hasNormals())      normals_var->updateVectors  (0, (intx)normals.size(),   &normals[0]);
    if (hasColors())       colors_var->updateColors    (0, (intx)colors.size(),    &colors[0]);
    if (hasTexCoords())    texcoords_var->updateVectors(0, (intx)texcoords.size(), &texcoords[0]);
  }
  else
  {
    if (!gpuBufferIsValid(BufferID::VERTEX) && !vertices.empty())
      vertices_var->updateVectors(0, (intx)vertices.size(), &vertices[0]);

    if (!gpuBufferIsValid(BufferID::NORMAL) && hasNormals())
      normals_var->updateVectors (0, (intx)normals.size(), &normals[0]);

    if (!gpuBufferIsValid(BufferID::COLOR) && hasColors())
      colors_var->updateColors(0, (intx)colors.size(), &colors[0]);

    if (!gpuBufferIsValid(BufferID::TEXCOORD) && hasTexCoords())
      texcoords_var->updateVectors(0, (intx)texcoords.size(), &texcoords[0]);
  }

  allGPUBuffersAreValid();
}

void
DisplayMesh::draw(RenderSystem & render_system, AbstractRenderOptions const & options) const
{
  if (options.drawEdges() && !wireframe_enabled)
    throw Error(getNameStr() + ": Can't draw mesh edges when wireframe mode is disabled");

  const_cast<DisplayMesh *>(this)->uploadToGraphicsSystem(render_system);

  if (!vertices_var) return;
  if (!options.drawFaces() && !options.drawEdges()) return;
  if (!options.drawFaces() && !edges_var) return;
  if (!options.drawEdges() && !tris_var && !quads_var) return;

  render_system.beginIndexedPrimitives();

    render_system.setVertexArray(vertices_var);
    if (options.sendNormals()    &&  normals_var)    render_system.setNormalArray(normals_var);
    if (options.sendColors()     &&  colors_var)     render_system.setColorArray(colors_var);
    if (options.sendTexCoords()  &&  texcoords_var)  render_system.setTexCoordArray(0, texcoords_var);

    if (options.drawFaces())
    {
      if (options.drawEdges())
      {
        render_system.pushShapeFlags();
        render_system.setPolygonOffset(true, 2);
      }

#ifdef THEA_DISPLAY_MESH_NO_INDEX_ARRAY
        if (!tris.empty()) render_system.sendIndices(RenderSystem::Primitive::TRIANGLES, (intx)tris.size(), &tris[0]);
        if (!quads.empty()) render_system.sendIndices(RenderSystem::Primitive::QUADS, (intx)quads.size(), &quads[0]);
#else
        if (!tris.empty())
        {
          render_system.setIndexArray(tris_var);
          render_system.sendIndicesFromArray(RenderSystem::Primitive::TRIANGLES, 0, (intx)tris.size());
        }

        if (!quads.empty())
        {
          render_system.setIndexArray(quads_var);
          render_system.sendIndicesFromArray(RenderSystem::Primitive::QUADS, 0, (intx)quads.size());
        }
#endif

      if (options.drawEdges())
        render_system.popShapeFlags();
    }

    if (options.drawEdges())
    {
      render_system.pushShader();
      render_system.pushColorFlags();
      render_system.pushTextures();

        render_system.setShader(nullptr);
        render_system.setColorArray(nullptr);
        render_system.setTexCoordArray(0, nullptr);
        render_system.setNormalArray(nullptr);
        render_system.setColor(ColorRGBA(options.edgeColor()));  // set default edge color (TODO: handle per-edge colors)
        render_system.setTexture(0, nullptr);

#ifdef THEA_DISPLAY_MESH_NO_INDEX_ARRAY
        if (!edges.empty()) render_system.sendIndices(RenderSystem::Primitive::LINES, (intx)edges.size(), &edges[0]);
#else
        if (!edges.empty())
        {
          render_system.setIndexArray(edges_var);
          render_system.sendIndicesFromArray(RenderSystem::Primitive::LINES, 0, (intx)edges.size());
        }
#endif

      render_system.popTextures();
      render_system.popColorFlags();
      render_system.popShader();
    }

  render_system.endIndexedPrimitives();
}

} // namespace Graphics
} // namespace Thea
