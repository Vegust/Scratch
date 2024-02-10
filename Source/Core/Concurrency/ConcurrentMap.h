#pragma once

#include <limits>
#include <type_traits>
#include <cstdlib>
#include <mutex>
#include <vector>
#include <atomic>
#include <thread>

namespace DaniilPavlenko {
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

template <typename key_type, typename value_type>
struct ConcurrentMap {
	using hash_type = size_t;
	using index = size_t;

	constexpr static hash_type EmptyHash = std::numeric_limits<hash_type>::max();
	constexpr static hash_type DeletedHash = std::numeric_limits<hash_type>::max() - 1;
	constexpr static hash_type RelocatedHash = std::numeric_limits<hash_type>::max() - 2;
	constexpr static hash_type LastValidHash = std::numeric_limits<hash_type>::max() - 3;
	constexpr static index MinCapacity = 32;
	constexpr static float MaxLoadFactor = 0.7f;

	__declspec(align(64)) struct map_element {
		hash_type Hash;
		spinlock AccessLock;
		key_type Key;
		value_type Value;
	};

	map_element* Data = nullptr;
	map_element* OldData = nullptr;
	index Capacity = MinCapacity;
	std::atomic<index> Size = 0;
	std::atomic<index> Deleted = 0;
	index MaxSize = static_cast<index>(MinCapacity * MaxLoadFactor);
	std::mutex RelocateMutex;

	__forceinline ConcurrentMap() {
		Data = (map_element*) _aligned_malloc(MinCapacity * sizeof(map_element), 64);
		for (map_element* MapElem = Data; MapElem != Data + MinCapacity; ++MapElem) {
			MapElem->Hash = EmptyHash;
			MapElem->AccessLock.Unlock();
		}
	}

	__forceinline ~ConcurrentMap() {
		for (map_element* Elem = Data; Elem != Data + Capacity; ++Elem) {
			if (Elem->Hash <= LastValidHash) {
				Elem->Value.~value_type();
				Elem->Key.~key_type();
			}
		}
		_aligned_free(Data);
		Data = nullptr;
		Capacity = 0;
		MaxSize = 0;
		Size.store(0, std::memory_order::memory_order_relaxed);
		Deleted.store(0, std::memory_order::memory_order_relaxed);
	}

	__forceinline value_type operator[](const key_type& Key) {
		value_type Result;
		hash_type Hash = GetHash(Key);
		while (true) {
			index HashMask = (1 << LogOfTwoCeil(Capacity)) - 1;
			index Iteration = 0;
			map_element* CurrentElem;
			bool Relocated = false;
			for (CurrentElem = Data + (Hash & HashMask);;
				 CurrentElem = Data + ((Hash + TriangleNumber(++Iteration)) & HashMask)) {
				CurrentElem->AccessLock.Lock();
				if (CurrentElem->Hash == RelocatedHash) [[unlikely]] {
					CurrentElem->AccessLock.Unlock();
					Relocated = true;
					break;
				}
				if (CurrentElem->Hash == EmptyHash) {
					break;
				}
				if (CurrentElem->Hash == Hash && (Key == CurrentElem->Key)) {
					Result = CurrentElem->Value;
					CurrentElem->AccessLock.Unlock();
					return Result;
				}
				CurrentElem->AccessLock.Unlock();
			}
			if (Relocated) [[unlikely]] {
				continue;
			}
			index PreLockSize = Size.load(std::memory_order_relaxed);
			index PreLockDeleted = Deleted.load(std::memory_order_relaxed);
			const bool RelocationNeeded = (PreLockSize + PreLockDeleted + 1) > MaxSize;
			if (RelocationNeeded) [[unlikely]] {
				CurrentElem->AccessLock.Unlock();
				RelocateMutex.lock();
				index PostLockSize = Size.load(std::memory_order_relaxed);
				index PostLockDeleted = Deleted.load(std::memory_order_relaxed);
				const bool RelocationStillNeeded = (PostLockSize + PostLockDeleted + 1) > MaxSize;
				if (RelocationStillNeeded) {
					map_element* OldData = Data;
					index OldCapacity = Capacity;
					LockAll();
					constexpr index MAGIC_NUMBER = 100;
					Relocate(PostLockSize + MAGIC_NUMBER);
					OldDataUnlockAll(OldData, OldCapacity);
				}
				RelocateMutex.unlock();
				continue;
			} else {
				Size.fetch_add(1, std::memory_order::memory_order_relaxed);
				CurrentElem->Hash = Hash;
				new (&(CurrentElem->Key)) key_type(Key);
				new (&(CurrentElem->Value)) value_type();
				Result = CurrentElem->Value;
				CurrentElem->AccessLock.Unlock();
				return Result;
			}
		}
	}

	__forceinline value_type& AtLock(key_type& Key) {
		hash_type Hash = GetHash(Key);
		while (true) {
			index HashMask = (1 << LogOfTwoCeil(Capacity)) - 1;
			index Iteration = 0;
			map_element* CurrentElem;
			bool Relocated = false;
			for (CurrentElem = Data + (Hash & HashMask);;
				 CurrentElem = Data + ((Hash + TriangleNumber(++Iteration)) & HashMask)) {
				CurrentElem->AccessLock.Lock();
				if (CurrentElem->Hash == RelocatedHash) [[unlikely]] {
					CurrentElem->AccessLock.Unlock();
					Relocated = true;
					break;
				}
				if (CurrentElem->Hash == EmptyHash) {
					break;
				}
				if (CurrentElem->Hash == Hash && (Key == CurrentElem->Key)) {
					return CurrentElem->Value;
				}
				CurrentElem->AccessLock.Unlock();
			}
			if (Relocated) [[unlikely]] {
				continue;
			}
			index PreLockSize = Size.load(std::memory_order_relaxed);
			index PreLockDeleted = Deleted.load(std::memory_order_relaxed);
			const bool RelocationNeeded = (PreLockSize + PreLockDeleted + 1) > MaxSize;
			if (RelocationNeeded) [[unlikely]] {
				CurrentElem->AccessLock.Unlock();
				RelocateMutex.lock();
				index PostLockSize = Size.load(std::memory_order_relaxed);
				index PostLockDeleted = Deleted.load(std::memory_order_relaxed);
				const bool RelocationStillNeeded = (PostLockSize + PostLockDeleted + 1) > MaxSize;
				if (RelocationStillNeeded) {
					map_element* OldData = Data;
					index OldCapacity = Capacity;
					LockAll();
					constexpr index MAGIC_NUMBER = 100;
					Relocate(PostLockSize + MAGIC_NUMBER);
					OldDataUnlockAll(OldData, OldCapacity);
				}
				RelocateMutex.unlock();
				continue;
			} else {
				Size.fetch_add(1, std::memory_order::memory_order_relaxed);
				CurrentElem->Hash = Hash;
				new (&(CurrentElem->Key)) key_type(Key);
				new (&(CurrentElem->Value)) value_type();
				return CurrentElem->Value;
			}
		}
	}

	__forceinline void Unlock(const key_type& Key) {
		const hash_type Hash = GetHash(Key);
		const index HashMask = (1 << LogOfTwoCeil(Capacity)) - 1;
		index Iteration = 0;
		map_element* CurrentElem;
		for (CurrentElem = Data + (Hash & HashMask); CurrentElem->Hash != EmptyHash;
			 CurrentElem = Data + ((Hash + TriangleNumber(++Iteration)) & HashMask)) {
			if (CurrentElem->Hash == Hash && (Key == CurrentElem->Key)) {
				CurrentElem->AccessLock.Unlock();
				return;
			}
		}
	}

	__forceinline void Remove(const key_type& Key) {
		const hash_type Hash = GetHash(Key);
		while (true) {
			bool Relocated = false;
			const index HashMask = (1 << LogOfTwoCeil(Capacity)) - 1;
			index Iteration = 0;
			map_element* CurrentElem;
			for (CurrentElem = Data + (Hash & HashMask);;
				 CurrentElem = Data + ((Hash + TriangleNumber(++Iteration)) & HashMask)) {
				CurrentElem->AccessLock.Lock();
				if (CurrentElem->Hash == RelocatedHash) [[unlikely]] {
					CurrentElem->AccessLock.Unlock();
					Relocated = true;
					break;
				}
				if (CurrentElem->Hash == EmptyHash) {
					CurrentElem->AccessLock.Unlock();
					break;
				}
				if (CurrentElem->Hash == Hash && (Key == CurrentElem->Key)) {
					CurrentElem->Value.~value_type();
					CurrentElem->Key.~key_type();
					CurrentElem->Hash = DeletedHash;
					Deleted.fetch_add(1, std::memory_order_relaxed);
					Size.fetch_sub(1, std::memory_order_relaxed);
					CurrentElem->AccessLock.Unlock();
					return;
				}
				CurrentElem->AccessLock.Unlock();
			}
			if (Relocated) {
				continue;
			}
			return;
		}
	}

	__forceinline void Relocate(index DesiredSize) {
		const index NewCapacity = 1 << (LogOfTwoCeil(DesiredSize) + 1);
		auto* const NewData = (map_element*) _aligned_malloc(NewCapacity * sizeof(map_element), alignof(map_element));
		for (map_element* MapElem = NewData; MapElem != NewData + NewCapacity; ++MapElem) {
			MapElem->Hash = EmptyHash;
			MapElem->AccessLock.Unlock();
		}
		for (map_element* MapElem = Data; MapElem != Data + Capacity; ++MapElem) {
			if (MapElem->Hash <= LastValidHash) {
				const index HashMask = (1 << LogOfTwoCeil(NewCapacity)) - 1;
				index Iteration = 0;
				map_element* CurrentElem;
				const hash_type Hash = MapElem->Hash;
				for (CurrentElem = NewData + (Hash & HashMask); CurrentElem->Hash != EmptyHash;
					 CurrentElem = NewData + ((Hash + TriangleNumber(++Iteration)) & HashMask)) {
				}
				new (&(CurrentElem->Value)) value_type(std::move(MapElem->Value));
				new (&(CurrentElem->Key)) key_type(std::move(MapElem->Key));
				CurrentElem->Hash = MapElem->Hash;
				MapElem->Value.~value_type();
				MapElem->Key.~key_type();
			}
		}
		for (map_element* MapElem = Data; MapElem != Data + Capacity; ++MapElem) {
			MapElem->Hash = RelocatedHash;
		}
		if (OldData) {
			_aligned_free(OldData);
		}
		OldData = Data;
		Data = NewData;
		Deleted.store(0, std::memory_order_relaxed);
		Capacity = NewCapacity;
		MaxSize = (index) (MaxLoadFactor * Capacity);
	}

	__forceinline static size_t GetHash(const key_type& Key) {
		size_t Hash = std::hash<key_type>{}(Key);
		return Hash - (Hash > LastValidHash) * 3;
	}

	__forceinline static index TriangleNumber(const index InIteration) {
		return (InIteration * (InIteration + 1)) >> 1;
	}

	[[nodiscard]] __forceinline index GetSize() const {
		return Size.load(std::memory_order_relaxed);
	}

	__forceinline void LockAll() {
		for (map_element* Iter = Data; Iter != Data + Capacity; ++Iter) {
			Iter->AccessLock.Lock();
		}
	}

	__forceinline void OldDataUnlockAll(map_element* InData, index InCapacity) {
		for (map_element* Iter = InData; Iter != InData + InCapacity; ++Iter) {
			Iter->AccessLock.Unlock();
		}
	}

	__forceinline static index LogOfTwoCeil(index Value) {
		unsigned long Index;
		if (_BitScanReverse64(&Index, Value)) {
			index result = Index;
			if ((Value & ~((index) 1 << Index)) > 0) {
				++result;
			}
			return result;
		} else {
			return 0ull;
		}
	}
};
}	 // namespace DaniilPavlenko
