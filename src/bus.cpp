#include "bus.h"

Bus::Bus(std::size_t  size)
	:_ram(size, 0){}

void Bus::CpuWrite(uint16_t addr, uint8_t data){
	if(addr >= 0x0000 && addr <= 0x1FFF)
		_ram[addr & 0x07FF] = data;
	else if(addr >= 0x2000 && addr <= 0x3FFF)
		_ppu->CpuWrite(addr & 0x0007, data);
}

uint8_t Bus::CpuRead(uint16_t addr, bool bReadOnly ){
	if(addr >= 0x0000 && addr <= 0x1FFF)
		return _ram[addr & 0x07FF];
	else if(addr >= 0x2000 && addr <= 0x3FFF)
		return _ppu->CpuRead(addr & 0x0007, bReadOnly);
	return 0x00;
}

void Bus::Reset(){
	_cpu->Reset();
	_systemClockCounter = 0;
	
}

void Bus::InsertCartridge(const std::shared_ptr<Cartridge>& cartridge){
	_cart = cartridge;
	_ppu->ConnectCartridge(cartridge); 
}

void Bus::Clock(){
	
}
