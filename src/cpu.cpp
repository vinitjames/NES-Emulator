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
	if(v)
		_regs.status |= flag;
	else
		_regs.status &= ~flag;
		
}

uint8_t OLC6502::IMP() {
	_fetchedData = _regs.accum;
	return 0;
}

uint8_t OLC6502::IMM() {
	_addrAbs = _regs.pc++;
	return 0;
}

uint8_t OLC6502::ZP0() {
	_addrAbs = ReadFromBus(_regs.pc++);
	_addrAbs &= 0x00FF;
	return 0;
}

uint8_t OLC6502::ZPX() {
	_addrAbs = ReadFromBus(_regs.pc++) + _regs.x;
	_addrAbs &= 0x00FF;
	return 0;
}

uint8_t OLC6502::ZPY() {
	_addrAbs = ReadFromBus(_regs.pc++) + _regs.y;
	_addrAbs &= 0x00FF;
	return 0;
}

uint8_t OLC6502::ABS() {
	uint8_t lsb = ReadFromBus(_regs.pc++);
	uint8_t msb = ReadFromBus(_regs.pc++);
	
	_addrAbs = (msb << 8) | lsb;
	return 0;
}

uint8_t OLC6502::ABX() {
	uint8_t lsb = ReadFromBus(_regs.pc++);
	uint8_t msb = ReadFromBus(_regs.pc++);
	
	_addrAbs = ((msb << 8) | lsb) + _regs.x;
	return ((_addrAbs & 0xFF00) != (msb << 8)) ? 1 : 0;
}

uint8_t OLC6502::ABY() {
	uint8_t lsb = ReadFromBus(_regs.pc++);
	uint8_t msb = ReadFromBus(_regs.pc++);
	
	_addrAbs = ((msb << 8) | lsb) + _regs.y;
	return ((_addrAbs & 0xFF00) != (msb << 8)) ? 1 : 0;
}

uint8_t OLC6502::IND() {
	uint8_t ptrLo = ReadFromBus(_regs.pc++);
	uint8_t ptrHi = ReadFromBus(_regs.pc++);
	
	uint8_t ptr = (ptrHi << 8) | ptrLo;
	//Simulating Hardware bug in 6502
	if(ptrLo == 0xFF)
		_addrAbs = (ReadFromBus(ptr & 0xFF00) << 8) | ReadFromBus(ptr);
	
	else
		_addrAbs = (ReadFromBus(ptr + 1) << 8) | ReadFromBus(ptr);
	return 0;
}


uint8_t OLC6502::IZX() {
	uint8_t  addr = ReadFromBus(_regs.pc++);
 
	uint8_t lsb = ReadFromBus((uint16_t)(addr +_regs.x) & 0x00FF);
	uint8_t msb = ReadFromBus((uint16_t)(addr +_regs.x + 1) & 0x00FF);
	
	_addrAbs = (msb << 8) | lsb;
	return 0;
}

uint8_t OLC6502::IZY() {
	uint8_t  addr = ReadFromBus(_regs.pc++);
 
	uint8_t lsb = ReadFromBus((uint16_t)(addr) & 0x00FF);
	uint8_t msb = ReadFromBus((uint16_t)(addr + 1) & 0x00FF);
	
	_addrAbs = ((msb << 8) | lsb) + _regs.y;

  	return ((_addrAbs & 0xFF00) != (msb << 8)) ? 1 : 0;
}

uint8_t OLC6502::REL() {
	_addrRel = ReadFromBus(_regs.pc++);

	if(_addrRel & 0x80)
		_addrRel |= 0xFF00;
  	return 0;
}

//fetch data using address mode
uint8_t OLC6502::FetchData(){
	if(_instructTable[_currOPcode].Addrmode != &OLC6502::IMP)
		_fetchedData = ReadFromBus(_addrAbs);
	return _fetchedData;
}

//Instructions

uint8_t OLC6502::ADC(){
	FetchData();
	uint16_t temp = (uint16_t)_fetchedData + (uint16_t)_regs.accum + (uint16_t)GetFlag(C);
	SetFlag(C, temp > 255);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x80);
	SetFlag(V, ((uint16_t)_regs.accum ^ temp) & (~((uint16_t)_regs.accum ^ (uint16_t)_fetchedData)));
	_regs.accum = temp & 0x00FF;
	return 1;
}

uint8_t OLC6502::AND(){
	FetchData();
	_regs.accum &= _fetchedData;
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 1;
}

uint8_t OLC6502::BCC(){
	if(GetFlag(C) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::BCS(){
	if(GetFlag(C) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::BEQ(){
	if(GetFlag(Z) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::BNE(){
	if(GetFlag(Z) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::BMI(){
	if(GetFlag(N) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::BPL(){
	if(GetFlag(N) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::BVS(){
	if(GetFlag(V) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::BVC(){
	if(GetFlag(V) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t OLC6502::CLC(){
	SetFlag(C, false);
	return 0;
}

uint8_t OLC6502::CLD(){
	SetFlag(D, false);
	return 0;
}

uint8_t OLC6502::CLI(){
	SetFlag(I, false);
	return 0;
}

uint8_t OLC6502::CLV(){
	SetFlag(V, false);
	return 0;
}


uint8_t OLC6502::ORA(){
	FetchData();
	_regs.accum |= _fetchedData;
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 0;
}

uint8_t OLC6502::SBC(){
	FetchData();
	uint16_t value = (uint16_t)_fetchedData ^ 0x00FF;
	uint16_t temp =  (uint16_t)_regs.accum + value + (uint16_t)GetFlag(C);
	SetFlag(C, temp > 255);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x80);
	SetFlag(V, ((uint16_t)_regs.accum ^ temp) & (~((uint16_t)_regs.accum ^ (uint16_t)_fetchedData)));
	_regs.accum = temp & 0x00FF;
	return 1;
}

uint8_t OLC6502::PHA(){
	WriteToBus(0x0100 + _regs.stkp, _regs.accum);
	_regs.stkp--;
	return 0;
}

uint8_t OLC6502::PHP(){
	WriteToBus(0x0100 + _regs.status, _regs.accum);
	_regs.stkp--;
	return 0;
}

uint8_t OLC6502::PLA(){
	_regs.stkp++;
	_regs.accum = ReadFromBus(0x0100 + _regs.stkp);
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 0;
}


void OLC6502::Reset(){
	_regs.accum  = 0x00;  
  	_regs.x      = 0x00;  
	_regs.y      = 0x00;
	_regs.stkp   = 0xFD;
	_regs.status = 0x00 | U;

	//reading program counter from hardcoded address 0xFFFC on Reset
	_addrAbs = 0xFFFC;
	uint8_t lsb = ReadFromBus(_addrAbs);
	uint8_t msb = ReadFromBus(_addrAbs + 1);
	_regs.pc = ((uint16_t)msb << 8) | (uint16_t)lsb;

	//resetting internal variables
	_fetchedData = 0x00;  
	_addrAbs = 0x0000;
	_addrRel = 0x0000;
	_currOPcode = 0x00;

	_cyclesLeft = 8;
	
}


