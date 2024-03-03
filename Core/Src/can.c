/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include <usart.h>
#include <string.h>
#include <stdio.h>
#include "IMU.h"

CAN_RxHeaderTypeDef   RxHeader;
uint8_t               RxData[8];
/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */
  CAN_FilterTypeDef  sFilterConfig;
  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 4;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
	Error_Handler();
  }
  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
HAL_StatusTypeDef CAN_Polling(void)
{
	uint32_t isCanRxFifoFilled = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);
	if (isCanRxFifoFilled < 1)
	{
		return HAL_ERROR;
	}

    HAL_StatusTypeDef isCanMsgReceived = HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData);
	if (isCanMsgReceived != HAL_OK)
	{
		return HAL_ERROR;
	}

	return HAL_OK;
}


void nullTerminate(char *str) {
    size_t length = strlen(str);
    uint8_t isNullTerminated = str[length - 1] != '\0';
    if (isNullTerminated == 0) {
        str[length] = '\0';
    }
}

void messageReceivedFromControlUnit(const char *unitType) {
    char canMsg[50];
    if (strcmp(unitType, "VCU") == 0) strncpy(canMsg, "SCU received a CAN message from the VCU.\r\n", sizeof(canMsg) - 1);
    else if (strcmp(unitType, "ACU") == 0) strncpy(canMsg, "SCU received a CAN message from the ACU.\r\n", sizeof(canMsg) - 1);
    else if (strcmp(unitType, "SCU") == 0) strncpy(canMsg, "SCU received a CAN message from the SCU.\r\n", sizeof(canMsg) - 1);
    nullTerminate(canMsg);
    HAL_USART_Transmit(&husart1, (uint8_t *)canMsg, strlen(canMsg), 10);
    strncpy(canMsg, (char *) RxData, RxHeader.DLC);
    nullTerminate(canMsg);
    HAL_USART_Transmit(&husart1, (uint8_t *)canMsg, strlen(canMsg), 10);
}

void StartCanTask(void const * argument)
{
//	imuState state;
	char canMsg[50];
	for (;;)
	{
		if (CAN_Polling() == HAL_OK)
		{
			if (RxHeader.IDE == CAN_ID_EXT)
			{
				switch (RxHeader.ExtId)
				{
					case IMU_ACCELERATION_CAN_EXT_ID:
						queueAccelerationPacket(RxData);
//						imuProcessAccelerationPacket(&state, RxData);
//						sprintf(canMsg, "IMU Acceleration Packet\r\n");
//						HAL_USART_Transmit(&husart1, (uint8_t *) canMsg, strlen(canMsg)+1, 10);
						break;
					case IMU_ANGULAR_RATE_CAN_EXT_ID:
						queueAngularRatePacket(RxData);
//						imuProcessAngularRatePacket(&state, RxData);
//						sprintf(canMsg, "IMU Angular Rate Packet\r\n");
//						HAL_USART_Transmit(&husart1, (uint8_t *) canMsg, strlen(canMsg)+1, 10);
						break;
				}
			}
			if (RxHeader.IDE == CAN_ID_STD)
			{
				switch (RxHeader.ExtId)
				{
					case CAN_VCU_CAN_ID:
                        messageReceivedFromControlUnit("VCU");
                        break;
                    case CAN_ACU_CAN_ID:
                        messageReceivedFromControlUnit("ACU");
                        break;
                    case CAN_SCU_CAN_ID:
                        messageReceivedFromControlUnit("SCU");
                        break;
				}
			}
		}
	}
}
/* USER CODE END 1 */
