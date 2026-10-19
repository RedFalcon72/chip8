#include <stdio.h>
#include <stdint.h>

int main(void) {
    uint8_t memory[4096] = {0};
    uint16_t pc = 0x200;

    memory[0x200] = 0x6A;   // 命令の前半
    memory[0x201] = 0x05;   // 命令の後半

    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    printf("opcode = 0x%04X\n", opcode);
    return 0;
}
