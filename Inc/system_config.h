

void SystemClock_Config(void);
//void Error_Handler(uint16_t error);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MyFlagInterruptHandler(void);
void MotorSetup(void);
//static void MyFlagInterruptHandler(void);
void BufferInit (uint8_t *pointer, int length);
void Timer2Setup(void);
