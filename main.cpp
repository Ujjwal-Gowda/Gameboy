// bool halfCarry = ((a & 0xF) + (b & 0xF)) > 0xF;
//
// bool halfBorrow = (a & 0xF) < (b & 0xF);
#include <cstdint>
#include <array>
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
    bool stopped=false;
    bool halted=false;
    bool ime = false;
    bool ime_delay = false;
    bool condNZ() { return !getflag(Z); } ;
    bool condZ()  { return  getflag(Z); } ;
    bool condNC() { return !getflag(CF); } ;
    bool condC()  { return  getflag(CF); } ;
    void reset();
    void step();
    void handleInterrupts();
    void NOP();
    void STOP();
    void HALT();
    void INC(uint8_t & r);
    void INCHL();
    void INC2(uint8_t & r,uint8_t & r1);
    void DEC_HL();
    void DEC_SP();
    void INCSP(uint16_t & r);
    void DEC(uint8_t & r);
    void DEC2(uint8_t & r,uint8_t & r1);
    void ADD_A(uint8_t value);
    void ADC_A(uint8_t value);
    void ADD_A_HL();
    void ADD_HL_SP();
    void SUB_A(uint8_t value);
    void SUB_A_HL();
    void SBC_A(uint8_t value);
    void AND_A(uint8_t value);
    void XOR_A(uint8_t value);
    void OR_A(uint8_t value);
    void CP_A(uint8_t value);
    void LD_rr_n16(uint8_t & r,uint8_t & r1);
    void LD_SP_n16();
    void LD_r_n8(uint8_t & r);
    void LD_HL_n8();
    void LD_a16_SP();
    void LD_A_RR(uint8_t & r,uint8_t & r1);
    void LD_RR_A(uint8_t & r,uint8_t & r1);
    void LD_RRP_A(uint8_t & r,uint8_t & r1);
    void LD_RRM_A(uint8_t & r,uint8_t & r1);
    void LD_R_R(uint8_t & r,uint8_t & r1);
    void LD_R_HL(uint8_t & r);
    void LD_HL_R(uint8_t & r);
    void ADD2(uint8_t & r,uint8_t & r1,uint8_t & r2,uint8_t & r3);
    void RLCA();
    void RLA();
    void RRCA();
    void RRA();
    void JR_NZ_e8();
    void JR_NC_e8();
    void JR_e8();
    void JR_Z_e8();
    void JR_C_e8();
    void DAA();
    void SCF();
    void CCF(); 
    void CPL();
    void RET();
    void RET_cond(bool cond);
    void RETI();
    void JP_a16();
    void JP_HL();
    void JP_cond_a16(bool cond); 
    void CALL_a16(); 
    void CALL_cond_a16(bool cond); 
    void RST(uint16_t addr); 
    void POP(uint8_t & r, uint8_t & r1); 
    void PUSH(uint8_t & r,uint8_t & r1);
    void  DI(); 
    void EI(); 
    void PREFIX_CB(); 
    void ADD_SP_e8(); 
    void LD_HL_SP_e8(); 
    void LD_SP_HL(); 
    void LDH_a8_A(); 
    void LDH_C_A(); 
    void LDH_A_a8(); 
    void LDH_A_C(); 
    void LD_a16_A();
    void LD_A_a16();
    void LD_A_HLP();
    void LD_A_HLM();
    uint8_t & getCBReg(uint8_t idx, bool& isHL);
    void RLC(uint8_t& v);
    void RRC(uint8_t& v);
    void RL(uint8_t& v);
    void RR(uint8_t& v);
    void SLA(uint8_t& v);
    void SRA(uint8_t& v);
    void SRL(uint8_t& v);
    void SWAP(uint8_t& v);

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
    if(stopped){
      cycles+=4;
    return ;
  }

    if (halted) {
        uint8_t ie = memory[0xFFFF];
        uint8_t iflag = memory[0xFF0F];
        if ((ie & iflag) == 0) {
            cycles += 4;
            return;
        }
        halted = false;
    }
    switch (opcode) {
        case 0x00: NOP(); pc++;cycles+=4;break;

        case 0x10: STOP();pc+=2;cycles+=4;break;
        // LD_rr_n16 
        case 0x01:LD_rr_n16(B,C); pc+=3; cycles+=12; break;
        case 0x11:LD_rr_n16(D,E); pc+=3; cycles+=12; break;
        case 0x21:LD_rr_n16(H,L); pc+=3; cycles+=12; break;

        case 0x31:LD_SP_n16(); pc+=3; cycles+=12; break;

        // LD_RR_A
        case 0x02:LD_RR_A(B,C);pc++,cycles+=8; break;
        case 0x12:LD_RR_A(D,E);pc++,cycles+=8; break;
        case 0x22:LD_RRP_A(H,L);pc++,cycles+=8; break;
        case 0x32:LD_RRM_A(H,L);pc++,cycles+=8; break;

        
        // INC R 
        case 0x04:INC(B); pc++;cycles+=4; break;
        case 0x14:INC(D); pc++;cycles+=4; break;
        case 0x24:INC(H); pc++;cycles+=4; break;
        case 0x0C:INC(C); pc++;cycles+=4; break;
        case 0x1C:INC(E); pc++;cycles+=4; break;
        case 0x2C:INC(L); pc++;cycles+=4; break;
        case 0x3C:INC(A); pc++;cycles+=4; break;

        case 0x34:INCHL(); pc++;cycles+=12; break;

      
        // INC RR
        case 0x03:INC2(B,C);pc+=1;cycles+=8;break;
        case 0x13:INC2(D,E);pc+=1;cycles+=8;break;
        case 0x23:INC2(H,L);pc+=1;cycles+=8;break;
        case 0x33:INCSP(sp);pc+=1;cycles+=8;break;

        // DEC R
        case 0x05:DEC(B); pc++;cycles+=4; break;
        case 0x15:DEC(D); pc++;cycles+=4; break;
        case 0x25:DEC(H); pc++;cycles+=4; break;
        case 0x0D:DEC(C); pc++;cycles+=4; break;
        case 0x1D:DEC(E); pc++;cycles+=4; break;
        case 0x2D:DEC(L); pc++;cycles+=4; break;
        case 0x3D:DEC(A); pc++;cycles+=4; break;

        case 0x35:DEC_HL(); pc++;cycles+=12; break;

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

        case 0x36:LD_HL_n8();pc+=2;cycles+=12; break;
        
        // LD_R_R
        case 0x40: LD_R_R(B, B); pc++; cycles += 4; break;
        case 0x41: LD_R_R(B, C); pc++; cycles += 4; break;
        case 0x42: LD_R_R(B, D); pc++; cycles += 4; break;
        case 0x43: LD_R_R(B, E); pc++; cycles += 4; break;
        case 0x44: LD_R_R(B, H); pc++; cycles += 4; break;
        case 0x45: LD_R_R(B, L); pc++; cycles += 4; break;
        case 0x47: LD_R_R(B, A); pc++; cycles += 4; break;

        case 0x48: LD_R_R(C, B); pc++; cycles += 4; break;
        case 0x49: LD_R_R(C, C); pc++; cycles += 4; break;
        case 0x4A: LD_R_R(C, D); pc++; cycles += 4; break;
        case 0x4B: LD_R_R(C, E); pc++; cycles += 4; break;
        case 0x4C: LD_R_R(C, H); pc++; cycles += 4; break;
        case 0x4D: LD_R_R(C, L); pc++; cycles += 4; break;
        case 0x4F: LD_R_R(C, A); pc++; cycles += 4; break;

        case 0x50: LD_R_R(D, B); pc++; cycles += 4; break;
        case 0x51: LD_R_R(D, C); pc++; cycles += 4; break;
        case 0x52: LD_R_R(D, D); pc++; cycles += 4; break;
        case 0x53: LD_R_R(D, E); pc++; cycles += 4; break;
        case 0x54: LD_R_R(D, H); pc++; cycles += 4; break;
        case 0x55: LD_R_R(D, L); pc++; cycles += 4; break;
        case 0x57: LD_R_R(D, A); pc++; cycles += 4; break;

        case 0x58: LD_R_R(E, B); pc++; cycles += 4; break;
        case 0x59: LD_R_R(E, C); pc++; cycles += 4; break;
        case 0x5A: LD_R_R(E, D); pc++; cycles += 4; break;
        case 0x5B: LD_R_R(E, E); pc++; cycles += 4; break;
        case 0x5C: LD_R_R(E, H); pc++; cycles += 4; break;
        case 0x5D: LD_R_R(E, L); pc++; cycles += 4; break;
        case 0x5F: LD_R_R(E, A); pc++; cycles += 4; break;

        case 0x60: LD_R_R(H, B); pc++; cycles += 4; break;
        case 0x61: LD_R_R(H, C); pc++; cycles += 4; break;
        case 0x62: LD_R_R(H, D); pc++; cycles += 4; break;
        case 0x63: LD_R_R(H, E); pc++; cycles += 4; break;
        case 0x64: LD_R_R(H, H); pc++; cycles += 4; break;
        case 0x65: LD_R_R(H, L); pc++; cycles += 4; break;
        case 0x67: LD_R_R(H, A); pc++; cycles += 4; break;

        case 0x68: LD_R_R(L, B); pc++; cycles += 4; break;
        case 0x69: LD_R_R(L, C); pc++; cycles += 4; break;
        case 0x6A: LD_R_R(L, D); pc++; cycles += 4; break;
        case 0x6B: LD_R_R(L, E); pc++; cycles += 4; break;
        case 0x6C: LD_R_R(L, H); pc++; cycles += 4; break;
        case 0x6D: LD_R_R(L, L); pc++; cycles += 4; break;
        case 0x6F: LD_R_R(L, A); pc++; cycles += 4; break;

        case 0x78: LD_R_R(A, B); pc++; cycles += 4; break;
        case 0x79: LD_R_R(A, C); pc++; cycles += 4; break;
        case 0x7A: LD_R_R(A, D); pc++; cycles += 4; break;
        case 0x7B: LD_R_R(A, E); pc++; cycles += 4; break;
        case 0x7C: LD_R_R(A, H); pc++; cycles += 4; break;
        case 0x7D: LD_R_R(A, L); pc++; cycles += 4; break;
        case 0x7F: LD_R_R(A, A); pc++; cycles += 4; break;

        case 0x46:LD_R_HL(B);pc++;cycles+=8; break;
        case 0x4E:LD_R_HL(C);pc++;cycles+=8; break;
        case 0x56:LD_R_HL(D);pc++;cycles+=8; break;
        case 0x5E:LD_R_HL(E);pc++;cycles+=8; break;
        case 0x66:LD_R_HL(H);pc++;cycles+=8; break;
        case 0x6E:LD_R_HL(L);pc++;cycles+=8; break;
        case 0x7E:LD_R_HL(A);pc++;cycles+=8; break;

        // LD_HL_R
        case 0x70: LD_HL_R(B); pc++; cycles += 8; break;
        case 0x71: LD_HL_R(C); pc++; cycles += 8; break;
        case 0x72: LD_HL_R(D); pc++; cycles += 8; break;
        case 0x73: LD_HL_R(E); pc++; cycles += 8; break;
        case 0x74: LD_HL_R(H); pc++; cycles += 8; break;
        case 0x75: LD_HL_R(L); pc++; cycles += 8; break;
        case 0x77: LD_HL_R(A); pc++; cycles += 8; break;


        case 0x76: HALT(); pc++; cycles += 4; break;
        
        // ADD_A  
        case 0x80: ADD_A(B); pc++; cycles+=4; break;
        case 0x81: ADD_A(C); pc++; cycles+=4; break;
        case 0x82: ADD_A(D); pc++; cycles+=4; break;
        case 0x83: ADD_A(E); pc++; cycles+=4; break;
        case 0x84: ADD_A(H); pc++; cycles+=4; break;
        case 0x85: ADD_A(L); pc++; cycles+=4; break;
        case 0x86: ADD_A(memory[(H<<8)|L]); pc++; cycles+=8; break;
        case 0x87: ADD_A(A); pc++; cycles+=4; break;

        // ADC A
        case 0x88: ADC_A(B); pc++; cycles+=4; break;
        case 0x89: ADC_A(C); pc++; cycles+=4; break;
        case 0x8A: ADC_A(D); pc++; cycles+=4; break;
        case 0x8B: ADC_A(E); pc++; cycles+=4; break;
        case 0x8C: ADC_A(H); pc++; cycles+=4; break;
        case 0x8D: ADC_A(L); pc++; cycles+=4; break;
        case 0x8E: ADC_A(memory[(H<<8)|L]); pc++; cycles+=8; break;
        case 0x8F: ADC_A(A); pc++; cycles+=4; break;

        // SUB A
        case 0x90: SUB_A(B); pc++; cycles+=4; break;
        case 0x91: SUB_A(C); pc++; cycles+=4; break;
        case 0x92: SUB_A(D); pc++; cycles+=4; break;
        case 0x93: SUB_A(E); pc++; cycles+=4; break;
        case 0x94: SUB_A(H); pc++; cycles+=4; break;
        case 0x95: SUB_A(L); pc++; cycles+=4; break;
        case 0x96: SUB_A(memory[(H<<8)|L]); pc++; cycles+=8; break;
        case 0x97: SUB_A(A); pc++; cycles+=4; break;

        // SBC A
        case 0x98: SBC_A(B); pc++; cycles+=4; break;
        case 0x99: SBC_A(C); pc++; cycles+=4; break;
        case 0x9A: SBC_A(D); pc++; cycles+=4; break;
        case 0x9B: SBC_A(E); pc++; cycles+=4; break;
        case 0x9C: SBC_A(H); pc++; cycles+=4; break;
        case 0x9D: SBC_A(L); pc++; cycles+=4; break;
        case 0x9E: SBC_A(memory[(H<<8)|L]); pc++; cycles+=8; break;
        case 0x9F: SBC_A(A); pc++; cycles+=4; break;

        // AND A
        case 0xA0: AND_A(B); pc++; cycles+=4; break;
        case 0xA1: AND_A(C); pc++; cycles+=4; break;
        case 0xA2: AND_A(D); pc++; cycles+=4; break;
        case 0xA3: AND_A(E); pc++; cycles+=4; break;
        case 0xA4: AND_A(H); pc++; cycles+=4; break;
        case 0xA5: AND_A(L); pc++; cycles+=4; break;
        case 0xA6: AND_A(memory[(H<<8)|L]); pc++; cycles+=8; break;
        case 0xA7: AND_A(A); pc++; cycles+=4; break;



        // RLCA
        case 0x07:RLCA();pc++;cycles+=4; break;
        case 0x17:RLA();pc++;cycles+=4; break;
        
        // LD_a16_SP
        case 0x08:LD_a16_SP();pc+=3;cycles+=20; break;
        
        // ADD2
        case 0x09:ADD2(H,L,B,C);pc+=1;cycles+=8; break;
        case 0x19:ADD2(H,L,D,E);pc+=1;cycles+=8; break;
        case 0x29:ADD2(H,L,H,L);pc+=1;cycles+=8; break;

        
        case 0x18: JR_e8(); break;
        case 0x28: JR_Z_e8(); break;
        case 0x38: JR_C_e8(); break;


        case 0x27: DAA(); pc++; cycles+=4; break;
        case 0x2F: CPL(); pc++; cycles+=4; break;
        case 0x37: SCF(); pc++; cycles+=4; break;
        case 0x3F: CCF(); pc++; cycles+=4; break;

        // LD_A_RR
        case 0x0A:LD_A_RR(B,C);pc++;cycles+=8; break;
        case 0x1A:LD_A_RR(D,E);pc++;cycles+=8; break;
        
        case 0x20:JR_NZ_e8(); break;
        case 0x30:JR_NC_e8(); break;
    
        // RRCA
        case 0x0F:RRCA();pc++;cycles+=4; break;

        // Cx
        case 0xC0: RET_cond(condNZ()); pc++; cycles += condNZ() ? 20 : 8; break;
        case 0xC1: POP(B, C); pc++; cycles += 12; break;
        case 0xC2: JP_cond_a16(condNZ()); cycles += condNZ() ? 16 : 12; break;
        case 0xC3: JP_a16(); cycles += 16; break;
        case 0xC4: CALL_cond_a16(condNZ()); cycles += condNZ() ? 24 : 12; break;
        case 0xC5: PUSH(B, C); pc++; cycles += 16; break;
        case 0xC6: ADD_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xC7: RST(0x00); cycles += 16; break;

        case 0xC8: RET_cond(condZ()); pc++; cycles += condZ() ? 20 : 8; break;
        case 0xC9: RET(); cycles += 16; break;
        case 0xCA: JP_cond_a16(condZ()); cycles += condZ() ? 16 : 12; break;
        case 0xCB: PREFIX_CB(); cycles += 4; break;
        case 0xCC: CALL_cond_a16(condZ()); cycles += condZ() ? 24 : 12; break;
        case 0xCD: CALL_a16(); cycles += 24; break;
        case 0xCE: ADC_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xCF: RST(0x08); cycles += 16; break;


        // Dx
        case 0xD0: RET_cond(condNC()); pc++; cycles += condNC() ? 20 : 8; break;
        case 0xD1: POP(D, E); pc++; cycles += 12; break;
        case 0xD2: JP_cond_a16(condNC()); cycles += condNC() ? 16 : 12; break;
        case 0xD4: CALL_cond_a16(condNC()); cycles += condNC() ? 24 : 12; break;
        case 0xD5: PUSH(D, E); pc++; cycles += 16; break;
        case 0xD6: SUB_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xD7: RST(0x10); cycles += 16; break;

        case 0xD8: RET_cond(condC()); pc++; cycles += condC() ? 20 : 8; break;
        case 0xD9: RETI(); cycles += 16; break;
        case 0xDA: JP_cond_a16(condC()); cycles += condC() ? 16 : 12; break;
        case 0xDC: CALL_cond_a16(condC()); cycles += condC() ? 24 : 12; break;
        case 0xDE: SBC_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xDF: RST(0x18); cycles += 16; break;

        // Ex
        case 0xE0: LDH_a8_A(); break;
        case 0xE1: POP(H, L); pc++; cycles += 12; break;
        case 0xE2: LDH_C_A(); break;
        case 0xE5: PUSH(H, L); pc++; cycles += 16; break;
        case 0xE6: AND_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xE7: RST(0x20); cycles += 16; break;

        case 0xE8: ADD_SP_e8(); break;
        case 0xE9: JP_HL(); cycles += 4; break;
        case 0xEA: LD_a16_A(); break;
        case 0xEE: XOR_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xEF: RST(0x28); cycles += 16; break;


        // Fx
        case 0xF0: LDH_A_a8(); break;
        case 0xF1: POP(A, F); F &= 0xF0; pc++; cycles += 12; break;
        case 0xF2: LDH_A_C(); break;
        case 0xF3: DI(); pc++; cycles += 4; break;
        case 0xF5: PUSH(A, F); pc++; cycles += 16; break;
        case 0xF6: OR_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xF7: RST(0x30); cycles += 16; break;

        case 0xF8: LD_HL_SP_e8(); break;
        case 0xF9: LD_SP_HL(); break;
        case 0xFA: LD_A_a16(); break;
        case 0xFB: EI(); pc++; cycles += 4; break;
        case 0xFE: CP_A(memory[pc+1]); pc += 2; cycles += 8; break;
        case 0xFF: RST(0x38); cycles += 16; break;


        case 0x1F:RRA();pc += 1;cycles += 4;break;
        case 0x2A:LD_A_HLP();pc += 1;cycles += 8;break;
        case 0x3A:LD_A_HLM();pc += 1;cycles += 8;break;
        case 0x39:ADD_HL_SP();pc += 1;cycles += 8;break;
        case 0x3B:DEC_SP();pc += 1;cycles += 8;break;
        default:
            printf("Unimplemented opcode %02X\n", opcode);
            exit(1);

    }

if (ime_delay) {
    ime = true;
    ime_delay = false;
}
}

void GameBoyColor::handleInterrupts() {
    if (!ime) return;
    uint8_t ie = memory[0xFFFF];
    uint8_t iflag = memory[0xFF0F];

    uint8_t pending = ie & iflag;
    if (!pending) return;
    ime = false;        
    halted = false;

    for (int i = 0; i < 5; i++) {
        if (pending & (1 << i)) {
            memory[0xFF0F] &= ~(1 << i);  
            uint16_t ret = pc;
            sp--;
            memory[sp] = (ret >> 8) & 0xFF;
            sp--;
            memory[sp] = ret & 0xFF;
            pc = 0x40 + i * 8; 
            cycles += 20;
            break;
        }
    }
}

void GameBoyColor::NOP(){
}

void GameBoyColor::STOP(){
  stopped=true;
}

void GameBoyColor::HALT(){
  halted=true;
}

void GameBoyColor::INC(uint8_t & r){
  uint8_t old=r;
  ++r;

 bool halfCarry = ( (old & 0x0F)+1) > 0x0F;
  setflag(Z,r==0 );
  setflag(N, false);
  setflag(HC,halfCarry);
}

void GameBoyColor::INCHL()
{
    uint16_t addr = (H << 8) | L;
    uint8_t val = memory[addr];

    bool halfCarry = (val & 0x0F) == 0x0F; 
    val++;  
    memory[addr] = val;

    setflag(Z, val == 0);
    setflag(N, false);
    setflag(HC, halfCarry);
}


void GameBoyColor::DEC(uint8_t & r) {
    uint8_t old = r;
    r--;

    setflag(Z, r == 0);
    setflag(N, true);
    setflag(HC, (old& 0x0F) == 0);
}

void GameBoyColor::DEC_HL()
{
    uint16_t addr = (H << 8) | L;
    uint8_t val = memory[addr];

    bool halfborrow = (val & 0x0F) == 0; 
    val--;  
    memory[addr] = val;

    setflag(Z, val == 0);
    setflag(N, true);
    setflag(HC, halfborrow);
}

void GameBoyColor::LD_rr_n16(uint8_t & r, uint8_t & r1){
  uint8_t low = memory[pc+1];
  uint8_t high = memory[pc+2];

  r=high;
  r1=low;
}
void GameBoyColor::LD_SP_n16(){
  uint8_t low = memory[pc+1];
  uint8_t high = memory[pc+2];

  sp =(high<<8)|low;
}

void GameBoyColor::LD_RR_A(uint8_t & r, uint8_t & r1){
  uint16_t address = (r<<8)|r1;
  memory[address]=A;
}

void GameBoyColor::LD_RRP_A(uint8_t & r, uint8_t & r1){
  uint16_t rr1 = (r<<8)|r1;
  memory[rr1]=A;
  rr1++;   
  H = (rr1 >> 8) & 0xFF;
  L = rr1 & 0xFF;
}

void GameBoyColor::LD_RRM_A(uint8_t & r, uint8_t & r1){
  uint16_t rr1 = (r<<8)|r1;
  memory[rr1]=A;
  rr1--;   
  H = (rr1 >> 8) & 0xFF;
  L = rr1 & 0xFF;
}


void GameBoyColor::INC2(uint8_t & r,uint8_t & r1){
    uint16_t rr = (r << 8) | r1;
    rr++;

    r  = (rr >> 8) & 0xFF;
    r1 = rr & 0xFF;
}

void GameBoyColor::INCSP(uint16_t & r){
  sp++;
}


void GameBoyColor::LD_r_n8(uint8_t & r) {
    r = memory[pc + 1];
}

void GameBoyColor::LD_R_R(uint8_t & r ,uint8_t & r1) {
    r = r1;
}

void GameBoyColor::LD_R_HL(uint8_t & r) {
  uint16_t HL=(H<<8)|L;
    r = memory[HL];
}

void GameBoyColor::LD_HL_R(uint8_t & r) {
  uint16_t HL=(H<<8)|L;
  memory[HL]=r;
}

void GameBoyColor::LD_HL_n8() {
    uint8_t val = memory[pc + 1];

    uint16_t address = (H << 8) | L;
    memory[address]=val;
}


void GameBoyColor::RLCA() {
  uint8_t MSB=(A >>7) & 1;
    A = (A << 1) | MSB;
    setflag(Z, false);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, MSB);
}

void GameBoyColor::RLA() {
  uint8_t oldCF=getflag(CF) ? 1 : 0;
  uint8_t MSB=(A >>7) & 1;
    A = (A << 1) | oldCF;
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

void GameBoyColor::JR_NZ_e8(){
  uint8_t offset = static_cast<uint8_t>(memory[pc+1]);

  if(!getflag(Z)){
    pc+=offset;
    cycles+=12;
  }else{
    cycles+=8;
  }
}

void GameBoyColor::JR_NC_e8(){
  uint8_t offset = static_cast<uint8_t>(memory[pc+1]);

  if(!getflag(CF)){
    pc+=offset;
    cycles+=12;
  }else{
    cycles+=8;
  }
}

void GameBoyColor::JR_e8()
{
    int8_t offset = static_cast<int8_t>(memory[pc + 1]);
    pc += 2;
    pc += offset;
    cycles += 12;
}

void GameBoyColor::JR_Z_e8()
{
    int8_t offset = static_cast<int8_t>(memory[pc + 1]);
    pc += 2;

    if (getflag(Z)) {
        pc += offset;
        cycles += 12;
    } else {
        cycles += 8;
    }
}

void GameBoyColor::JR_C_e8()
{
    int8_t offset = static_cast<int8_t>(memory[pc + 1]);
    pc += 2;

    if (getflag(CF)) {
        pc += offset;
        cycles += 12;
    } else {
        cycles += 8;
    }
}

void GameBoyColor::DAA()
{
    uint8_t correction = 0;
    bool carry = getflag(CF);

    if (!getflag(N)) {
        if (getflag(HC) || (A & 0x0F) > 9)
            correction |= 0x06;
        if (carry || A > 0x99) {
            correction |= 0x60;
            carry = true;
        }
        A += correction;
    } else {
        if (getflag(HC))
            correction |= 0x06;
        if (carry)
            correction |= 0x60;
        A -= correction;
    }

    setflag(Z, A == 0);
    setflag(HC, false);
    setflag(CF, carry);
}

void GameBoyColor::CCF()
{
    setflag(CF, !getflag(CF));
    setflag(N, false);
    setflag(HC, false);
}

void GameBoyColor::SCF()
{
    setflag(CF, true);
    setflag(N, false);
    setflag(HC, false);
}

void GameBoyColor::CPL()
{
    A = ~A;
    setflag(N, true);
    setflag(HC, true);
}

void GameBoyColor::ADD_A(uint8_t value){
    uint8_t oldA = A;
    uint16_t result = oldA + value;

    A = result & 0xFF;

    setflag(Z, A==0);
    setflag(N, false);
    setflag(HC, ((oldA & 0x0F) + (value & 0x0F)) > 0x0F);
    setflag(CF, result > 0xFF);
}


void GameBoyColor::SUB_A(uint8_t value){
    uint8_t oldA = A;
    uint16_t result = oldA - value;

    A = result & 0xFF;

    setflag(Z, A==0);
    setflag(N, true);
    setflag(HC, ((oldA & 0xF) <( value & 0xF)));
    setflag(CF, oldA < value);
}


void GameBoyColor::ADC_A(uint8_t value){
    uint8_t oldA = A;

    uint8_t carry = getflag(CF) ? 1 : 0;
    uint16_t result = oldA + value+carry;
    A = result & 0xFF;

    setflag(Z, A==0);
    setflag(N, false);
    setflag(HC, ((oldA & 0x0F) + (value & 0x0F)+carry) > 0x0F);
    setflag(CF, result > 0xFF);
}

void GameBoyColor::SBC_A(uint8_t value){
    uint8_t oldA = A;

    uint8_t carry = getflag(CF) ? 1 : 0;
    uint16_t result = oldA - value -carry ;

    A = result & 0xFF;

    setflag(Z, A==0);
    setflag(N, true);
    setflag(HC, ((oldA & 0xF) <( value & 0xF)+carry));
    setflag(CF, oldA < value+ carry);
}

void GameBoyColor::AND_A(uint8_t value){

    uint16_t result = A & value;
    A = result & 0xFF;

    setflag(Z, A==0);
    setflag(N, false);
    setflag(HC, true);
    setflag(CF, false);
}

void GameBoyColor::XOR_A(uint8_t value){
  
    uint16_t result = A ^ value;
    A = result & 0xFF;

    setflag(Z, A==0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, false);
}

void GameBoyColor::OR_A(uint8_t value){
  
    uint16_t result = A | value;
    A = result & 0xFF;

    setflag(Z, A==0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, false);
}
void GameBoyColor::CP_A(uint8_t value){
    uint16_t result = A - value;

    result=result & 0xFF;

    setflag(Z,( A-value )==0);
    setflag(N, true);
    setflag(HC, ((A & 0xF) <( value & 0xF)));
    setflag(CF, A < value);
}

void GameBoyColor::RET(){
    uint8_t low  = memory[sp];
    sp++;
    uint8_t high = memory[sp];
    sp++;
    uint16_t addr=(high << 8) | low;
    pc = addr;
}

void GameBoyColor::RET_cond(bool cond) {
    if (cond) {
        RET();
        cycles += 4;   
    } else {
        cycles += 8;
    }
}

void GameBoyColor::RETI() {
    uint8_t low  = memory[sp];
    sp++;
    uint8_t high = memory[sp];
    sp++;
    uint16_t addr=(high << 8) | low;
    pc = addr;
}

void GameBoyColor::JP_a16() {
    uint8_t low  = memory[pc + 1];
    uint8_t high = memory[pc + 2];
    pc = (high << 8) | low;
}

void GameBoyColor::JP_HL() {
    uint16_t addr=(H << 8) | L;
    pc = addr;
}

void GameBoyColor::JP_cond_a16(bool cond) {
  if( cond){
    uint8_t low  = memory[pc + 1];
    uint8_t high = memory[pc + 2];
    pc = (high << 8) | low;
  }else{
    pc+=3;
  }
}

void GameBoyColor::CALL_a16(){
    uint8_t low  = memory[pc + 1];
    uint8_t high = memory[pc + 2];
    uint16_t target = (high << 8) | low;
    uint16_t ret = pc + 3;
    sp--;
    memory[sp] = (ret >> 8) & 0xFF;
    sp--;
    memory[sp] = ret & 0xFF;
    pc = target;

}
void GameBoyColor::CALL_cond_a16(bool cond){
  if(cond){
    uint8_t low  = memory[pc + 1];
    uint8_t high = memory[pc + 2];
    uint16_t target = (high << 8) | low;
    uint16_t ret = pc + 3;
    sp--;
    memory[sp] = (ret >> 8) & 0xFF;
    sp--;
    memory[sp] = ret & 0xFF;
    pc = target;
    }else{
    pc+=3;
  }
}

void GameBoyColor::RST(uint16_t addr){
  uint16_t ret = pc+1;
  uint8_t high = (ret>>8) & 0xFF;
  uint8_t low = ret & 0xFF;

  PUSH(high,low);
  pc=addr;
}

void GameBoyColor::PUSH(uint8_t & r,uint8_t & r1 ){
  sp--;
  memory[sp]=r;
  sp--;
  memory[sp]=r1;
}

void GameBoyColor::POP(uint8_t & r,uint8_t & r1){
  uint8_t low =memory[sp];
  uint8_t high =memory[sp+1];
  sp+=2;
  r=high;
  r1=low;
}

void GameBoyColor::DI() {
    ime = false;
    ime_delay = false;
}

void GameBoyColor::EI() {
    ime_delay = true;
}

uint8_t& GameBoyColor::getCBReg(uint8_t idx, bool& isHL) {
    isHL = false;
    switch (idx) {
        case 0: return B;
        case 1: return C;
        case 2: return D;
        case 3: return E;
        case 4: return H;
        case 5: return L;
        case 7: return A;
        default:
            isHL = true;
            return memory[(H << 8) | L]; 
    }
}

void GameBoyColor::PREFIX_CB() {
    uint8_t cb = memory[pc + 1];
    pc += 2;

    uint8_t reg = cb & 0x07;
    uint8_t y   = (cb >> 3) & 0x07;
    uint8_t grp = cb >> 6;

    bool isHL = false;
    uint8_t& v = getCBReg(reg, isHL);

    cycles += isHL ? 16 : 8;

    switch (grp) {

    // ================= ROTATE / SHIFT =================
    case 0x00:
        switch (y) {
            case 0: RLC(v); break;
            case 1: RRC(v); break;
            case 2: RL(v);  break;
            case 3: RR(v);  break;
            case 4: SLA(v); break;
            case 5: SRA(v); break;
            case 6: SWAP(v);break;
            case 7: SRL(v); break;
        }
        break;

    // ================= BIT =================
    case 0x01:
        setflag(Z, !(v & (1 << y)));
        setflag(N, false);
        setflag(HC, true);
        break;

    // ================= RES =================
    case 0x02:
        v &= ~(1 << y);
        break;

    // ================= SET =================
    case 0x03:
        v |= (1 << y);
        break;
    }
}


void GameBoyColor::ADD_SP_e8() {
    int8_t e = (int8_t)memory[pc + 1];
    uint16_t old = sp;
    uint16_t result = sp + e;

    setflag(Z, false);
    setflag(N, false);
    setflag(HC, ((old & 0xF) + (e & 0xF)) > 0xF);
    setflag(CF, ((old & 0xFF) + (e & 0xFF)) > 0xFF);

    sp = result;
}

void GameBoyColor::LD_HL_SP_e8() {
    int8_t e = (int8_t)memory[pc + 1];
    uint16_t result = sp + e;

    setflag(Z, false);
    setflag(N, false);
    setflag(HC, ((sp & 0xF) + (e & 0xF)) > 0xF);
    setflag(CF, ((sp & 0xFF) + (e & 0xFF)) > 0xFF);

    H = (result >> 8) & 0xFF;
    L = result & 0xFF;

}

void GameBoyColor::LD_SP_HL() {
    sp = (H << 8) | L;
}

void GameBoyColor::LDH_a8_A() {
    uint8_t a8 = memory[pc + 1];
    memory[0xFF00 + a8] = A;
}

void GameBoyColor::LDH_C_A() {
    memory[0xFF00 + C] = A;
}

void GameBoyColor::LDH_A_a8() {
    uint8_t a8 = memory[pc + 1];
    A = memory[0xFF00 + a8];
}

void GameBoyColor::LDH_A_C() {
    A = memory[0xFF00 + C];
}

void GameBoyColor::LD_a16_A() {
    uint16_t addr = memory[pc + 1] | (memory[pc + 2] << 8);
    memory[addr] = A;
}

void GameBoyColor::LD_A_a16() {
    uint16_t addr = memory[pc + 1] | (memory[pc + 2] << 8);
    A = memory[addr];
}
void GameBoyColor::RRA(){
  uint8_t oldCarry = getflag(CF);     // 0 or 1
  uint8_t newCarry = A & 0x01;
  A = (A >> 1) | (oldCarry << 7);
  setflag(Z, false);
  setflag(N, false);
  setflag(HC, false);
  setflag(CF, newCarry);
}
void GameBoyColor::LD_A_HLP(){
  uint16_t HL= (H<<8)|L;
  A=memory[HL];
  HL++;
  H = (HL >> 8) & 0xFF;
  L = HL & 0xFF;
}

void GameBoyColor::LD_A_HLM(){
  uint16_t HL= (H<<8)|L;
  A=memory[HL];
  HL--;
  H = (HL >> 8) & 0xFF;
  L = HL & 0xFF;
}

void GameBoyColor::ADD_HL_SP(){
  uint16_t HL = (H << 8) | L;
  uint32_t result = HL + sp;

  setflag(N, false);
  setflag(HC, ((HL & 0x0FFF) + (sp & 0x0FFF)) > 0x0FFF);
  setflag(CF, result > 0xFFFF);

  HL = result & 0xFFFF;
  H = (HL >> 8) & 0xFF;
  L = HL & 0xFF;
}

void GameBoyColor::DEC_SP()
{
  sp--;
}

void GameBoyColor::RLC(uint8_t& v) {
    uint8_t msb = (v >> 7) & 1;
    v = (v << 1) | msb;
    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, msb);
}

void GameBoyColor::RRC(uint8_t &v) {
    uint8_t bit0 = v & 0x01;
    v = (v >> 1) | (bit0 << 7);
    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, bit0);
}
void GameBoyColor::RL(uint8_t &v) {
    uint8_t oldC = getflag(CF);
    uint8_t bit7 = (v >> 7) & 1;

    v = (v << 1) | oldC;

    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, bit7);
}

void GameBoyColor::SLA(uint8_t &v) {
    uint8_t bit7 = (v >> 7) & 1;
    v <<= 1;

    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, bit7);
}

void GameBoyColor::SRA(uint8_t &v) {
    uint8_t bit0 = v & 1;
    uint8_t msb  = v & 0x80;

    v = (v >> 1) | msb;

    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, bit0);
}
void GameBoyColor::RR(uint8_t &v) {
    uint8_t oldC = getflag(CF);
    uint8_t bit0 = v & 1;

    v = (v >> 1) | (oldC << 7);

    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, bit0);
}

void GameBoyColor::SWAP(uint8_t &v) {
    v = (v << 4) | (v >> 4);

    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, false);
}

void GameBoyColor::SRL(uint8_t &v) {
    uint8_t bit0 = v & 1;
    v >>= 1;

    setflag(Z, v == 0);
    setflag(N, false);
    setflag(HC, false);
    setflag(CF, bit0);
}
int main() {
    GameBoyColor gameboy;
    gameboy.step();
}
