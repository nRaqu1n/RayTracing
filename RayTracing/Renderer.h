#pragma once
#include "Walnut/Image.h"
#include<memory>
#include<vector>
#include<glm/glm.hpp>

struct Sphere
{
	glm::vec3 Position{ 0.0f };
	float Radius = 0.5f;
	glm::vec3 Albedo{ 1.0f };
};

struct Scene
{
	std::vector<Sphere> Spheres;
};

class Renderer
{
public:
	Renderer() = default;

	void Render(const Scene& scene);
	void OnResize(uint32_t width, uint32_t height);


	std::shared_ptr<Walnut::Image> GetFinalImage();

private:
	struct HitPayload
	{
		float HitDistance = -1.0f;
		glm::vec3 WorldPosition{ 0.0f };
		glm::vec3 WorldNormal{ 0.0f };
		int ObjectIndex = -1;
	};

	glm::vec4 PerPixel(const Scene& scene, glm::vec2 coord);
	HitPayload TraceRay(const Scene& scene, const glm::vec3& rayOrigin, const glm::vec3& rayDirection);
	HitPayload ClosestHit(const Scene& scene, const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float hitDistance, int objectIndex);
	HitPayload Miss();

private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
};
