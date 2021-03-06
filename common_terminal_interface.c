/* Common Terminal Interface Library
 * Sarker Nadir Afridi Azmi
 *
 * This file contains the implementation of the functions prototypes defined in
 * common_terminal_interface.h.
 *
 * The functions declared here are used to get, parse, and put data onto a terminal
 * interface like Putty
 */

#include <stdint.h>
#include <stdbool.h>
#include "uart0.h"
#include "common_terminal_interface.h"

static uint8_t count = 0;
bool isCarriageReturn = false;

// This code has been modified since our keyboard input is not interrupt driven
// Gets a user defined string using the serial peripheral Uart0
void getsUart0(USER_DATA* data)
{
    // Keeps count of how many characters we currently have in the buffer
    // Also serves to provide us with the position of the last character input
    //uint8_t count = 0;

    //while(true)
    {
        char c = getcUart0();

        // If the character is a backspace
        // Ctrl-H or Ctrl-?
        if(c == 8 || c == 127)
        {
            // If the count is greater than zero, we decrement the count
            // This means we are essentially erasing the previous character
            // Else, we just look for new characters
            if(count > 0)
                count--;
            /*
            else
                continue;
            */
        }
        // If the character is a carriage return (13) or a line feed (10)
        else if(c == 13 || c == 10)
        {
            data->buffer[count] = 0;
            count = 0;
            isCarriageReturn = true;
            return;
        }
        /*
        else if(c < 32)
            continue;
        */
        // If the character is anything greater than a space
        else if(c >= 32)
        {
            data->buffer[count++] = c;
            if(count == MAX_CHARS)
            {
                data->buffer[count] = '\0';
                count = 0;
                isCarriageReturn = true;
                return;
            }
        }
    }
}

void printUint8InDecimal(uint8_t n)
{
    /*
     * Depending how many digits n has, set the divider
     * If n has only one digit, this will prevent leading
     * zeros from being printed
     */
    uint16_t divider = 0;
    if(n < 10)
        divider = 1;
    else if(n < 100)
        divider = 10;
    else if(n < 1000)
        divider = 100;

    while(divider)
    {
        uint8_t currentDigit = n / divider;
        divider /= 10;
        putcUart0((currentDigit % 10) + '0');
    }
}

void printUint8InHex(uint8_t n)
{
    // Print 4 bits at a time
    int8_t i = 0;
    for(i = 1; i >= 0; i--)
    {
        uint8_t currentNumber = (n >> (i << 2)) & 0xF;
        if(currentNumber > 9)
        {
            switch(currentNumber)
            {
            case 10:
                putcUart0('a');
                break;
            case 11:
                putcUart0('b');
                break;
            case 12:
                putcUart0('c');
                break;
            case 13:
                putcUart0('d');
                break;
            case 14:
                putcUart0('e');
                break;
            case 15:
                putcUart0('f');
                break;
            }
        }
        else
            printUint8InDecimal(currentNumber);
    }

}

// Tokenizes the string in place
void parseField(USER_DATA* data)
{
    data->fieldCount = 0;
    bool IsNewToken = false;
    uint8_t i = 0;
    for(i = 0; (data->buffer[i] != '\0') && (data->fieldCount < MAX_FIELDS); i++)
    {
        // Only tokenize alpha numeric characters
        if(((data->buffer[i] >= 'a' && data->buffer[i] <= 'z') ||
            (data->buffer[i] >= '0' && data->buffer[i] <= '9') ||
            (data->buffer[i] >= 'A' && data->buffer[i] <= 'Z')) &&
            !IsNewToken)
        {
            IsNewToken = true;
            // Record where this new token starts
            data->fieldPosition[data->fieldCount] = i;
            if(data->buffer[i] >= '0' && data->buffer[i] <= '9')
                data->fieldType[data->fieldCount++] = 'n';
            else
                data->fieldType[data->fieldCount++] = 'a';
        }
        else if(!(data->buffer[i] >= 'a' && data->buffer[i] <= 'z') &&
                !(data->buffer[i] >= '0' && data->buffer[i] <= '9') &&
                !(data->buffer[i] >= 'A' && data->buffer[i] <= 'Z'))
        {
            IsNewToken = false;
            // Replace the delimeters with null terminators
            data->buffer[i] = '\0';
        }
    }
    return;
}

// Compares two strings to see if they are equal or not
bool stringCompare(const char string1[], const char string2[])
{
    uint8_t index = 0;
    // The comparison with MAX_CHARS is a bit unnecessary
    while((string1[index] != '\0') && (string2[index] != '\0') && index < MAX_CHARS)
    {
        if(string1[index] != string2[index])
            return false;
        index++;
    }
    return !string1[index] && !string2[index];
}

// Checks to see if a particular command is valid or not
bool isCommand(USER_DATA* data, const char strCommand[], uint8_t minArguments)
{
    if(data->fieldCount - 1 < minArguments) return false;
    if(stringCompare(data->buffer, strCommand)) return true;
    return false;
}

// Gets the requested integer field from the input
uint32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber)
{
    uint32_t sum = 0;
    if((fieldNumber < MAX_FIELDS) &&
       (fieldNumber < data->fieldCount) &&
       (data->fieldType[fieldNumber] == 'n'))
    {
        uint8_t pos = data->fieldPosition[fieldNumber];
        while(data->buffer[pos] != '\0')
            sum = (sum * 10) + (data->buffer[pos++] - '0');
    }
    return sum;
}

// Get a pointer to the requested string field
char* getFieldString(USER_DATA* data, uint8_t fieldNumber)
{
    if((fieldNumber < MAX_FIELDS) &&
       (fieldNumber < data->fieldCount) &&
        (data->fieldType[fieldNumber] == 'a'))
        return data->buffer + data->fieldPosition[fieldNumber];
    return 0;
}

void strCpy(const char* str1, char* str2)
{
    uint8_t i = 0;
    for(i = 0; str1[i] != '\0'; i++)
    {
        str2[i] = str1[i];
    }
    str2[i] = '\0';
}
