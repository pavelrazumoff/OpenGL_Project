#include "MainApp.h"

void MainApp::loadShaders()
{
	basic_shader.load("Shaders//Basic//VertexShader.glsl", "Shaders//Basic//FragmentShader.glsl");
	//basic_shader.loadGeometryShader("Shaders//Basic//GeometryShader.glsl");
	simpleDepth_shader.load("Shaders//Basic//ShadowDepthVS.glsl", "Shaders//Basic//ShadowDepthFS.glsl");
	simpleDepth_shader.setUseOnlyDepth(true);
	debugDepthQuad.load("Shaders//Debug//DebugQuadVS.glsl", "Shaders//Debug//DebugQuadFS.glsl");

	lamp_shader.load("Shaders//Lamp//LampVS.glsl", "Shaders//Lamp//LampFS.glsl");
	screen_shader.load("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//ScreenShaderFS.glsl");
	skybox_shader.load("Shaders//Skybox//SkyboxShaderVS.glsl", "Shaders//Skybox//SkyboxShaderFS.glsl");
	final_shader.load("Shaders//Screen//ScreenShaderVS.glsl", "Shaders//Screen//FinalShaderFS.glsl");
	shaderBlur.load("Shaders//Blur//BlurShaderVS.glsl", "Shaders//Blur//BlurShaderFS.glsl");

	// ssao.
	shaderGeometryPass.load("Shaders//Basic//SSAO//SsaoShaderVS.glsl", "Shaders//Basic//SSAO//SsaoShaderFS.glsl");
	shaderSSAO.load("Shaders//SSAO//SsaoVS.glsl", "Shaders//SSAO//SsaoFS.glsl");
	shaderSSAOBlur.load("Shaders//SSAO//SsaoVS.glsl", "Shaders//SSAO//SsaoBlurFS.glsl");
	shaderLightingPass.load("Shaders//SSAO//SsaoVS.glsl", "Shaders//SSAO//SsaoLightingFS.glsl");
}

void MainApp::loadTextures()
{
	std::vector<std::string> faces = {
		"Images//Skybox//right.jpg",
		"Images//Skybox//left.jpg",
		"Images//Skybox//top.jpg",
		"Images//Skybox//bottom.jpg",
		"Images//Skybox//back.jpg",
		"Images//Skybox//front.jpg"
	};

	cubemapTexture = loadCubemap(faces, useGammaCorrection);

	woodTexture = TextureFromFile("floor.jpg", "Images", useGammaCorrection);
}