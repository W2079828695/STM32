/**
 * @file    main.c
 * @brief   示例 01 - LED 闪烁（GPIO 输出）
 *
 * 硬件连接：
 *   - 板载 LED 连接至 PC13（低电平点亮）
 *
 * 功能描述：
 *   每隔 500 ms 翻转一次 PC13，使板载 LED 以 1 Hz 的频率闪烁。
 *
 * CubeMX 配置：
 *   - PC13：GPIO_Output，初始电平 High，推挽输出，无上下拉，低速
 *   - SYS：Debug = Serial Wire (SWD)
 *   - RCC：HSE = Crystal/Ceramic Resonator（外部 8 MHz 晶振）
 *   - 时钟树：SYSCLK = 72 MHz（PLL × 9）
 */

#include "main.h"

/* 函数声明 ------------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/**
 * @brief  程序入口
 */
int main(void)
{
    /* MCU 初始化：HAL 库、系统时钟、外设 */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    /* 主循环 */
    while (1)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); /* 翻转 LED */
        HAL_Delay(500);                          /* 延时 500 ms */
    }
}

/**
 * @brief  系统时钟配置（72 MHz，使用外部 8 MHz 晶振 + PLL）
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* 使能 HSE，配置 PLL（8 MHz × 9 = 72 MHz） */
    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
    RCC_OscInitStruct.HSEPredivValue      = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* 配置 AHB / APB 总线时钟 */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;   /* HCLK  = 72 MHz */
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;     /* PCLK1 = 36 MHz */
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;     /* PCLK2 = 72 MHz */
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief  GPIO 初始化：配置 PC13 为推挽输出（板载 LED）
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* 使能 GPIOC 时钟 */
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* PC13 初始电平：高电平（LED 熄灭） */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    /* PC13：推挽输出，无上下拉，低速 */
    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
 * @brief  错误处理：进入死循环（调试时可在此处设置断点）
 */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
