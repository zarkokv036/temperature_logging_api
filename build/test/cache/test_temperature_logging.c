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

   ), (UNITY_UINT)(68), UNITY_DISPLAY_STYLE_INT);



    uint32_t checkMagic = 0;

    memcpy(&checkMagic, simulated_eeprom, sizeof(checkMagic));

    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)(((0x10101010))), (UNITY_INT)(UNITY_INT32)((checkMagic)), (

   ((void *)0)

   ), (UNITY_UINT)(72), UNITY_DISPLAY_STYLE_HEX32);



    uint16_t readIndex = 54, writeIndex = 47;

    memcpy(&readIndex, &simulated_eeprom[4], sizeof(readIndex));

    memcpy(&writeIndex, &simulated_eeprom[6], sizeof(writeIndex));

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(77), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(78), UNITY_DISPLAY_STYLE_INT);

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

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(100), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((readIndex)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(102), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((writeIndex)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(103), UNITY_DISPLAY_STYLE_INT);

}

void test_write_funcionality(void)

{



    memset(simulated_eeprom, 0xFF, 8192);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    for(uint16_t i = 0; i < 5; i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(125), UNITY_DISPLAY_STYLE_INT);



        uint16_t writtenValue = 0;

        memcpy(&writtenValue, (simulated_eeprom + 8 + (i * 2)), sizeof(writtenValue));



        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((writtenValue)), (

       ((void *)0)

       ), (UNITY_UINT)(130), UNITY_DISPLAY_STYLE_INT);



        uint16_t writeIndex = 0 , readIndex = 0;

        memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));

        memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));

        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(135), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(136), UNITY_DISPLAY_STYLE_INT);

    }



}

void test_read_funcionality(void)

{



    memset(simulated_eeprom, 0xFF, 8192);



    uint32_t checkMagic = (0x10101010);

    memcpy(simulated_eeprom, &checkMagic, sizeof(checkMagic));



    uint16_t readIndex = 0, writeIndex = 7;

    memcpy((simulated_eeprom + 4), &readIndex, sizeof(readIndex));

    memcpy((simulated_eeprom + 6), &writeIndex, sizeof(writeIndex));



    for(uint16_t i = 0; i < 7; i++)

    {

        memcpy(simulated_eeprom + 8 + (i * 2), &i, sizeof(i));

    }





    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(170), UNITY_DISPLAY_STYLE_INT);



    for(uint16_t i = 0; i<7; i++)

    {

        uint16_t readValue = 0;

        status = tempLogging_read(&emptyBuffer, &readValue);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(176), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((readValue)), (

       ((void *)0)

       ), (UNITY_UINT)(178), UNITY_DISPLAY_STYLE_INT);

    }





    uint16_t readValue = 0;

    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT16)((0xFFFF)), (UNITY_INT)(UNITY_INT16)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(184), UNITY_DISPLAY_STYLE_HEX16);





    readIndex = 99;

    memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));



    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(190), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)((7)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(192), UNITY_DISPLAY_STYLE_INT);



}

void test_read_write_when_eeprom_is_almost_empty(void)

{

    memset(simulated_eeprom, 0xFF, 8192);



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    uint16_t readValue = 0;

    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(214), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((0xFFFF)), (UNITY_INT)(UNITY_INT32)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(216), UNITY_DISPLAY_STYLE_HEX32);





    for(uint16_t i = 0; i < 2; i++)

    {

        status = tempLogging_write(&emptyBuffer, &i);

        UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((TL_OK)), (UNITY_INT)(UNITY_INT32)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(222), UNITY_DISPLAY_STYLE_HEX32);

    }



    uint16_t readIndex = 99, writeIndex = 54;



    for(uint16_t i = 0; i < 2; i++)

    {

        status = tempLogging_read(&emptyBuffer, &readValue);

        UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

       ((void *)0)

       ), (UNITY_UINT)(230), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((i)), (UNITY_INT)((readValue)), (

       ((void *)0)

       ), (UNITY_UINT)(232), UNITY_DISPLAY_STYLE_INT);



        UnityAssertEqualNumber((UNITY_INT)((i + 1)), (UNITY_INT)((emptyBuffer.readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(234), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((emptyBuffer.writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(235), UNITY_DISPLAY_STYLE_INT);



        memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));

        memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));

        UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((readIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(239), UNITY_DISPLAY_STYLE_INT);

        UnityAssertEqualNumber((UNITY_INT)((2)), (UNITY_INT)((writeIndex)), (

       ((void *)0)

       ), (UNITY_UINT)(240), UNITY_DISPLAY_STYLE_INT);

    }





    status = tempLogging_read(&emptyBuffer, &readValue);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(245), UNITY_DISPLAY_STYLE_INT);



    UnityAssertEqualNumber((UNITY_INT)(UNITY_INT32)((0xFFFF)), (UNITY_INT)(UNITY_INT32)((readValue)), (

   ((void *)0)

   ), (UNITY_UINT)(247), UNITY_DISPLAY_STYLE_HEX32);





    uint16_t value = 1l;

    status = tempLogging_write(&emptyBuffer, &value);

    memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));

    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(254), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(255), UNITY_DISPLAY_STYLE_INT);

}

void test_read_write_when_eeprom_is_almost_full(void)

{



    memset(simulated_eeprom, 0xFF, 8192);



    uint32_t checkMagic = (0x10101010);

    memcpy(simulated_eeprom, &checkMagic, sizeof(checkMagic));

    for(uint16_t i = 0; i < ((8192 - 8) / 2) - 1; i++)

    {

        memcpy(simulated_eeprom + 8 + (i * 2), &i, sizeof(i));

    }





    uint16_t readIndex = 0, writeIndex = ((8192 - 8) / 2) - 1;

    memcpy((simulated_eeprom + 4), &readIndex, sizeof(readIndex));

    memcpy((simulated_eeprom + 6), &writeIndex, sizeof(writeIndex));





    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);





    uint16_t value = 88;

    status = tempLogging_write(&emptyBuffer, &value);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(291), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((0)), (UNITY_INT)((emptyBuffer.writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(292), UNITY_DISPLAY_STYLE_INT);





    status = tempLogging_write(&emptyBuffer, &value);

    memcpy(&value, (simulated_eeprom+8), sizeof(value));

    UnityAssertEqualNumber((UNITY_INT)((88)), (UNITY_INT)((value)), (

   ((void *)0)

   ), (UNITY_UINT)(297), UNITY_DISPLAY_STYLE_INT);



    memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));

    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));

    UnityAssertEqualNumber((UNITY_INT)((readIndex)), (UNITY_INT)((writeIndex + 1)), (

   ((void *)0)

   ), (UNITY_UINT)(301), UNITY_DISPLAY_STYLE_INT);







    uint16_t readValue = 0;

    for(uint16_t i = 0; i < ((8192 - 8) / 2); i++)

    {

        tempLogging_read(&emptyBuffer, &readValue);

    }



    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((emptyBuffer.readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(311), UNITY_DISPLAY_STYLE_INT);

}















void test_flush(void)

{



    memset(simulated_eeprom, 0xFF, 8192);



    uint32_t checkMagic = (0x10101010);

    memcpy(simulated_eeprom, &checkMagic, sizeof(checkMagic));



    uint16_t readIndex = 0, writeIndex = 7;

    memcpy((simulated_eeprom + 4), &readIndex, sizeof(readIndex));

    memcpy((simulated_eeprom + 6), &writeIndex, sizeof(writeIndex));



    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);



    for(uint16_t i = 0; i < 3; i++)

    {

        uint16_t readValue = 0;

        tempLogging_read(&emptyBuffer, &readValue);

    }

    tempLogging_flush(&emptyBuffer);

    memcpy(&readIndex, (simulated_eeprom + 4), sizeof(readIndex));

    memcpy(&writeIndex, (simulated_eeprom + 6), sizeof(writeIndex));



    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.readIndex)), (UNITY_INT)((readIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(344), UNITY_DISPLAY_STYLE_INT);

    UnityAssertEqualNumber((UNITY_INT)((emptyBuffer.writeIndex)), (UNITY_INT)((writeIndex)), (

   ((void *)0)

   ), (UNITY_UINT)(345), UNITY_DISPLAY_STYLE_INT);

}
