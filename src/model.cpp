#include "model/brick.h"

void brick_model()
{
  make_buffer(GL_ARRAY_BUFFER, brick_vertices, (sizeof (brick_vertices)));
  make_buffer(GL_ELEMENT_ARRAY_BUFFER, brick_Cube_triangles, (sizeof (brick_Cube_triangles)));
}
