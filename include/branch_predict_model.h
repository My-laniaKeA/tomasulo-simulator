// Optional TODO: Change this file to whatever you like!
// NOTE: Please keep these signals that are already in used.
#pragma once

#include <optional>
#include "defines.h"

struct BpuUpdateData {
    unsigned pc;
    bool isCall, isReturn, isBranch, branchTaken;
    unsigned jumpTarget;
};

struct BranchPredictBundle {
    bool predictJump = false;
    unsigned predictTarget = 0;
};

enum class CounterState {
	STRONG_NOT_TAKEN,
	WEAK_NOT_TAKEN,
	WEAK_TAKEN,
	STRONG_TAKEN
};

struct BranchPredictEntry {
	bool valid;
	unsigned instAddr;
	unsigned targetAddr;
	CounterState counterState;
};

class BranchPredictUnit {
	BranchPredictEntry table[BPU_SIZE];

public:
	BranchPredictUnit();
	BranchPredictEntry query(unsigned addr) const;
	std::optional<BranchPredictBundle> getPred(unsigned instAddr) const;
	void update(unsigned instAddr, unsigned targetAddr, bool taken);
};