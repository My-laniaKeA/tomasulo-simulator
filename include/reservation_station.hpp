#pragma once
#include <memory>
#include <sstream>

#include "instructions.h"
#include "issue_slot.h"
#include "logger.h"
#include "register_file.h"
#include "rob.h"

template <unsigned size>
class ReservationStation {
    IssueSlot buffer[size];

public:
    ReservationStation();
    bool hasEmptySlot() const;
    void insertInstruction(const Instruction &inst,
                           unsigned robIdx,
                           std::shared_ptr<RegisterFile> regFile,
                           const ReorderBuffer &reorderBuffer);
    void wakeup(const ROBStatusWritePort &x);
    bool canIssue() const;
    IssueSlot issue();
    void flush();
	void showContent();
};

template <unsigned size>
ReservationStation<size>::ReservationStation() {
    for (auto &slot : buffer) {
        slot.busy = false;
    }
}

template <unsigned size>
bool ReservationStation<size>::hasEmptySlot() const {
    for (auto const &slot : buffer) {
        if (!slot.busy) return true;
    }
    return false;
}

template <unsigned size>
void ReservationStation<size>::insertInstruction(
    const Instruction &inst,
    unsigned robIdx,
    std::shared_ptr<RegisterFile> regFile,
    const ReorderBuffer &reorderBuffer) {
    for (auto &slot : buffer) {
        if (slot.busy) continue;
        // TODO: Dispatch instruction to this slot
		std::stringstream ss;
		ss << inst;
		slot.inst = inst;
    	slot.robIdx = robIdx;
    	slot.busy = true;
		unsigned rs1 = inst.getRs1();
		unsigned robIdx1 = regFile->getBusyIndex(rs1);
		if (regFile->isBusy(rs1)) {
			if (reorderBuffer.checkReady(robIdx1)) {
				slot.readPort1.waitForWakeup = false;
				slot.readPort1.robIdx = 0;
				slot.readPort1.value = reorderBuffer.read(robIdx1);
			}
			else {
				slot.readPort1.waitForWakeup = true;
				slot.readPort1.robIdx = robIdx1;
				slot.readPort1.value = 0;
			}		
		}
		else {
			slot.readPort1.waitForWakeup = false;
			slot.readPort1.robIdx = 0;
			slot.readPort1.value = regFile->read(rs1);
			// regFile->markBusy(rs1);
		}
		unsigned rs2 = inst.getRs2();
		unsigned robIdx2 = regFile->getBusyIndex(rs2);
		if (regFile->isBusy(rs2)) {
			if (reorderBuffer.checkReady(robIdx2)) {
				slot.readPort2.waitForWakeup = false;
				slot.readPort2.robIdx = 0;
				slot.readPort2.value = reorderBuffer.read(robIdx2);
			}
			else {
				slot.readPort2.waitForWakeup = true;
				slot.readPort2.robIdx = robIdx2;
				slot.readPort2.value = 0;
			}
		}
		else {
			slot.readPort2.waitForWakeup = false;
			slot.readPort2.robIdx = 0;
			slot.readPort2.value = regFile->read(rs2);
			// regFile->markBusy(rs2);
		}
		return;
        Logger::Error("Dispatching instruction is not implemented");
        std::__throw_runtime_error(
            "Dispatching instruction is not implemented");
    }
    Logger::Error("ReservationStation::insertInstruction no empty slots!");
    std::__throw_runtime_error(
        "No empty slots when inserting instruction into reservation "
        "station.");
}

template <unsigned size>
void ReservationStation<size>::wakeup(
    [[maybe_unused]] const ROBStatusWritePort &x) {
    // TODO: Wakeup instructions according to ROB Write
	std::stringstream ss;
	// Logger::setDebugOutput(true);	
	for (auto &slot : buffer) {
		if (slot.busy) {
			if (slot.readPort1.waitForWakeup 
			 && x.robIdx == slot.readPort1.robIdx) {
				slot.readPort1.value = x.result;
				slot.readPort1.waitForWakeup = false;
				ss << slot.inst;
				// Logger::Debug("RS wake up rs1 of %s", ss.str().c_str());
			}
			if (slot.readPort2.waitForWakeup 
			 && x.robIdx == slot.readPort2.robIdx) {
				slot.readPort2.value = x.result;
				slot.readPort2.waitForWakeup = false;
				ss << slot.inst;
				// Logger::Debug("RS wake up rs2 of %s", ss.str().c_str());
			}
		}
	}
    // Logger::Error("Wakeup not implemented");
    // std::__throw_runtime_error("Wakeup not implemented");
}

template <unsigned size>
bool ReservationStation<size>::canIssue() const {
    // TODO: Decide whether an issueSlot is ready to issue.
	 for (auto &slot : buffer) {
		if (slot.busy && !slot.readPort1.waitForWakeup && !slot.readPort2.waitForWakeup) {
			return true;
		}
	}
    return false;
}

template <unsigned size>
IssueSlot ReservationStation<size>::issue() {
    // TODO: Issue a ready issue slot and remove it from reservation station.
	IssueSlot result;
	for (auto &slot : buffer) {
		if (slot.busy && !slot.readPort1.waitForWakeup && !slot.readPort2.waitForWakeup) {
			result = slot;
			slot.busy = false;
			std::stringstream ss;
            ss << slot.inst;
			// Logger::setDebugOutput(true);
			// Logger::Debug("RS::issue %s", ss.str().c_str());
			return result;
		}
	}
    Logger::Error("No available slots for issuing");
    std::__throw_runtime_error("No available slots for issuing");
}

template <unsigned size>
void ReservationStation<size>::flush() {
    for (auto &slot : buffer) {
        slot.busy = false;
    }
}


template <unsigned size>
void ReservationStation<size>::showContent() {
	std::stringstream ss;
	// Logger::setDebugOutput(true);
	Logger::Debug("Instructions in Reservation Station: ");
    for (auto &slot : buffer) {
        ss << slot.inst << "\n";
    }
	Logger::Debug("%s", ss.str().c_str());
}