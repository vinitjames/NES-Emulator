#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <cstdint>
#include <vector>
#include <string>

class Cartridge
{
public:
	//! Default constructor
	Cartridge(const std::string& filename);

	//! Copy constructor
	Cartridge(const Cartridge &other);

	//! Move constructor
	Cartridge(Cartridge &&other) noexcept;

	//! Destructor
	virtual ~Cartridge() noexcept;

	//! Copy assignment operator
	Cartridge& operator=(const Cartridge &other);

	//! Move assignment operator
	Cartridge& operator=(Cartridge &&other) noexcept;



protected:
private:
	std::vector<uint8_t> _memPGR;
	std::vector<uint8_t> _memCHR;
	uint8_t _mapperID    = 0;
	uint8_t _numBanksPGR = 0;
	uint8_t _numBanksCHR = 0;
	
};
#endif /* CARTRIDGE_H */
