#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <vector>

class Bus;

class OLC6502 {

public:
	OLC6502() = default;
	~OLC6502() = default;
	void WriteToBus(uint16_t addr, uint8_t data);
	uint8_t  ReadFromBus(uint16_t addr) const;
	bool ConnectBus(Bus* bus);

	enum FLAGS6502 {
					C = (1 << 0), //Carry Bit
					Z = (1 << 1), //Zero
					I = (1 << 2), //Disable Interrupts
					D = (1 << 3), //Decimal Mode unusedin current implementation
					B = (1 << 4), //Break
					U = (1 << 5), //Unused
					V = (1 << 6), //Overflow
					N = (1 << 7), //Negative
	};

private:
	struct Registers{
		
		uint8_t accum  = 0x00;  
  		uint8_t x      = 0x00;  
		uint8_t y      = 0x00;
		uint8_t stkp   = 0x00;
		uint16_t pc    = 0x0000; 
		uint8_t status = 0x00;
	};
	
	Registers _regs;
	Bus* _bus = nullptr;
	
	//helper variables for the internal functionality of the CPU 
	uint8_t _fetchedData = 0x00;  
	uint16_t _addrAbs = 0x0000;
	uint8_t _addRel = 0x00;
	uint8_t _currOPcode = 0x00;
	uint8_t _cyclesLeft = 0;

	uint8_t GetFlag(FLAGS6502 flag);
	void    SetFlag(FLAGS6502 flag, bool v);

	//Addressing Modes add funtions

	uint8_t IMP();    uint8_t IMM();
	uint8_t ABS();    uint8_t ABX();
	uint8_t ABY();    uint8_t REL();
	uint8_t ZP0();    uint8_t ZPX();
	uint8_t ZPY();    uint8_t IND();
	uint8_t IZX();    uint8_t IZY(); 

	//OPcodes add funtions
	uint8_t ADC();    uint8_t AND();  uint8_t ASL();  uint8_t BCC();
	uint8_t BCS();    uint8_t BEQ();  uint8_t BIT();  uint8_t BMI();
	uint8_t BNE();    uint8_t BPL();  uint8_t BRK();  uint8_t BVC();
	uint8_t BVS();    uint8_t CLC();  uint8_t CLD();  uint8_t CLI();
	uint8_t CLV();    uint8_t CMP();  uint8_t CPX();  uint8_t CPY();
	uint8_t DEC();    uint8_t DEX();  uint8_t DEY();  uint8_t EOR();
	uint8_t INC();    uint8_t INX();  uint8_t INY();  uint8_t JMP();
	uint8_t JSR();    uint8_t LDA();  uint8_t LDX();  uint8_t LDY();
	uint8_t LSR();    uint8_t NOP();  uint8_t ORA();  uint8_t PHA();
	uint8_t PHP();    uint8_t PLA();  uint8_t PLP();  uint8_t ROL();
	uint8_t ROR();    uint8_t RTI();  uint8_t RTS();  uint8_t SBC();
	uint8_t SEC();    uint8_t SED();  uint8_t SEI();  uint8_t STA();
	uint8_t STX();    uint8_t STY();  uint8_t TAX();  uint8_t TAY();
	uint8_t TSX();    uint8_t TXA();  uint8_t TXS();  uint8_t TYA();

	// functions for internal cpu functionalities
	void Clock();
	void Reset();
	void IRQ();
	void NMI();

	uint8_t FetchData();
	
	
};



#endif /* CPU_H */
