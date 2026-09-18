#include "Renderer.h"
#include "Walnut/Random.h"

#include <algorithm>
#include <execution>
#include <limits>
#include <numeric>

namespace Utils {

	static uint32_t ConvertToRGBA(const glm::vec4& color)
	{
		uint8_t r = (uint8_t)(color.r * 255.0f);
		uint8_t g = (uint8_t)(color.g * 255.0f);
		uint8_t b = (uint8_t)(color.b * 255.0f);
		uint8_t a = (uint8_t)(color.a * 255.0f);

		return (a << 24) | (b << 16) | (g << 8) | r;
	}

}

void Renderer::Render(const Scene& scene)
{
	uint32_t width = m_FinalImage->GetWidth();
	uint32_t height = m_FinalImage->GetHeight();

	// One entry per row so std::for_each can hand whole scanlines to worker threads.
	std::vector<uint32_t> rowIndices(height);
	std::iota(rowIndices.begin(), rowIndices.end(), 0);

	std::for_each(std::execution::par, rowIndices.begin(), rowIndices.end(),
		[this, &scene, width, height](uint32_t y)
		{
			for (uint32_t x = 0; x < width; x++)
			{
				glm::vec2 coord = { (float)x / (float)width , (float)y / (float)height };
				coord = coord * 2.0f - 1.0f;

				glm::vec4 color = PerPixel(scene, coord);
				color = glm::clamp(color, glm::vec4(0.0f), glm::vec4(1.0f));
				m_ImageData[x + y * width] = Utils::ConvertToRGBA(color);
			}
		});

	m_FinalImage->SetData(m_ImageData);//upload this to gpu
}

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		//No resize necessary
		if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
			return;

		m_FinalImage->Resize(width, height);

	}
	else
	{
		m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
	}

	delete[] m_ImageData;
	m_ImageData = new uint32_t[width * height];
}

std::shared_ptr<Walnut::Image> Renderer::GetFinalImage()
{
	return m_FinalImage;
}

glm::vec4 Renderer::PerPixel(const Scene& scene, glm::vec2 coord)
{
	glm::vec3 rayOrigin(0.0f, 0.0f, 2.0f);
	glm::vec3 rayDirection = glm::normalize(glm::vec3(coord.x, coord.y, -1.0f));

	HitPayload payload = TraceRay(scene, rayOrigin, rayDirection);
	if (payload.HitDistance < 0.0f)
		return glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	const Sphere& sphere = scene.Spheres[payload.ObjectIndex];

	glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
	float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir), 0.0f);

	glm::vec3 color = sphere.Albedo * lightIntensity;
	return glm::vec4(color, 1.0f);
}

Renderer::HitPayload Renderer::TraceRay(const Scene& scene, const glm::vec3& rayOrigin, const glm::vec3& rayDirection)
{
	//(bx^2 + by^2)t^2 + 2(axbx + ayby)t + ( ax^2 + ay^2 - r^2) = 0
	//a = ray origin
	//b = ray direction
	//r = radius
	//t = hit distance

	int closestSphere = -1;
	float closestHitDistance = std::numeric_limits<float>::max();

	for (size_t i = 0; i < scene.Spheres.size(); i++)
	{
		const Sphere& sphere = scene.Spheres[i];
		glm::vec3 origin = rayOrigin - sphere.Position;

		float a = glm::dot(rayDirection, rayDirection);
		float b = 2.0f * glm::dot(origin, rayDirection);
		float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

		//Quadratic formula discriminant: b^2 - 4ac
		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
			continue;

		//Nearest root (nearest hit distance along the ray)
		float closestT = (-b - glm::sqrt(discriminant)) / (2.0f * a);
		if (closestT > 0.0f && closestT < closestHitDistance)
		{
			closestHitDistance = closestT;
			closestSphere = (int)i;
		}
	}

	if (closestSphere < 0)
		return Miss();

	return ClosestHit(scene, rayOrigin, rayDirection, closestHitDistance, closestSphere);
}

Renderer::HitPayload Renderer::ClosestHit(const Scene& scene, const glm::vec3& rayOrigin, const glm::vec3& rayDirection, float hitDistance, int objectIndex)
{
	HitPayload payload;
	payload.HitDistance = hitDistance;
	payload.ObjectIndex = objectIndex;

	const Sphere& closestSphere = scene.Spheres[objectIndex];
	glm::vec3 origin = rayOrigin - closestSphere.Position;

	payload.WorldPosition = origin + rayDirection * hitDistance;
	payload.WorldNormal = glm::normalize(payload.WorldPosition);

	payload.WorldPosition += closestSphere.Position;

	return payload;
}

Renderer::HitPayload Renderer::Miss()
{
	HitPayload payload;
	payload.HitDistance = -1.0f;
	return payload;
}
