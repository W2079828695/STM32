/**
 * @file    main.c
 * @brief   示例 04 - 定时器中断（TIM2，1 s 周期）
 *
 * 硬件连接：
 *   - 板载 LED：PC13（低电平点亮）
 *
 * 功能描述：
 *   配置 TIM2 以 1 s 为周期产生更新中断（Update Interrupt），
 *   在中断回调函数中翻转 PC13，使 LED 以 0.5 Hz 频率闪烁。
 *
 * 定时器周期计算（SYSCLK = 72 MHz，TIM2 挂载 APB1，经 ×2 倍频后为 72 MHz）：
 *   定时周期 = (Prescaler + 1) × (Period + 1) / TIM_CLK
 *            = (7199 + 1) × (9999 + 1) / 72 000 000
 *            = 7200 × 10000 / 72 000 000
 *            = 1 s
 *
 * CubeMX 配置：
 *   - TIM2：内部时钟，Prescaler = 7199，Counter Period = 9999
 *   - NVIC：TIM2 全局中断使能，抢占优先级 0，子优先级 0
 *   - PC13：GPIO_Output
 *   - 时钟树：SYSCLK = 72 MHz，APB1 = 36 MHz（TIM2 时钟 = 72 MHz）
 */

#include "main.h"

/* 外设句柄 ------------------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* 函数声明 ------------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/**
 * @brief  程序入口
 */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM2_Init();

    /* 启动 TIM2 更新中断 */
    HAL_TIM_Base_Start_IT(&htim2);

    /* 主循环无需轮询，中断负责翻转 LED */
    while (1)
    {
        /* 可在此处执行其他低优先级任务 */
    }
}

/**
 * @brief  TIM2 更新中断回调（每 1 s 触发一次）
 * @note   HAL 库在 TIM2_IRQHandler → HAL_TIM_IRQHandler 中调用此函数
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
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
 * @brief  GPIO 初始化：PC13 推挽输出（板载 LED）
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET); /* 初始熄灭 */

    GPIO_InitStruct.Pin   = GPIO_PIN_13;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
 * @brief  TIM2 初始化：内部时钟，1 s 更新中断
 *         Prescaler = 7199，Period = 9999
 *         定时周期 = 7200 × 10000 / 72 000 000 = 1 s
 */
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef  sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig      = {0};

    htim2.Instance               = TIM2;
    htim2.Init.Prescaler         = 7199;
    htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim2.Init.Period            = 9999;
    htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode     = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
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
