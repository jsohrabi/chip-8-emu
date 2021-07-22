#include "chip8.hpp"

const std::array<uint8_t, 80> Chip8::fontset = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
    std::fill(memory, memory+4096, 0);
    std::fill(registers, registers+16, 0);
    std::fill(stack, stack+16, 0);
    std::fill(keyPressedState, keyPressedState+16, 0);
    std::fill(display, display+2048, 0);

    sp = 0;
    indexReg = 0;
    pc = 0x200;
    rng = std::default_random_engine(std::chrono::system_clock::now().time_since_epoch().count());
    rand = std::uniform_int_distribution<uint8_t>(0, 255u);
    
    for (int i = 0; i < 80; ++i) {
        memory[i] = fontset[i];
    }
}

Chip8::~Chip8() {
}

void Chip8::loadROM(char const* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (file.is_open()) {
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for (long i = 0; i < size; i++) {
            memory[0x200 + i] = buffer[i];
        }

        delete[] buffer;
    }
}

void Chip8::printMemory() {
    for (int i = 0; i < 4096; ++i) {
        if (i % 32 == 0) {
            std::cout << '\n';
        }
        std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(memory[i]) << ' ';
    }
    std::cout << std::dec;
}

void Chip8::printRegisters() {
    for (int i = 0; i < 16; ++i) {
        std::cout << i << ": " << std::hex << std::setw(2) << std::setfill('0') << unsigned(registers[i]) << '\n';
    }
    std::cout << std::dec;
}

void Chip8::printDisplay() {
    std::cout << "Display:\n";
    for (int i = 0; i < 2048; ++i) {
        if (i % 32 == 0) {
            std::cout << '\n';
        }
        std::cout << std::hex << std::setw(2) << std::setfill('0') << unsigned(display[i]) << ' ';
    }
    std::cout << std::dec;
}

unsigned short Chip8::popFromStack() {
    sp--;
    return stack[sp];
}

void Chip8::pushToStack(unsigned short address) {
    stack[sp] = address;
    sp++;
}

void Chip8::op_0NNN(unsigned short opcode) {
    unsigned short nnn = opcode & 0x0FFFu;
    pc = nnn;
}

void Chip8::op_00E0(unsigned short opcode) {
    std::fill(display, display+2048, 0);
}

void Chip8::op_00EE(unsigned short opcode) {
    pc = popFromStack();
}

void Chip8::op_1NNN(unsigned short opcode) {
    unsigned short jumpAddress = opcode & 0x0FFFu;
    pc = jumpAddress;
    // std::cout << "Set pc to address " << std::hex << std::setw(3) << std::setfill('0') << jumpAddress << std::dec << '\n';
}

void Chip8::op_2NNN(unsigned short opcode) {
    unsigned short jumpAddress = opcode & 0x0FFFu;
    pushToStack(pc);
    pc = jumpAddress;
}

void Chip8::op_3XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char compareTo = opcode & 0x00FFu;

    if (registers[x] == compareTo) {
        instructionStep();
    }

}

void Chip8::op_4XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char compareTo = opcode & 0x00FFu;

    if (registers[x] != compareTo) {
        instructionStep();
    }
}

void Chip8::op_5XY0(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[x] == registers[y]) {
        instructionStep();
    }
}

void Chip8::op_6XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char data = opcode & 0x00FFu;
    registers[x] = data;
    // std::cout << "Set register " << +x << " to value " << std::hex << std::setw(2) << std::setfill('0') << data << std::dec << '\n';
}

void Chip8::op_7XNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char data = opcode & 0x00FFu;

    registers[x] += data;
}

void Chip8::op_8XY0(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;
    
    registers[x] = registers[y];
}

void Chip8::op_8XY1(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    registers[x] |= registers[y];
}

void Chip8::op_8XY2(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    registers[x] &= registers[y];
}

void Chip8::op_8XY3(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    registers[x] ^= registers[y];
}

void Chip8::op_8XY4(unsigned short opcode) {
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

void Chip8::op_8XY5(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[x] > registers[y]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[x] -= registers[y];
}

void Chip8::op_8XY6(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;

    registers[0xF] = registers[x] & 1;
    registers[x] >>= 1;
}

void Chip8::op_8XY7(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[y] > registers[x]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[x] = registers[y] - registers[x];
}

void Chip8::op_8XYE(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;

    registers[0xF] = (registers[x] & 0x80u) >> 7;
    registers[x] <<= 1;
}

void Chip8::op_9XY0(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char y = (opcode & 0x00F0u) >> 4;

    if (registers[x] != registers[y]) {
        instructionStep();
    }
}

void Chip8::op_ANNN(unsigned short opcode) {
    unsigned short address = opcode & 0x0FFFu;
    indexReg = address;
    // std::cout << "Set index to " << indexReg << '\n';
}

void Chip8::op_BNNN(unsigned short opcode) {
    unsigned short address = opcode & 0x0FFFu;
    pc = registers[0] + address;
}

void Chip8::op_CXNN(unsigned short opcode) {
    unsigned char x = (opcode & 0x0F00u) >> 8;
    unsigned char byte = opcode & 0x00FFu;

    registers[x] = rand(rng) & byte;
}

void Chip8::op_DXYN(unsigned short opcode) {
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

void Chip8::op_EX9E(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    if (keyPressedState[registers[x]] == 1) {
        instructionStep();
    }
}

void Chip8::op_EXA1(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    if (keyPressedState[registers[x]] == 0) {
        instructionStep();
    }
}

void Chip8::op_FX07(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    registers[x] = delayTimer;
}

void Chip8::op_FX0A(unsigned short opcode) {
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

void Chip8::op_FX15(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    delayTimer = registers[x];
}

void Chip8::op_FX18(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    soundTimer = registers[x];
}

void Chip8::op_FX1E(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    indexReg += registers[x];
}

void Chip8::op_FX29(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t fontCharacter = registers[x];

    indexReg = 0x50 + (5 * fontCharacter);
}

void Chip8::op_FX33(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t decimal = registers[x];

    for (int i = 2; i >= 0; --i) {
        memory[indexReg] = decimal % 10;
        decimal /= 10;
    }
}

void Chip8::op_FX55(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i) {
        memory[indexReg + i] = registers[i];
    }
}

void Chip8::op_FX65(unsigned short opcode) {
    uint8_t x = (opcode & 0x0F00) >> 8;

    for (int i = 0; i <= x; ++i) {
        registers[i] = memory[indexReg + i];
    }
}

void Chip8::instructionStep() {
    pc += 2;
}

void Chip8::cycle() {
    uint16_t opcode = fetch();
    instructionStep();
    // std::cout << "Fetched opcode: " << std::hex << std::setw(4) << std::setfill('0') << opcode << '\n';
    decodeAndExecute(opcode);
    // std::cout << "opcode executed successfully\n";
    // std::cout << std::hex << std::setw(2) << std::setfill('0') << pc << '\n';
    //printDisplay();
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

uint16_t Chip8::fetch() {
    uint16_t opcode = memory[pc] << 8;
    // instructionStep();
    opcode |= memory[pc+1];
    return opcode;
}

void Chip8::decodeAndExecute(unsigned short opcode) {
    switch ((opcode & 0xF000) >> 12) {
        case 0x0:
            switch0Function(opcode);
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
            switch8Function(opcode);
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
            switchEFunction(opcode);
            break;
        case 0xF:
            switchFFunction(opcode);
            break;
        default:
            break;
    }
}

void Chip8::switch0Function(unsigned short opcode) {
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

void Chip8::switch8Function(unsigned short opcode) {
    unsigned char lastHex = opcode & 0x000F;

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

void Chip8::switchEFunction(unsigned short opcode) {
    unsigned char lastHex = opcode & 0x000F;

    switch (lastHex) {
        case 0xE:
            op_EX9E(opcode);
            break;
        case 0x1:
            op_EXA1(opcode);
            break;
    }
}

void Chip8::switchFFunction(unsigned short opcode) {
    unsigned char lastChar = opcode & 0x00FF;

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
