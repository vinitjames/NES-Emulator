#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <vector>
#include <string>
#include <iostream>

class Cartridge
{
public:
	//! Default constructor
	Cartridge(const std::string& filename);



protected:
private:

	struct INESHeader{
		char name[4];
		uint8_t prgRomSize;
		uint8_t chrRomSize;
		uint8_t mapperflag1;
		uint8_t mapperflag2;
		uint8_t prgRamSize;
		uint8_t tvSystem1;
		uint8_t tvSystem2;
		char unused[5];		
	}; 

	INESHeader _header;
	std::vector<uint8_t> _memPGR;
	std::vector<uint8_t> _memCHR;
	uint8_t _mapperID    = 0;
	uint8_t _numBanksPGR = 0;
	uint8_t _numBanksCHR = 0;
	
	
};
#endif /* CARTRIDGE_H */
