#pragma once
#include <glm/glm.hpp>
#include <SFML/Window.hpp>

class Camera {
public:
	Camera(const glm::vec3& position, const glm::vec3& front, float yaw, float pitch);

	glm::mat4 View() const { return m_lookAt; }
	glm::mat4 Projection() const { return m_projection; }

	void Rotate(const sf::Vector2i& mouseDelta);
	void MoveForward(float dt);
	void MoveBackward(float dt);
	void MoveLeft(float dt);
	void MoveRight(float dt);
	void MoveUp(float dt);
	void MoveDown(float dt);

	const glm::mat4& GetProjection() const;
	const glm::mat4& GetLookAt() const;

	const glm::vec3& GetPosition() const { return m_position; }
	const glm::vec3& GetFront() const { return m_front; }


private:
	void RecreateLootAt();

	glm::mat4 m_projection;
	glm::mat4 m_lookAt;
	glm::vec3 m_position;
	glm::vec3 m_up;
	glm::vec3 m_front;
	glm::vec3 m_right;
	
	float m_yaw;
	float m_pitch;

	static glm::vec3 s_worldUp;
};