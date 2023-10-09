#pragma once

#ifdef WIN32

struct platform_windows {
	static float GetTime();
};

#endif