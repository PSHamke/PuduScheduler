#include "Renderer.h"

#include "Walnut/Random.h"

#include "..\..\vendor\stb_image\stb_image.h"

void Renderer::OnResize(uint32_t width, uint32_t height)
{
	if (m_FinalImage)
	{
		// No resize necessary
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

void Renderer::Render()
{

	//m_FinalImage->SetData(m_ImageData);
	
}

void Renderer::SetFinalImage(std::shared_ptr<Walnut::Image> image)
{
	
	
}

