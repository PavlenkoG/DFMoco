/** System Clock Configuration
 */

#include <stdio.h>
#include <stm32f0xx_hal.h>
#include <l6474.h>
#include <motorcontrol.h>
#include <stm32_hal_legacy.h>

void SystemClock_Config(void);
void Error_Handler(uint16_t error);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
//void MyFlagInterruptHandler(void);
static void MyFlagInterruptHandler(void);
void MotorSetup(void);
void Timer2Setup(void);
void BufferInit(uint8_t *pointer, int length);

static volatile uint16_t gLastError;
UART_HandleTypeDef huart2;

TIM_HandleTypeDef htimer2;
TIM_HandleTypeDef htimer16;

void SystemClock_Config(void) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    /* No HSE Oscillator on Nucleo, Activate PLL with HSI as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler(0);
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
            | RCC_CLOCKTYPE_PCLK1);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler(1);
    }

}

void MX_GPIO_Init(void) {

    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIO Ports Clock Enable */
    __GPIOC_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
    __GPIOA_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    /*Configure GPIO pin : PC13 */
    GPIO_InitStruct.Pin = GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /*Configure GPIO pin : PA5 */
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pin : PB7 Shoot and focus*/
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : PA15 Shoot and focus*/
    GPIO_InitStruct.Pin = GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_SetPriority(TIM16_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    HAL_NVIC_EnableIRQ(TIM16_IRQn);

}

/* USART2 init function */
void MX_USART2_UART_Init(void) {

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 57600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&huart2);

    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);

}

void Error_Handler(uint16_t error) {
    /* Backup error number */
    gLastError = error;

    /* Infinite loop */
    while (1) {
    }
}

void MyFlagInterruptHandler(void) {
    /* Get the value of the status register via the L6474 command GET_STATUS */
    uint16_t statusRegister = BSP_MotorControl_CmdGetStatus(0);

    /* Check HIZ flag: if set, power brigdes are disabled */
    if ((statusRegister & L6474_STATUS_HIZ) == L6474_STATUS_HIZ) {
        // HIZ state
        // Action to be customized
    }

    /* Check direction bit */
    if ((statusRegister & L6474_STATUS_DIR) == L6474_STATUS_DIR) {
        // Forward direction is set
        // Action to be customized
    } else {
        // Backward direction is set
        // Action to be customized
    }

    /* Check NOTPERF_CMD flag: if set, the command received by SPI can't be performed */
    /* This often occures when a command is sent to the L6474 */
    /* while it is in HIZ state */
    if ((statusRegister & L6474_STATUS_NOTPERF_CMD)
            == L6474_STATUS_NOTPERF_CMD) {
        // Command received by SPI can't be performed
        // Action to be customized
    }

    /* Check WRONG_CMD flag: if set, the command does not exist */
    if ((statusRegister & L6474_STATUS_WRONG_CMD) == L6474_STATUS_WRONG_CMD) {
        //command received by SPI does not exist
        // Action to be customized
    }

    /* Check UVLO flag: if not set, there is an undervoltage lock-out */
    if ((statusRegister & L6474_STATUS_UVLO) == 0) {
        //undervoltage lock-out
        // Action to be customized
    }

    /* Check TH_WRN flag: if not set, the thermal warning threshold is reached */
    if ((statusRegister & L6474_STATUS_TH_WRN) == 0) {
        //thermal warning threshold is reached
        // Action to be customized
    }

    /* Check TH_SHD flag: if not set, the thermal shut down threshold is reached */
    if ((statusRegister & L6474_STATUS_TH_SD) == 0) {
        //thermal shut down threshold is reached
        // Action to be customized
    }

    /* Check OCD  flag: if not set, there is an overcurrent detection */
    if ((statusRegister & L6474_STATUS_OCD) == 0) {
        //overcurrent detection
        // Action to be customized
    }

}
void MotorSetup(void) {
    BSP_MotorControl_Init(BSP_MOTOR_CONTROL_BOARD_ID_L6474, 1);
    BSP_MotorControl_AttachFlagInterrupt(MyFlagInterruptHandler);
    BSP_MotorControl_AttachErrorHandler(Error_Handler);
}
void Timer2Setup(void) {

    __HAL_RCC_TIM2_CLK_ENABLE()
    ;
    __HAL_RCC_TIM16_CLK_ENABLE()
    ;

    TIM_OC_InitTypeDef sConfigOC;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_HandleTypeDef *htim2;

    TIM_OC_InitTypeDef sConfigOC16;
    TIM_MasterConfigTypeDef sMasterConfig16;
    TIM_HandleTypeDef *htim16;

    uint32_t channel;

    htim2 = &htimer2;
    htim2->Instance = TIM2;
    channel = TIM_CHANNEL_1;

    htim2->Init.Prescaler = 0xFFFF;
    htim2->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2->Init.Period = 3;
    htim2->Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    HAL_TIM_Base_Init(htim2);
    __HAL_TIM_SetAutoreload(htim2, 0x250);
    sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
    sConfigOC.Pulse = TIM_OPMODE_REPETITIVE;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(htim2, &sConfigOC, channel);

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(htim2, &sMasterConfig);
//	HAL_TIM_OC_Start_IT(htim2, channel);

    channel = TIM_CHANNEL_2;
    htim16 = &htimer16;
    htim16->Instance = TIM16;

    htim16->Init.Prescaler = 0xFFFF;
    htim16->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim16->Init.Period = 3;
    htim16->Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    HAL_TIM_Base_Init(htim16);
    __HAL_TIM_SetAutoreload(htim16, 0x500);
    sConfigOC16.OCMode = TIM_OCMODE_ACTIVE;
    sConfigOC16.Pulse = TIM_OPMODE_REPETITIVE;
    sConfigOC16.OCPolarity = TIM_OCPOLARITY_HIGH;
    HAL_TIM_PWM_ConfigChannel(htim16, &sConfigOC16, channel);

    sMasterConfig16.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig16.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    HAL_TIMEx_MasterConfigSynchronization(htim16, &sMasterConfig16);

}
void BufferInit(uint8_t *pointer, int length) {
    int i;
    for (i = 0; i < length; i++) {
        *pointer = 0x0;
        pointer++;
    }
}
