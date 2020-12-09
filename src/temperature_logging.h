/**
* @file temperature_logging.h
* @author Zarko Milojicic
*/

/**************************************
* @mainpage Temperature logging
* 
* On every new run tempLogging_init() must be used to check if eeprom is initialized or not.
* If it is, control block in RAM will be updated if not control block in eeprom will be updated.
* To write a new value to eeprom use tempLogging_write().
* Control block in eeprom is updated only when write function is used. If buffer in eeprom is full
* when new value is being inserted, the oldest value will be overwritten.
* To read a value from eeprom use tempLogging_read(). If buffer is empty return value will be
* 0xFFFF. Control block in eeprom is not updated when read() is used.
* tempLogging_flush() is used for updating control block in eeprom.
*/

#ifndef TEMPERATURE_LOGGING_H
#define TEMPERATURE_LOGGING_H

#include <stdint.h>
#include <stdbool.h>

#define TEMP_LOGGING_MAGIC_NUMBER_ADDRESS   (0x0000)                //first address location of eeprom
#define TEMP_LOGGING_MAGIC_NUMBER           (0x10101010)

typedef struct TempLogging_ControlBlock {
    
    uint16_t   readIndex;       /**< where to get next. Write this to eeprom */
    uint16_t   writeIndex;      /**< where to put next. Write this to eeprom */
    bool       dirtyFlag;       /**< set in read() and write(), clear in flush() */
    
} TempLogging_ControlBlock_t;

/************************************************
* @brief return status message of some functions
*
*/
typedef enum TempLogging_Status {
    
    TL_OK,
    TL_BAD_PARAM,
    TL_EEPROM_RW_ERROR    //read or write error
    
} TempLogging_Status_t;



/************************************************
* @brief initialization of EEPROM memory.
*
* This should be called on every new startup of the program.
* On the first run of the program EEPROM will be initialized
* (magic number and control block are stored),on every other 
* startup the memory is checked to make sure it is initialized
* (reading magic number and control block from eeprom).
*
* @param controlBlock Pointer to control block.
* @return TL_OK on success.
* @return TL_BAD_PARAM if an illegal parameter is provided.
* @return TL_EEPROM_RW_ERROR if the eeprom driver reports an error.
*/
TempLogging_Status_t tempLogging_init(TempLogging_ControlBlock_t *controlBlock);

/************************************************
* @brief Insert a new value into the temperature log
*
* The new value and updated control block are immediatelly
* stored in EEPROM.
* There is no need to manually flush the control block afterwards.
* If the log is full at the moment of inserting a new value, the oldest
* log entry will be overwritten.
*
* @param[in] controlBlock Pointer to control block
* @param[in] writeValue Pointer to a temperature value in range [0, 0xFFFE].
* @return TL_OK on success.
* @return TL_BAD_PARAM if an illegal parameter is provided.
* @return TL_EEPROM_RW_ERROR if the eeprom driver reports an error.
*/
TempLogging_Status_t tempLogging_write(TempLogging_ControlBlock_t *controlBlock, uint16_t *writeValue);


/*************************************************
* @brief Read the oldest value from EEPROM memory.
*
* After reading, control block in RAM is updated, but in EEPROM NOT.
*
* @param[out] readValue Pointer to the var where you want to push the read value
* if buffer is empty return value will be 0xFFFF.
* @param[in] controlBlock Pointer to control block.
* @return TL_OK on success.
* @return TL_BAD_PARAM if an illegal parameter is provided.
* @return TL_EEPROM_RW_ERROR if the eeprom driver reports an error.
*/
TempLogging_Status_t tempLogging_read(TempLogging_ControlBlock_t *controlBlock, uint16_t *readValue);

/*************************************************
* Function for updating control block in EEPROM.
* 
* This function will immediatelly update control block in EEPROM.
*
* @param controlBlock Pointer to control block you want to put in eeprom.
* @return TL_OK on success.
* @return TL_BAD_PARAM if an illegal parameter is provided.
* @return TL_EEPROM_RW_ERROR if the eeprom driver reports an error.
*/
TempLogging_Status_t tempLogging_flush(TempLogging_ControlBlock_t *controlBlock);

#endif
