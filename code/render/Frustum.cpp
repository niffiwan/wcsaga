#include "Frustum.h"

#include "Math/vecmat.h"

extern matrix Eye_matrix;

Frustum::Frustum(float fov, float aspect_ratio, float near_dist,
                 float far_dist, vec3d * view_position, matrix * view_matrix)
{
  float height = (float) tan((double) fov * 0.5) * near_dist;
  float width = height * aspect_ratio;

  // Near plane
  near_normal = view_matrix->vec.fvec;
  vm_vec_scale_add(&near_point, view_position, &view_matrix->vec.fvec,
                   near_dist);
  near_lambda = vm_vec_dotprod(&near_normal, &near_point);

  // Top plane
  vec3d top_helper;
  vm_vec_scale_add(&top_helper, &near_point, &view_matrix->vec.uvec, height);
  vm_vec_sub2(&top_helper, view_position);
  vm_vec_normalize_safe(&top_helper);
  vm_vec_crossprod(&top_normal, &view_matrix->vec.rvec, &top_helper);
  vm_vec_normalize_safe(&top_normal);
  top_point = *view_position;
  top_lambda = vm_vec_dotprod(&top_normal, &top_point);

  // Bottom plane
  vec3d bottom_helper;
  vm_vec_scale_add(&bottom_helper, &near_point, &view_matrix->vec.uvec,
                   -height);
  vm_vec_sub2(&bottom_helper, view_position);
  vm_vec_normalize_safe(&bottom_helper);
  vm_vec_crossprod(&bottom_normal, &bottom_helper, &view_matrix->vec.rvec);
  vm_vec_normalize_safe(&bottom_normal);
  bottom_point = *view_position;
  bottom_lambda = vm_vec_dotprod(&bottom_normal, &bottom_point);

  // Left plane
  vec3d left_helper;
  vm_vec_scale_add(&left_helper, &near_point, &view_matrix->vec.rvec, -width);
  vm_vec_sub2(&left_helper, view_position);
  vm_vec_normalize_safe(&left_helper);
  vm_vec_crossprod(&left_normal, &view_matrix->vec.uvec, &left_helper);
  vm_vec_normalize_safe(&left_normal);
  left_point = *view_position;
  left_lambda = vm_vec_dotprod(&left_normal, &left_point);

  // Right plane
  vec3d right_helper;
  vm_vec_scale_add(&right_helper, &near_point, &view_matrix->vec.rvec, width);
  vm_vec_sub2(&right_helper, view_position);
  vm_vec_normalize_safe(&right_helper);
  vm_vec_crossprod(&right_normal, &right_helper, &view_matrix->vec.uvec);
  vm_vec_normalize_safe(&right_helper);
  right_point = *view_position;
  right_lambda = vm_vec_dotprod(&right_normal, &right_point);
}

bool Frustum::sphereInFrustum(vec3d * position, float radius)
{
  // Check the object
  float distance = 0.0f;

  // Near check
  distance = (vm_vec_dotprod(&near_normal, position) - near_lambda);
  if (distance < -radius)
    {
      return false;
    }

  // Top check
  distance = (vm_vec_dotprod(&top_normal, position) - top_lambda);
  if (distance < -radius)
    {
      return false;
    }

  // Buttom check
  distance = (vm_vec_dotprod(&bottom_normal, position) - bottom_lambda);
  if (distance < -radius)
    {
      return false;
    }

  // Left check
  distance = (vm_vec_dotprod(&left_normal, position) - left_lambda);
  if (distance < -radius)
    {
      return false;
    }

  // Right check
  distance = (vm_vec_dotprod(&right_normal, position) - right_lambda);
  if (distance < -radius)
    {
      return false;
    }

  // Is inside
  return true;
}

bool Frustum::sphereBehindPlane(vec3d * position, float radius,
                                vec3d * planePoint, vec3d * planeNormal)
{
  // Check the object
  float distance = 0.0f;

  // User plane check
  vec3d tmp;
  vm_vec_sub(&tmp, position, planePoint);
  distance = vm_vec_dotprod(planeNormal, &tmp);

  if (distance < -radius)
    {
      return true;
    }

  return false;
}
