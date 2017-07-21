/*******************************************************************************
* File Name: main.c
*
* Version: 1.0
*
* Description:
*  This is source code for the PSoC 4 BLE Find Me Profile Target example project.
*
* Note:
*
* Owners:
*  OLEG@CYPRESS.COM, KRIS@CYPRESS.COM
*
* Related Document:
*
* Hardware Dependency:
*  1. PSoC 4 BLE device
*  2. CY8CKIT-042 BLE Pioneer Kit
*
* Code Tested With:
*  1. PSoC Creator 3.1
*  2. ARM CM3-RVDS
*  3. ARM CM3-GCC
*  4. ARM CM3-MDK
*
********************************************************************************
* Copyright 2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include <project.h>
#include "BLE_Custom.h"
#include <stdio.h>
#include "common.h"

uint8 red, green, blue;

/*******************************************************************************
* Function Name: StackEventHandler
********************************************************************************
*
* Summary:
*  This is an event callback function to receive events from the BLE Component.
*
* Parameters:
*  uint8 event:       Event from the CYBLE component
*  void* eventParams: A structure instance for corresponding event type. The
*                     list of event structure is described in the component
*                     datasheet.
*
* Return:
*  None
*
*******************************************************************************/
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
            break;

        case CYBLE_EVT_GATTS_WRITE_REQ:
            /* Extract the Write data sent by Client */
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            if (CYBLE_POOLLIGHTSERVICE_RED_COMPONENT_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                red = wrReqParam->handleValPair.value.val[0];    
                sprintf(tmp, "<R%3d>", red);
                UART_1_UartPutString(tmp);
            }

            if (CYBLE_POOLLIGHTSERVICE_GREEN_COMPONENT_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                red = wrReqParam->handleValPair.value.val[0];    
                sprintf(tmp, "<G%3d>", red);
                UART_1_UartPutString(tmp);
            }

            if (CYBLE_POOLLIGHTSERVICE_BLUE_COMPONENT_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                red = wrReqParam->handleValPair.value.val[0];    
                sprintf(tmp, "<B%3d>", red);
                UART_1_UartPutString(tmp);
            }

            if (CYBLE_POOLLIGHTSERVICE_COMMAND_CHAR_HANDLE == wrReqParam->handleValPair.attrHandle)
            {
                red = wrReqParam->handleValPair.value.val[0];    
                sprintf(tmp, "<C%3d>", red);
                UART_1_UartPutString(tmp);
            }
            break;
            
        default:
            break;
    }
}


/*******************************************************************************
* Function Name: main
********************************************************************************
*
* Summary:
*  Main function.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
int main()
{
    CYBLE_API_RESULT_T apiResult;

    CyGlobalIntEnable;

    apiResult = CyBle_Start(StackEventHandler);

    if(apiResult != CYBLE_ERROR_OK)
    {
        /* BLE stack initialization failed, check your configuration */
        CYASSERT(0);
    }

    UART_1_Start();

    for(;;)
    {
        /* Single API call to service all the BLE stack events. Must be
         * called at least once in a BLE connection interval */
        CyBle_ProcessEvents();
    UART_1_UartPutChar('A');
    UART_1_UartPutChar('B');
    UART_1_UartPutChar('C');
    UART_1_UartPutChar('D');
    UART_1_UartPutChar('E');
        
    }
}


/* [] END OF FILE */
