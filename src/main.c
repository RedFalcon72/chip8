#include <stdint.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    uint8_t memory[4096] = {0};
    uint16_t pc = 0x200;

    FILE *fp = fopen(argv[1], "rb");
    fread(&memory[0x200], 1, 4096 - 0x200, fp);
    fclose(fp);

    uint8_t V[16] = {0};
    uint16_t stack[16];
    uint8_t sp = 0;
    uint16_t I = 0;
    uint8_t display[64 * 32] = {0};

    for (int cycle = 0; cycle < 200; cycle++) {
        /*fetch  命令を16ビットに結合*/
        uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
        // printf("opcode = 0x%04X\n", opcode);

        pc += 2;

        /*decode*/
        uint8_t type = (opcode & 0xF000) >> 12;
        // printf("type=%X\n", type);
        uint8_t x = (opcode & 0x0F00) >> 8;
        // printf("x=%X\n",x);
        uint8_t nn = (opcode & 0x00FF);
        // printf("nn=%02X\n",nn);
        uint16_t nnn = (opcode & 0x0FFF);

        uint8_t y = (opcode & 0x00F0) >> 4;

        uint8_t n = (opcode & 0x000F);

        /*execute 命令ごとに動かす*/
        switch (type) {
        case 0x0:
            if (opcode == 0x00E0) {
                for (int i = 0; i < 64 * 32; i++) {
                    display[i] = 0;
                }
            } else if (opcode == 0x00EE) {
                sp--;
                pc = stack[sp];
            }
            break;
        case 0x1: // 1NNN pcをnnnに飛ばす
            pc = nnn;
            break;
        case 0x2:
            stack[sp] = pc;
            sp++;
            pc = nnn;
            break;
        case 0x3:
            if(V[x] == nn){
                pc += 2;
            }
            break;
        case 0x4:
            if(V[x] != nn) {
                pc += 2;
            }
            break;
        case 0x5:
            if(V[x] == V[y]){
                pc += 2;
            }
            break;
        case 0x6: // 6XNN V[x]にnnを入れる
            V[x] = nn;
            break;
        case 0x7: // 7XNN V[x]にnnを足す
            V[x] += nn;
            break;
        case 0x8:
            switch (n) {
                case 0x0:
                    V[x] = V[y];
                    break;
                case 0x1:
                    V[x] |= V[y];
                    break;
                case 0x2:
                    V[x] &= V[y];
                    break;
                case 0x3:
                    V[x] ^= V[y];
                    break;
                case 0x4:{
                    int sum = V[x] + V[y];
                    V[x] = sum & 0xFF;
                    if(sum > 255) {
                        V[0xF] = 1;
                    }else{
                        V[0xF] = 0;
                    }
                    break;
                 }
                case 0x5:{
                    uint8_t flag = (V[x] >= V[y]) ? 1:0;
                    V[x] = V[x] - V[y];
                    V[0xF] = flag;
                    break;
                }
                case 0x7: {
                    uint8_t flag = (V[y] >= V[x]) ? 1 : 0;
                    V[x] = V[y] - V[x];
                    V[0xF] = flag;
                    break;
                }
                case 0x6: {
                    uint8_t flag = V[x] & 1;
                    V[x] >>= 1;
                    V[0xF] = flag;
                    break;
                }
                case 0xE: {
                    uint8_t flag = (V[x] >> 7) & 1;
                    V[x] <<= 1;
                    V[0xF] = flag;
                    break;
                }
            }
            break;
        case 0x9:
            if (V[x] != V[y]) {
                pc += 2;
             }
             break;
        case 0xA: // ANNN  Iにnnnを入れる
            I = nnn;
            break;
        case 0xD: {
            int px = V[x] % 64; // 横の幅
            int py = V[y] % 32; // 縦の高さ
            V[0xF] = 0;
            for (int row = 0; row < n; row++) {
                for (int col = 0; col < 8; col++) {
                    if (px + col >= 64)
                        continue;
                    if (py + row >= 32)
                        continue;

                    if ((memory[I + row] >> (7 - col)) & 1) {
                        if (display[(py + row) * 64 + (px + col)]) {
                            V[0xF] = 1;
                        }
                        display[(py + row) * 64 + (px + col)] ^= 1;
                    }
                }
            }
            break;
        }
        }
    }

    /*表示*/
    for (int row = 0; row < 32; row++) {
        for (int col = 0; col < 64; col++)
            printf("%c", display[row * 64 + col] ? '#' : ' ');
        printf("\n");
    }

    return 0;
}
