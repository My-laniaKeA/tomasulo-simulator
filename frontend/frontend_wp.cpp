#include "with_predict.h"

FrontendWithPredict::FrontendWithPredict(const std::vector<unsigned> &inst)
    : Frontend(inst) {
    // Optional TODO: initialize your prediction structures here.
}

/**
 * @brief 获取指令的分支预测结果，分支预测时需要
 * 
 * @param pc 指令的pc
 * @return BranchPredictBundle 分支预测的结构
 */
BranchPredictBundle FrontendWithPredict::bpuFrontendUpdate(unsigned int pc) {
    // Optional TODO: branch predictions 
	auto entry = bpu.query(pc);
	BranchPredictBundle ret;
	if (entry.valid && entry.instAddr == pc) { 
		if (entry.counterState == CounterState::STRONG_TAKEN || 
		    entry.counterState == CounterState::WEAK_TAKEN) {
			ret.predictJump = true;
			ret.predictTarget = entry.targetAddr;
		}
		else {
			ret.predictJump = false;
			ret.predictTarget = entry.targetAddr;	
		}
		return ret;
	}
	else 
		return Frontend::bpuFrontendUpdate(pc);
	// Logger::Error("Instruction %x not in BPU!", pc);
	// std::__throw_runtime_error("Instruction not in BPU!");
}

/**
 * @brief 用于计算NextPC，分支预测时需要
 * 
 * @param pc 
 * @return unsigned 
 */
unsigned FrontendWithPredict::calculateNextPC(unsigned pc) const {
    // Optional TODO: branch predictions
	auto entry = bpu.query(pc);
	unsigned nextPC;
	if (entry.valid && entry.instAddr == pc && 
		(entry.counterState == CounterState::STRONG_TAKEN || entry.counterState == CounterState::WEAK_TAKEN))
		nextPC = entry.targetAddr;
	else
		nextPC = Frontend::calculateNextPC(pc);

	// Logger::Debug("inst: %x, target: %x, counter: %d", bpu.query(0x800002ec).instAddr,
	// 			bpu.query(0x800002ec).targetAddr, (bpu.query(0x800002ec).counterState ));
	// Logger::Info("Current PC = %x, Next PC = %x", pc, nextPC);
	return nextPC;
	
}	

/**
 * @brief 用于后端提交指令时更新分支预测器状态，分支预测时需要
 * 
 * @param x 
 */
void FrontendWithPredict::bpuBackendUpdate(const BpuUpdateData &x) {
    // Optional TODO: branch predictions
    // Frontend::bpuBackendUpdate(x);
	if (x.isBranch) {
		bpu.update(x.pc, x.jumpTarget, x.branchTaken);
	}
	else {
		bpu.update(x.pc, x.jumpTarget, true);
	}
	// Logger::Debug("inst: %x, target: %x, counter: %d", bpu.query(0x800002ec).instAddr,
	// 			bpu.query(0x800002ec).targetAddr, (bpu.query(0x800002ec).counterState ));
}
