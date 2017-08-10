#include <project.h>
#include "BLE_Custom.h"
#include <stdio.h>
#include "common.h"
#include "portable.h"
#include "timer.h"

#define START_CHAR '['
#define FALSE 0
#define TRUE (!FALSE)
#define UPTATE_TIME_MS (5000)

uint8 redRequest, greenRequest, blueRequest;
uint8 redActual, greenActual, blueActual;
uint8 ledTemp, cpuTemp;

typedef enum 
{
	WAIT_FOR_START,
    READ_RED,
    READ_GREEN,
    READ_BLUE,
    READ_CPU_TEMP,
    READ_LED_TEMP
}state_t;

static state_t state = WAIT_FOR_START;

//this function updates the characteristic values in the GATT DB so that when the 
//client reads these values, they are there
void UpdateGATTDB(void)
{
    CYBLE_GATT_HANDLE_VALUE_PAIR_T  tempHandle;	
    
    // red
	tempHandle.attrHandle = CYBLE_POOLLIGHTSERVICE_RED_COMPONENT_CHAR_HANDLE;
	tempHandle.value.val = &redActual;
	tempHandle.value.len = 1;
	tempHandle.value.actualLen = 1;
	CyBle_GattsWriteAttributeValue(&tempHandle, FALSE, &cyBle_connHandle, FALSE);  

    // green
	tempHandle.attrHandle = CYBLE_POOLLIGHTSERVICE_GREEN_COMPONENT_CHAR_HANDLE;
	tempHandle.value.val = &greenActual;
	tempHandle.value.len = 1;
	tempHandle.value.actualLen = 1;
	CyBle_GattsWriteAttributeValue(&tempHandle, FALSE, &cyBle_connHandle, FALSE);  

    // blue
	tempHandle.attrHandle = CYBLE_POOLLIGHTSERVICE_BLUE_COMPONENT_CHAR_HANDLE;
	tempHandle.value.val = &blueActual;
	tempHandle.value.len = 1;
	tempHandle.value.actualLen = 1;
	CyBle_GattsWriteAttributeValue(&tempHandle, FALSE, &cyBle_connHandle, FALSE);  

    // CPU Temp
	tempHandle.attrHandle = CYBLE_POOLLIGHTSERVICE_CPU_TEMPERATURE_CHAR_HANDLE;
	tempHandle.value.val = &cpuTemp;
	tempHandle.value.len = 1;
	tempHandle.value.actualLen = 1;
	CyBle_GattsWriteAttributeValue(&tempHandle, FALSE, &cyBle_connHandle, FALSE);  

    // LED Temp
	tempHandle.attrHandle = CYBLE_POOLLIGHTSERVICE_LED_TEMPERATURE_CHAR_HANDLE;
	tempHandle.value.val = &ledTemp;
	tempHandle.value.len = 1;
	tempHandle.value.actualLen = 1;
	CyBle_GattsWriteAttributeValue(&tempHandle, FALSE, &cyBle_connHandle, FALSE);  
}

void ProcessReceiveChar(uint8_t c)
{
	// anytime there's a start char, reset everything
 	if (c == START_CHAR)
	{
        redActual = 0;
		state = READ_RED;
	}
	else
	{
		switch (state)
		{
            case WAIT_FOR_START:
                break;
            
			case READ_RED:
				if (isdigit(c))
				{
					redActual *= 10;
					redActual += (c - 0x30);
				}
                else if (c == ' ')
                {
                    greenActual = 0;
                    state = READ_GREEN;
                }
				break;
		
			case READ_GREEN:
				if (isdigit(c))
				{
					greenActual *= 10;
					greenActual += (c - 0x30);
				}
                else if (c == ' ')
                {
                    blueActual = 0;
                    state = READ_BLUE;
                }
				break;
		
			case READ_BLUE:
				if (isdigit(c))
				{
					blueActual *= 10;
					blueActual += (c - 0x30);
				}
                else if (c == ' ')
                {
                    cpuTemp = 0;
                    state = READ_CPU_TEMP;
                }
				break;
		
			case READ_CPU_TEMP:
				if (isdigit(c))
				{
					cpuTemp *= 10;
					cpuTemp += (c - 0x30);
				}
                else if (c == ' ')
                {
                    ledTemp = 0;
                    state = READ_LED_TEMP;
                }
				break;
		
			case READ_LED_TEMP:
				if (isdigit(c))
				{
					ledTemp *= 10;
					ledTemp += (c - 0x30);
				}
                else
                {
                    UpdateGATTDB();
                    state = WAIT_FOR_START;
                }
				break;
		}
	}	
}

void StackEventHandler(uint32 event, void *eventParam)
{
    /* Structure to store data written by Client */
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    char tmp[20];
    
    switch(event)
    {
        /* Mandatory events to be handled by Find Me Target design */
        case CYBLE_EVT_STACK_ON:
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            /* Start BLE advertisement for 30 seconds and update link
             * status on LEDs */
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            /* BLE link is established */

        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;

        case CYBLE_EVT_GATTS_WRITE_REQ:
            /* Extract the Write data sent by Client */
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            if (CYBLE_POOLLIGHTSERVICE_RED_COMPONENT_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                redRequest = wrReqParam->handleValPair.value.val[0];    
                sprintf(tmp, "<R %3d>", redRequest);
                UART_UartPutString(tmp);
            }

            if (CYBLE_POOLLIGHTSERVICE_GREEN_COMPONENT_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                greenRequest = wrReqParam->handleValPair.value.val[0];    
                sprintf(tmp, "<G %3d>", greenRequest);
                UART_UartPutString(tmp);
            }

            if (CYBLE_POOLLIGHTSERVICE_BLUE_COMPONENT_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                blueRequest = wrReqParam->handleValPair.value.val[0];    
                sprintf(tmp, "<B %3d>", blueRequest);
                UART_UartPutString(tmp);
            }
            
            CyBle_GattsWriteRsp(cyBle_connHandle);
            break;
            
        default:
            break;
    }
}

  
CY_ISR(uartInterrupt)
{
    uint32 source = 0;
    char ch;
    
    ch = UART_UartGetChar();
    ProcessReceiveChar(ch);

    source = UART_GetRxInterruptSourceMasked();
    UART_ClearRxInterruptSource(source);
}

void GetUpdatesFromLEDController( void )
{
    char tmp[10];
    
    // just send the update command
    sprintf(tmp, "<U>");
    UART_UartPutString(tmp);
}

int main()
{
    TIME tUpdate;
    CYBLE_API_RESULT_T apiResult;

    Timer_Init();
    CyGlobalIntEnable;

    apiResult = CyBle_Start(StackEventHandler);

    if(apiResult != CYBLE_ERROR_OK)
    {
        /* BLE stack initialization failed, check your configuration */
        CYASSERT(0);
    }

    UART_Start();
    /* Start the Interrupt */
    UART_RX_ISR_StartEx(uartInterrupt); 
    
    // get the initial update from the controller
    GetUpdatesFromLEDController();
    
    tUpdate = Timer_ElapsedTime(0);
    
    for(;;)
    {
        /* Single API call to service all the BLE stack events. Must be
         * called at least once in a BLE connection interval */
        CyBle_ProcessEvents();
        
        if (Timer_ElapsedTime(tUpdate) > UPTATE_TIME_MS)
        {
            GetUpdatesFromLEDController();
            tUpdate = Timer_ElapsedTime(0);
        }
    }
}
