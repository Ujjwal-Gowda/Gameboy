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

    void NOP();
    void INC(uint8_t & r);
    void INC2(uint8_t & r,uint8_t & r1);
    void DEC(uint8_t & r);
    void DEC2(uint8_t & r,uint8_t & r1);
    void LD_rr_n16(uint8_t & r,uint8_t & r1);
    void LD_r_n8(uint8_t & r);
    void LD_a16_SP();
    void LD_A_RR(uint8_t & r,uint8_t & r1);
    void LD_RR_A(uint8_t & r,uint8_t & r1);
    void ADD2(uint8_t & r,uint8_t & r1,uint8_t & r2,uint8_t & r3);
    void RLCA();
    void RRCA();
    
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
        case 0x00: NOP(); pc++;cycles+=4;break;

        // LD_rr_n16 
        case 0x01:LD_rr_n16(B,C); pc+=3; cycles+=12; break;
        case 0x11:LD_rr_n16(D,E); pc+=3; cycles+=12; break;
        case 0x21:LD_rr_n16(H,L); pc+=3; cycles+=12; break;

        // LD_RR_A
        case 0x02:LD_RR_A(B,C);pc++,cycles+=8; break;
        case 0x12:LD_RR_A(D,E);pc++,cycles+=8; break;

        
        // INC R 
        case 0x04:INC(B); pc++;cycles+=4; break;
        case 0x14:INC(D); pc++;cycles+=4; break;
        case 0x24:INC(H); pc++;cycles+=4; break;
        case 0x0C:INC(C); pc++;cycles+=4; break;
        case 0x1C:INC(E); pc++;cycles+=4; break;
        case 0x2C:INC(L); pc++;cycles+=4; break;
        case 0x3C:INC(A); pc++;cycles+=4; break;

      
        // INC RR
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

        // DEC RR
        case 0x0B:DEC2(B,C);pc++;cycles+=8; break;
        case 0x1B:DEC2(D,E);pc++;cycles+=8; break;
        case 0x2B:DEC2(H,L);pc++;cycles+=8; break;


        // LD_r_n8
        case 0x06:LD_r_n8(B);pc+=2;cycles+=8; break;
        case 0x16:LD_r_n8(D);pc+=2;cycles+=8; break;
        case 0x26:LD_r_n8(H);pc+=2;cycles+=8; break;
        case 0x0E:LD_r_n8(C);pc+=2;cycles+=8; break;
        case 0x1E:LD_r_n8(E);pc+=2;cycles+=8; break;
        case 0x2E:LD_r_n8(L);pc+=2;cycles+=8; break;
        case 0x3E:LD_r_n8(A);pc+=2;cycles+=8; break;

        // RLCA
        case 0x07:RLCA();pc++;cycles+=4; break;
        
        // LD_a16_SP
        case 0x08:LD_a16_SP();pc+=3;cycles+=20; break;
        
        // ADD2
        case 0x09:ADD2(H,L,B,C);pc+=1;cycles+=8; break;
        case 0x19:ADD2(H,L,D,E);pc+=1;cycles+=8; break;
        case 0x29:ADD2(H,L,H,L);pc+=1;cycles+=8; break;

        // LD_A_RR
        case 0x0A:LD_A_RR(B,C);pc++;cycles+=8; break;
        case 0x1A:LD_A_RR(D,E);pc++;cycles+=8; break;
        
        // RRCA
        case 0x0F:RRCA();pc++;cycles+=4; break;
        default:
            printf("Unimplemented opcode %02X\n", opcode);
            exit(1);
    }
}

void GameBoyColor::NOP(){
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

void GameBoyColor::LD_RR_A(uint8_t & r, uint8_t & r1){
  uint16_t address = (r<<8)|r1;
  memory[address]=A;
}


void GameBoyColor::INC2(uint8_t & r,uint8_t & r1){
    uint16_t rr = (r << 8) | r1;
    rr++;

    r  = (rr >> 8) & 0xFF;
    r1 = rr & 0xFF;
}


void GameBoyColor::LD_r_n8(uint8_t & r) {
    r = memory[pc + 1];
}


void GameBoyColor::RLCA() {
  uint8_t MSB=(A >>7) & 1;
    A = (A << 1) | MSB;
    setflag(Z, false);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, MSB);
}


void GameBoyColor::LD_a16_SP() {
    uint16_t address = memory[pc+1] |( memory[pc+2]<<8);
    uint8_t high= (sp>>8) & 0xFF;
    uint8_t low =sp & 0xFF;
    memory[address]=low;
    memory[address+1]=high;
}


void GameBoyColor::ADD2(uint8_t & r,uint8_t & r1 ,uint8_t & r2,uint8_t & r3 ) {
    uint16_t rr1 = (r << 8) | r1;
    uint16_t r2r3 = (r2 << 8) | r3;

    uint32_t result = rr1 + r2r3;

    setflag(N, false);
    setflag(HC, ((rr1 & 0x0FFF) + (r2r3 & 0x0FFF)) > 0x0FFF);
    setflag(CF, result > 0xFFFF);

    rr1 = result & 0xFFFF;

    r = (rr1 >> 8) & 0xFF;
    r1 = rr1 & 0xFF;
}


void GameBoyColor::LD_A_RR(uint8_t & r ,uint8_t & r1) {
    uint16_t rr1 = (r << 8) | r1;

    A=memory[rr1];
}


void GameBoyColor::DEC2(uint8_t & r,uint8_t & r1){
    uint16_t rr1 = (r << 8) | r1;
    rr1 -=1;  
    r=(rr1 >> 8) & 0xFF;
    r1=rr1 & 0xFF;
}

void GameBoyColor::RRCA() {
    uint8_t LSB= A & 0x01;
    A = (A >> 1) | (LSB<<7);
    setflag(Z, false);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, LSB);
}


int main() {
    GameBoyColor gameboy;
    gameboy.step();
}
