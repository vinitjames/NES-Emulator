#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <vector>
#include "2co2.h"
#include "memory"
#include "cpu.h"
#include "cartridge.h"

class Bus {

public:
	Bus(std::size_t size = 64*1024);
	~Bus();
	void CpuWrite(uint16_t addr, uint8_t data);
	uint8_t  CpuRead(uint16_t addr, bool bReadOnly = false);

	void InsertCartridge(const std::shared_ptr<Cartridge>& cartridge);
	void Reset();
	void Clock();
	

private:
	std::vector<uint8_t> _ram;
	std::unique_ptr<PPU2CO2> _ppu;
	std::unique_ptr<CPU6502> _cpu;
	std::shared_ptr<Cartridge> _cart;
	
	uint32_t _systemClockCounter = 0;
};

#endif /* BUS_H */
