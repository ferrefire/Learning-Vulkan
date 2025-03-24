#include "ui.hpp"
#include "manager.hpp"

#include <iostream>

void UI::Create()
{
	UI::CreateContext();
}

/*static void check_vk_result(VkResult err)
{
	if (err == 0)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}*/

void UI::CreateContext()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO &io = ImGui::GetIO();
	io = &ImGui::GetIO();
	//(void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls
	io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;	  // Enable Docking
	//io->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;	  // Enable Multi-Viewport / Platform Windows
	// io->ConfigViewportsNoAutoMerge = true;
	// io->ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	//ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForVulkan(Manager::currentWindow.data, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
	init_info.Instance = Manager::currentGraphics.instance;
	init_info.PhysicalDevice = Manager::currentDevice.physicalDevice;
	init_info.Device = Manager::currentDevice.logicalDevice;
	init_info.QueueFamily = Manager::currentDevice.queueFamilies.graphicsFamily;
	init_info.Queue = Manager::currentDevice.graphicsQueue;
	init_info.PipelineCache = nullptr;
	init_info.DescriptorPool = Manager::currentDevice.descriptorPool;
	init_info.RenderPass = Manager::currentWindow.renderPass;
	init_info.Subpass = 0;
	init_info.MinImageCount = 2;
	init_info.ImageCount = 2;
	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.Allocator = nullptr;
	//init_info.CheckVkResultFn = check_vk_result;

	if (!ImGui_ImplVulkan_Init(&init_info))
	{
		std::cout << "error: imgui vulkan initiation failed" << std::endl;
	}

	if (io->BackendRendererUserData == nullptr)
	{
		std::cout << "error: no backend renderer user data" << std::endl;
	}

	std::cout << "imgui initialized" << std::endl;
}

void UI::Destroy()
{
	UI::DestroyContext();
}

void UI::DestroyContext()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void UI::Frame()
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	static float f = 0.0f;
	static int counter = 0;

	ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!" and append into it.

	ImGui::Text("This is some useful text.");		   // Display some text (you can use a format strings too)
	//ImGui::Checkbox("Demo Window", &show_demo_window); // Edit bools storing our window open/close state
	//ImGui::Checkbox("Another Window", &show_another_window);

	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);			 // Edit 1 float using a slider from 0.0f to 1.0f
	//ImGui::ColorEdit3("clear color", (float *)&clear_color); // Edit 3 floats representing a color

	if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
	ImGui::End();

	ImGui::Render();
	//ImDrawData *main_draw_data = ImGui::GetDrawData();
	//const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
	//wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
	//wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
	//wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
	//wd->ClearValue.color.float32[3] = clear_color.w;
	//if (!main_is_minimized)
	//	FrameRender(wd, main_draw_data);

	// Update and Render additional Platform Windows
	//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	//{
	//	ImGui::UpdatePlatformWindows();
	//	ImGui::RenderPlatformWindowsDefault();
	//}

	//// Present Main Platform Window
	//if (!main_is_minimized)
	//	FramePresent(wd);
}

void UI::RecordGraphicsCommands(VkCommandBuffer commandBuffer)
{
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}

ImGuiContext* UI::context = nullptr;
ImGuiIO* UI::io = nullptr;