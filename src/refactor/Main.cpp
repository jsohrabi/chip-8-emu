#include "GPU.hpp"
#include "Memory.hpp"
#include "CPU.hpp"

int main() {
    Memory* memory;
    GPU* gpu;
    CPU* cpu(memory, gpu);

    delete cpu;
    delete gpu;
    delete memory;
}