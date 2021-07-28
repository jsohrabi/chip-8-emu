#include <cstdint>

class Memory {
    private:
        uint8_t memory[4096];
        uint16_t stack[16];
        uint8_t sp;
        static const uint8_t fontset[80];

    public:
        Memory();
        ~Memory();

        uint16_t popFromStack();
        void pushToStack(uint16_t address);

        uint16_t readMemory(uint16_t address);
        void writeMemory(uint16_t address, uint8_t data);
};