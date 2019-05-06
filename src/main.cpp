//===================================================================
// File: main.cpp
//
// Desc: Main entry point for application.
//
// Copyright © 2019 Edwin Cloud. All rights reserved.
//
// * Attribution to Alexander Overvoorde at vulkan-tutorial.com *
// * for the general syntax, variable names and conventions.    *
//===================================================================

//-------------------------------------------------------------------
// Includes
//-------------------------------------------------------------------
#include "../includes/main.h"

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
// HelloTriangleApplication ( Public Class Methods)
//-------------------------------------------------------------------

// Runs application.
void HelloTriangleApplication::run() {
  initWindow();
  initVulkan();
  mainLoop();
  cleanup();
}

//-----------------------------------------------------------------
// HelloTriangleApplication (Static Class Methods)
//-----------------------------------------------------------------

// Prints validation layer debug messages to stdout.
// ~Returns: 0
VKAPI_ATTR VkBool32 VKAPI_CALL HelloTriangleApplication::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) {
  std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

  return VK_FALSE;
}

//-----------------------------------------------------------------
// HelloTriangleApplication (Private Class Methods)
//-----------------------------------------------------------------

// Initializes GLFW window.
void HelloTriangleApplication::initWindow() {

  // initialize glfw
  glfwInit();

  // set glfw window options
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  // create glfw window
  window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

// Initializes Vulkan instance.
void HelloTriangleApplication::initVulkan() {
  createInstance();
  setupDebugMessenger();
  createSurface();
  pickPhysicalDevice();
  createLogicalDevice();
  createSwapChain();
}

// Sets up debug messenger extension.
void HelloTriangleApplication::setupDebugMessenger() {
  if (!enableValidationLayers)
    return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
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
void HelloTriangleApplication::createInstance() {
  // Check and make sure required validation layers are available
  // if in debug mode
  if (enableValidationLayers && !checkValidationLayerSupport()) {
    throw std::runtime_error("validation layers requested, but not available!");
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
std::vector<const char *> HelloTriangleApplication::getRequiredExtensions() {
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
void HelloTriangleApplication::checkSupportedExtensions() {
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
bool HelloTriangleApplication::checkValidationLayerSupport() {
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
void HelloTriangleApplication::mainLoop() {
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
  }
}

// Cleans up after GLFW window has been closed.
void HelloTriangleApplication::cleanup() {
  vkDestroySwapchainKHR(device, swapChain, nullptr);
  vkDestroyDevice(device, nullptr);
  if (enableValidationLayers) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }
  vkDestroySurfaceKHR(instance, surface, nullptr);
  vkDestroyInstance(instance, nullptr);
  glfwDestroyWindow(window);
  glfwTerminate();
}

// Selects a graphics device that supports needed features.
void HelloTriangleApplication::pickPhysicalDevice() {
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
  if (deviceCount == 0) {
    throw std::runtime_error("failed to find GPUs with Vulkan support!");
  }

  // create vector of available devices
  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // find the first suitable device
  for (const VkPhysicalDevice &device : devices) {
    if (isDeviceSuitable(device)) {
      physicalDevice = device;
      break;
    }
  }

  // throw exception if no suitable device found
  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("failed to find a suitable GPU!");
  }
}

// Checks to see if a specified physical graphics device is suitable for the
// application to use.
// ~Returns: true if device is suitable, false otherwise
bool HelloTriangleApplication::isDeviceSuitable(VkPhysicalDevice device) {

  // // get device properties
  // VkPhysicalDeviceProperties deviceProperties;
  // vkGetPhysicalDeviceProperties(device, &deviceProperties);

  // // get device features
  // VkPhysicalDeviceFeatures deviceFeatures;
  // vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

  QueueFamilyIndices indices = findQueueFamilies(device);

  bool extensionsSupported = checkDeviceExtensionSupport(device);

  bool swapChainAdequate = false;
  if (extensionsSupported) {
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
    swapChainAdequate = !swapChainSupport.formats.empty() &&
                        !swapChainSupport.presentModes.empty();
  }

  return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

// Looks up device queue families to ensure at least one supported
// VK_QUEUE_GRAPHICS_BIT.
// ~Returns: QueueFamilyIndices struct with optional graphicsFamily.
HelloTriangleApplication::QueueFamilyIndices
HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device) {
  QueueFamilyIndices indices;

  // retreive list of queue families
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount,
                                           queueFamilies.data());

  // find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT
  // and a device that supports presenting to window surface
  int i = 0;
  for (const VkQueueFamilyProperties &queueFamily : queueFamilies) {
    if (queueFamily.queueCount > 0 &&
        queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      indices.graphicsFamily = i;
    }
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
    if (queueFamily.queueCount > 0 && presentSupport) {
      indices.presentFamily = i;
    }
    if (indices.isComplete()) {
      break;
    }
    i++;
  }

  return indices;
}

// Creates a logical device using the available physical graphics device.
void HelloTriangleApplication::createLogicalDevice() {
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(),
                                            indices.presentFamily.value()};

  // assign a queue priority to influence scheduling of command buffer execution
  // (0.0f-1.0f)
  float queuePriority = 1.0f;
  for (uint32_t queueFamily : uniqueQueueFamilies) {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueCreateInfo);
  }

  // specify set of device features to use
  VkPhysicalDeviceFeatures deviceFeatures = {};

  // configure logical device
  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pEnabledFeatures = &deviceFeatures;
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(deviceExtensions.size());
  createInfo.ppEnabledExtensionNames = deviceExtensions.data();

  // enable validation layers if in debug
  // in newer versions of Vulkan these settings are completely ignored for
  // logical devices, however older versions of Vulkan supported this so these
  // settings are for backwards compatibility with previous versions
  if (enableValidationLayers) {
    createInfo.enabledLayerCount =
        static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    createInfo.enabledLayerCount = 0;
  }

  // create the logical device, or throw exception
  if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create logical device!");
  }

  // retreive queue handles for single queue family with logical device -
  // this essentially registers a graphics queue with the logical device
  vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);

  // register present queue with logical device
  vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

// Creates a window surface, establishing a connection between the Vulkan API
// and GLFW window.
void HelloTriangleApplication::createSurface() {
  if (glfwCreateWindowSurface(instance, window, nullptr, &surface) !=
      VK_SUCCESS) {
    throw std::runtime_error("failed to create window surface!");
  }
}

// Checks that required extensions are available to be used by the physical
// graphics device.
// ~Returns: true if all required extensions are available, false otherwise.
bool HelloTriangleApplication::checkDeviceExtensionSupport(
    VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(deviceExtensions.begin(),
                                           deviceExtensions.end());

  for (const VkExtensionProperties &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();

  return true;
}

// Queries device for supported swap chain details.
// ~Returns: SwapChainSupportDetails struct with swap chain support details.
HelloTriangleApplication::SwapChainSupportDetails
HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device) {
  SwapChainSupportDetails details;

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
                                            &details.capabilities);

  // query surface formats(color depth)
  uint32_t formatCount;
  vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

  if (formatCount != 0) {
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
                                         details.formats.data());
  }

  // query presentation modes(conditions for "swapping" images to the screen)
  uint32_t presentModeCount;
  vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
                                            nullptr);

  if (presentModeCount != 0) {
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        device, surface, &presentModeCount, details.presentModes.data());
  }

  return details;
}

// Returns an appropriate surface format for the swap chain.
VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(
    const std::vector<VkSurfaceFormatKHR> &availableFormats) {

  if (availableFormats.size() == 1 &&
      availableFormats[0].format == VK_FORMAT_UNDEFINED) {
    return {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR};
  }

  for (const auto &availableFormat : availableFormats) {
    if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
        availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
      return availableFormat;
    }
  }

  return availableFormats[0];
}

// Returns an appropriate swap presentation mode.
VkPresentModeKHR HelloTriangleApplication::chooseSwapPresentMode(
    const std::vector<VkPresentModeKHR> &availablePresentModes) {
  // FIFO is a first-in-first out queue mode (basically vertical sync)
  VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

  // try to use triple-buffering, or fallback to immediate rendering for
  // compatibility
  for (const auto &availablePresentMode : availablePresentModes) {
    if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
      return availablePresentMode;
    } else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      bestMode = availablePresentMode;
    }
  }

  return bestMode;
}

// Returns the best resolution of images for the swap chain based on current
// window size.
VkExtent2D HelloTriangleApplication::chooseSwapExtent(
    const VkSurfaceCapabilitiesKHR &capabilities) {
  if (capabilities.currentExtent.width !=
      std::numeric_limits<uint32_t>::max()) {
    return capabilities.currentExtent;
  } else {
    VkExtent2D actualExtent = {WIDTH, HEIGHT};

    actualExtent.width = std::max(
        capabilities.minImageExtent.width,
        std::min(capabilities.minImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(
        capabilities.minImageExtent.height,
        std::min(capabilities.minImageExtent.height, actualExtent.height));

    return actualExtent;
  }
}

void HelloTriangleApplication::createSwapChain() {
  SwapChainSupportDetails swapChainSupport =
      querySwapChainSupport(physicalDevice);

  VkSurfaceFormatKHR surfaceFormat =
      chooseSwapSurfaceFormat(swapChainSupport.formats);
  VkPresentModeKHR presentMode =
      chooseSwapPresentMode(swapChainSupport.presentModes);
  VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

  // min number of images for swap chain buffering
  uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
  if (swapChainSupport.capabilities.maxImageCount > 0 &&
      imageCount > swapChainSupport.capabilities.maxImageCount) {
    imageCount = swapChainSupport.capabilities.maxImageCount;
  }

  // configure swap chain
  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat.format;
  createInfo.imageColorSpace = surfaceFormat.colorSpace;
  createInfo.imageExtent = extent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  // specify how to handle swap chain images used across multiple queue families
  // (ie. graphics family queue is different from presentation queue)
  QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
  uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(),
                                   indices.presentFamily.value()};
  if (indices.graphicsFamily != indices.presentFamily) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  // apply transformations if supported
  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

  // ignore alpha channel for blending with other windows
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

  // set present mode and enable clipping for images that go outside the window
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;

  // in case window in resizes, recreate swap chain (pass in handle of null as
  // old swap chain)
  createInfo.oldSwapchain = VK_NULL_HANDLE;

  // create the swap chain
  if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) !=
      VK_SUCCESS) {
    throw std::runtime_error("unable to create swap chain!");
  }

  // get swap chain images
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
  swapChainImages.resize(imageCount);
  vkGetSwapchainImagesKHR(device, swapChain, &imageCount,
                          swapChainImages.data());

  // set swap chain image format and extent
  swapChainImageFormat = surfaceFormat.format;
  swapChainExtent = extent;
}

//-----------------------------------------------------------------
// Main Function of Application
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