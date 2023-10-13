#pragma once

#ifdef _WIN32
#include "Windows/windows_filesystem.h"
using filesystem = windows_filesystem;
#endif