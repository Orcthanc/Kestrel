#include "Core/Platform.hpp"

#ifdef _WIN32
Kestrel::Platform Kestrel::platform = Kestrel::Platform::Windows;
#elif defined __linux__
Kestrel::Platform Kestrel::platform = Kestrel::Platform::Linux;
#else
Kestrel::Platform Kestrel::platform = Kestrel::Platform::None;
#endif
Kestrel::RenderPath Kestrel::render_path = Kestrel::RenderPath::Vulkan;
