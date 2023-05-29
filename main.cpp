// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

/*
Copyright 2021-2023 Stephane Cuillerdier (aka aiekick)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// dear imgui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)

#include "imgui.h"
#include "imgui_internal.h"

#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <Gui/MainFrame.h>
#include <Res/CustomFont.cpp>
#include <Res/CustomFont2.cpp>
#include <Res/Roboto_Medium.cpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ImGuiFileDialog/ImGuiFileDialog.h>
#include <ctools/FileHelper.h>
#include <ctools/Logger.h>
#include <Headers/Build.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static void SetConsoleVisibility(bool vShow)
{
	if (vShow)
	{
		// on cache la console
		// on l'affichera au besoin comme blender fait
#ifdef WIN32
		ShowWindow(GetConsoleWindow(), SW_SHOW);
#endif
	}
	else
	{
		// on cache la console
		// on l'affichera au besoin comme blender fait
#ifdef WIN32
		ShowWindow(GetConsoleWindow(), SW_HIDE);
#endif
	}
}

int main(int /*argc*/, char** argv)
{
#ifdef _MSC_VER
	// active memory leak detector
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	_wsetlocale(LC_ALL, L".UTF8");
	setlocale(LC_ALL, ".UTF8");
	setlocale(LC_NUMERIC, "C");

	FileHelper::Instance()->SetAppPath(std::string(argv[0])); 
#ifdef _DEBUG
	#ifdef _MSC_VER
		auto ps = FileHelper::Instance()->ParsePathFileName(FileHelper::Instance()->GetAppPath());
		if (ps.isOk)
		{
			std::string path = ps.path;
			size_t buildPos = path.find("build");
			if (buildPos != std::string::npos)
			{
				path = path.substr(0, buildPos);
				FileHelper::Instance()->SetAppPath(path);
				FileHelper::Instance()->SetCurDirectory(path/* + "/bin"*/);
			}
		}
	#endif // _MSC_VER
#else // _DEBUG
	FileHelper::Instance()->SetCurDirectory(FileHelper::Instance()->GetAppPath());
#endif // _DEBUG

	LogVarDebug("App path : %s", FileHelper::AppPath.c_str());

#ifdef _DEBUG
	SetConsoleVisibility(true);
#else
	SetConsoleVisibility(true);
#endif
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

	// Create window with graphics context
	MainFrame::sMainWindow = glfwCreateWindow(1500, 600, "DBCHelper", NULL, NULL);
	if (MainFrame::sMainWindow == NULL)
		return 1;
	glfwMakeContextCurrent(MainFrame::sMainWindow);
	glfwSwapInterval(1); // Enable vsync

	// Initialize OpenGL loader
	bool err = (gladLoadGL() == 0);
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Viewport
	io.FontAllowUserScaling = true; // activate zoom feature with ctrl + mousewheel
#ifdef USE_DECORATIONS_FOR_RESIZE_CHILD_WINDOWS
	io.ConfigViewportsNoDecoration = false; // toujours mettre une frame au fenetre enfant
#endif
	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(MainFrame::sMainWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	//ImGui::GetIO().Fonts->AddFontDefault();
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_RM, 15.0f);
	static ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	static const ImWchar icons_ranges[] = { ICON_MIN_NDP, ICON_MAX_NDP, 0 };
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDP, 15.0f, &icons_config, icons_ranges);
	static const ImWchar icons_ranges2[] = { ICON_MIN_NDP2, ICON_MAX_NDP2, 0 };
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_NDP2, 15.0f, &icons_config, icons_ranges2);

	ImGui::StyleColorsDark();
	static std::string filePathName = "";
	static std::string filePath = "";
	static std::string filter = "";
	static std::string userDatas = "";
	static std::vector<std::pair<std::string, std::string>> selection = {};

	if (MainFrame::Instance()->Init())
	{
		int display_w, display_h;
		ImVec2 pos, size;
		while (!glfwWindowShouldClose(MainFrame::sMainWindow))
		{
			// maintain active, prevent user change via imgui dialog
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

			glfwGetFramebufferSize(MainFrame::sMainWindow, &display_w, &display_h);
			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				ImGuiViewport* viewport = ImGui::GetMainViewport();
				if (viewport)
				{
					pos = viewport->WorkPos;
					size = viewport->WorkSize;
				}
			}
			else
			{
				pos = ImVec2(0, 0);
				size = ImVec2((float)display_w, (float)display_h);
			}
			MainFrame::Instance()->Display(pos, size);

			ImGui::Render();

			glViewport(0, 0, display_w, display_h);
			const auto& bg_color = ImGui::GetStyle().Colors[ImGuiCol_WindowBg];
			glClearColor(bg_color.x, bg_color.y, bg_color.z, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
			//  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow* backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwSwapBuffers(MainFrame::sMainWindow);
		}
	}

	MainFrame::Instance()->Unit();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(MainFrame::sMainWindow);
	glfwTerminate();

	return 0;
}
