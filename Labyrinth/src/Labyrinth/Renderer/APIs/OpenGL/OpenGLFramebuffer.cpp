#include "Lpch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>

namespace Labyrinth {

	static const uint32_t sMaxFramebufferSize = 8192;

	namespace Utils {

		static GLenum TextureTarget(bool multisampled)
		{
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count)
		{
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id)
		{
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
			}
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
		{
			bool multisampled = samples > 1;
			if (multisampled)
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static bool IsDepthFormat(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			}

			return false;
		}

		static GLenum LabyrinthFBTextureFormatToGL(FramebufferTextureFormat format)
		{
			switch (format)
			{
				case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
				case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			}

			LAB_CORE_ASSERT(false);
			return 0;
		}
	}

	OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpec& spec)
		: mSpecification(spec)
	{
		for (auto spec : mSpecification.attachments.attachments)
		{
			if (!Utils::IsDepthFormat(spec.textureFormat))
				mColourAttachmentSpecs.emplace_back(spec);
			else
				mDepthAttachmentSpec = spec;
		}

		Invalidate();
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		glDeleteFramebuffers(1, &mRendererID);
		glDeleteTextures(mColourAttachments.size(), mColourAttachments.data());
		glDeleteTextures(1, &mDepthAttachment);
	}

	void OpenGLFramebuffer::Invalidate()
	{
		if (mRendererID)
		{
			glDeleteFramebuffers(1, &mRendererID);
			glDeleteTextures(mColourAttachments.size(), mColourAttachments.data());
			glDeleteTextures(1, &mDepthAttachment);

			mColourAttachments.clear();
			mDepthAttachment = 0;
		}

		glCreateFramebuffers(1, &mRendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);

		bool multisample = mSpecification.samples > 1;

		// Attachments
		if (mColourAttachmentSpecs.size())
		{
			mColourAttachments.resize(mColourAttachmentSpecs.size());
			Utils::CreateTextures(multisample, mColourAttachments.data(), mColourAttachments.size());

			for (size_t i = 0; i < mColourAttachments.size(); i++)
			{
				Utils::BindTexture(multisample, mColourAttachments[i]);
				switch (mColourAttachmentSpecs[i].textureFormat)
				{
					case FramebufferTextureFormat::RGBA8:
						Utils::AttachColorTexture(mColourAttachments[i], mSpecification.samples, GL_RGBA8, GL_RGBA, mSpecification.width, mSpecification.height, i);
						break;
					case FramebufferTextureFormat::RED_INTEGER:
						Utils::AttachColorTexture(mColourAttachments[i], mSpecification.samples, GL_R32I, GL_RED_INTEGER, mSpecification.width, mSpecification.height, i);
						break;
				}
			}
		}

		if (mDepthAttachmentSpec.textureFormat != FramebufferTextureFormat::None)
		{
			Utils::CreateTextures(multisample, &mDepthAttachment, 1);
			Utils::BindTexture(multisample, mDepthAttachment);
			switch (mDepthAttachmentSpec.textureFormat)
			{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(mDepthAttachment, mSpecification.samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, mSpecification.width, mSpecification.height);
				break;
			}
		}

		if (mColourAttachments.size() > 1)
		{
			LAB_CORE_ASSERT(mColourAttachments.size() <= 4);
			GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(mColourAttachments.size(), buffers);
		}
		else if (mColourAttachments.empty())
		{
			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		LAB_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mRendererID);
		glViewport(0, 0, mSpecification.width, mSpecification.height);
	}

	void OpenGLFramebuffer::unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLFramebuffer::resize(uint32_t width, uint32_t height)
	{
		mSpecification.width = width;
		mSpecification.height = height;

		Invalidate();
	}

	int OpenGLFramebuffer::readPixel(uint32_t attachmentIndex, int x, int y)
	{
		LAB_CORE_ASSERT(attachmentIndex < mColourAttachments.size());

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
		return pixelData;
	}

	void OpenGLFramebuffer::clearAttachment(uint32_t attachmentIndex, int value)
	{
		LAB_CORE_ASSERT(attachmentIndex < mColourAttachments.size());

		auto& spec = mColourAttachmentSpecs[attachmentIndex];
		glClearTexImage(mColourAttachments[attachmentIndex], 0,
			Utils::LabyrinthFBTextureFormatToGL(spec.textureFormat), GL_INT, &value);
	}

	void OpenGLFramebuffer::bindColourAttachment(uint32_t index)
	{
		LAB_CORE_ASSERT(index < mColourAttachments.size(), "Binding attachment out of range!");

		bool multisample = mSpecification.samples > 1;
		Utils::BindTexture(multisample, mColourAttachments[index]);
	}
}