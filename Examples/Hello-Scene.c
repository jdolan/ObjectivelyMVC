/*
 * ObjectivelyMVC: Object oriented MVC framework for OpenGL, SDL3 and GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

/**
 * @file
 * @brief Rotating cube scene for the Hello example, demonstrating ObjectivelyMVC
 * integrating with a vanilla SDL3 / OpenGL 3.3 Core Profile application.
 */

#include <math.h>
#include <stddef.h>

#include <ObjectivelyMVC/Renderer+OpenGL.h>

#include "Hello-Scene.h"

typedef struct {
  GLfloat x, y, z;
  GLfloat r, g, b;
} CubeVertex;

static const CubeVertex cube_verts[] = {
  // Top (green)
  { 1, 1,-1,  0,1,0 }, {-1, 1,-1,  0,1,0 }, {-1, 1, 1,  0,1,0 },
  { 1, 1,-1,  0,1,0 }, {-1, 1, 1,  0,1,0 }, { 1, 1, 1,  0,1,0 },
  // Bottom (orange)
  { 1,-1, 1,  1,.5,0 }, {-1,-1, 1,  1,.5,0 }, {-1,-1,-1,  1,.5,0 },
  { 1,-1, 1,  1,.5,0 }, {-1,-1,-1,  1,.5,0 }, { 1,-1,-1,  1,.5,0 },
  // Front (red)
  { 1, 1, 1,  1,0,0 }, {-1, 1, 1,  1,0,0 }, {-1,-1, 1,  1,0,0 },
  { 1, 1, 1,  1,0,0 }, {-1,-1, 1,  1,0,0 }, { 1,-1, 1,  1,0,0 },
  // Back (yellow)
  { 1,-1,-1,  1,1,0 }, {-1,-1,-1,  1,1,0 }, {-1, 1,-1,  1,1,0 },
  { 1,-1,-1,  1,1,0 }, {-1, 1,-1,  1,1,0 }, { 1, 1,-1,  1,1,0 },
  // Left (blue)
  {-1, 1, 1,  0,0,1 }, {-1, 1,-1,  0,0,1 }, {-1,-1,-1,  0,0,1 },
  {-1, 1, 1,  0,0,1 }, {-1,-1,-1,  0,0,1 }, {-1,-1, 1,  0,0,1 },
  // Right (magenta)
  { 1, 1,-1,  1,0,1 }, { 1, 1, 1,  1,0,1 }, { 1,-1, 1,  1,0,1 },
  { 1, 1,-1,  1,0,1 }, { 1,-1, 1,  1,0,1 }, { 1,-1,-1,  1,0,1 },
};

static const char *cube_vs =
  "precision highp float;\n"
  "uniform mat4 mvp;\n"
  "layout(location=0) in vec3 in_pos;\n"
  "layout(location=1) in vec3 in_color;\n"
  "out vec3 frag_color;\n"
  "void main(void) {\n"
  "  gl_Position = mvp * vec4(in_pos, 1.0);\n"
  "  frag_color = in_color;\n"
  "}\n";

static const char *cube_fs =
  "precision highp float;\n"
  "in vec3 frag_color;\n"
  "out vec4 out_color;\n"
  "void main(void) {\n"
  "  out_color = vec4(frag_color, 1.0);\n"
  "}\n";

static GLuint cube_program, cube_vao, cube_vbo;
static GLint  cube_mvp_loc;

static void mat4_mul(GLfloat *out, const GLfloat *a, const GLfloat *b) {
  for (int r = 0; r < 4; r++) {
    for (int c = 0; c < 4; c++) {
      out[r*4+c] = 0;
      for (int k = 0; k < 4; k++) {
        out[r*4+c] += a[r*4+k] * b[k*4+c];
      }
    }
  }
}

void initScene(void) {

  cube_program = MVC_LinkProgram(
    MVC_CompileShader(GL_VERTEX_SHADER, "cube_vs", cube_vs),
    MVC_CompileShader(GL_FRAGMENT_SHADER, "cube_fs", cube_fs)
  );

  cube_mvp_loc = gl.GetUniformLocation(cube_program, "mvp");

  gl.GenVertexArrays(1, &cube_vao);
  gl.BindVertexArray(cube_vao);

  gl.GenBuffers(1, &cube_vbo);
  gl.BindBuffer(GL_ARRAY_BUFFER, cube_vbo);
  gl.BufferData(GL_ARRAY_BUFFER, sizeof(cube_verts), cube_verts, GL_STATIC_DRAW);

  gl.EnableVertexAttribArray(0);
  gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void *) offsetof(CubeVertex, x));
  gl.EnableVertexAttribArray(1);
  gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void *) offsetof(CubeVertex, r));

  gl.BindVertexArray(0);
}

void drawScene(SDL_Window *window) {

  int w, h;
  SDL_GetWindowSizeInPixels(window, &w, &h);

  glEnable(GL_DEPTH_TEST);

  const float angle = SDL_GetTicks() * 0.001f;
  const float c = cosf(angle), s = sinf(angle);

  const GLfloat rot[16] = {
     c, s*s, s*c, 0,
     0, c,  -s,   0,
    -s, s*c, c*c, 0,
     0, 0,   0,   1,
  };

  const float aspect = (float) w / (float) h;
  const float fov    = 1.0f;
  const float znear  = 0.1f, zfar = 10.0f;
  const float f = 1.0f / tanf(fov * 0.5f);
  const GLfloat proj[16] = {
    f/aspect, 0,  0,                         0,
    0,        f,  0,                         0,
    0,        0,  (zfar + znear) / (znear - zfar), -1,
    0,        0,  (2 * zfar * znear) / (znear - zfar), 0,
  };

  const GLfloat trans[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0,-4, 1,
  };

  GLfloat tmp[16], mvp[16];
  mat4_mul(tmp, rot, trans);
  mat4_mul(mvp, tmp, proj);

  gl.UseProgram(cube_program);
  gl.UniformMatrix4fv(cube_mvp_loc, 1, GL_FALSE, mvp);
  gl.BindVertexArray(cube_vao);

  glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(sizeof(cube_verts) / sizeof(cube_verts[0])));

  gl.BindVertexArray(0);
  gl.UseProgram(0);

  glDisable(GL_DEPTH_TEST);
}
