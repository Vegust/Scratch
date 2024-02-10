#pragma once

#include <atomic>
#include <thread>

struct spinlock {
private:
	std::atomic<bool> Flag{0};

public:
	void Lock() {
		while (Flag.exchange(1, std::memory_order_acquire)) {
			_mm_pause();
		}
	}

	void Unlock() {
		Flag.store(0, std::memory_order_release);
	}
};