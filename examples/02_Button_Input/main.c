/**
 * @file    main.c
 * @brief   示例 02 - 按键输入（GPIO 输入）
 *
 * 硬件连接：
 *   - 按键：一端接 PA0，另一端接 GND（使用内部上拉）
 *   - 板载 LED：PC13（低电平点亮）
 *
 * 功能描述：
 *   轮询 PA0 状态：按键按下（PA0 = 低电平）时点亮 LED，
 *   松开（PA0 = 高电平）时熄灭 LED。
 *   加入 10 ms 延时作为简单软件消抖。
 *
 * CubeMX 配置：
 *   - PA0：GPIO_Input，Pull-up（内部上拉）
 *   - PC13：GPIO_Output，初始电平 High
 *   - 时钟树：SYSCLK = 72 MHz
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
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    while (1)
    {
        if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        {
            /* 按键按下：PA0 被下拉至 GND，读到低电平 */
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); /* LED 亮 */
        }
        else
        {
            /* 按键松开：内部上拉使 PA0 保持高电平 */
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   /* LED 灭 */
        }

        HAL_Delay(10); /* 简单消抖：跳过 10 ms 内的抖动 */
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
 * @brief  GPIO 初始化
 *         PA0：输入，内部上拉
 *         PC13：推挽输出，初始高电平（LED 熄灭）
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    /* PC13 初始电平：高（LED 灭） */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

    /* PA0：输入，内部上拉 */
    GPIO_InitStruct.Pin  = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* PC13：推挽输出 */
    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
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
