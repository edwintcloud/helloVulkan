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
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
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

  //-----------------------------------------------------------------
  // HelloTriangleApplication - Static Methods
  //-----------------------------------------------------------------

  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData);

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
};