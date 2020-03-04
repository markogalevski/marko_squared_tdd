/*
 * COPYRIGHT (C) STMicroelectronics 2015. All rights reserved.
 *
 * This software is the confidential and proprietary information of
 * STMicroelectronics ("Confidential Information").  You shall not
 * disclose such Confidential Information and shall use it only in
 * accordance with the terms of the license agreement you entered into
 * with STMicroelectronics
 *
 * Programming Golden Rule: Keep it Simple!
 *
 */

/*!
 * \file   VL53L0X_platform.c
 * \brief  Code function defintions for Doppler Testchip Platform Layer
 *
 */


#include <stdio.h>    // sprintf(), vsnprintf(), printf()

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

#include "vl53l0x_i2c_platform.h"
#include "vl53l0x_def.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include <time.h>

//#include "comms_platform.h"

#include "vl53l0x_platform_log.h"

#ifdef VL53L0X_LOG_ENABLE
#define trace_print(level, ...) trace_print_module_function(TRACE_MODULE_PLATFORM, level, TRACE_FUNCTION_NONE, ##__VA_ARGS__)
#define trace_i2c(...) trace_print_module_function(TRACE_MODULE_NONE, TRACE_LEVEL_NONE, TRACE_FUNCTION_I2C, ##__VA_ARGS__)
#endif

char  debug_string[VL53L0X_MAX_STRING_LENGTH_PLT];

uint8_t cached_page = 0;

#define MIN_COMMS_VERSION_MAJOR     1
#define MIN_COMMS_VERSION_MINOR     8
#define MIN_COMMS_VERSION_BUILD     1
#define MIN_COMMS_VERSION_REVISION  0


#define MAX_STR_SIZE 255
#define MAX_MSG_SIZE 100
#define MAX_DEVICES 4
#define STATUS_OK              0x00
#define STATUS_FAIL            0x01
#define DWORD uint32_t
static unsigned char _dataBytes[MAX_MSG_SIZE];

extern I2C_HandleTypeDef hi2c2;

bool_t _check_min_version(void)
{
    return true;
}

int VL53L0X_i2c_init(char *comPortStr, unsigned int baudRate) // mja
{
    const int cArgc = 7;
    int argc = cArgc;
    unsigned int deviceCount = 0;
    unsigned int comPortId = 0;
    char errorText[MAX_STR_SIZE];
    unsigned char devIds[MAX_DEVICES];
    unsigned int status = STATUS_FAIL;
    char comPortTmpStr[MAX_STR_SIZE] = "";
    char baudRateTmpStr[MAX_STR_SIZE] = "";
    //char tmpStr[MAX_STR_SIZE] = "";
    //static char argv[7][MAX_STR_SIZE];
    int i =  0;
    char **argv = NULL;
    int ownsMutex = 0;


        ownsMutex = 1;
        argv = (char **)malloc(cArgc * sizeof(char *));
        for(i=0; i < cArgc; i++)
        {
            argv[i] = (char *)malloc(MAX_STR_SIZE);
            argv[i][0] = (char)'\0';
        }

        sscanf(comPortStr, "COM%d", &comPortId);



            /* sprintf(comPortTmpStr,  MAX_STR_SIZE, "COM_PORT=%d",  comPortId); */
            /* sprintf(baudRateTmpStr, MAX_STR_SIZE, "BAUD_RATE=%d", baudRate); */
            sprintf(comPortTmpStr,  "COM_PORT=%d",  comPortId);
            sprintf(baudRateTmpStr, "BAUD_RATE=%d", baudRate);

            /* strncpy(argv[0], MAX_STR_SIZE, comPortTmpStr,                   MAX_STR_SIZE); */
            /* strncpy(argv[1], MAX_STR_SIZE, baudRateTmpStr,                  MAX_STR_SIZE); */
            /* strncpy(argv[2], MAX_STR_SIZE, "READ_TIMEOUT_PER_BYTE_MS=2000", MAX_STR_SIZE); */
            /* strncpy(argv[3], MAX_STR_SIZE, "REPLY_TIMEOUT_MS=2000",         MAX_STR_SIZE); */
            /* strncpy(argv[4], MAX_STR_SIZE, "REPLY_TERMINATOR=u-boot> ",     MAX_STR_SIZE); */
            /* strncpy(argv[5], MAX_STR_SIZE, "ALIGN_STRICT=0",                MAX_STR_SIZE); */
            /* strncpy(argv[6], MAX_STR_SIZE, "FLUSH_ON_ERROR=1",              MAX_STR_SIZE); */
            strncpy(argv[0], comPortTmpStr,                   MAX_STR_SIZE);
            strncpy(argv[1], baudRateTmpStr,                  MAX_STR_SIZE);
            strncpy(argv[2], "READ_TIMEOUT_PER_BYTE_MS=2000", MAX_STR_SIZE);
            strncpy(argv[3], "REPLY_TIMEOUT_MS=2000",         MAX_STR_SIZE);
            strncpy(argv[4], "REPLY_TERMINATOR=u-boot> ",     MAX_STR_SIZE);
            strncpy(argv[5], "ALIGN_STRICT=0",                MAX_STR_SIZE);
            strncpy(argv[6], "FLUSH_ON_ERROR=1",              MAX_STR_SIZE);


            status = HAL_I2C_Init(&hi2c2);

    
        for(i=0; i < cArgc; i++)
        {
            free(argv[i]);
        }
        free(argv);


    return status;
}
int32_t VL53L0X_comms_close(void)
{
    unsigned int status = STATUS_FAIL;
    char errorText[MAX_STR_SIZE];

    status =  HAL_I2C_DeInit(&hi2c2);


    return status;
}

int32_t VL53L0X_write_multi(uint8_t address, uint8_t reg, uint8_t *pdata, int32_t count)
{
    int32_t status = STATUS_OK;

    unsigned int retries = 3;
    uint8_t *pWriteData    = pdata;
    uint8_t writeDataCount = count;
    uint8_t writeReg       = reg;

    /* For multi writes, the serial comms dll requires multiples 4 bytes or
     * anything less than 4 bytes. So if an irregular size is required, the
     * message is broken up into two writes.
     */
    if((count > 4) && (count % 4 != 0))
    {
        writeDataCount = 4*(count/4);
        status = VL53L0X_write_multi(address, writeReg, pWriteData, writeDataCount);

        writeReg = reg + writeDataCount;
        pWriteData += writeDataCount;
        writeDataCount = count - writeDataCount;
    }

    if(status == STATUS_OK)
    {

            do
            {
        	status = HAL_I2C_Mem_Write(&hi2c2, address, writeReg, 1, pWriteData, writeDataCount, 100);
                // note : the field dwIndexHi is ignored. dwIndexLo will
                // contain the entire index (bits 0..15).

            } while ((status != 0) && (retries-- > 0));

    }

    return status;
}

int32_t VL53L0X_read_multi(uint8_t address, uint8_t index, uint8_t *pdata, int32_t count)
{
    int32_t status = STATUS_OK;
    int32_t readDataCount = count;

    unsigned int retries = 3;

        /* The serial comms interface requires multiples of 4 bytes so we
         * must apply padding if required.
         */
        if((count % 4) != 0)
        {
            readDataCount = (4*(count/4)) + 4;
        }

        if(readDataCount > MAX_MSG_SIZE)
        {
            status = STATUS_FAIL;
        }

        if(status == STATUS_OK)
        {
            do
            {
        	status = (int32_t)HAL_I2C_Mem_Read(&hi2c2, address, index, 1, _dataBytes, readDataCount, 100);

                if(status == STATUS_OK)
                {
                    memcpy(pdata, &_dataBytes, count);
                }
                else
                {
                }
                    
            } while ((status != 0) && (retries-- > 0));
        }

    return status;
}


int32_t VL53L0X_write_byte(uint8_t address, uint8_t index, uint8_t data)
{
    int32_t status = STATUS_OK;
    const int32_t cbyte_count = 1;

#ifdef VL53L0X_LOG_ENABLE
    trace_print(TRACE_LEVEL_INFO,"Write reg : 0x%02X, Val : 0x%02X\n", index, data);
#endif

    status = VL53L0X_write_multi(address, index, &data, cbyte_count);

    return status;

}


int32_t VL53L0X_write_word(uint8_t address, uint8_t index, uint16_t data)
{
    int32_t status = STATUS_OK;

    uint8_t  buffer[BYTES_PER_WORD];

    // Split 16-bit word into MS and LS uint8_t
    buffer[0] = (uint8_t)(data >> 8);
    buffer[1] = (uint8_t)(data &  0x00FF);

    if(index%2 == 1)
    {
        status = VL53L0X_write_multi(address, index, &buffer[0], 1);
        status = VL53L0X_write_multi(address, index + 1, &buffer[1], 1);
        // serial comms cannot handle word writes to non 2-byte aligned registers.
    }
    else
    {
        status = VL53L0X_write_multi(address, index, buffer, BYTES_PER_WORD);
    }

    return status;

}


int32_t VL53L0X_write_dword(uint8_t address, uint8_t index, uint32_t data)
{
    int32_t status = STATUS_OK;
    uint8_t  buffer[BYTES_PER_DWORD];

    // Split 32-bit word into MS ... LS bytes
    buffer[0] = (uint8_t) (data >> 24);
    buffer[1] = (uint8_t)((data &  0x00FF0000) >> 16);
    buffer[2] = (uint8_t)((data &  0x0000FF00) >> 8);
    buffer[3] = (uint8_t) (data &  0x000000FF);

    status = VL53L0X_write_multi(address, index, buffer, BYTES_PER_DWORD);

    return status;

}


int32_t VL53L0X_read_byte(uint8_t address, uint8_t index, uint8_t *pdata)
{
    int32_t status = STATUS_OK;
    int32_t cbyte_count = 1;

    status = VL53L0X_read_multi(address, index, pdata, cbyte_count);

#ifdef VL53L0X_LOG_ENABLE
    trace_print(TRACE_LEVEL_INFO,"Read reg : 0x%02X, Val : 0x%02X\n", index, *pdata);
#endif

    return status;

}


int32_t VL53L0X_read_word(uint8_t address, uint8_t index, uint16_t *pdata)
{
    int32_t  status = STATUS_OK;
	uint8_t  buffer[BYTES_PER_WORD];

    status = VL53L0X_read_multi(address, index, buffer, BYTES_PER_WORD);
	*pdata = ((uint16_t)buffer[0]<<8) + (uint16_t)buffer[1];

    return status;

}

int32_t VL53L0X_read_dword(uint8_t address, uint8_t index, uint32_t *pdata)
{
    int32_t status = STATUS_OK;
	uint8_t  buffer[BYTES_PER_DWORD];

    status = VL53L0X_read_multi(address, index, buffer, BYTES_PER_DWORD);
    *pdata = ((uint32_t)buffer[0]<<24) + ((uint32_t)buffer[1]<<16) + ((uint32_t)buffer[2]<<8) + (uint32_t)buffer[3];

    return status;

}



// 16 bit address functions


int32_t VL53L0X_write_multi16(uint8_t address, uint16_t index, uint8_t *pdata, int32_t count)
{
    int32_t status = STATUS_OK;
    unsigned int retries = 3;

        do
        {

            status = SERIAL_COMMS_Write_UBOOT(address, 0, index, pdata, count);
            // note : the field dwIndexHi is ignored. dwIndexLo will
            // contain the entire index (bits 0..15).

        } while ((status != 0) && (retries-- > 0));


    // store the page from the high byte of the index
    cached_page = HIBYTE(index);



    return status;
}

int32_t VL53L0X_read_multi16(uint8_t address, uint16_t index, uint8_t *pdata, int32_t count)
{
    int32_t status = STATUS_OK;
    unsigned int retries = 3;
        do
        {
            status = SERIAL_COMMS_Read_UBOOT(address, 0, index, pdata, count);

        } while ((status != 0) && (retries-- > 0));

    // store the page from the high byte of the index
    cached_page = HIBYTE(index);

    return status;
}



int32_t VL53L0X_write_byte16(uint8_t address, uint16_t index, uint8_t data)
{
    int32_t status = STATUS_OK;
    const int32_t cbyte_count = 1;

    status = VL53L0X_write_multi16(address, index, &data, cbyte_count);

    return status;

}


int32_t VL53L0X_write_word16(uint8_t address, uint16_t index, uint16_t data)
{
    int32_t status = STATUS_OK;

    uint8_t  buffer[BYTES_PER_WORD];

    // Split 16-bit word into MS and LS uint8_t
    buffer[0] = (uint8_t)(data >> 8);
    buffer[1] = (uint8_t)(data &  0x00FF);

    if(index%2 == 1)
    {
        status = VL53L0X_write_multi16(address, index, &buffer[0], 1);
        status = VL53L0X_write_multi16(address, index + 1, &buffer[1], 1);
        // serial comms cannot handle word writes to non 2-byte aligned registers.
    }
    else
    {
        status = VL53L0X_write_multi16(address, index, buffer, BYTES_PER_WORD);
    }

    return status;

}


int32_t VL53L0X_write_dword16(uint8_t address, uint16_t index, uint32_t data)
{
    int32_t status = STATUS_OK;
    uint8_t  buffer[BYTES_PER_DWORD];

    // Split 32-bit word into MS ... LS bytes
    buffer[0] = (uint8_t) (data >> 24);
    buffer[1] = (uint8_t)((data &  0x00FF0000) > 16);
    buffer[2] = (uint8_t)((data &  0x0000FF00) > 8);
    buffer[3] = (uint8_t) (data &  0x000000FF);

    status = VL53L0X_write_multi16(address, index, buffer, BYTES_PER_DWORD);

    return status;

}


int32_t VL53L0X_read_byte16(uint8_t address, uint16_t index, uint8_t *pdata)
{
    int32_t status = STATUS_OK;
    int32_t cbyte_count = 1;

    status = VL53L0X_read_multi16(address, index, pdata, cbyte_count);

#ifdef VL53L0X_LOG_ENABLE
    trace_print(TRACE_LEVEL_INFO,"Read reg : 0x%02X, Val : 0x%02X\n", index, *pdata);
#endif

    return status;

}


int32_t VL53L0X_read_word16(uint8_t address, uint16_t index, uint16_t *pdata)
{
    int32_t  status = STATUS_OK;
    uint8_t  buffer[BYTES_PER_WORD];

    status = VL53L0X_read_multi16(address, index, buffer, BYTES_PER_WORD);
    *pdata = ((uint16_t)buffer[0]<<8) + (uint16_t)buffer[1];

    return status;

}

int32_t VL53L0X_read_dword16(uint8_t address, uint16_t index, uint32_t *pdata)
{
    int32_t status = STATUS_OK;
    uint8_t  buffer[BYTES_PER_DWORD];

    status = VL53L0X_read_multi16(address, index, buffer, BYTES_PER_DWORD);
    *pdata = ((uint32_t)buffer[0]<<24) + ((uint32_t)buffer[1]<<16) + ((uint32_t)buffer[2]<<8) + (uint32_t)buffer[3];

    return status;

}




int32_t VL53L0X_platform_wait_us(int32_t wait_us)
{
    int32_t status = STATUS_OK;
    float wait_ms = (float)wait_us/1000.0f;


#ifdef VL53L0X_LOG_ENABLE
    trace_i2c("Wait us : %6d\n", wait_us);
#endif

    return status;

}


int32_t VL53L0X_wait_ms(int32_t wait_ms)
{
    int32_t status = STATUS_OK;

    /*
     * Use windows event handling to perform non-blocking wait.
     */
    HAL_Delay(wait_ms);

#ifdef VL53L0X_LOG_ENABLE
    trace_i2c("Wait ms : %6d\n", wait_ms);
#endif

    return status;

}


int32_t VL53L0X_set_gpio(uint8_t level)
{
    int32_t status = STATUS_OK;
    //status = VL53L0X_set_gpio_sv(level);
#ifdef VL53L0X_LOG_ENABLE
    trace_i2c("// Set GPIO = %d;\n", level);
#endif
    return status;

}


int32_t VL53L0X_get_gpio(uint8_t *plevel)
{
    int32_t status = STATUS_OK;
#ifdef VL53L0X_LOG_ENABLE
    trace_i2c("// Get GPIO = %d;\n", *plevel);
#endif
    return status;
}


int32_t VL53L0X_release_gpio(void)
{
    int32_t status = STATUS_OK;
#ifdef VL53L0X_LOG_ENABLE
    trace_i2c("// Releasing force on GPIO\n");
#endif
    return status;

}

int32_t VL53L0X_cycle_power(void)
{
    int32_t status = STATUS_OK;
#ifdef VL53L0X_LOG_ENABLE
    trace_i2c("// cycle sensor power\n");
#endif
	return status;
}


int32_t VL53L0X_get_timer_frequency(int32_t *ptimer_freq_hz)
{
       *ptimer_freq_hz = 0;
       return STATUS_FAIL;
}


int32_t VL53L0X_get_timer_value(int32_t *ptimer_count)
{
       *ptimer_count = 0;
       return STATUS_FAIL;
}
