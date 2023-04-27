#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "defines.h"
#include "instructions.h"
#include "register_file.h"
#include "reservation_station.hpp"
#include "rob.h"
#include "store_buffer.h"

class Frontend {
    unsigned int pc = 0x80000000;
    unsigned data[INST_MEM_SIZE >> 2];

    bool dispatchHalt = false;

    std::optional<Instruction> IF1, IF2, ID, DISPATCH;

protected:
    virtual BranchPredictBundle bpuFrontendUpdate(unsigned int pc);

public:
    Frontend(const std::vector<unsigned> &inst);
    virtual std::optional<Instruction> step() final;
    virtual void jump(unsigned int jumpAddress) final;
    virtual void haltDispatch() final;
    virtual unsigned calculateNextPC(unsigned pc) const;
    virtual void bpuBackendUpdate(const BpuUpdateData &x);
};

class ExecutePipeline {
    const std::string name;
    IssueSlot executeSlot;
    unsigned counter;

public:
    ExecutePipeline(std::string name);
    std::optional<ROBStatusWritePort> step(unsigned *dataMem,
                                           StoreBuffer &stBuf);
    void execute(const IssueSlot &x);
    bool canExecute() const;
    void flush();
};

class Backend {
    ReorderBuffer rob;
    ReservationStation<4> rsALU, rsBRU, rsMUL, rsDIV;
    ReservationStation<1> rsLSU;  // LSU is not ooo.
    ExecutePipeline alu, bru, lsu, mul, div;
    std::shared_ptr<RegisterFile> regFile;
    StoreBuffer storeBuffer;

    unsigned data[DATA_MEM_SIZE >> 2];

protected:
    void executeAndWakeUp(ExecutePipeline &pipeline);
    void flush();

public:
    Backend(const std::vector<unsigned> &data);
    bool dispatchInstruction(const Instruction &inst);
    bool step(Frontend &frontend);
    unsigned read(unsigned addr) const;
	void write(unsigned addr, unsigned value);
    bool commitInstruction(const ROBEntry &inst, Frontend &frontend);
    void attachRegFile(const std::shared_ptr<RegisterFile> &regFile);
};

class Processor {
    Frontend frontend;
    Backend backend;
    RegisterFile regFile;

public:
    Processor(const std::vector<unsigned> &inst,
              const std::vector<unsigned> &data,
              unsigned entry);
    bool step();
    unsigned readMem(unsigned addr) const;
    unsigned readReg(unsigned addr) const;
};
