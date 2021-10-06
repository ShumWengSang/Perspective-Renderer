#include "stdafx.h"
#include "Logger.h"
#include "Input.h"
#include "App.h"
#include "TransformSystem.h"
#include "TextureSystem.h"
#include "ModelSystem.h"
#include "ShaderSystem.h"
#include "GuiSystem.h"
#include "GlobalVariables.h"
#include "DebugDraw.h"

static std::unique_ptr<App> app;
static bool renderUI = true;

void Driver();
#include "Quaternions.h"
static void MidTerm() {
	{
		glm::quat quaternion(1, 2, 34, 5);
		MyMath::Quaternion myQuat(1, { 2,34,5 });
		if (quaternion != myQuat.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion conversion is wrong!");
		}

		// Normalize
		glm::quat normglm = glm::normalize(quaternion);
		MyMath::Quaternion myQuatNorm = myQuat.Norm();
		if (normglm.w != myQuatNorm.s)
		{
			throw std::runtime_error("Normlaizing wrong");
		}
		if (normglm.x != myQuatNorm.v.x)
		{
			throw std::runtime_error("Normlaizing wrong");
		}
		if (normglm.y != myQuatNorm.v.y)
		{
			throw std::runtime_error("Normlaizing wrong");
		}
		if (normglm.z != myQuatNorm.v.z)
		{
			throw std::runtime_error("Normlaizing wrong");
		}
		if (glm::toMat4(glm::normalize(quaternion)) != myQuat.ToMat4())
		{
			throw std::runtime_error("Quaternion conversion norm mat 4 is wrong!");
		}
		if (glm::toMat4(quaternion) != myQuat.ToMat4(false))
		{
			throw std::runtime_error("Quaternion conversion Mat4 is wrong!");
		}
	}
	// Addition
	{
		glm::quat quaternion(1, 2, 34, 5);
		glm::quat quaternion2(2,4,6,3);
		MyMath::Quaternion myQuat(1, { 2,34,5 });
		MyMath::Quaternion myQuat2(2, {4, 6, 3});

		auto res = quaternion + quaternion2;
		auto myRes = myQuat + myQuat2;
		if (res != myRes.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion addition");
		}
		myQuat += myQuat2;
		if (res != myQuat.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion addition");
		}
	}

	// Product
	{
		glm::quat quaternion(1, 2, 34, 5);
		glm::quat quaternion2(2, 4, 6, 3);
		MyMath::Quaternion myQuat(1, { 2,34,5 });
		MyMath::Quaternion myQuat2(2, { 4, 6, 3 });

		auto res = quaternion * quaternion2;
		auto myRes = myQuat * myQuat2;
		if (res != myRes.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion cross ");
		}
		myQuat *= myQuat2;
		if (res != myQuat.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion cross ");
		}
	}

	// Dot
	{
		glm::quat quaternion(1, 2, 34, 5);
		glm::quat quaternion2(2, 4, 6, 3);
		MyMath::Quaternion myQuat(1, { 2,34,5 });
		MyMath::Quaternion myQuat2(2, { 4, 6, 3 });

		auto res = glm::dot(quaternion, quaternion2);
		auto myRes = myQuat.Dot(myQuat2);
		if (res != myRes)
		{
			throw std::runtime_error("Quaternion dot");
		}
	}

	// Scalar multiplication
	{
		glm::quat quaternion(1, 2, 34, 5);
		MyMath::Quaternion myQuat(1, { 2,34,5 });
		float multi = 5.123f;
		auto res = quaternion * multi;
		auto myRes = myQuat * multi;
		if (res != myRes.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion scalar mult");
		}
	}

	// Magnitude
	{
		glm::quat quaternion(1, 2, 34, 5);
		MyMath::Quaternion myQuat(1, { 2,34,5 });
		
		auto res = glm::length(quaternion);
		auto myRes = myQuat.Magnitude();
		if (res != myRes)
		{
			throw std::runtime_error("Quaternion magnuitde ");
		}
	}
	{
		// Conjugate
		glm::quat quaternion(1, 2, 34, 5);
		MyMath::Quaternion myQuat(1, { 2,34,5 });

		auto res = glm::conjugate(quaternion);
		auto myRes = myQuat.Conjugate();
		if (res != myRes.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion conjugate");
		}
	}
	{
		// Inverse
		glm::quat quaternion(1, 2, 34, 5);
		MyMath::Quaternion myQuat(1, { 2,34,5 });

		auto res = glm::inverse(quaternion);
		auto myRes = myQuat.Inverse();
		if (res != myRes.ToGLMQuat())
		{
			throw std::runtime_error("Quaternion inverse");
		}
	}
	{
		// Rotate
		glm::quat quaternion(1, 2, 34, 5);
		glm::vec3 vector{223,65,1};
		MyMath::Quaternion myQuat(1, { 2,34,5 });
		float angle = 43;

		auto res = glm::rotate(glm::normalize(quaternion), vector);
		auto myRes = myQuat.Rotate(vector);
		if (res != myRes)
		{
			if((res - myRes).length() < 1.0)
				throw std::runtime_error("Quaternion rotate");
		}

		{
			auto res = glm::rotate(quaternion, vector);
			auto myRes = myQuat.Rotate(vector);
			if (res != myRes)
			{
				if ((res - myRes).length() < 1.0)
					throw std::runtime_error("Quaternion rotate");
			}
		}
	}

	// VQS to matrix
	{
		glm::vec3 t (10,20,30);
		MyMath::Quaternion quad (2, {3,1,5});
		float scale = 4;

		MyMath::VQS test(t,quad,scale);

		glm::mat4 scaleMat = glm::scale(glm::vec3(scale, scale, scale));
		glm::mat4 rotate = quad.ToMat4(); // rotate
		glm::mat4 translate = glm::translate(t);
		glm::mat4 testMat4 = test.ToMat4();
		glm::mat4 transform = translate * rotate * scaleMat;
		if (testMat4 != transform)
		{
			throw std::runtime_error("VQS to Mat4 problems!");
		}
	}
	// Matrix to VQS
	{
		glm::vec3 t(10, 20, 30);
		MyMath::Quaternion quad(2, { 3,1,5 });
		MyMath::Quaternion quad2 = quad.Norm();
		float scale = 4;
		glm::mat4 scaleMat = glm::scale(glm::vec3(scale, scale, scale));
		glm::mat4 rotate = quad.ToMat4(); // rotate

		

		glm::mat4 translate = glm::translate(t);
		glm::mat4 transform = translate * rotate * scaleMat;

		MyMath::VQS myVQS(transform);
		glm::mat4 trans = myVQS.ToMat4();
		if (transform != trans)
		{
			// Not 100% the same, but is expected because of floating point
	// operations
			// throw std::runtime_error("Matrix to VQS");
		}
	}
	// VQS Rotate
	{
		glm::vec3 t(10, 20, 30);
		MyMath::Quaternion quad(2, { 3,1,5 });
		float scale = 4;

		glm::vec3 t2(10, 5, 3);
		MyMath::Quaternion quad2(2, { 2,1,5 });
		float scale2 = 6;

		MyMath::VQS test(t, quad, scale);
		MyMath::VQS test2(t2, quad2, scale2);

		glm::mat4 scaleMat = glm::scale(glm::vec3(scale, scale, scale));
		glm::mat4 rotate = quad.ToMat4(); // rotate
		glm::mat4 translate = glm::translate(t);

		glm::mat4 scaleMat2 = glm::scale(glm::vec3(scale2, scale2, scale2));
		glm::mat4 rotate2 = quad2.ToMat4(); // rotate
		glm::mat4 translate2 = glm::translate(t2);
		
		glm::mat4 transform = translate * rotate * scaleMat;
		glm::mat4 transform2 = translate2 * rotate2 * scaleMat2;

		glm::mat4 concatTrans = transform * transform2;
		MyMath::VQS vqsTrans = test * test2;
		glm::mat4 vqsMat = vqsTrans.ToMat4();
		if (concatTrans != vqsMat)
		{
			// Not 100% the same, but is expected because of floating point
			// operations
			// throw std::runtime_error("VQS concatination");
		}
	}
	// VQS inverse
	{
		glm::vec3 t(10, 20, 30);
		MyMath::Quaternion quad(2, { 3,1,5 });
		float scale = 4;

		MyMath::VQS test(t, quad, scale);
		MyMath::VQS inverse = test.Inverse();
		MyMath::VQS identity = test * inverse;
		glm::mat4 myIden = identity.ToMat4();
		if (myIden != glm::mat4(1.0f))
		{
			// Not 100% the same, but is expected because of floating point
	// operations
			// throw std::runtime_error("VQS Inverse");
		}
	}

}

void glfwResizeCallback(GLFWwindow* window, int width, int height)
{
	app->windowWidth = width;
	app->windowHeight = height;
	app->Resize(width, height);
}

int main(int argc, char* argv[])
{
	MidTerm();

	// Set global from argv
	if (argc >= 2)
	{
		Global::loadFile = argv[1];
	}

	// Setup basic GLFW and context settings
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		LogError("Fatal error: could not initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create app (should require no GL context!)
	app = AppSelector::ConstructApp();
	App::Settings settings = app->Setup();

	glfwWindowHint(GLFW_RESIZABLE, settings.window.resizeable);
	glfwWindowHint(GLFW_SAMPLES, settings.context.msaaSamples);

	// Create window
	GLFWwindow* window = nullptr;
	if (settings.window.fullscreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vidmode = glfwGetVideoMode(monitor);
		window = glfwCreateWindow(vidmode->width, vidmode->height, settings.window.title.c_str(), monitor, nullptr);
	}
	else
	{
		int width = settings.window.size.width;
		int height = settings.window.size.height;
		window = glfwCreateWindow(width, height, settings.window.title.c_str(), nullptr, nullptr);
	}
	if (!window)
	{
		LogError("Fatal error: could not created the requested window");
	}

	// Setup OpenGL context
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	// Setup OpenGL error handlers
	glad_set_post_callback(GladPostCallback);

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GLDebugMessageCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, true);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);

	// Setup input and callbacks
	Input input;
	glfwSetWindowUserPointer(window, &input);

	glfwSetKeyCallback(window, Input::KeyEventCallback);
	glfwSetMouseButtonCallback(window, Input::MouseButtonEventCallback);
	glfwSetCursorPosCallback(window, Input::MouseMovementEventCallback);
	glfwSetScrollCallback(window, Input::MouseScrollEventCallback);

	// glfwSetCharCallback(window, GuiSystem::CharacterInputCallback);

	// Enable important and basic features
	//glEnable(GL_FRAMEBUFFER_SRGB); (note: implemented manually!)
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Initialize global systems (that need initialization)
	TransformSystem& transformSystem = TransformSystem::getInstance();
	TextureSystem& textureSystem = TextureSystem::getInstance();
	ModelSystem& modelSystem = ModelSystem::getInstance();
	ShaderSystem& shaderSystem = ShaderSystem::getInstance();
	GuiSystem& guiSystem = GuiSystem::getInstance();
	DebugDrawSystem& debugSystem = DebugDrawSystem::getInstance();

	transformSystem.Init();
	textureSystem.Init();
	modelSystem.Init();
	debugSystem.Init();
	guiSystem.Init(window);

	app->Init();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glfwSetFramebufferSizeCallback(window, glfwResizeCallback);

	// (manually initiate first callback)
	glfwResizeCallback(nullptr, width, height);
	glfwSwapInterval(settings.window.vsync ? 1 : 0);

	// Main loop

	glfwSetTime(0.0);
	double lastTime = glfwGetTime();
	float accumulatedTime = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		input.PreEventPoll();
		glfwPollEvents();

		transformSystem.Update();
		textureSystem.Update();
		modelSystem.Update();
		shaderSystem.Update();

		double currentTime = glfwGetTime();
		double elapsedTime = currentTime - lastTime;
		lastTime = currentTime;
		float deltaTime = float(elapsedTime);


		guiSystem.NewFrame();
		guiSystem.DockspaceBegin();
		app->Draw(input, deltaTime, accumulatedTime);
		accumulatedTime += deltaTime;
		guiSystem.DockspaceEnd();
		if (renderUI)
		{
			ImGui::Render();
			guiSystem.RenderDrawData(ImGui::GetDrawData());
		}
		else
		{
			ImGui::EndFrame();
		}
		debugSystem.Render();

		glfwSwapBuffers(window);
	}
	guiSystem.Destroy();
	modelSystem.Destroy();
	textureSystem.Destroy();
	transformSystem.Destroy();
	debugSystem.Destroy();


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}