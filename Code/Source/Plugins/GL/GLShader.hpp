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

#ifndef __Thea_Graphics_GLShader_hpp__
#define __Thea_Graphics_GLShader_hpp__

#include "../../Graphics/Shader.hpp"
#include "../../Array.hpp"
#include "../../Map.hpp"
#include "GLCommon.hpp"
#include "GLTexture.hpp"
#include "GLHeaders.hpp"

namespace Thea {
namespace Graphics {
namespace GL {

/** An OpenGL shader. */
class THEA_GL_DLL_LOCAL GLShader : public Shader
{
  public:
    GLShader(std::string const & name_);

    ~GLShader();

    bool isComplete() const { return complete; }

    void attachModuleFromFile(ModuleType type, std::string const & path);
    void attachModuleFromString(ModuleType type, std::string const & source);

    bool hasUniform(std::string const & uniform_name) const { return uniforms.find(uniform_name) != uniforms.end(); }

    void setUniform(std::string const & uniform_name, float value);
    void setUniform(std::string const & uniform_name, int value);
    void setUniform(std::string const & uniform_name, Vector2 const & value);
    void setUniform(std::string const & uniform_name, Vector3 const & value);
    void setUniform(std::string const & uniform_name, Vector4 const & value);
    void setUniform(std::string const & uniform_name, Color1uint8 const & value);
    void setUniform(std::string const & uniform_name, Color1 const & value);
    void setUniform(std::string const & uniform_name, Color3uint8 const & value);
    void setUniform(std::string const & uniform_name, Color3 const & value);
    void setUniform(std::string const & uniform_name, Color4uint8 const & value);
    void setUniform(std::string const & uniform_name, Color4 const & value);
    void setUniform(std::string const & uniform_name, Matrix2 const & value);
    void setUniform(std::string const & uniform_name, Matrix3 const & value);
    void setUniform(std::string const & uniform_name, Matrix4 const & value);
    void setUniform(std::string const & uniform_name, Texture * value);

    void setUniform(std::string const & uniform_name, TheaArray<float> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<int> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Vector2> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Vector3> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Vector4> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Color1uint8> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Color1> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Color3uint8> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Color3> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Color4uint8> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Color4> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Matrix2> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Matrix3> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Matrix4> const & value);
    void setUniform(std::string const & uniform_name, TheaArray<Texture *> const & value);

    /** Link the various modules of the shader into a single program. */
    void link();

    /** Use the shader for rendering. */
    void use();

    /** Get the OpenGL ID of the shader. */
    GLhandleARB getGLID() const { return program_id; }

  private:
    /** A value for a uniform variable. */
    struct UniformValue
    {
      float f_val;
      int i_val;
      TheaArray<float> f_array;
      TheaArray<int> i_array;
      GLTexture * texture;
    };

    /** Data related to an uniform variable. */
    struct UniformData
    {
      GLenum type;
      GLint size;
      GLint location;
      int texunit;
      bool has_value;
      UniformValue value;
      bool requires_rebind;

      /** Constructor. */
      UniformData() : has_value(false), requires_rebind(false) {}

      /** Note that the value has been changed. */
      void valueChanged() { has_value = true; requires_rebind = true; }
    };

    /** A set of uniforms read from source code. */
    typedef TheaMap<std::string, UniformData> Uniforms;

    /** Read the list of active uniforms in the shader object. */
    void readActiveUniforms();

    /** Bind the user-provided uniforms to the shader object. */
    void bindUniforms();

    /** Check if a build step (compile or link) succeeded, and throw a custom error if it did not. */
    void checkBuildStatus(GLhandleARB obj_id, GLenum status_field, std::string const & error_msg);

    bool complete;
    bool linked;
    bool has_vertex_module;
    bool has_fragment_module;
    GLhandleARB program_id;
    Uniforms uniforms;

}; // class GLShader

} // namespace GL
} // namespace Graphics
} // namespace Thea

#endif