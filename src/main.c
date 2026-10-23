#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint8_t memory[4096] = {0};
    uint16_t pc = 0x200;

    memory[0x200] = 0x63; 
    memory[0x201] = 0x2A; 
    memory[0x202] = 0x73;
    memory[0x203] = 0x03;
    memory[0x204] = 0xA2;
    memory[0x205] = 0x2A;

    memory[0x206] = 0x12; memory[0x207] = 0x0A;  // 120A: 0x20A へジャンプ
    memory[0x208] = 0x63; memory[0x209] = 0x63;  // 6363: 飛ばされる
    memory[0x20A] = 0x73; memory[0x20B] = 0x01;        
        
    uint8_t V[16] = {0};
    uint16_t I = 0;

    while(pc < 0x20C) {
        uint16_t opcode =
            (memory[pc] << 8) | memory[pc + 1]; // 命令を16ビットに結合
        printf("opcode = 0x%04X\n", opcode);

        pc += 2;

        uint8_t type = (opcode & 0xF000) >> 12;
        // printf("type=%X\n", type);

        uint8_t x = (opcode & 0x0F00) >> 8;
        // printf("x=%X\n",x);

        uint8_t nn = (opcode & 0x00FF);
        // printf("nn=%02X\n",nn);

        uint16_t nnn = (opcode & 0x0FFF);

        switch (type) {
        case 0x1:
            pc = nnn;
            break;
        case 0x6:  //入れる
            V[x] = nn;
            break;   
        case 0x7:  //足す
            V[x] += nn;
            break;
        case 0xA:
            I = nnn;
            break;
        }
       // printf("I=%X\n",I);
       // printf("V[%X]=%d\n", x, V[x]);
    }
    printf("V[3]=%d\n", V[3]);   
    return 0;
}
