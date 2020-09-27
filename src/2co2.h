#ifndef PPU2CO2_H
#define PPU2CO2_H

#include <cstdint>
#include <memory>
#include "cartridge.h"

class PPU2CO2
{
public:
	//! Default constructor
	PPU2CO2();

	//! Copy constructor
	PPU2CO2(const PPU2CO2 &other);

	//! Move constructor
	PPU2CO2(PPU2CO2 &&other) noexcept;

	//! Destructor
	virtual ~PPU2CO2() noexcept;

	//! Copy assignment operator
	PPU2CO2& operator=(const PPU2CO2 &other);

	//! Move assignment operator
	PPU2CO2& operator=(PPU2CO2 &&other) noexcept;

	void CpuWrite(uint16_t addr, uint8_t data);
	uint8_t  CpuRead(uint16_t addr, bool bReadOnly = false);

	void SelfWrite(uint16_t addr, uint8_t data);
	uint8_t  SelfRead(uint16_t addr, bool bReadOnly = false);

	void ConnectCartridge(std::shared_ptr<Cartridge>& cartridge);

protected:
private:
	uint8_t _nameTable[2][1024];
	uint8_t _palette[32];
	
	std::shared_ptr<Cartridge> _cart;
};

#endif /* 2CO2_H */
