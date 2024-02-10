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
	
	bool TryLock() {
		if (!Flag.exchange(1, std::memory_order_acquire)) {
			return true;
		}
		return false;
	}

	void Unlock() {
		Flag.store(0, std::memory_order_release);
	}

	bool Locked() {
		return Flag.load(std::memory_order_acquire);
	}
};