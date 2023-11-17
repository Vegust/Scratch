#pragma once

#ifdef _WIN32
#include "Windows/windows_filesystem.h"
namespace filesystem = windows_filesystem;
#endif