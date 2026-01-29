// bool halfCarry = ((a & 0xF) + (b & 0xF)) > 0xF;
//
// bool halfBorrow = (a & 0xF) < (b & 0xF);
#include <cstdint>
#include <array>
#include <iostream>
#include <cstdio>
#include <cstdlib>

class GameBoyColor {
public:
    uint8_t A=0,F=0,B=0,C=0,D=0,E=0,H=0,L=0;
    uint16_t sp=0, pc=0;
    uint32_t cycles=0;
    uint16_t opcode;

    std::array<uint8_t, 0x10000> memory{};

    GameBoyColor();
    void reset();
    void step();
    void OP_0x00(); 
    void OP_0x01(); 
    void OP_0x02();
    void OP_0x03();
    void OP_0x04();
    
  enum flags{
    Z = 0x80,
    N = 0x40,
    HC = 0x20,
    CF = 0x10
  };
  void setflag(flags f,bool v){
    if(v){
      F|=f;
    }else{
      F &= ~f;
    }
  }

  bool getflag(flags f){
    return F & f;
  }

};

GameBoyColor::GameBoyColor() {
    reset();
}

void GameBoyColor::reset() {
    sp = 0xFFFE;
    pc = 0x0100;
}

void GameBoyColor::step() {
    uint8_t opcode = memory[pc];
    printf("PC: %04X OPCODE: %02X\n", pc, opcode);

    switch (opcode) {
        case 0x00: OP_0x00(); break;
        case 0x01:OP_0x01(); break;
        case 0x02:OP_0x02(); break;
        case 0x03:OP_0x03(); break;
        case 0x04:OP_0x04(); break;

            pc++;
            break;
        default:
            printf("Unimplemented opcode %02X\n", opcode);
            exit(1);
    }
}

void GameBoyColor::OP_0x00(){
     pc += 1;
    cycles += 4;   
}

void GameBoyColor::OP_0x01(){
  uint8_t low = memory[pc+1];
  uint8_t high = memory[pc+2];

  B=high;
  C=low;
  pc+=2;
  cycles+=12;
}

void GameBoyColor::OP_0x02(){
  uint8_t address =(B<<8)| C   ;
  memory[address]=A;
  pc+=1;
  cycles+=8;
}

void GameBoyColor::OP_0x03(){

  uint8_t BC =(B<<8)| C;
  BC+=1;
  pc+=1;
  cycles+=8;
}


void GameBoyColor::OP_0x04(){
  uint8_t oldB=B;
  ++B;

 bool halfCarry = ( (oldB & 0x0F)+1) > 0x0F;
  setflag(Z,B==0 );
  setflag(N, false);
  setflag(HC,halfCarry);
  pc+=1;
  cycles+=4;
}




int main() {
    GameBoyColor gameboy;
    gameboy.step();
}
