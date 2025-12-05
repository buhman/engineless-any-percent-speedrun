#pragma once

const int plane_Plane_triangles[] = {
    0,   1,   2,
    0,   3,   1,
};

const int plane_Plane_triangles_length = (sizeof (plane_Plane_triangles)) / (sizeof (plane_Plane_triangles[0]));

const float plane_vertices[] = {
   1.000000f,  0.000000f,  1.000000f,  1.000000f,  0.000000f, -0.0000f,  1.0000f, -0.0000f,
  -1.000000f,  0.000000f, -1.000000f,  0.000000f,  1.000000f, -0.0000f,  1.0000f, -0.0000f,
  -1.000000f,  0.000000f,  1.000000f,  0.000000f,  0.000000f, -0.0000f,  1.0000f, -0.0000f,
   1.000000f,  0.000000f, -1.000000f,  1.000000f,  1.000000f, -0.0000f,  1.0000f, -0.0000f,
};

const int plane_vertices_length = (sizeof (plane_vertices)) / (sizeof (plane_vertices[0]));

