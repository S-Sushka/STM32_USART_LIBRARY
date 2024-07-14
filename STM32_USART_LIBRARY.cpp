#include "USART_LIBRARY.h"

void USART_INTERFACE::begin(long baudRate)
{
	float BRR_VALUE = 0;

	baudRate *= 16;
	BRR_VALUE = APB2_busFreq / baudRate;
	BRR_VALUE *= 16;


	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	USART1->BRR = BRR_VALUE;

	USART1->CR1 = USART_CR1_UE;
	USART1->CR1 |= (USART_CR1_TE | USART_CR1_RE);

	USART1->CR2 = 0;
	USART1->CR3 = 0;
}



// >>> READ FUNCTIONS

bool USART_INTERFACE::available()
{
	if (USART1->SR & USART_SR_RXNE == 0)
		return false;
	else
		return true;
}

uint8_t USART_INTERFACE::read()
{
	if (USART1->SR & USART_SR_RXNE == 1) // ?
		USART1->SR &= ~USART_SR_RXNE;

	return USART1->DR;
}


// >>> PRINT FUNCTIONS

void USART_INTERFACE::print(const std::string &data)
{
	for (uint16_t i = 0; i < data.length(); i++)
		write(data[i]);
}
void USART_INTERFACE::print(const char data[])
{
	for (uint16_t i = 0; i < 32766; i++)
	{
		if (data[i] == '\0') break;
		write(data[i]);
	}
}
void USART_INTERFACE::print(double data, int afterDot)
{
	// ?
}

void USART_INTERFACE::print(char data) { write(data); }
void USART_INTERFACE::print(int data, int numSystem) { if (data < 0) write('-'); print((unsigned long)data, numSystem); }
void USART_INTERFACE::print(long data,int numSystem) { if (data < 0) write('-'); print((unsigned long)data, numSystem); }
void USART_INTERFACE::print(unsigned char data, int numSystem) { print((unsigned long)data, numSystem); }
void USART_INTERFACE::print(unsigned int data,  int numSystem) { print((unsigned long)data, numSystem); }
void USART_INTERFACE::print(unsigned long data, int numSystem)
{
	switch(numSystem)
	{
	case 16: printNumber_HEX(data); break;
	case 10: printNumber_DEC(data); break;
	case 2:  printNumber_BIN(data); break;
	}
}

// >>> PRINT NUMBERS FUNCTIONS
void USART_INTERFACE::printNumber_DEC(unsigned long number)
{
	std::string resultBuffer = "";

    for (uint8_t i = 0; i < 10; i++)
    {
        resultBuffer += number % 10 + '0';
        number /= 10;

        if (number <= 0) break;
    }

    for (uint16_t i = resultBuffer.length()-1; i > 0; i--)
    	write(resultBuffer.at(i));
    write(resultBuffer.at(0));
}

void USART_INTERFACE::printNumber_BIN(unsigned long number)
{
	unsigned long mask = 0x80000000;

	uint8_t byteBuffer = 0;
	uint8_t byteBuffer_mask = 0x80;

	bool zeroBytesIsEnd = false;

	write('0');
	write('b');
    for (uint8_t i = 0; i < 4; i++) // 8*4 = 32 bits, size of long
    {
    	for (uint8_t j = 0; j < 8; j++) // GET BYTE
    	{
    		if ((number & mask) != 0)
    			byteBuffer |= byteBuffer_mask;

    		mask >>= 1;
    		byteBuffer_mask >>= 1;
    	}


    	if (PRINT_RULE_showZeroBytes_BIN)
    	{
    		printByte_BIN(byteBuffer);
    		if (PRINT_RULE_separateBytesWithSpaces_BIN) write(' ');
    	}
    	else
    	{
    		if (byteBuffer != 0 || zeroBytesIsEnd)
    		{
    			printByte_BIN(byteBuffer);
    			zeroBytesIsEnd = true;

    			if (PRINT_RULE_separateBytesWithSpaces_BIN) write(' ');
    		}
    	}


    	byteBuffer = 0;
    	byteBuffer_mask = 0x80;
    }
}
void USART_INTERFACE::printByte_BIN(uint8_t byte)
{
	for (uint8_t i = 0x80; i > 1; i >>= 1)
	{
		if ((byte & i) == 0)
			write('0');
		else
			write('1');
	}
	if ((byte & 1) == 0) write('0');
	else write('1');
}

void USART_INTERFACE::printNumber_HEX(unsigned long number)
{
	unsigned long mask = 0x80000000;

	uint8_t nibbleBuffer = 0;
	uint8_t nibbleMask = 0b00001000;

	uint8_t previousNibble = 0;
	uint8_t secondNibble = false;

	bool zeroBytesIsEnd = false;


	write('0');
	write('x');
    for (uint8_t i = 0; i < 8; i++) // 8*4 = 32 bits, size of long
    {
        for (uint8_t j = 0; j < 4; j++) // GET NIBBLE
        {
            if ((number & mask) != 0)
                nibbleBuffer |= nibbleMask;

            mask >>= 1;
            nibbleMask >>= 1;
        }


        if (PRINT_RULE_showZeroBytes_HEX)
        {
        	if (!secondNibble)
        	{
        		previousNibble = nibbleBuffer;
        		secondNibble = true;
        	}
        	else
        	{
        		printNibble_HEX(previousNibble);
        		printNibble_HEX(nibbleBuffer);

        		secondNibble = false;
        		if (PRINT_RULE_separateBytesWithSpaces_HEX) write(' ');
        	}
        }
        else
        {
        	if (!secondNibble)
        	{
        		previousNibble = nibbleBuffer;
        		secondNibble = true;
        	}
        	else
        	{
        		if ((previousNibble != 0 || nibbleBuffer != 0) || zeroBytesIsEnd)
        		{
        			printNibble_HEX(previousNibble);
        			printNibble_HEX(nibbleBuffer);

        			zeroBytesIsEnd = true;

        			if (PRINT_RULE_separateBytesWithSpaces_HEX) write(' ');
        		}

        		secondNibble = false;
        	}
        }


        nibbleBuffer = 0;
        nibbleMask = 0b00001000;
    }
}
void USART_INTERFACE::printNibble_HEX(uint8_t nibble)
{
    if (nibble >= 10)
    	write(nibble-10+'A');
    else
    	write(nibble+'0');
}

//  >>> WRITE FUNCTION

void USART_INTERFACE::write(uint8_t byte)
{
	while((USART1->SR & USART_SR_TXE) == 0);
	USART1->DR = byte;
}


// >>> PRINT RULES SETTERS

void USART_INTERFACE::separateBytesWithSpaces_HEX(bool separate) { PRINT_RULE_separateBytesWithSpaces_HEX = separate; }
void USART_INTERFACE::showZeroBytes_HEX(bool show) { PRINT_RULE_showZeroBytes_HEX = show; }
void USART_INTERFACE::separateBytesWithSpaces_BIN(bool separate) { PRINT_RULE_separateBytesWithSpaces_BIN = separate; }
void USART_INTERFACE::showZeroBytes_BIN(bool show) { PRINT_RULE_showZeroBytes_BIN = show; }
