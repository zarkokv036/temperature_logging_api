#include "build/temp/_test_temperature_logging.c"
#include "src/temperature_logging.h"
#include "build/test/mocks/mock_eeprom.h"
#include "C:/Ruby27-x64/lib/ruby/gems/2.7.0/gems/ceedling-0.30.0/vendor/unity/src/unity.h"






uint8_t simulated_eeprom[8192];



static int mock_eeprom_read(uint16_t address, uint8_t *data,

  uint16_t length, int num_calls)

{

    if (address >= 8192)

    {

        return 1;

    }

    else

    {

        memcpy(data, &simulated_eeprom[address], length);

        return 0;

    }

}



static int mock_eeprom_write(uint16_t address, const uint8_t* data,

  uint16_t length, int num_calls)

{

    if (address >= 8192)

    {

        return 1;

    }

    else

    {

        memcpy(&simulated_eeprom[address], data, length);

        return 0;

    }

}







void setUp(void)

{

    eeprom_read_Stub(mock_eeprom_read);

    eeprom_write_Stub(mock_eeprom_write);

}



void tearDown(void)

{

}











void test_init_when_eeprom_is_empty(void)

{



    memset(simulated_eeprom, 0xFF, 8192);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(60), UNITY_DISPLAY_STYLE_INT);



    uint32_t checkMagic = 0;

    memcpy(&checkMagic, simulated_eeprom, sizeof(checkMagic));

    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)(((0x10101010))), (UNITY_INT)(UNITY_INT32)((checkMagic)), (

   ((void *)0)

   ), (UNITY_UINT)(64), UNITY_DISPLAY_STYLE_HEX32);



    uint16_t readIndex = 54, writeIndex = 47;

    memcpy(&readIndex, &simulated_eeprom[4], sizeof(readIndex));

    memcpy(&writeIndex, &simulated_eeprom[6], sizeof(writeIndex));

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(69), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(70), UNITY_DISPLAY_STYLE_INT);

}











void test_of_init_when_eeprom_is_previously_initiated(void)

{



    memset(simulated_eeprom, 0xFF, 8192);



    uint32_t checkMagic = (0x10101010);

    memcpy(simulated_eeprom, &checkMagic, sizeof(checkMagic));



    uint16_t readIndex = 1, writeIndex = 7;

    memcpy((simulated_eeprom + 4), &readIndex, sizeof(readIndex));

    memcpy((simulated_eeprom + 6), &writeIndex, sizeof(writeIndex));



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    UnityAssertEqualNumber((UNITY_INT)((readIndex)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(92), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((writeIndex)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(93), UNITY_DISPLAY_STYLE_INT);

}















void test_basic_write_funcionality(void)

{



    memset(simulated_eeprom, 0xFF, 8192);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    for(uint16_t i = 0; i<5; i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(113), UNITY_DISPLAY_STYLE_INT);



        uint16_t writtenValue = 0;

        memcpy(&writtenValue, (simulated_eeprom + 8 + (i * 2)), sizeof(writtenValue));



        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((writtenValue)), (

       ((void *)0)

       ), (UNITY_UINT)(118), UNITY_DISPLAY_STYLE_INT);



        uint16_t writeIndex = 0;

        memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));

        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(122), UNITY_DISPLAY_STYLE_INT);







    }



}
