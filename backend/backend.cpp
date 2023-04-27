#include <sstream>

#include "logger.h"
#include "processor.h"

/**
 * @brief Construct a new Backend:: Backend object
 * 用于构造后端流水线，同时初始化数据内存
 * @param rf 寄存器堆指针
 * @param data 数据内存初始化数组，从0x80400000开始
 */
Backend::Backend(const std::vector<unsigned> &data)
    : alu("ALU"), bru("BRU"), lsu("LSU"), mul("MUL"), div("DIV") {
    for (unsigned long p = 0; p < data.size(); p++) {
        this->data[p] = data[p];
    }
}

/**
 * @brief 执行某流水线后，通过cdb唤醒其他保留站中的指令
 *
 * @param pipeline 被执行的流水线
 */
void Backend::executeAndWakeUp(ExecutePipeline &pipeline) {
    auto tmp = pipeline.step(data, storeBuffer);
    if (tmp.has_value()) {
        rsALU.wakeup(tmp.value());
        rsBRU.wakeup(tmp.value());
        rsMUL.wakeup(tmp.value());
        rsDIV.wakeup(tmp.value());
        rsLSU.wakeup(tmp.value());
        rob.writeState(tmp.value());
    }
}

/**
 * @brief 后端执行函数
 *
 * @param frontend cpu的前端，用于调用前端接口
 * @return true 提交了EXIT指令
 * @return false 未提交EXIT指令
 */
bool Backend::step(Frontend &frontend) {
    executeAndWakeUp(alu);
    executeAndWakeUp(bru);
    executeAndWakeUp(lsu);
    executeAndWakeUp(mul);
    executeAndWakeUp(div);

    if (rsALU.canIssue() && alu.canExecute()) alu.execute(rsALU.issue());
    if (rsBRU.canIssue() && bru.canExecute()) bru.execute(rsBRU.issue());
    if (rsMUL.canIssue() && mul.canExecute()) mul.execute(rsMUL.issue());
    if (rsDIV.canIssue() && div.canExecute()) div.execute(rsDIV.issue());
    if (rsLSU.canIssue() && lsu.canExecute()) lsu.execute(rsLSU.issue());

    auto commit = rob.getFront();

	// std::stringstream ss;
	// Logger::setDebugOutput(true);
	// if (commit.has_value()) {	
	// 	ss << commit.value().inst;
	// 	Logger::Debug("ROB front is %s", ss.str().c_str());
	// }
	// else {
	// 	Logger::Debug("Empty ROB, can not commit.");
	// }

    bool executeExit = false;

    if (commit.has_value() && commit.value().state.ready) {
        auto &entry = commit.value();
        executeExit = commitInstruction(entry, frontend);
    }
    return executeExit;
}

/**
 * @brief 用于刷新后端状态
 * 清空所有正在后端执行的指令数据，以及寄存器的busy标记
 *
 */
void Backend::flush() {
    rsALU.flush();
    rsBRU.flush();
    rsMUL.flush();
    rsDIV.flush();
    rsLSU.flush();

    alu.flush();
    bru.flush();
    lsu.flush();
    mul.flush();
    div.flush();

    regFile->flush();
    storeBuffer.flush();
    rob.flush();
}

/**
 * @brief 用于读取数据内存中的数据
 *
 * @param addr 大于等于0x80400000的地址
 * @return unsigned 地址对应的数据
 */
unsigned Backend::read(unsigned addr) const {
    return data[(addr - 0x80400000) >> 2];
}

/**
 * @brief 用于将数据写入数据内存
 *
 * @param addr 大于等于0x80400000的地址
 * @param value 需要写入的数据
 */
void Backend::write(unsigned addr, unsigned value) {
    data[(addr - 0x80400000) >> 2] = value;
}

void Backend::attachRegFile(const std::shared_ptr<RegisterFile> &regFile) {
    this->regFile = regFile;
}