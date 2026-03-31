/**
 * @file    main.c
 * @brief   示例 03 - 串口通信（USART1，轮询模式）
 *
 * 硬件连接：
 *   - PA9  → USART1_TX → USB转TTL 的 RX
 *   - PA10 → USART1_RX → USB转TTL 的 TX
 *   - GND  → USB转TTL 的 GND
 *
 * 功能描述：
 *   1. 每隔 1 s 向串口发送 "Hello STM32!\r\n"
 *   2. 若在 100 ms 内收到任意字节，将其原样回显
 *
 * 串口助手设置：波特率 115200，8 位数据，无校验，1 位停止位
 *
 * CubeMX 配置：
 *   - USART1：异步模式，波特率 115200，8N1，无硬件流控
 *   - PA9 → USART1_TX（复用推挽），PA10 → USART1_RX（输入浮空）
 *   - 时钟树：SYSCLK = 72 MHz
 */

#include "main.h"
#include <string.h>

/* 外设句柄 ------------------------------------------------------------------*/
UART_HandleTypeDef huart1;

/* 函数声明 ------------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);

/**
 * @brief  程序入口
 */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();

    const char *hello = "Hello STM32!\r\n";
    uint8_t rx_byte;

    while (1)
    {
        /* 发送问候字符串 */
        HAL_UART_Transmit(&huart1, (uint8_t *)hello, strlen(hello), HAL_MAX_DELAY);

        /* 在接下来 1 s 内持续尝试接收并回显 */
        uint32_t start = HAL_GetTick();
        while (HAL_GetTick() - start < 1000U)
        {
            if (HAL_UART_Receive(&huart1, &rx_byte, 1, 10) == HAL_OK)
            {
                HAL_UART_Transmit(&huart1, &rx_byte, 1, HAL_MAX_DELAY);
            }
        }
    }
}

/**
 * @brief  系统时钟配置（72 MHz）
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief  GPIO 初始化（PA9/PA10 由 UART 初始化函数配置）
 */
static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
}

/**
 * @brief  USART1 初始化：115200，8N1，无流控
 */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief  错误处理
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
