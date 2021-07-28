#include "Memory.hpp"
#include "GPU.hpp"

#include <cstdint>
#include <random>
#include <chrono>

class CPU {
    private:
        uint16_t pc;            // Program counter
        uint16_t indexRegister; // Index registers
        uint8_t delayTimer;     // Delay timer
        uint8_t soundTimer;     // Sound timer
        uint8_t registers[16];  // 16 8-bit registers
        std::default_random_engine rng; // Used for random number generation
        std::uniform_int_distribution<uint8_t> rand;    // Used for random number generation
        Memory* memory;
        GPU* gpu;

    public:
        CPU(Memory* m, GPU* g);
        ~CPU();

        void instructionStep();
        uint16_t fetch(uint8_t* memory);
        void decodeAndExecute(uint16_t opcode);
        void cycle(uint8_t* memory);
        
        void switch0Opcode(uint16_t opcode);
        void switch8Opcode(uint16_t opcode);
        void switchEOpcode(uint16_t opcode);
        void switchFOpcode(uint16_t opcode);

        void op_0NNN(uint16_t opcode);
        void op_00E0(uint16_t opcode);
        void op_00EE(uint16_t opcode);
        void op_1NNN(uint16_t opcode);
        void op_2NNN(uint16_t opcode);
        void op_3XNN(uint16_t opcode);
        void op_4XNN(uint16_t opcode);
        void op_5XY0(uint16_t opcode);
        void op_6XNN(uint16_t opcode);
        void op_7XNN(uint16_t opcode);
        void op_8XY0(uint16_t opcode);
        void op_8XY1(uint16_t opcode);
        void op_8XY2(uint16_t opcode);
        void op_8XY3(uint16_t opcode);
        void op_8XY4(uint16_t opcode);
        void op_8XY5(uint16_t opcode);
        void op_8XY6(uint16_t opcode);
        void op_8XY7(uint16_t opcode);
        void op_8XYE(uint16_t opcode);
        void op_9XY0(uint16_t opcode);
        void op_ANNN(uint16_t opcode);
        void op_BNNN(uint16_t opcode);
        void op_CXNN(uint16_t opcode);
        void op_DXYN(uint16_t opcode);
        void op_EX9E(uint16_t opcode);
        void op_EXA1(uint16_t opcode);
        void op_FX07(uint16_t opcode);
        void op_FX0A(uint16_t opcode);
        void op_FX15(uint16_t opcode);
        void op_FX18(uint16_t opcode);
        void op_FX1E(uint16_t opcode);
        void op_FX29(uint16_t opcode);
        void op_FX33(uint16_t opcode);
        void op_FX55(uint16_t opcode);
        void op_FX65(uint16_t opcode);
};