#include <iostream>
#include <windows.h>
#include <iomanip>
#include <array>
#include <fstream>
#include <chrono>
#include <random>

class Chip8 {
    private:
        uint8_t memory[4096];       // Memory of 4096 8-bit addresses
        uint8_t registers[16];      // 16 8-bit Registers
        uint16_t stack[16];         // Stack
        uint8_t sp;                 // Stack pointer
        uint8_t delayTimer;         // Delay timer
        uint8_t soundTimer;         // Sound timer
        uint16_t indexReg;
        uint16_t pc;
        std::default_random_engine rng;
        std::uniform_int_distribution<uint8_t> rand;
        static const std::array<uint8_t, 80> fontset;

    public:
        bool keyPressedState[16];   // Keys for input
        uint32_t display[2048];     // Display of 64x32 pixels
        
        Chip8();
        ~Chip8();

        void loadROM(char const* filename);
        void printMemory();
        void printRegisters();
        void printDisplay();

        unsigned short popFromStack();
        void pushToStack(unsigned short address);

        void op_0NNN(unsigned short opcode);
        void op_00E0(unsigned short opcode);
        void op_00EE(unsigned short opcode);
        void op_1NNN(unsigned short opcode);
        void op_2NNN(unsigned short opcode);
        void op_3XNN(unsigned short opcode);
        void op_4XNN(unsigned short opcode);
        void op_5XY0(unsigned short opcode);
        void op_6XNN(unsigned short opcode);
        void op_7XNN(unsigned short opcode);
        void op_8XY0(unsigned short opcode);
        void op_8XY1(unsigned short opcode);
        void op_8XY2(unsigned short opcode);
        void op_8XY3(unsigned short opcode);
        void op_8XY4(unsigned short opcode);
        void op_8XY5(unsigned short opcode);
        void op_8XY6(unsigned short opcode);
        void op_8XY7(unsigned short opcode);
        void op_8XYE(unsigned short opcode);
        void op_9XY0(unsigned short opcode);
        void op_ANNN(unsigned short opcode);
        void op_BNNN(unsigned short opcode);
        void op_CXNN(unsigned short opcode);
        void op_DXYN(unsigned short opcode);
        void op_EX9E(unsigned short opcode);
        void op_EXA1(unsigned short opcode);
        void op_FX07(unsigned short opcode);
        void op_FX0A(unsigned short opcode);
        void op_FX15(unsigned short opcode);
        void op_FX18(unsigned short opcode);
        void op_FX1E(unsigned short opcode);
        void op_FX29(unsigned short opcode);
        void op_FX33(unsigned short opcode);
        void op_FX55(unsigned short opcode);
        void op_FX65(unsigned short opcode);

        void instructionStep();
        void cycle();
        uint16_t fetch();
        void decodeAndExecute(unsigned short opcode);

        void switch0Function(unsigned short opcode);
        void switch8Function(unsigned short opcode);
        void switchEFunction(unsigned short opcode);
        void switchFFunction(unsigned short opcode);
};