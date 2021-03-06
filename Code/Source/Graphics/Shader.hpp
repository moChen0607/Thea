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

#ifndef __Thea_Graphics_Shader_hpp__
#define __Thea_Graphics_Shader_hpp__

#include "../Common.hpp"
#include "../Array.hpp"
#include "../Colors.hpp"
#include "../MatVec.hpp"
#include "../NamedObject.hpp"
#include "Texture.hpp"

namespace Thea {
namespace Graphics {

/**
 * An interface for a shader.
 *
 * @todo Make this safe for passing across shared library boundaries.
 */
class THEA_API Shader : public AbstractNamedObject
{
  public:
    /** %Shader module types (enum class). */
    struct THEA_API ModuleType
    {
      /** Supported values. */
      enum Value
      {
        VERTEX    =  0,  ///< Vertex shader.
        FRAGMENT  =  1,  ///< Fragment (pixel) shader.
        GEOMETRY  =  2   ///< Geometry shader.
      };

      THEA_ENUM_CLASS_BODY(ModuleType)
    };

    /** Destructor. */
    virtual ~Shader() {}

    /**
     * Check if the shader is ready to be used for rendering or not. Typically this requires both a vertex and a fragment
     * program to be attached.
     */
    virtual int8 isComplete() const = 0;

    /** Attach a program module to the shader from a file containing its source code. */
    virtual void attachModuleFromFile(ModuleType type, char const * path) = 0;

    /** Attach a program module to the shader from a string containing its source code. */
    virtual void attachModuleFromString(ModuleType type, char const * source) = 0;

    /** Check if the shader has an active uniform of the given name. */
    virtual int8 hasUniform(char const * uniform_name) const = 0;

    /** Set a floating-point uniform. */
    virtual void setUniform(char const * uniform_name, float32 value) = 0;

    /** Set an integer uniform. */
    virtual void setUniform(char const * uniform_name, int32 value) = 0;

    /** Set a 2-vector uniform. */
    virtual void setUniform(char const * uniform_name, Vector2 const & value) = 0;

    /** Set a 3-vector uniform. */
    virtual void setUniform(char const * uniform_name, Vector3 const & value) = 0;

    /** Set a 4-vector uniform. */
    virtual void setUniform(char const * uniform_name, Vector4 const & value) = 0;

    /** Set a single-channel byte color uniform. */
    virtual void setUniform(char const * uniform_name, ColorL8 const & value) = 0;

    /** Set a single-channel floating-point color uniform. */
    virtual void setUniform(char const * uniform_name, ColorL const & value) = 0;

    /** Set a 3-channel byte color uniform. */
    virtual void setUniform(char const * uniform_name, ColorRGB8 const & value) = 0;

    /** Set a 3-channel floating-point color uniform. */
    virtual void setUniform(char const * uniform_name, ColorRGB const & value) = 0;

    /** Set a 4-channel byte color uniform. */
    virtual void setUniform(char const * uniform_name, ColorRGBA8 const & value) = 0;

    /** Set a 4-channel floating-point color uniform. */
    virtual void setUniform(char const * uniform_name, ColorRGBA const & value) = 0;

    /** Set a 2x2 matrix uniform. */
    virtual void setUniform(char const * uniform_name, Matrix2 const & value) = 0;

    /** Set a 3x3 matrix uniform. */
    virtual void setUniform(char const * uniform_name, Matrix3 const & value) = 0;

    /** Set a 4x4 matrix uniform. */
    virtual void setUniform(char const * uniform_name, Matrix4 const & value) = 0;

    /** Set a texture uniform. */
    virtual void setUniform(char const * uniform_name, Texture * value) = 0;

    /** Set a floating-point array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, float32 const * values) = 0;

    /** Set an integer array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, int32 const * values) = 0;

    /** Set a 2-vector array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, Vector2 const * values) = 0;

    /** Set a 3-vector array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, Vector3 const * values) = 0;

    /** Set a 4-vector array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, Vector4 const * values) = 0;

    /** Set a single-channel byte color array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, ColorL8 const * values) = 0;

    /** Set a single-channel floating-point color array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, ColorL const * values) = 0;

    /** Set a 3-channel byte color array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, ColorRGB8 const * values) = 0;

    /** Set a 3-channel floating-point color array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, ColorRGB const * values) = 0;

    /** Set a 4-channel byte color array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, ColorRGBA8 const * values) = 0;

    /** Set a 4-channel floating-point color array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, ColorRGBA const * values) = 0;

    /** Set a 2x2 matrix array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, Matrix2 const * values) = 0;

    /** Set a 3x3 matrix array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, Matrix3 const * values) = 0;

    /** Set a 4x4 matrix array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, Matrix4 const * values) = 0;

    /** Set a texture array uniform. */
    virtual void setUniform(char const * uniform_name, int64 num_values, Texture * const * values) = 0;

}; // class Shader

} // namespace Graphics
} // namespace Thea

#endif
