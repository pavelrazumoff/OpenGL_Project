#include "MainApp.h"

void MainApp::render()
{
	// Shadow mapping.
	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	glm::vec3 lightPos(30.0f, 20.0f, 10.0f);
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 10.0f, far_plane = 100.0f;

	if (useShadowMapping)
	{
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

		drawScene(simpleDepth_shader);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// reset viewport
		glViewport(0, 0, screenWidth, screenHeight);
	}

	// first render scene in specific framebuffer.
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we’re not using the stencil buffer now.

	// Draw Lamp.
	lamp_shader.use();

	glm::mat4 modelMat;
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)screenWidth / (float)screenHeight, 0.1f, 5000.0f);

	// Store uniform buffer data.
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindVertexArray(lightVAO);

	glm::vec3 lightColors[4];
	lightColors[0] = glm::vec3(2.0f, 2.0f, 2.0f);
	lightColors[1] = glm::vec3(1.5f, 0.0f, 0.0f);
	lightColors[2] = glm::vec3(0.0f, 0.0f, 1.5f);
	lightColors[3] = glm::vec3(0.0f, 1.5f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		modelMat = glm::mat4();
		modelMat = glm::translate(modelMat, pointLightPositions[i]);
		modelMat = glm::scale(modelMat, glm::vec3(0.2f));

		lamp_shader.setMat4("model", modelMat);
		lamp_shader.setVec3("lightColor", lightColors[i]);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	// Use our shader program when we want to render an object.
	basic_shader.use();

	// Wireframe mode.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - lightPos);

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

	basic_shader.setVec3("dirLight[0].direction", lightDir);
	basic_shader.setVec3("dirLight[0].ambient", 0.05f, 0.05f, 0.05f);
	basic_shader.setVec3("dirLight[0].diffuse", 1.0f, 1.0f, 1.0f);
	basic_shader.setVec3("dirLight[0].specular", 0.5f, 0.5f, 0.5f);

	for (int i = 0; i < 4; ++i)
	{
		std::string var = "pointLights[" + std::to_string(i) + "].";
		std::string param = var + "position";

		basic_shader.setVec3(param.c_str(), pointLightPositions[i]);
		param = var + "ambient";
		basic_shader.setVec3(param.c_str(), 0.05f, 0.05f, 0.05f);
		param = var + "diffuse";
		basic_shader.setVec3(param.c_str(), lightColors[i]);
		param = var + "specular";
		basic_shader.setVec3(param.c_str(), 1.0f, 1.0f, 1.0f);
		param = var + "constant";
		basic_shader.setFloat(param.c_str(), 1.0f);
		param = var + "linear";
		basic_shader.setFloat(param.c_str(), 0.09f);
		param = var + "quadratic";
		basic_shader.setFloat(param.c_str(), 0.032f);
	}

	basic_shader.setInt("dirCount", 1);
	basic_shader.setInt("pointCount", 4);
	basic_shader.setInt("spotCount", 0);

	basic_shader.setVec3("viewPos", camera.getPosition());
	basic_shader.setVec3("lightPos", lightPos);
	basic_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	basic_shader.setFloat("near_plane", near_plane);
	basic_shader.setFloat("far_plane", far_plane);

	glActiveTexture(GL_TEXTURE0);
	basic_shader.setInt("material.texture_diffuse1", 0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);

	if (useShadowMapping)
	{
		glActiveTexture(GL_TEXTURE4);
		basic_shader.setInt("shadowMap", 4);
		glBindTexture(GL_TEXTURE_2D, depthMap);
	}

	drawScene(basic_shader);

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	screen_shader.use();
	screen_shader.setBool("gammaCorrection", useGammaCorrection);
	screen_shader.setBool("useHDR", useHDR);
	screen_shader.setInt("useBloom", useBloom);
	screen_shader.setFloat("exposure", 1.0f);

	screen_shader.setInt("screenTexture", 0);
	screen_shader.setInt("bloomTexture", 1);

	glBindVertexArray(quadVAO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, finalTextures[0]);

	if (useBloom)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[!horizontal]);
	}
	glDrawArrays(GL_TRIANGLES, 0, 6);

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

void MainApp::drawScene(Shader shader)
{
	glm::mat4 modelMat;

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