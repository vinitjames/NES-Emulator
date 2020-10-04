
#include <fstream>
#include <stdexcept>
#include "log.h"
#include <iostream>
#include "cartridge.h"
Cartridge::Cartridge(const std::string& filename){
	std::ifstream infs;
	infs.open(filename, std::ifstream::binary);
	if(!infs.is_open()){
		throw std::invalid_argument("Cartridge: Could not open file");
	}
	
	infs.read((char*)&_header, sizeof(INESHeader));
	
	if (_header.mapperflag1 & 0x04){
		infs.seekg(512, std::ios_base::cur);
	}
	
	_mapperID = (_header.mapperflag2 & 0xF0) | (_header.mapperflag1 >> 4);

	_numBanksPGR = _header.prgRomSize;
	_memPGR.resize(_numBanksPGR*16384);
	infs.read((char*)_memPGR.data(), _memPGR.size());

	_numBanksCHR = _header.chrRomSize;
	_memCHR.resize(_numBanksCHR * 8192);
	infs.read((char*)_memCHR.data(), _memCHR.size());
	infs.close();
}


int main(int argc, char *argv[])
{
    Cartridge cart{"zelda.nes"};
	cart.PrintCartrideInfo();
		
    return 0;
}
