#include "main.h"
#include "DFMoco.h"
#include "stm32f0xx_hal.h"
#include "system_config.h"
#include "motorcontrol.h"

extern UART_HandleTypeDef huart2;


struct RxBufferStruct {
    uint8_t RxSymbols[RX_BUFFER_SIZE];
    uint8_t len;
} RxBuffer[CMD_BUFFER_SIZE];

uint8_t RxBufferCounter;

extern struct UserCmd {
    uint8_t command;
    uint8_t argCount;
    int32_t args[USER_CMD_ARGS];
} userCmd;

int main(void) {


    uint8_t RxBufferCounter_del;
    uint16_t motor_stat;
    uint16_t motor_stat_del;
    uint16_t max_speed;
    extern TIM_HandleTypeDef htimer2;

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_USART2_UART_Init();

    BufferInit ((uint8_t *)(&RxBuffer[0]), sizeof(RxBuffer));
    huart2.pRxBuffPtr = (uint8_t *)(&RxBuffer[0]);
    RxBufferCounter = 0;
    RxBufferCounter_del = RxBufferCounter;

    MotorSetup();
    Timer2Setup();
    motor_stat = BSP_MotorControl_GetDeviceState(0);
    motor_stat_del = motor_stat;

    while (1) {
        if (RxBufferCounter_del != RxBufferCounter){
            userCmd = processUserMessage ((uint8_t *)(&RxBuffer[RxBufferCounter_del]),RxBuffer[RxBufferCounter_del].len);

            if (RxBufferCounter_del == CMD_BUFFER_SIZE-1)
                    RxBufferCounter_del = 0;
                else
                    RxBufferCounter_del ++;
            processSerialCommand(userCmd);
        }
        else {
            motor_stat = BSP_MotorControl_GetDeviceState(0);
            if (motor_stat != INACTIVE)
                HAL_TIM_Base_Start_IT(&htimer2);//, TIM_CHANNEL_1);

            else if (motor_stat_del != motor_stat)
            {
                HAL_TIM_Base_Stop_IT(&htimer2);//, TIM_CHANNEL_1);
                userCmd.args[0]=1;
                processMessage (MSG_GET_POSITION);
                processMessage (MSG_GETSTAT);
            }
            motor_stat_del = motor_stat;
        }
    }

}


/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(char * file, int line) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif

