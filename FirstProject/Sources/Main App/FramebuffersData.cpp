#include "MainApp.h"

void MainApp::generateFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useMultisampling, bool useHDR)
{
	glGenFramebuffers(1, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	// generate texture.
	glGenTextures(1, texBuffer);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	if (!useMultisampling)
	{
		glBindTexture(GL_TEXTURE_2D, *texBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glBindTexture(GL_TEXTURE_2D, 0);
		// attach it to currently bound framebuffer object.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *texBuffer, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, *texBuffer);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, dataFormat, screenWidth, screenHeight, GL_TRUE);

		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
		// attach it to currently bound framebuffer object.
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, *texBuffer, 0);
	}

	// generate Renderbuffer.
	glGenRenderbuffers(1, RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, *RBO);

	if (!useMultisampling)
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	else
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach it to currently bound framebuffer object.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, *RBO);

	// check for complition.
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainApp::generateBloomFramebuffer(unsigned int* FBO, unsigned int* texBuffer, unsigned int* RBO, bool useHDR)
{
	glGenFramebuffers(1, FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, *FBO);

	glGenTextures(2, texBuffer);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, texBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texBuffer[i], 0);
	}

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, *RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, *RBO);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainApp::resizeFramebuffer(unsigned int FBO, unsigned int texBuffer, unsigned int RBO, bool useMultisampling, bool useHDR)
{
	// Resize framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	if (!useMultisampling)
	{
		glBindTexture(GL_TEXTURE_2D, texBuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (RBO != -1)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, RBO);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	else
	{
		glDeleteTextures(1, &texColorMSBuffer);
		glDeleteRenderbuffers(1, &rbo);
		glDeleteFramebuffers(1, &framebuffer);

		generateFramebuffer(&framebuffer, &texColorMSBuffer, &rbo, useMultisampling, useHDR);
	}
}

void MainApp::resizeBloomFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO, bool useHDR)
{
	// Resize framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	GLenum dataFormat;
	if (useHDR)
		dataFormat = GL_RGB16F;
	else
		dataFormat = GL_RGB;

	for (int i = 0; i < 2; ++i)
	{
		glBindTexture(GL_TEXTURE_2D, texBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainApp::resizeGFramebuffer(unsigned int FBO, unsigned int texBuffer[], unsigned int RBO)
{
	// Resize framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	for (int i = 0; i < 3; ++i)
	{
		GLenum dataFormat;
		if (i < 2)
			dataFormat = GL_RGB16F;
		else
			dataFormat = GL_RGB;

		glBindTexture(GL_TEXTURE_2D, texBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, dataFormat, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainApp::resizeSsaoFramebuffer(unsigned int FBO, unsigned int texBuffer)
{
	// Resize framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glBindTexture(GL_TEXTURE_2D, texBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenWidth, screenHeight, 0, GL_RGB, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}