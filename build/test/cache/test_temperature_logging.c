#include "build/temp/_test_temperature_logging.c"
#include "src/temperature_logging.h"
#include "build/test/mocks/mock_eeprom.h"
#include "C:/Ruby27-x64/lib/ruby/gems/2.7.0/gems/ceedling-0.30.0/vendor/unity/src/unity.h"






uint8_t simulated_eeprom[8192];



static int mock_eeprom_read(uint8_t* dest_buffer, uint8_t* source_address,

  size_t count, int num_calls)

{

    if (source_address >= (uint8_t*)8192)

    {

        return 1;

    }

    else

    {

        memcpy(dest_buffer, &simulated_eeprom[(int)source_address], count);

        return 0;

    }

}







static int mock_eeprom_write(uint8_t* source_data, uint8_t* target_address,

  size_t count, int num_calls)

{

    if (target_address >= (uint8_t*)8192)

    {

        UnityFail( (("invalid flash memory address")), (UNITY_UINT)(30));

    }

    else

    {

        memcpy(&simulated_eeprom[(int)target_address], source_data, count);

        return 0;

    }



}







void setUp(void)

{

    eeprom_read_Stub(mock_eeprom_read);

    eeprom_write_Stub(mock_eeprom_read);

}



void tearDown(void)

{

}



void test_temperature_logging_NeedToImplement(void)

{

    memset(simulated_eeprom, 0xff, 8192);

    TempLogging_ControlBlock_t emptyBuffer;

    TempLogging_Status_t status = tempLogging_init(&emptyBuffer);

    UnityAssertEqualNumber((UNITY_INT)((TL_OK)), (UNITY_INT)((status)), (

   ((void *)0)

   ), (UNITY_UINT)(57), UNITY_DISPLAY_STYLE_INT);



}
