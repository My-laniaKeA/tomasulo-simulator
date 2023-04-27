#include "../include/branch_predict_model.h"
#include "../include/logger.h"

BranchPredictUnit::BranchPredictUnit() {
	for (auto &e: table) {
		e.valid = false;
	}
}

/**
 * @brief 获取分支预测结果
 *
 * @param instAddr 指令地址
 * @return BranchPredictBundle
 */
std::optional<BranchPredictBundle> BranchPredictUnit::getPred(unsigned instAddr) const{
	BranchPredictBundle ret;
	auto entry = table[instAddr % BPU_SIZE];
	if (entry.valid && entry.instAddr == instAddr) {
		ret.predictJump = 
			(entry.counterState == CounterState::WEAK_TAKEN ||
			 entry.counterState == CounterState::STRONG_TAKEN);
		ret.predictTarget = entry.targetAddr;
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
	auto entry = table[instAddr % BPU_SIZE];
	entry.valid = true;
	if (taken) {
		switch (entry.counterState) {
		case CounterState::STRONG_NOT_TAKEN:
			entry.counterState = CounterState::WEAK_NOT_TAKEN;
			break;
		case CounterState::WEAK_NOT_TAKEN:
		case CounterState::WEAK_TAKEN:
		case CounterState::STRONG_TAKEN:
			entry.counterState = CounterState::STRONG_TAKEN;
			break;
		default:
			Logger::Error("Unknown CounterState!");
			std::__throw_runtime_error("Unknown CounterState!");
			break;
		}
	}
	else {
		switch (entry.counterState)
		{
		case CounterState::STRONG_NOT_TAKEN:
		case CounterState::WEAK_NOT_TAKEN:
		case CounterState::WEAK_TAKEN:
			entry.counterState = CounterState::STRONG_NOT_TAKEN;
			break;
		case CounterState::STRONG_TAKEN:
			entry.counterState =  CounterState::WEAK_TAKEN;
			break;
		default:
			Logger::Error("Unknown CounterState!");
			std::__throw_runtime_error("Unknown CounterState!");
			break;
		}
	}
}