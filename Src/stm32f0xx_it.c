/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "stm32f0xx_it.h"
#include "DFMoco.h"
/* USER CODE BEGIN 0 */

extern TIM_HandleTypeDef hTimPwm1;
extern TIM_HandleTypeDef hTimPwm2;
extern TIM_HandleTypeDef hTimPwm3;

extern TIM_HandleTypeDef htimer2;
extern TIM_HandleTypeDef htimer16;
extern struct RxBufferStruct {
    uint8_t RxSymbols[RX_BUFFER_SIZE];
    uint8_t len;
} RxBuffer[CMD_BUFFER_SIZE];
extern uint8_t RxBufferCounter;
extern struct RxBufferStruct *pRxBufferStruct;

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
DMA_HandleTypeDef hdma_usart2_tx; //TODO
extern UART_HandleTypeDef huart2;


/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles DMA1 channel 2 to 3 and DMA2 channel 1 to 2 interrupts.
*/
void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Ch2_3_DMA2_Ch1_2_IRQn 0 */

  /* USER CODE END DMA1_Ch2_3_DMA2_Ch1_2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Ch2_3_DMA2_Ch1_2_IRQn 1 */

  /* USER CODE END DMA1_Ch2_3_DMA2_Ch1_2_IRQn 1 */
}

/**
* @brief This function handles USART2 global interrupt / USART2 wake-up interrupt through EXTI line 26.
*/
void USART2_IRQHandler(void)
{

  /* USER CODE BEGIN USART2_IRQn 0 */
    if (huart2.ErrorCode != HAL_UART_ERROR_ORE){
        char rx_symbol;
        // start timer
//      HAL_TIM_Base_Start_IT(&htimer16);

        rx_symbol = huart2.Instance->RDR;
        if (rx_symbol != 0x0D && rx_symbol != 0x0A){
            *huart2.pRxBuffPtr = rx_symbol;
            if (huart2.RxXferCount < sizeof(RxBuffer[0].RxSymbols)){
                huart2.RxXferCount ++;
                huart2.pRxBuffPtr ++;
            }
            else {
                huart2.RxXferCount = 0;
                huart2.pRxBuffPtr = (uint8_t *)(&RxBuffer[RxBufferCounter]);//(uint8_t *)(pRxBufferStruct);
            }

        }
        else if (rx_symbol == 0x0D || rx_symbol == 0x0A) {
//          HAL_TIM_Base_Stop_IT(&htimer16);
            if (huart2.RxXferCount) {
                RxBuffer[RxBufferCounter].len = huart2.RxXferCount;
                huart2.RxXferCount = 0;
                if (RxBufferCounter == CMD_BUFFER_SIZE-1)
                    RxBufferCounter = 0;
                else
                    RxBufferCounter ++;
                huart2.pRxBuffPtr = (uint8_t *)(&RxBuffer[RxBufferCounter]);
            }
        }
    }
    /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/******************************************************************************/
/*                 STM32L0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l0xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles interrupt for External lines 4 to 15
  * @param  None
  * @retval None
  */
void EXTI4_15_IRQHandler(void)
{
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
}
void EXTI2_3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void TIM2_IRQHandler(void)
{

    processMessage (MSG_GET_POSITION);
    HAL_TIM_IRQHandler(&htimer2);
}
void TIM16_IRQHandler(void)
{
    if (huart2.RxXferCount) {
                RxBuffer[RxBufferCounter].len = huart2.RxXferCount;
                huart2.RxXferCount = 0;
                if (RxBufferCounter == CMD_BUFFER_SIZE-1)
                    RxBufferCounter = 0;
                else
                    RxBufferCounter ++;
                huart2.pRxBuffPtr = (uint8_t *)(&RxBuffer[RxBufferCounter]);
    }
    HAL_TIM_IRQHandler(&htimer16);
}
/**
  * @brief  This function handles TIM2 interrupt request.
  * @param  None
  * @retval None
  */
void TIM3_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hTimPwm1);
}

/**
  * @brief  This function handles TIM22 interrupt request.
  * @param  None
  * @retval None
  */
void TIM22_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hTimPwm1);
}

/**
  * @brief  This function handles TIM21 interrupt request.
  * @param  None
  * @retval None
  */
void TIM21_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&hTimPwm3);
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
