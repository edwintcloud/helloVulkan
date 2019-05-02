//===================================================================
// File: main.cpp
//
// Desc: Entrypoint of the program.
//
// Copyright © 2019 Edwin Cloud. All rights reserved.
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
// Global Functions
//-------------------------------------------------------------------

// ProxyFunc: get running proc address and register debug messenger extension
VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
    const VkAllocationCallbacks *pAllocator,
    VkDebugUtilsMessengerEXT *pDebugMessenger) {
  auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkCreateDebugUtilsMessengerEXT");
  if (func != nullptr) {
    return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
  } else {
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }
}

// ProxyFunc: destroy debug messenger on program exit
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks *pAllocator) {
  auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
      instance, "vkDestroyDebugUtilsMessengerEXT");
  if (func != nullptr) {
    func(instance, debugMessenger, pAllocator);
  }
}

//-------------------------------------------------------------------
// HelloTriangleApplication (Class)
//-------------------------------------------------------------------
class HelloTriangleApplication {
public:
  //-----------------------------------------------------------------
  // HelloTriangleApplication - Public Functions
  //-----------------------------------------------------------------

  // Runs application.
  void run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
  }

private:
  //-----------------------------------------------------------------
  // HelloTriangleApplication - Private Member Variables
  //-----------------------------------------------------------------
  GLFWwindow *window;
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  //-----------------------------------------------------------------
  // HelloTriangleApplication - Static Member Functions
  //-----------------------------------------------------------------

  // Prints validation layer debug messages to stdout.
  // ~Returns: 0
  static VKAPI_ATTR VkBool32 VKAPI_CALL
  debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                VkDebugUtilsMessageTypeFlagsEXT messageType,
                const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                void *pUserData) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
  }

  //-----------------------------------------------------------------
  // HelloTriangleApplication - Public Functions
  //-----------------------------------------------------------------

  // Initializes GLFW window.
  void initWindow() {

    // initialize glfw
    glfwInit();

    // set glfw window options
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // create glfw window
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
  }

  // Initializes Vulkan instance.
  void initVulkan() {
    createInstance();
    setupDebugMessenger();
  }

  // Sets up debug messenger extension.
  void setupDebugMessenger() {
    if (!enableValidationLayers)
      return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr; // optional

    // create debug messenger
    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr,
                                     &debugMessenger) != VK_SUCCESS) {
      throw std::runtime_error("failed to set up debug messenger!");
    }
  }

  // Creates Vulkan instance.
  void createInstance() {
    // Check and make sure required validation layers are available
    // if in debug mode
    if (enableValidationLayers && !checkValidationLayerSupport()) {
      throw std::runtime_error(
          "validation layers requested, but not available!");
    }

    // Application Info (optional)
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Instance Info (required)
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // get needed extension info from glfw
    // uint32_t glfwExtensionCount = 0;
    // const char** glfwExtensions;
    // glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // set instance info to needed extensions
    // createInfo.enabledExtensionCount = glfwExtensionCount;
    // createInfo.ppEnabledExtensionNames = glfwExtensions;
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // check supported extensions
    // checkSupportedExtensions();

    // number of global validation layers
    if (enableValidationLayers) {
      createInfo.enabledLayerCount =
          static_cast<uint32_t>(validationLayers.size());
      createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
      createInfo.enabledLayerCount = 0;
    }

    // create the instance, ensuring if there is an error, we throw a runtime
    // exception
    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
      throw std::runtime_error("failed to create instance!");
    }
  }

  // Gets a list of required extensions needed.
  // ~Returns: Vector of extensions.
  std::vector<const char *> getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions =
        glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> extensions(glfwExtensions,
                                         glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers)
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
  }

  // Gets a list of supported extensions.
  void checkSupportedExtensions() {
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> extensions(extensionCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount,
                                           extensions.data());

    std::cout << "available extensions:" << std::endl;

    for (const auto &extension : extensions) {
      std::cout << "\t" << extension.extensionName << std::endl;
    }
  }

  // Check that configured validation layers are valid and supported.
  // ~Returns: false if configured layer is not found in available layers, true
  // otherwise.
  bool checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName : validationLayers) {
      bool layerFound = false;

      for (const auto &layerProperties : availableLayers) {
        if (strcmp(layerName, layerProperties.layerName) == 0) {
          layerFound = true;
          break;
        }
      }

      if (!layerFound)
        return false;
    }

    return true;
  }

  // Listens for events until GLFW window closes.
  void mainLoop() {
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();
    }
  }

  // Cleans up after GLFW window has been closed.
  void cleanup() {
    if (enableValidationLayers) {
      DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
  }
};

//-----------------------------------------------------------------
// Main Entrypoint
//-----------------------------------------------------------------
int main() {

  // create instance of triangle app
  HelloTriangleApplication app;

  // run the application -- safely checking for thrown exceptions
  try {
    app.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE; // return 1
  }

  // return 0
  return EXIT_SUCCESS;
}