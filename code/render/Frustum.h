#ifndef _FRUSTUM_H
#define _FRUSTUM_H

#include "globalincs/pstypes.h"

class Frustum
{
public:
  Frustum(float fov, float aspect_ratio, float near_dist, float far_dist,
          vec3d * view_position, matrix * view_matrix);

  bool sphereInFrustum(vec3d * position, float radius);
  bool sphereBehindPlane(vec3d * position, float radius, vec3d * planePoint,
                         vec3d * planeNormal);

private:
    vec3d near_normal;
  vec3d near_point;
  float near_lambda;

  /*vec3d far_normal;
     vec3d far_point;
     float far_lambda; */

  vec3d top_normal;
  vec3d top_point;
  float top_lambda;

  vec3d bottom_normal;
  vec3d bottom_point;
  float bottom_lambda;

  vec3d left_normal;
  vec3d left_point;
  float left_lambda;

  vec3d right_normal;
  vec3d right_point;
  float right_lambda;
};

#endif
