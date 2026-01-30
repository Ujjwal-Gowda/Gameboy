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



    void OP_0x06();
    void OP_0x07();
    void OP_0x08();
    void OP_0x09();
    void OP_0x0A();
    void OP_0x0B();
    void OP_0x0E();
    void OP_0x0F();


    void NOP();
    void INC(uint8_t & r);
    void INC2(uint8_t & r,uint8_t & r1);
    void DEC(uint8_t & r);
    void LD_rr_n16(uint8_t & r,uint8_t & r1);
    
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
        case 0x00: NOP(); break;

        // LD_rr_n16 
        case 0x01:LD_rr_n16(B,C); pc+=3; cycles+=12; break;
        case 0x11:LD_rr_n16(D,E); pc+=3; cycles+=12; break;
        case 0x21:LD_rr_n16(H,L); pc+=3; cycles+=12; break;

        case 0x02:OP_0x02(); break;

        
        // INC R 
        case 0x04:INC(B); pc++;cycles+=4; break;
        case 0x14:INC(D); pc++;cycles+=4; break;
        case 0x24:INC(H); pc++;cycles+=4; break;
        case 0x0C:INC(C); pc++;cycles+=4; break;
        case 0x1C:INC(E); pc++;cycles+=4; break;
        case 0x2C:INC(L); pc++;cycles+=4; break;
        case 0x3C:INC(A); pc++;cycles+=4; break;

      
        // INC AB
        case 0x03:INC2(B,C);pc+=1;cycles+=8;break;
        case 0x13:INC2(D,E);pc+=1;cycles+=8;break;
        case 0x23:INC2(H,L);pc+=1;cycles+=8;break;

        // DEC R
        case 0x05:DEC(B); pc++;cycles+=4; break;
        case 0x15:DEC(D); pc++;cycles+=4; break;
        case 0x25:DEC(H); pc++;cycles+=4; break;
        case 0x0D:DEC(C); pc++;cycles+=4; break;
        case 0x1D:DEC(E); pc++;cycles+=4; break;
        case 0x2D:DEC(L); pc++;cycles+=4; break;
        case 0x3D:DEC(A); pc++;cycles+=4; break;

        case 0x06:OP_0x06(); break;
        case 0x07:OP_0x07(); break;
        case 0x08:OP_0x08(); break;
        case 0x09:OP_0x09(); break;
        case 0x0A:OP_0x0A(); break;
        case 0x0B:OP_0x0B(); break;
        case 0x0E:OP_0x0E(); break;
        case 0x0F:OP_0x0F(); break;

            pc++;
            break;
        default:
            printf("Unimplemented opcode %02X\n", opcode);
            exit(1);
    }
}

void GameBoyColor::NOP(){
     pc += 1;
    cycles += 4;   
}

void GameBoyColor::INC(uint8_t & r){
  uint8_t old=r;
  ++r;

 bool halfCarry = ( (old & 0x0F)+1) > 0x0F;
  setflag(Z,r==0 );
  setflag(N, false);
  setflag(HC,halfCarry);
}


void GameBoyColor::DEC(uint8_t & r) {
    uint8_t old = r;
    r--;

    setflag(Z, r == 0);
    setflag(N, true);
    setflag(HC, (old& 0x0F) == 0);
}

void GameBoyColor::LD_rr_n16(uint8_t & r, uint8_t & r1){
  uint8_t low = memory[pc+1];
  uint8_t high = memory[pc+2];

  r=high;
  r1=low;
}

void GameBoyColor::INC2(uint8_t & r,uint8_t & r1){
  uint8_t address =(r<<8)| r1   ;
  memory[address]=A;
}

void GameBoyColor::OP_0x03(){

  uint16_t BC =(B<<8)| C;
  BC++;
  B=(BC>>8)& 0xFF;
  C=BC&0xFF;
  pc+=1;
  cycles+=8;
}




void GameBoyColor::OP_0x06() {
    B = memory[pc + 1];
    pc += 2;
    cycles += 8;
}

void GameBoyColor::OP_0x07() {
    
  uint8_t MSB=(A >>7) & 1;
    A = (A << 1) | MSB;
    setflag(Z, false);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, MSB);
    pc += 1;
    cycles += 4;
}

void GameBoyColor::OP_0x08() {
    uint16_t address = memory[pc+1] |( memory[pc+2]<<8);
    uint8_t high= (sp>>8) & 0xFF;
    uint8_t low =sp & 0xFF;
    memory[address]=low;
    memory[address+1]=high;
  
    pc += 3;
    cycles += 20;
}


void GameBoyColor::OP_0x09() {
    uint16_t hl = (H << 8) | L;
    uint16_t bc = (B << 8) | C;

    uint32_t result = hl + bc;

    setflag(N, false);
    setflag(HC, ((hl & 0x0FFF) + (bc & 0x0FFF)) > 0x0FFF);
    setflag(CF, result > 0xFFFF);

    hl = result & 0xFFFF;

    H = (hl >> 8) & 0xFF;
    L = hl & 0xFF;

    pc += 1;
    cycles += 8;
}

void GameBoyColor::OP_0x0A() {
    uint16_t bc = (B << 8) | C;

    A=memory[bc];

    pc += 1;
    cycles += 8;
}

void GameBoyColor::OP_0x0B() {
    uint16_t bc = (B << 8) | C;
    bc -=1;  
    B=(bc >> 8) & 0xFF;
    C=bc & 0xFF;
    

    pc += 1;
    cycles += 8;
}

void GameBoyColor::OP_0x0F() {
    uint8_t LSB= A & 0x01;
    A = (A >> 1) | (LSB<<7);
    setflag(Z, false);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, LSB);
    pc += 1;
    cycles += 4;
}


int main() {
    GameBoyColor gameboy;
    gameboy.step();
}
