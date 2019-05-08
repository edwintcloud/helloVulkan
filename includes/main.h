//===================================================================
// File: main.h
//
// Desc: Main definitions for application.
//
// Copyright © 2019 Edwin Cloud. All rights reserved.
//
// * Attribution to Alexander Overvoorde at vulkan-tutorial.com *
// * for the general syntax, variable names and conventions.    *
//===================================================================

//-------------------------------------------------------------------
// Hash Defines
//-------------------------------------------------------------------

#define GLFW_INCLUDE_VULKAN

//-------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iostream>
#include <set>
#include <stdexcept>
#include <vector>

//-------------------------------------------------------------------
// Conditional Global Constants
//-------------------------------------------------------------------

#ifndef _DEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true; // Debug Only
#endif

//-------------------------------------------------------------------
// Global Constants
//-------------------------------------------------------------------

const int WIDTH = 800;  // GLFW Window Width
const int HEIGHT = 600; // GLFW Window Height
const std::vector<const char *> validationLayers = {
    "VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME};

//-------------------------------------------------------------------
// HelloTriangleApplication (Class Definition)
//-------------------------------------------------------------------
class HelloTriangleApplication {
public:
  //-----------------------------------------------------------------
  // HelloTriangleApplication - Public Methods
  //-----------------------------------------------------------------

  void run();

private:
  //-----------------------------------------------------------------
  // HelloTriangleApplication - Private Member Variables
  //-----------------------------------------------------------------
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VkDevice device;
  VkQueue graphicsQueue;
  VkQueue presentQueue;
  VkSurfaceKHR surface;
  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;
  std::vector<VkImageView> swapChainImageViews;

  //-----------------------------------------------------------------
  // HelloTriangleApplication - Private Member Substructures
  //-----------------------------------------------------------------

  struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    bool isComplete() {
      return graphicsFamily.has_value() && presentFamily.has_value();
    }
  };

  struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
  };

  //-----------------------------------------------------------------
  // HelloTriangleApplication - Static Methods
  //-----------------------------------------------------------------

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);
  static std::vector<char> readFile(const std::string &filename);

  //-----------------------------------------------------------------
  // HelloTriangleApplication - Private Methods
  //-----------------------------------------------------------------

  void initWindow();
  void initVulkan();
  void setupDebugMessenger();
  void createInstance();
  std::vector<const char *> getRequiredExtensions();
  void checkSupportedExtensions();
  bool checkValidationLayerSupport();
  void mainLoop();
  void cleanup();
  void pickPhysicalDevice();
  bool isDeviceSuitable(VkPhysicalDevice device);
  QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
  void createLogicalDevice();
  void createSurface();
  bool checkDeviceExtensionSupport(VkPhysicalDevice device);
  SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
  VkSurfaceFormatKHR chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  VkPresentModeKHR chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);
  void createSwapChain();
  void createImageViews();
  void createGraphicsPipeline();
  VkShaderModule createShaderModule(const std::vector<char> &code);
};