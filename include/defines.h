#pragma once

#include <vector>
#include <string>

constexpr int XLEN = 32;

enum class InstructionType { R, I, S, B, J, U };

extern std::string xreg_name[32];
extern std::string freg_name[32];

unsigned readElf(const std::string &elfFileName,
             std::vector<unsigned> &instruction,
             std::vector<unsigned> &data);

constexpr int INST_MEM_SIZE = 0x400000;
constexpr int DATA_MEM_SIZE = 0x400000;

enum class FUType {
    ALU, BRU, LSU, MUL, DIV, NONE
};

constexpr int ROB_SIZE = 16;

constexpr int BPU_SIZE = 32;