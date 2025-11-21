#ifndef CAMERA_H_
#define CAMERA_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
	Camera(glm::vec3 pos = glm::vec3(0.0f))
		: pitch_(0.0f), yaw_(-90.0f), fov_(45.0f), pos_(pos), front_(glm::vec3(0.0f, 0.0f, -1.0f)), right_(glm::vec3(1.0f, 0.0f, 0.0f)), up_(glm::vec3(0.0f, 1.0f, 0.0f)) {}

	glm::mat4 getLookAt() {
		return glm::lookAt(pos_, pos_ + front_, up_);
	}
	glm::mat4 getPerspective() {
		return glm::perspective(glm::radians(fov_), (float)800 / 600, 0.1f, 100.0f);
	}
	void angleProcess(float xoffset, float yoffset) {
		xoffset *= angle_speed_;
		yoffset *= angle_speed_;

		pitch_ += yoffset;
		yaw_ += xoffset;

		front_.x = cos(glm::radians(pitch_)) * cos(glm::radians(yaw_));
		front_.y = sin(glm::radians(pitch_));
		front_.z = cos(glm::radians(pitch_)) * sin(glm::radians(yaw_));

		front_ = glm::normalize(front_);
		right_ = glm::normalize(glm::cross(front_, world_up_));
		up_ = glm::normalize(glm::cross(right_, front_));
	}

private:
	float pitch_;
	float yaw_;
	float fov_;
	glm::vec3 pos_;
	glm::vec3 front_;
	glm::vec3 right_;
	glm::vec3 up_;
	static constexpr glm::vec3 world_up_ = glm::vec3(0.0f, 1.0f, 0.0f);
	static constexpr float angle_speed_ = 0.1f;
	static constexpr float movement_speed_ = 2.5f;
};

#endif  // CAMERA_H_
