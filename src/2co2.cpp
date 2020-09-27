#include "2co2.h"

void PPU2CO2::CpuWrite(uint16_t addr, uint8_t data){
	switch (addr) {
	case 0x0000: //Control 
		break;
    case 0x0001: //Mask 
		break;
	case 0x0002: //Status
		break;
	case 0x0003 //OAM Address 
		break;
	case 0x0004: //OAM Data
		break;
	case 0x0005: //Scroll 
		break;
	case 0x0006: //PPU Address 
		break;
	case 0x0007: //PPU Data
		break;
	
	}
}

uint8_t  PPU2CO2::CpuRead(uint16_t addr, bool bReadOnly){
	switch (addr) {
	case 0x0000: //Control 
		break;
    case 0x0001: //Mask 
		break;
	case 0x0002: //Status
		break;
	case 0x0003 //OAM Address 
		break;
	case 0x0004: //OAM Data
		break;
	case 0x0005: //Scroll 
		break;
	case 0x0006: //PPU Address 
		break;
	case 0x0007: //PPU Data
		break;
	
	}
	return 0x00;
}

void PPU2CO2::SelfWrite(uint16_t addr, uint8_t data){
	addr &= 0x3FFF;
}

uint8_t  PPU2CO2::SelfRead(uint16_t addr, bool bReadOnly){
	addr &= 0x3FFF;
	return 0x00;
}

void PPU2CO2::ConnectCartridge(std::shared_ptr<Cartridge>& cartridge){
	_cart = cartridge;
}
