#ifndef USART_LIBRARY_H
#define USART_LIBRARY_H

#include "stm32f4xx_hal.h"
#include <string>
#define APB2_busFreq 48000000

#define DEC 10

class USART_INTERFACE
{
	bool PRINT_RULE_separateBytesWithSpaces_HEX = false;
	bool PRINT_RULE_separateBytesWithSpaces_BIN = false;
	bool PRINT_RULE_showZeroBytes_HEX = true;
	bool PRINT_RULE_showZeroBytes_BIN = true;

public:
	void begin(long baudRate);

	bool available();
	uint8_t read();

    void print(const std::string &);
    void print(const char data[]);
    void print(double data, int numSystem = 2);

    void print(char data);
    void print(int data, int numSystem = DEC);
    void print(long data, int numSystem = DEC);
    void print(unsigned char data, int numSystem = DEC);
    void print(unsigned int data, int numSystem = DEC);
    void print(unsigned long data, int numSystem = DEC);

    void separateBytesWithSpaces_HEX(bool separate);
    void showZeroBytes_HEX(bool show);
    void separateBytesWithSpaces_BIN(bool separate);
    void showZeroBytes_BIN(bool show);

    virtual void write(uint8_t byte);

private:
	void printNumber_DEC(unsigned long number);

	void printNumber_BIN(unsigned long number);
	void printByte_BIN(uint8_t byte);

	void printNumber_HEX(unsigned long number);
	void printNibble_HEX(uint8_t nibble);
};


#endif // USART_LIBRARY_H
