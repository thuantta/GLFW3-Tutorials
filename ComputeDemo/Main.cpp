// ComputeDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "GLWin.h"
#include "RenderSample.h"
#include "ComputeShaders.h"

int main()
{
	IGLWin* test = IGLWin::Create();
	RenderSample* sample = RenderSample::Create();
	ComputeShaders* css = ComputeShaders::Create();

	glm::ivec2 imageSize(256, 256);//this magic number is the size of sample->m_textureId

	//create buffers
	std::vector<GLuint> createdBuffers;
	css->compute_extract.CreateBuffers(glm::ivec3(imageSize.x, imageSize.y, 1), createdBuffers);

	std::vector<float> kbuf = { 77.f, 88.f, 99.f, 77.f, 88.f, 99.f, 77.f, 88.f, 99.f };
	css->compute_extract.SetUniformData3x3("K", kbuf);
	css->compute_extract.SetUniformData3x3("D", kbuf);

	//some dummy data to upload..
	std::vector<float> upData(imageSize.x*imageSize.y);
	for (int i = 0; i < upData.size(); i++){
		upData[i] = (float)(upData.size()-i)/(float)upData.size(); // 0.f~1.f
	}

	//set dispatch size to worker/pixel
	css->compute_extract.dispatchDimensionX = imageSize.x;
	css->compute_extract.dispatchDimensionY = imageSize.y;
	css->compute_extract.dispatchDimensionZ = 1;


	css->compute_extract.SetUniformImg("img_output", sample->m_textureId);
	assert(glGetError() == 0);

	ComputeHelper::UploadDataToBuffer(css->compute_extract.myBoundBufferInfo[0].Id, upData);
	css->compute_extract.Dispatch();

	std::vector <float> dumpTest;
	css->compute_extract.Dump(1, dumpTest);

	// set OpenGL Options:
	glm::ivec2 dim = test->getDimension();
	glViewport(0, 0, dim[0], dim[1]);
	glClearColor(0, 0, 0.25f, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	while (1)
	{
		// Keep Running!
		// get delta time for this iteration:
		float fDeltaTime = (float)glfwGetTime();

		// clear the backbuffer to our clear colour and clear the depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		css->compute_extract.SetUniformImg("img_output", sample->m_textureId);
		css->compute_extract.Dispatch();

		glm::mat4 identity;
		sample->g_ModelMatrix = glm::rotate(identity, fDeltaTime * 10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		sample->Render(test->getProjection(), test->getView());
		test->SwapBuffers();

		// join second render thread
		glfwPollEvents(); // process events!
	}
    return 0;
}

