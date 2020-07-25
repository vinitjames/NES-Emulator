#include <cstdint>
#include <vector>
#include "bus.h"
#include "cpu.h"
#include "log.h"

void OLC6502::WriteToBus(uint16_t addr, uint8_t data){
	if(_bus == nullptr){
		LOG_ERROR("OLC6502 Error: Cpu cannot write, not connected to bus ");
		return;
	}
	return _bus->Write(addr, data);
}

uint8_t  OLC6502::ReadFromBus(uint16_t addr) const{
	if(_bus == nullptr){
		LOG_ERROR("OLC6502 Error: Cpu cannot read, not connected to bus ");
		return 0;
	}
	return _bus->Read(addr, false);
}

bool OLC6502::ConnectBus(Bus* bus){

	if(_bus != nullptr){
		LOG_ERROR("OLC6502 Error: Cpu already connected to another Bus disconnect before connecting again");
		return false;
	}
	if(bus==nullptr){
		LOG_ERROR("OLC6502 Error: Null Bus passed to connect bus function");
		return false;
	}
	_bus = bus;
	return true;
}

uint8_t OLC6502::GetFlag(FLAGS6502 flag){
	return _regs.status & flag;
	}

void  OLC6502:: SetFlag(FLAGS6502 flag, bool v){
	_regs.status |= flag;
}



	
