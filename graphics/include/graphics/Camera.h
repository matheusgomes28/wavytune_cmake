#include <glm/glm.hpp>

namespace wt
{
	class Camera
	{
	public:
		glm::mat4 transform;
		glm::vec3 getDirection() const;
		glm::vec3 getUp() const;
		glm::vec3 getRight() const;

	private:
		glm::vec3 pos;
		glm::vec3 front;
		glm::vec3 right;
		glm::vec3 up;
	};
} // namespace wt