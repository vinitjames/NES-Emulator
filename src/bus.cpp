#include "bus.h"

Bus::Bus(std::size_t  size)
	:_ram(size, 0){}

void Bus::Write(uint16_t addr, uint8_t data){
	_ram[addr] = data;
}

uint8_t Bus::Read(uint16_t addr, bool bReadOnly ){
	return _ram[addr];
}
