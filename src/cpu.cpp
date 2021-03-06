#include <cstdint>
#include <vector>
#include "bus.h"
#include "cpu.h"
#include "log.h"

void CPU6502::WriteToBus(uint16_t addr, uint8_t data){
	if(_bus == nullptr){
		LOG_ERROR("CPU6502 Error: Cpu cannot write, not connected to bus ");
		return;
	}
	return _bus->Write(addr, data);
}

uint8_t  CPU6502::ReadFromBus(uint16_t addr) const{
	if(_bus == nullptr){
		LOG_ERROR("CPU6502 Error: Cpu cannot read, not connected to bus ");
		return 0;
	}
	return _bus->Read(addr, false);
}

bool CPU6502::ConnectBus(Bus* bus){

	if(_bus != nullptr){
		LOG_ERROR("CPU6502 Error: Cpu already connected to another Bus disconnect before connecting again");
		return false;
	}
	if(bus==nullptr){
		LOG_ERROR("CPU6502 Error: Null Bus passed to connect bus function");
		return false;
	}
	_bus = bus;
	return true;
}

uint8_t CPU6502::GetFlag(FLAGS6502 flag){
	return _regs.status & flag;
	}

void  CPU6502:: SetFlag(FLAGS6502 flag, bool v){
	if(v)
		_regs.status |= flag;
	else
		_regs.status &= ~flag;
		
}

uint8_t CPU6502::IMP() {
	_fetchedData = _regs.accum;
	return 0;
}

uint8_t CPU6502::IMM() {
	_addrAbs = _regs.pc++;
	return 0;
}

uint8_t CPU6502::ZP0() {
	_addrAbs = ReadFromBus(_regs.pc++);
	_addrAbs &= 0x00FF;
	return 0;
}

uint8_t CPU6502::ZPX() {
	_addrAbs = ReadFromBus(_regs.pc++) + _regs.x;
	_addrAbs &= 0x00FF;
	return 0;
}

uint8_t CPU6502::ZPY() {
	_addrAbs = ReadFromBus(_regs.pc++) + _regs.y;
	_addrAbs &= 0x00FF;
	return 0;
}

uint8_t CPU6502::ABS() {
	uint8_t lsb = ReadFromBus(_regs.pc++);
	uint8_t msb = ReadFromBus(_regs.pc++);
	
	_addrAbs = (msb << 8) | lsb;
	return 0;
}

uint8_t CPU6502::ABX() {
	uint8_t lsb = ReadFromBus(_regs.pc++);
	uint8_t msb = ReadFromBus(_regs.pc++);
	
	_addrAbs = ((msb << 8) | lsb) + _regs.x;
	return ((_addrAbs & 0xFF00) != (msb << 8)) ? 1 : 0;
}

uint8_t CPU6502::ABY() {
	uint8_t lsb = ReadFromBus(_regs.pc++);
	uint8_t msb = ReadFromBus(_regs.pc++);
	
	_addrAbs = ((msb << 8) | lsb) + _regs.y;
	return ((_addrAbs & 0xFF00) != (msb << 8)) ? 1 : 0;
}

uint8_t CPU6502::IND() {
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


uint8_t CPU6502::IZX() {
	uint8_t  addr = ReadFromBus(_regs.pc++);
 
	uint8_t lsb = ReadFromBus((uint16_t)(addr +_regs.x) & 0x00FF);
	uint8_t msb = ReadFromBus((uint16_t)(addr +_regs.x + 1) & 0x00FF);
	
	_addrAbs = (msb << 8) | lsb;
	return 0;
}

uint8_t CPU6502::IZY() {
	uint8_t  addr = ReadFromBus(_regs.pc++);
 
	uint8_t lsb = ReadFromBus((uint16_t)(addr) & 0x00FF);
	uint8_t msb = ReadFromBus((uint16_t)(addr + 1) & 0x00FF);
	
	_addrAbs = ((msb << 8) | lsb) + _regs.y;

  	return ((_addrAbs & 0xFF00) != (msb << 8)) ? 1 : 0;
}

uint8_t CPU6502::REL() {
	_addrRel = ReadFromBus(_regs.pc++);

	if(_addrRel & 0x80)
		_addrRel |= 0xFF00;
  	return 0;
}

//fetch data using address mode
uint8_t CPU6502::FetchData(){
	if(_instructTable[_currOPcode].Addrmode != &CPU6502::IMP)
		_fetchedData = ReadFromBus(_addrAbs);
	return _fetchedData;
}

//Instructions

uint8_t CPU6502::ADC(){
	FetchData();
	uint16_t temp = (uint16_t)_fetchedData + (uint16_t)_regs.accum + (uint16_t)GetFlag(C);
	SetFlag(C, temp > 255);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x80);
	SetFlag(V, ((uint16_t)_regs.accum ^ temp) & (~((uint16_t)_regs.accum ^ (uint16_t)_fetchedData)));
	_regs.accum = temp & 0x00FF;
	return 1;
}

uint8_t CPU6502::AND(){
	FetchData();
	_regs.accum &= _fetchedData;
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 1;
}

uint8_t CPU6502::ASL(){
	FetchData();
	uint16_t temp = (uint16_t)_fetchedData << 1;
	SetFlag(C, (temp & 0xFF00) > 0);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x80);
	if (_instructTable[_currOPcode].Addrmode == &CPU6502::IMP)
		_regs.accum = temp & 0x00FF;
	else
		WriteToBus(_addrAbs, temp & 0x00FF);
	return 0;
}

uint8_t CPU6502::BCC(){
	if(GetFlag(C) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t CPU6502::BCS(){
	if(GetFlag(C) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t CPU6502::BEQ(){
	if(GetFlag(Z) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t CPU6502::BIT(){
	FetchData();
	uint16_t temp = _regs.accum & _fetchedData;
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, _fetchedData & (1 << 7));
	SetFlag(V, _fetchedData & (1 << 6));
			
	return 0;
}

uint8_t CPU6502::BMI(){
	if(GetFlag(N) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t CPU6502::BNE(){
	if(GetFlag(Z) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}



uint8_t CPU6502::BPL(){
	if(GetFlag(N) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t CPU6502::BRK(){

	_regs.pc++;
	SetFlag(I, 1);
	WriteToBus(0x0100 + _regs.stkp, (_regs.pc >> 8) & 0x00FF);
	_regs.stkp--;
	WriteToBus(0x0100 + _regs.stkp, _regs.pc & 0x00FF);
	_regs.stkp--;

	SetFlag(B, 1);
	WriteToBus(0x0100 + _regs.stkp, _regs.status);
	_regs.stkp--;
	SetFlag(B, 0);
	
	_addrAbs = 0xFFFE;
	uint8_t lsb = ReadFromBus(_addrAbs);
	uint8_t msb = ReadFromBus(_addrAbs + 1);
	_regs.pc = ((uint16_t)msb << 8) | (uint16_t)lsb;

	return 0;
	
}

uint8_t CPU6502::BVC(){
	if(GetFlag(V) == 0){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}

uint8_t CPU6502::BVS(){
	if(GetFlag(V) == 1){
		_cyclesLeft++;
		_addrAbs = _regs.pc + _addrRel;

		if((_addrAbs & 0xFF00) != (_regs.pc & 0xFF00))
			_cyclesLeft++;
		_regs.pc = _addrAbs;
	}	
	return 0;
}


uint8_t CPU6502::CLC(){
	SetFlag(C, false);
	return 0;
}

uint8_t CPU6502::CLD(){
	SetFlag(D, false);
	return 0;
}

uint8_t CPU6502::CLI(){
	SetFlag(I, false);
	return 0;
}

uint8_t CPU6502::CLV(){
	SetFlag(V, false);
	return 0;
}

uint8_t CPU6502::CMP(){
	FetchData();
	uint8_t temp = _regs.accum - _fetchedData;
	SetFlag(C, _regs.accum >= _fetchedData);
	SetFlag(Z, temp == 0x00);
	SetFlag(N, temp & 0x80);
	return 1;
}

uint8_t CPU6502::CPX(){
	FetchData();
	uint8_t temp = _regs.x - _fetchedData;
	SetFlag(C, _regs.x >= _fetchedData);
	SetFlag(Z, temp == 0x00);
	SetFlag(N, temp & 0x80);
	return 1;
}

uint8_t CPU6502::CPY(){
	FetchData();
	uint8_t temp = _regs.y - _fetchedData;
	SetFlag(C, _regs.y >= _fetchedData);
	SetFlag(Z, temp == 0x00);
	SetFlag(N, temp & 0x80);
	return 1;
}

uint8_t CPU6502::DEC(){
	FetchData();
	uint8_t temp = _fetchedData - 1;
	WriteToBus(_addrAbs, temp);
	SetFlag(Z, temp == 0x00);
	SetFlag(N, temp & 0x80);
	return 0;
}

uint8_t CPU6502::DEX(){
	_regs.x -= 1;
	SetFlag(Z, _regs.x == 0x00);
	SetFlag(N, _regs.x & 0x80);
	return 0;
}

uint8_t CPU6502::DEY(){
	_regs.y -= 1;
	SetFlag(Z, _regs.y == 0x00);
	SetFlag(N, _regs.y & 0x80);
	return 0;
}

uint8_t CPU6502::EOR(){
	FetchData();
	_regs.accum ^= _fetchedData;
	SetFlag(Z, _regs.accum  == 0x00);
	SetFlag(N, _regs.accum  & 0x80);
	return 1;
}

uint8_t CPU6502::INC(){
	FetchData();
	uint8_t temp = _fetchedData + 1;
	WriteToBus(_addrAbs, temp);
	SetFlag(Z, temp == 0x00);
	SetFlag(N, temp & 0x80);
	return 0;
}

uint8_t CPU6502::INX(){
	_regs.x++;
	SetFlag(Z, _regs.x == 0x00);
	SetFlag(N, _regs.x & 0x80);
	return 0;
}

uint8_t CPU6502::INY(){
	_regs.y++;
	SetFlag(Z, _regs.y == 0x00);
	SetFlag(N, _regs.y & 0x80);
	return 0;
}

uint8_t CPU6502::JMP(){
	_regs.pc = _addrAbs;
	return 0;
}

uint8_t CPU6502::JSR(){
	_regs.pc--;
	WriteToBus(0x0100 +_regs.stkp, (_regs.pc >> 8) & 0x00FF);
	_regs.stkp--;
	WriteToBus(0x0100 +_regs.stkp, _regs.pc & 0x00FF);
	_regs.stkp--;
	_regs.pc = _addrAbs;
	return 0;
}

uint8_t CPU6502::LDA(){
	FetchData();
	_regs.accum = _fetchedData;
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 1;
}

uint8_t CPU6502::LDX(){
	FetchData();
	_regs.x = _fetchedData;
	SetFlag(Z, _regs.x == 0x00);
	SetFlag(N, _regs.x & 0x80);
	return 1;
}

uint8_t CPU6502::LDY(){
	FetchData();
	_regs.y = _fetchedData;
	SetFlag(Z, _regs.y == 0x00);
	SetFlag(N, _regs.y & 0x80);
	return 1;
}

uint8_t CPU6502::LSR(){
	FetchData();
	SetFlag(C, _fetchedData & 0x01);
	uint16_t temp = (uint16_t)(_fetchedData >> 1);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x0080);
	if (_instructTable[_currOPcode].Addrmode == &CPU6502::IMP)
		_regs.accum = temp & 0x00FF;
	else
		WriteToBus(_addrAbs, temp & 0x00FF);
   return 0;
}

uint8_t CPU6502::NOP(){
	switch (_currOPcode) {
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
		break;
	}
	return 0;
}

uint8_t CPU6502::ORA(){
	FetchData();
	_regs.accum |= _fetchedData;
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 0;
}

uint8_t CPU6502::PHA(){
	WriteToBus(0x0100 + _regs.stkp, _regs.accum);
	_regs.stkp--;
	return 0;
}

uint8_t CPU6502::PHP(){
	WriteToBus(0x0100 + _regs.stkp, _regs.status | B | U);
	SetFlag(B, false);
	SetFlag(U, false);
	_regs.stkp--;
	return 0;
}

uint8_t CPU6502::PLA(){
	_regs.stkp++;
	_regs.accum = ReadFromBus(0x0100 + _regs.stkp);
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 0;
}

uint8_t CPU6502::PLP(){
	_regs.stkp++;
	_regs.status = ReadFromBus(0x0100 + _regs.stkp);
	SetFlag(U, true);
	return 0;
}

uint8_t CPU6502::ROL(){
	FetchData();
	uint16_t temp = (uint16_t)(_fetchedData << 1) | GetFlag(C);
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x0080);
	
	if (_instructTable[_currOPcode].Addrmode == &CPU6502::IMP)
		_regs.accum = temp & 0x00FF;
	else
		WriteToBus(_addrAbs, temp & 0x00FF);
   return 0;
}

uint8_t CPU6502::ROR(){
	FetchData();
	uint16_t temp = (uint16_t)(GetFlag(C) << 7) | (_fetchedData >> 1);
	SetFlag(C, _fetchedData & 0x01);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x0080);
	
	if (_instructTable[_currOPcode].Addrmode == &CPU6502::IMP)
		_regs.accum = temp & 0x00FF;
	else
		WriteToBus(_addrAbs, temp & 0x00FF);
   return 0;
}

uint8_t CPU6502::RTI(){
	_regs.stkp++;
	_regs.status = ReadFromBus(0x0100 + _regs.stkp);
	_regs.status &= ~B;
	_regs.status &= ~U;

	_regs.stkp++;
	_regs.pc = (uint16_t)ReadFromBus(0x0100 + _regs.stkp);
   _regs.stkp++;
   _regs.pc |= (uint16_t)ReadFromBus(0x0100 + _regs.stkp) << 8;
   return 0;
	
}

uint8_t CPU6502::RTS(){
	_regs.stkp++;
	_regs.pc = (uint16_t)ReadFromBus(0x0100 + _regs.stkp);
	_regs.stkp++;
	_regs.pc |= (uint16_t)ReadFromBus(0x0100 + _regs.stkp) << 8;
	_regs.pc++;
	return 0;
	
}

uint8_t CPU6502::SBC(){
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

uint8_t CPU6502::SEC(){
	SetFlag(C, true);
	return 0;
}

uint8_t CPU6502::SED(){
	SetFlag(D, true);
	return 0;
}

uint8_t CPU6502::SEI(){
	SetFlag(I, true);
	return 0;
}

uint8_t CPU6502::STA(){
	WriteToBus(_addrAbs, _regs.accum);
	return 0;
}

uint8_t CPU6502::STX(){
	WriteToBus(_addrAbs, _regs.x);
	return 0;
}

uint8_t CPU6502::STY(){
	WriteToBus(_addrAbs, _regs.y);
	return 0;
}

uint8_t CPU6502::TAX(){
	_regs.x = _regs.accum;
	SetFlag(Z, _regs.x == 0x00);
	SetFlag(N, _regs.x & 0x80);
	return 0;
}

uint8_t CPU6502::TAY(){
	_regs.y = _regs.accum;
	SetFlag(Z, _regs.y == 0x00);
	SetFlag(N, _regs.y & 0x80);
	return 0;
}

uint8_t CPU6502::TSX(){
	_regs.x = _regs.stkp;
	SetFlag(Z, _regs.x == 0x00);
	SetFlag(N, _regs.x & 0x80);
	return 0;
}

uint8_t CPU6502::TXA(){
	_regs.accum = _regs.x;
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 0;
}

uint8_t CPU6502::TXS(){
	_regs.stkp = _regs.x;
	return 0;
}

uint8_t CPU6502::TYA(){
	_regs.accum = _regs.y;
	SetFlag(Z, _regs.accum == 0x00);
	SetFlag(N, _regs.accum & 0x80);
	return 0;
}

uint8_t CPU6502::XXX(){
	
	return 0;
}


void CPU6502::Reset(){
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

void CPU6502::IntReq(){
	if(GetFlag(I) != 0)
		return;
	
	WriteToBus(0x0100 + _regs.stkp, (_regs.pc >> 8) & 0x00FF);
	_regs.stkp--;
	WriteToBus(0x0100 + _regs.stkp, _regs.pc & 0x00FF);
	_regs.stkp--;

	SetFlag(B, 0);
	SetFlag(I, 1);
	SetFlag(U, 1);
	WriteToBus(0x0100 + _regs.stkp, _regs.status);
	_regs.stkp--;

	_addrAbs = 0xFFFE;
	uint8_t lsb = ReadFromBus(_addrAbs);
	uint8_t msb = ReadFromBus(_addrAbs + 1);
	_regs.pc = ((uint16_t)msb << 8) | (uint16_t)lsb;

	_cyclesLeft = 7;
}

void CPU6502::NonMaskInt(){
	
	WriteToBus(0x0100 + _regs.stkp, (_regs.pc >> 8) & 0x00FF);
	_regs.stkp--;
	WriteToBus(0x0100 + _regs.stkp, _regs.pc & 0x00FF);
	_regs.stkp--;

	SetFlag(B, 0);
	SetFlag(I, 1);
	SetFlag(U, 1);
	WriteToBus(0x0100 + _regs.stkp, _regs.status);
	_regs.stkp--;

	_addrAbs = 0xFFFA;
	uint8_t lsb = ReadFromBus(_addrAbs);
	uint8_t msb = ReadFromBus(_addrAbs + 1);
	_regs.pc = ((uint16_t)msb << 8) | (uint16_t)lsb;

	_cyclesLeft = 7;
}

