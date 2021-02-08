/**
* @file eeprom.h
* @author Zarko Milojicic
*/


#include <stdint.h>

#ifndef EEPROM_H
#define EEPROM_H


/**
* @brief Function for writing data to EEPROM
*
* Writing data with specific length to adress in memory.
*
* @param adress Address where the data should be stored. 
* @param data   pointer to data which need to be stored.
* @param length length of data that should be stored.
* @return  0 On success
* @return  platform-specific error On failure
*/
int eeprom_write(uint16_t address, const uint8_t *data, uint16_t length);

/**
* @brief Function for reading data from EEPROM
*
* Writing data with specific length to adress in memory.
*
* @param adress Address where the data should be read from. 
* @param data   pointer variable where data need to be stored.
* @param length length of data that should be read.
* @return  0 On success
* @return  platform-specific error If error happens
*/
int eeprom_read(uint16_t address, uint8_t *data, uint16_t length);

#endif
