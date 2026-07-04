#include <vector>
#include <string>
#include <cstdint>
#include <iostream>

enum class Indicator {
	Sell,
	Buy
};

struct ITCHAddOrder {
	char msg_type;
	uint16_t locate_code_stock;
	uint16_t tracking_number;
	uint32_t time_stamp;
	uint32_t order_reference;
	Indicator indic;
	uint16_t shares;
	char stock_symbol[8];
	uint16_t price;
};

class ParseAddOrder {

};

int main() {
	std::cout << sizeof(ITCHAddOrder) << std::endl;
	std::cout << sizeof(std::string) << std::endl;
	std::cout << sizeof(Indicator) << std::endl;
}