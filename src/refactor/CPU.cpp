#pragma once

#include "CPU.hpp"
#include <iostream>
#include <iomanip>

// Constructor
CPU::CPU(Memory* m, GPU* g) {
    pc = 0x200;             // Initialize program counter to 0x200
    indexRegister = 0x0;    // Initialize index register to 0
    delayTimer = 0x0;       // Initialize delay timer to 0
    soundTimer = 0x0;       // Initialize sound timer to 0
    std::fill(registers, registers+16, 0);  // Zero-initialize all registers
    memory = m;
    gpu = g;

    rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    rand = std::uniform_int_distribution<uint8_t>(0, 255u);
}

// Deconstructor
CPU::~CPU() {

}

void CPU::instructionStep() {
    pc += 2;
}

uint16_t CPU::fetch(uint8_t* memory) {
    uint16_t opcode = memory[pc] << 8;
    opcode |= memory[pc+1];
    
    return opcode;
}

void CPU::decodeAndExecute(uint16_t opcode) {
    switch ((opcode & 0xF000) >> 12) {
        case 0x0:
            switch0Opcode(opcode);
            break;
        case 0x1:
            op_1NNN(opcode);
            break;
        case 0x2:
            op_2NNN(opcode);
            break;
        case 0x3:
            op_3XNN(opcode);
            break;
        case 0x4:
            op_4XNN(opcode);
            break;
        case 0x5:
            op_5XY0(opcode);
            break;
        case 0x6:
            op_6XNN(opcode);
            break;
        case 0x7:
            op_7XNN(opcode);
            break;
        case 0x8:
            switch8Opcode(opcode);
            break;
        case 0x9:
            op_9XY0(opcode);
            break;
        case 0xA:
            op_ANNN(opcode);
            break;
        case 0xB:
            op_BNNN(opcode);
            break;
        case 0xC:
            op_CXNN(opcode);
            break;
        case 0xD:
            op_DXYN(opcode);
            break;
        case 0xE:
            switchEOpcode(opcode);
            break;
        case 0xF:
            switchFOpcode(opcode);
            break;
        default:
            break;
    }
}

void CPU::cycle(uint8_t* memory) {
    uint16_t opcode = fetch(memory);
    instructionStep();
    decodeAndExecute(opcode);
    if (delayTimer > 0) {
        delayTimer--;
    }

    if (soundTimer > 0) {
        soundTimer--;

        if (soundTimer == 0) {
            std::cout << "Beep\n";
        }
    }
}

void CPU::switch0Opcode(uint16_t opcode) {
    switch (opcode) {
        case 0x00E0:
            op_00E0(opcode);
            break;
        case 0x00EE:
            op_00EE(opcode);
            break;
        default:
            op_0NNN(opcode);
            break;
    }
}

void CPU::switch8Opcode(uint16_t opcode) {
    uint16_t lastHex = opcode & 0x000F;

    switch (lastHex) {
        case 0x0:
            op_8XY0(opcode);
            break;
        case 0x1:
            op_8XY1(opcode);
            break;
        case 0x2:
            op_8XY2(opcode);
            break;
        case 0x3:
            op_8XY3(opcode);
            break;
        case 0x4:
            op_8XY4(opcode);
            break;
        case 0x5:
            op_8XY5(opcode);
            break;
        case 0x6:
            op_8XY6(opcode);
            break;
        case 0x7:
            op_8XY7(opcode);
            break;
        case 0xE:
            op_8XYE(opcode);
            break;
    }
}

void CPU::switchEOpcode(uint16_t opcode) {
    uint16_t lastHex = opcode & 0x000F;

    switch (lastHex) {
        case 0xE:
            op_EX9E(opcode);
            break;
        case 0x1:
            op_EXA1(opcode);
            break;
    }
}

void CPU::switchFOpcode(uint16_t opcode) {
    uint16_t lastChar = opcode & 0x00FF;

    switch (lastChar) {
        case 0x07:
            op_FX07(opcode);
            break;
        case 0x0A:
            op_FX0A(opcode);
            break;
        case 0x15:
            op_FX15(opcode);
            break;
        case 0x18:
            op_FX18(opcode);
            break;
        case 0x1E:
            op_FX1E(opcode);
            break;
        case 0x29:
            op_FX29(opcode);
            break;
        case 0x33:
            op_FX33(opcode);
            break;
        case 0x55:
            op_FX55(opcode);
            break;
        case 0x65:
            op_FX65(opcode);
            break;
    }
}

void CPU::op_0NNN(unsigned short opcode) {
    unsigned short nnn = opcode & 0x0FFFu;
    pc = nnn;
}

void CPU::op_00E0(unsigned short opcode) {
    std::fill(display, display+2048, 0);
}

void CPU::op_00EE(unsigned short opcode) {
    pc = popFromStack();
}

void CPU::op_1NNN(unsigned short opcode) {
    unsigned short jumpAddress = opcode & 0x0FFFu;
    pc = jumpAddress;
    // std::cout << "Set pc to address " << std::hex << std::setw(3) << std::setfill('0') << jumpAddress << std::dec << '\n';
}

void CPU::op_2NNN(unsigned short opcode) {
    unsigned short jumpAddress = opcode & 0x0FFFu;
    pushToStack(pc);
    pc = jumpAddress;
}

void CPU::op_3XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char compareTo = opcode & 0x00FFu;

    if (registers[x] == compareTo) {
        instructionStep();
    }

}

void CPU::op_4XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char compareTo = opcode & 0x00FFu;

    if (registers[x] != compareTo) {
        instructionStep();
    }
}

void CPU::op_5XY0(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[x] == registers[y]) {
        instructionStep();
    }
}

void CPU::op_6XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char data = opcode & 0x00FFu;
    registers[x] = data;
    // std::cout << "Set register " << +x << " to value " << std::hex << std::setw(2) << std::setfill('0') << data << std::dec << '\n';
}

void CPU::op_7XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char data = opcode & 0x00FFu;

    registers[x] += data;
}

void CPU::op_8XY0(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;
    
    registers[x] = registers[y];
}

void CPU::op_8XY1(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    registers[x] |= registers[y];
}

void CPU::op_8XY2(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    registers[x] &= registers[y];
}

void CPU::op_8XY3(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    registers[x] ^= registers[y];
}

void CPU::op_8XY4(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;
    unsigned int sum = registers[x] + registers[y];

    if (sum > 255) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[x] += registers[y];
}

void CPU::op_8XY5(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[x] > registers[y]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[x] -= registers[y];
}

void CPU::op_8XY6(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;

    registers[0xF] = registers[x] & 1;
    registers[x] >>= 1;
}

void CPU::op_8XY7(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[y] > registers[x]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[x] = registers[y] - registers[x];
}

void CPU::op_8XYE(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;

    registers[0xF] = (registers[x] & 0x80u) >> 7;
    registers[x] <<= 1;
}

void CPU::op_9XY0(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[x] != registers[y]) {
        instructionStep();
    }
}

void CPU::op_ANNN(unsigned short opcode) {
    unsigned short address = opcode & 0x0FFFu;
    indexReg = address;
    // std::cout << "Set index to " << indexReg << '\n';
}

void CPU::op_BNNN(unsigned short opcode) {
    unsigned short address = opcode & 0x0FFFu;
    pc = registers[0] + address;
}

void CPU::op_CXNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char byte = opcode & 0x00FFu;

    registers[x] = rand(rng) & byte;
}

void CPU::op_DXYN(unsigned short opcode) {
    uint8_t Vx = (opcode & 0x0F00) >> 8u;
    uint8_t Vy = (opcode & 0x00F0) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % 64;
    uint8_t yPos = registers[Vy] % 32;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[indexReg + row];

        for (unsigned int col = 0; col < 8; ++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t* screenPixel = &display[(yPos + row) * 64 + (xPos + col)];

            if (spritePixel) {
                if (*screenPixel == 0xFFFFFFFF) {
                    registers[0xF] = 1;
                }
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
    // printDisplay();
}

void CPU::op_EX9E(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    if (keyPressedState[registers[x]] == 1) {
        instructionStep();
    }
}

void CPU::op_EXA1(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    if (keyPressedState[registers[x]] == 0) {
        instructionStep();
    }
}

void CPU::op_FX07(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    registers[x] = delayTimer;
}

void CPU::op_FX0A(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    
    if (keyPressedState[0]) {
        registers[x] = 0x0;
    } else if (keyPressedState[1]) {
        registers[x] = 0x1;
    } else if (keyPressedState[2]) {
        registers[x] = 0x2;
    } else if (keyPressedState[3]) {
        registers[x] = 0x3;
    } else if (keyPressedState[4]) {
        registers[x] = 0x4;
    } else if (keyPressedState[5]) {
        registers[x] = 0x5;
    } else if (keyPressedState[6]) {
        registers[x] = 0x6;
    } else if (keyPressedState[7]) {
        registers[x] = 0x7;
    } else if (keyPressedState[8]) {
        registers[x] = 0x8;
    } else if (keyPressedState[9]) {
        registers[x] = 0x9;
    } else if (keyPressedState[10]) {
        registers[x] = 0xA;
    } else if (keyPressedState[11]) {
        registers[x] = 0xB;
    } else if (keyPressedState[12]) {
        registers[x] = 0xC;
    } else if (keyPressedState[13]) {
        registers[x] = 0xD;
    } else if (keyPressedState[14]) {
        registers[x] = 0xE;
    } else if (keyPressedState[15]) {
        registers[x] = 0xF;
    } else {
        pc -= 2;
    }
}

void CPU::op_FX15(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    delayTimer = registers[x];
}

void CPU::op_FX18(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    soundTimer = registers[x];
}

void CPU::op_FX1E(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    indexReg += registers[x];
}

void CPU::op_FX29(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t fontCharacter = registers[x];

    indexReg = 0x50 + (5 * fontCharacter);
}

void CPU::op_FX33(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t decimal = registers[x];

    for (int i = 2; i >= 0; --i) {
        memory[indexReg] = decimal % 10;
        decimal /= 10;
    }
}

void CPU::op_FX55(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i) {
        memory[indexReg + i] = registers[i];
    }
}

void CPU::op_FX65(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i) {
        registers[i] = memory[indexReg + i];
    }
}