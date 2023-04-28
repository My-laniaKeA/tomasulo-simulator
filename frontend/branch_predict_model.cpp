#include "../include/branch_predict_model.h"
#include "../include/logger.h"

BranchPredictUnit::BranchPredictUnit() {
	for (auto &e: table) {
		e.valid = false;
		e.counterState = CounterState::STRONG_NOT_TAKEN;
	}
}

BranchPredictEntry BranchPredictUnit::query(unsigned addr) const {
	return table[(addr & 0x7C) >> 2];
}

/**
 * @brief 获取分支预测结果
 *
 * @param instAddr 指令地址
 * @return BranchPredictBundle
 */
std::optional<BranchPredictBundle> BranchPredictUnit::getPred(unsigned instAddr) const{
	BranchPredictBundle ret;
	auto entry = table[(instAddr & 0x7C) >> 2];
	// Logger::Info("getPred(): valid: %d, candidate inst: %x, candidate target: %x, candidate state: %d",
		// entry.valid, entry.instAddr, entry.targetAddr, entry.counterState);
	if (entry.valid && entry.instAddr == instAddr) {
		if (entry.counterState == CounterState::WEAK_TAKEN ||
			 entry.counterState == CounterState::STRONG_TAKEN) {
			ret.predictJump = true;
			ret.predictTarget = entry.targetAddr;
		}
		else {
			ret.predictJump = false;
			ret.predictTarget = instAddr + 4;
		}
		return std::make_optional(ret);
	}	
	return std::nullopt;
}

/**
 * @brief 更新分支预测器状态
 * 
 * @param unsigned instAddr 指令地址
 * @param unsigned targetAddr 跳转目标地址 
 * @param bool taken 是否跳转
 */
void BranchPredictUnit::update(unsigned instAddr, unsigned targetAddr, bool taken) {
	int idx = (instAddr & 0x7C) >> 2;
	table[idx].valid = true;
	table[idx].instAddr = instAddr;
	table[idx].targetAddr = targetAddr;
	if (taken) {
		switch (table[idx].counterState) {
		case CounterState::STRONG_NOT_TAKEN:
			table[idx].counterState = CounterState::WEAK_NOT_TAKEN;
			break;
		case CounterState::WEAK_NOT_TAKEN:
		case CounterState::WEAK_TAKEN:
		case CounterState::STRONG_TAKEN:
			table[idx].counterState = CounterState::STRONG_TAKEN;
			break;
		default:
			Logger::Error("Unknown CounterState!");
			std::__throw_runtime_error("Unknown CounterState!");
			break;
		}
	}
	else {
		switch (table[idx].counterState)
		{
		case CounterState::STRONG_NOT_TAKEN:
		case CounterState::WEAK_NOT_TAKEN:
		case CounterState::WEAK_TAKEN:
			table[idx].counterState = CounterState::STRONG_NOT_TAKEN;
			break;
		case CounterState::STRONG_TAKEN:
			table[idx].counterState =  CounterState::WEAK_TAKEN;
			break;
		default:
			Logger::Error("Unknown CounterState!");
			std::__throw_runtime_error("Unknown CounterState!");
			break;
		}
	}

	// Logger::Info("update(): taken: %d, valid: %d, inst: %x, target: %x, state: %d", taken,
	// 	table[idx].valid, table[idx].instAddr, table[idx].targetAddr, table[idx].counterState);
}