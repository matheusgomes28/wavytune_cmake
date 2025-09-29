#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>

namespace wt {
    struct Camera {
        glm::vec3 pos;
        glm::vec3 front;
        glm::vec3 right;
        glm::vec3 up;

        glm::mat4 transform;

        glm::vec3 getDirection() const {
            return transform * glm::vec4(front, 1);
        }

        glm::vec3 getUp() const {
            return transform * glm::vec4(up, 1);
        }

        glm::vec3 getRight() const {
            return transform * glm::vec4(right, 1);
        }
    };
} // namespace wt


#endif // CAMERA_H
