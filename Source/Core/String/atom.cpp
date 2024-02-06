#include "atom.h"

static bool PoolInitialized;
alignas(atom::atom_pool) static array<u8, sizeof(atom::atom_pool)> PoolBytes;

atom::atom_pool& atom::GetAtomPool() {
	if (PoolInitialized) {
		return *(atom_pool*) (&PoolBytes);
	}
	atom_pool* Singleton = new (&PoolBytes) atom_pool{};
	PoolInitialized = true;
	return *Singleton;
}

hash::hash_type atom::atom_pool::index_hasher::Hash(index Index) {
	return hash::Hash((*(atom_pool*) (&PoolBytes)).Strings[Index]);
}
