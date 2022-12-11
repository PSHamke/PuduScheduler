#pragma once

#include "Walnut/Image.h"

#include <memory>
#include <glm/glm.hpp>

class Renderer
{
public:
	Renderer() = default;

	void OnResize(uint32_t width, uint32_t height);
	void Render();
	// 

	std::shared_ptr<Walnut::Image> GetFinalImage() const { return m_FinalImage; }
	void SetFinalImage(std::shared_ptr<Walnut::Image> image);
private:
	std::shared_ptr<Walnut::Image> m_FinalImage;
	uint32_t* m_ImageData = nullptr;
	uint8_t* data = nullptr;
};