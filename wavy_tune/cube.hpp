#ifndef CUBE_H
#define CUBE_H

#include "graphics/draw_buffer.hpp"

namespace wt {

    /// @brief Gets the unit cube (centered at 0,0,0).
    /// @return DrawBuffer representing the unit cube.
    inline DrawBuffer unit_cube() {
        // clang-format off
        std::vector<glm::vec3> const vertices{
            // Front face
            {-0.5f, 0.0f, 0.5f}, // Bottom-left
            {0.5f, 0.0f, 0.5f}, // Bottom-right
            {0.5f, 1.0f, 0.5f}, // Top-right
            {-0.5f, 1.0f, 0.5f}, // Top-left

            // Back face
            {-0.5f, 0.0f, -0.5f}, // Bottom-left
            {0.5f, 0.0f, -0.5f}, // Bottom-right 5 2 6
            {0.5f, 1.0f, -0.5f}, // Top-right
            {-0.5f, 1.0f, -0.5f} // Top-left
        };
        // clang-format on

        // clang-format off
        std::vector<glm::vec3> const normals{
            // Front face
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f},

            // Back face (all normals point in -z direction)
            {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f, -1.0f},
            {0.0f, 0.0f, -1.0f}
        };
        // clang-format on

        // clang-format off
        std::vector<glm::vec4> const colors{
            // Front face
            {1.0f, 0.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 0.0f, 1.0f},
            {0.0f, 0.0f, 1.0f, 1.0f},
            {1.0f, 1.0f, 0.0f, 1.0f},
            // Back face
            {1.0f, 0.0f, 1.0f, 1.0f},
            {0.0f, 1.0f, 1.0f, 1.0f},
            {0.5f, 0.5f, 0.5f, 1.0f},
            {1.0f, 0.5f, 0.0f, 1.0f}
        };
        // clanf-format on

        // Assuming we're looking from the front up
        // clang-format off
        std::vector<std::uint32_t> const indices{
            // Front face
            0, 3, 1, 1, 3, 2,
            // Back face
            5, 4, 7, 5, 6, 7,
            // Top face
            2, 7, 6, 2, 3, 7,
            // Bottom Face
            1, 4, 0, 1, 5, 4,
            // Left face
            0, 4, 3, 4, 7, 3,
            // Right face
            1, 2, 5, 5, 2, 6
        };
        // clang-format on


        DrawBufferBuilder buffer_builder;
        return buffer_builder.add_vertices(std::move(vertices))
            .add_normals(std::move(normals))
            .add_colors(std::move(colors))
            .add_indices(std::move(indices))
            .build();
    }


} // namespace wt

#endif // CUBE_H
