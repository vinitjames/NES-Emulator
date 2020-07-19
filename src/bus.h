#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <vector>

class Bus {

public:
	Bus(std::size_t size = 64*1024);
	~Bus();
	void Write(uint16_t addr, uint8_t data);
	uint8_t  Read(uint16_t addr, bool bReadOnly = false);

private:
	std::vector<uint8_t> _ram;
	
};

#endif /* BUS_H */
