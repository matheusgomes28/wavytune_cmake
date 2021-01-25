#include <graphics/Camera.h>

using namespace wt;

glm::vec3 Camera::getDirection() const
{
  return transform * glm::vec4(front, 1);
}

glm::vec3 Camera::getUp() const { return transform * glm::vec4(up, 1); }

glm::vec3 Camera::getRight() const { return transform * glm::vec4(right, 1); }
