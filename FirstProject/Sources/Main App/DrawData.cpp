#include "MainApp.h"

void MainApp::render()
{
	float near_plane = 10.0f, far_plane = 100.0f;
	glm::mat4 lightSpaceMatrix;

	glm::mat4 modelMat;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 5000.0f);

	// Store uniform buffer data.
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// Note, that using ambient occlusion will increase perfomance drop near two times,
	// because it is not optimized to reuse G-buffer for later lighting calculations.
	if (useAmbientOcclusion)
	{
		//SSAO.
		// 1. geometry pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderGeometryPass.use();
		shaderGeometryPass.setBool("useInstances", false);

		drawScene(shaderGeometryPass, false);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 2. generate SSAO texture
		// ------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderSSAO.use();
		// Send kernel + rotation 
		for (unsigned int i = 0; i < 64; ++i)
			shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		shaderSSAO.setMat4("projection", projection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// 3. blur SSAO texture to remove noise
		// ------------------------------------
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		shaderSSAOBlur.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Drow some shadows.
	if (useShadowMapping)
	{
		// Shadow mapping.
		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;

		// note that if you use a perspective projection matrix you'll have to change the light position as the current light position isn't enough to reflect the whole scene.
		//lightProjection = glm::perspective(glm::radians(45.0f), (GLfloat)shadowWidth / (GLfloat)shadowHeight, near_plane, far_plane);
		lightProjection = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleDepth_shader.use();
		simpleDepth_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		simpleDepth_shader.setBool("useInstances", false);

		glViewport(0, 0, shadowWidth, shadowHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);

		drawScene(simpleDepth_shader, false);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, screenWidth, screenHeight);
	}

	// Draw scene as normal.
	// first render scene in specific framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we’re not using the stencil buffer now.

	// Use our shader program when we want to render an object.
	basic_shader.use();

	// Wireframe mode.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	basic_shader.setBool("useInstances", false);
	basic_shader.setBool("useShadowMapping", useShadowMapping);
	basic_shader.setFloat("heightScale", 0.1f);
	basic_shader.setBool("material.use_texture_diffuse", true);
	basic_shader.setBool("material.use_texture_specular", false);
	basic_shader.setBool("material.use_texture_normal", false);
	basic_shader.setBool("material.use_texture_height", false);

	basic_shader.setVec4("material.diffuse_color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	basic_shader.setVec4("material.specular_color", glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	basic_shader.setFloat("material.shininess", 32);

	basic_shader.setVec3("viewPos", camera.getPosition());
	basic_shader.setVec3("lightPos", lightPos);
	basic_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	basic_shader.setFloat("near_plane", near_plane);
	basic_shader.setFloat("far_plane", far_plane);

	// Link diffuse texture to be able to render textured primitives before render models.
	glActiveTexture(GL_TEXTURE0);
	basic_shader.setInt("material.texture_diffuse1", 0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);

	// Connect shadow map to the 4-th slot.
	if (useShadowMapping)
	{
		glActiveTexture(GL_TEXTURE4);
		basic_shader.setInt("shadowMap", 4);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}

	// Render scene as normal.
	drawScene(basic_shader, true);

	// draw skybox as last.
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content.

	skybox_shader.use();

	view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix.

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to default.

	// second render framebuffer as texture for post-processing.
	// blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture.
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
	glBlitFramebuffer(0, 0, screenWidth, screenHeight, 0, 0, screenWidth, screenHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	// Draw intermediate's screenTexture to the bloom framebuffer as normal.
	glBindFramebuffer(GL_FRAMEBUFFER, finalFBO); // back to default
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Here we have to render to final fbo with two outputs.
	final_shader.use();
	glBindVertexArray(quadVAO);
	glDisable(GL_DEPTH_TEST);

	final_shader.setInt("screenTexture", 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, screenTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Blur here.
	// 2. blur bright fragments with two-pass Gaussian Blur 
	// --------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	bool horizontal = true, first_iteration = true;

	// If we use bloom, blur rendered scene, if not pass further.
	if (useBloom)
	{
		unsigned int amount = 10;
		shaderBlur.use();

		for (unsigned int i = 0; i < amount; i++)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			shaderBlur.setInt("horizontal", horizontal);
			shaderBlur.setInt("image", 0);
			glBindTexture(GL_TEXTURE_2D, first_iteration ? finalTextures[1] : pingpongColorbuffers[!horizontal]);  // bind texture of other framebuffer (or scene if first iteration)

			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);

			horizontal = !horizontal;
			if (first_iteration)
				first_iteration = false;
		}
	}

	// Bind final source framebuffer and render finalTextures[0] for final post-processing.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	screen_shader.use();
	screen_shader.setBool("gammaCorrection", useGammaCorrection);
	screen_shader.setBool("useHDR", useHDR);
	screen_shader.setInt("useBloom", useBloom);
	screen_shader.setInt("useAmbientOcclusion", useAmbientOcclusion);
	screen_shader.setFloat("exposure", 1.0f);

	screen_shader.setInt("screenTexture", 0);
	screen_shader.setInt("bloomTexture", 1);
	screen_shader.setInt("ssaoTexture", 2);

	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, finalTextures[0]);

	if (useBloom)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	}

	if (useAmbientOcclusion)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
	}

	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Draw text.
	drawTextData();

	// Uncomment it for debugging purposes.
	// It will draw current depth map.
	/*
	debugDepthQuad.use();
	debugDepthQuad.setInt("depthMap", 0);
	debugDepthQuad.setFloat("near_plane", near_plane);
	debugDepthQuad.setFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);*/

	glEnable(GL_DEPTH_TEST);
}

void MainApp::drawScene(Shader shader, bool finalDraw)
{
	glm::mat4 modelMat;

	// Draw Lamp.
	Shader currentShader = shader;
	if (finalDraw)
		currentShader = lamp_shader;
	currentShader.use();

	glBindVertexArray(lightVAO);

	for (int i = 0; i < 4; ++i)
	{
	    modelMat = glm::mat4();
		modelMat = glm::translate(modelMat, pointLightPositions[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.2f));

		currentShader.setMat4("model", modelMat);
		currentShader.setVec3("lightColor", lightColors[i]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	shader.use();

	// Draw scene.
	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(10.0f, -5.0f, -10.0f));
	modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(100.0f, 100.0f, 0.1f));

	glBindVertexArray(floorVAO);

	shader.setMat4("model", modelMat);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(10.0f, -4.5f, 10.0f));
	modelMat = glm::scale(modelMat, glm::vec3(1.0f, 1.0f, 1.0f));

	shader.setMat4("model", modelMat);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	// trans - rotate - scale.
	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(5.0f, -5.0f, -10.0f));
	modelMat = glm::rotate(modelMat, glm::radians(-180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(0.05f, 0.05f, 0.05f));

	shader.setMat4("model", modelMat);

	model.Draw(shader);

	// draw planet
	modelMat = glm::mat4();
	modelMat = glm::translate(modelMat, glm::vec3(-20.0f, -3.0f, 0.0f));
	modelMat = glm::scale(modelMat, glm::vec3(4.0f, 4.0f, 4.0f));
	shader.setMat4("model", modelMat);
	planet.Draw(shader);

	rock.Draw(shader, amount);
}

void MainApp::drawTextData()
{
	GLint prevBlendFunc;
	glGetIntegerv(GL_BLEND_SRC_ALPHA, &prevBlendFunc);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(screenWidth), 0.0f, static_cast<GLfloat>(screenHeight));

	font_shader.use();
	font_shader.setMat4("projection", projection);

	std::string textArray[] = {
		"Multisampling",
		"GammaCorrection",
		"ShadowMapping",
		"HDR",
		"Bloom",
		"AmbientOcclusion"
	};

	bool params[] = {
		useMultisampling,
		useGammaCorrection,
		useShadowMapping,
		useHDR,
		useBloom,
		useAmbientOcclusion
	};

	for (int i = 0; i < 6; ++i)
	{
		std::string out_text;
		
		if(params[i])
			out_text = " - disable ";
		else
			out_text = " - enable ";
		RenderText(font_shader, std::to_string(i + 1) + out_text + textArray[i], 25.0f, screenHeight - 100.0f - i * 25.0f, 1.0f, glm::vec3(0.0, 0.68f, 1.0f));
	}

	glBlendFunc(GL_SRC_ALPHA, prevBlendFunc);
	glDisable(GL_BLEND);
}

void MainApp::RenderText(Shader &s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	// Activate corresponding render state
	s.use();
	glUniform3f(glGetUniformLocation(s.getShaderProgram(), "textColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(fontVAO);
	// Iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
		{ xpos, ypos + h, 0.0, 0.0 },
		{ xpos, ypos, 0.0, 1.0 },
		{ xpos + w, ypos, 1.0, 1.0 },
		{ xpos, ypos + h, 0.0, 0.0 },
		{ xpos + w, ypos, 1.0, 1.0 },
		{ xpos + w, ypos + h, 1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}