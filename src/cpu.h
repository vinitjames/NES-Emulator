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

	uint8_t GetFlag(FLAGS6502 flag);
	void    SetFlag(FLAGS6502 flag, bool v);
	
};

#endif /* CPU_H */
