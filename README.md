# STM32F103C8T6 基础入门

> STM32F103C8T6（俗称"蓝色小药丸" / Blue Pill）是最常见的 STM32 入门芯片之一。

---

## 目录

1. [芯片简介](#1-芯片简介)
2. [开发环境搭建](#2-开发环境搭建)
3. [引脚说明](#3-引脚说明)
4. [基础示例](#4-基础示例)
   - [01 - LED 闪烁（GPIO 输出）](#01---led-闪烁gpio-输出)
   - [02 - 按键输入（GPIO 输入）](#02---按键输入gpio-输入)
   - [03 - 串口通信（UART）](#03---串口通信uart)
   - [04 - 定时器中断（Timer）](#04---定时器中断timer)
5. [常用外设速查](#5-常用外设速查)
6. [参考资料](#6-参考资料)

---

## 1. 芯片简介

| 参数 | 说明 |
|------|------|
| 型号 | STM32F103C8T6 |
| 内核 | ARM Cortex-M3，72 MHz |
| Flash | 64 KB |
| SRAM | 20 KB |
| GPIO | 37 个可用 I/O |
| 定时器 | 3 个通用定时器 + 1 个高级定时器 |
| 通信接口 | 2× USART、2× SPI、2× I²C、1× CAN、1× USB（全速） |
| ADC | 2 个 12 位 ADC，10 个通道 |
| 工作电压 | 2.0 V – 3.6 V |
| 封装 | LQFP-48 |

STM32F103C8T6 属于 STM32F1 系列的"基本型"产品线，拥有丰富的外设资源，是学习嵌入式开发的理想入门芯片。

---

## 2. 开发环境搭建

### 方式 A：STM32CubeIDE（推荐新手）

1. 下载并安装 [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html)（免费，跨平台）。
2. 安装 STM32F1 的固件包：**Help → Manage Embedded Software Packages → STM32F1**。
3. 新建工程：**File → New → STM32 Project**，搜索芯片型号 `STM32F103C8T6`，按向导完成。
4. 使用 STM32CubeMX 图形化配置引脚和外设，点击 **Generate Code** 自动生成初始化代码。
5. 编写业务逻辑后，点击 **Run** 或 **Debug** 通过 ST-Link 烧录并调试。

### 方式 B：Keil MDK + STM32CubeMX

1. 安装 [Keil MDK](https://www.keil.com/download/product/)，在 Pack Installer 中安装 `Keil.STM32F1xx_DFP`。
2. 使用 [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) 生成 MDK 工程。
3. 在 Keil 中打开工程，编写代码，编译后通过 ST-Link 烧录。

### 烧录工具

| 工具 | 说明 |
|------|------|
| ST-Link V2 | 最常用的 SWD 调试/烧录器，支持单步调试 |
| J-Link | 高速调试器，兼容性好 |
| USB转TTL（串口） | 通过 BOOT0=1 进入 Bootloader，用 FlyMCU 烧录 |

---

## 3. 引脚说明

```
             STM32F103C8T6 (LQFP-48) 引脚布局（Blue Pill 板）
             ┌──────────────────────────────┐
             │  VCC   GND   3.3V   GND      │
             │  PC13  PC14  PC15   PD0(OSC) │
  SWDIO ─── │  PA13         ...   PA0  ─── ADC
  SWCLK ─── │  PA14         ...   PA1  ─── ADC
             │  BOOT0(0=Flash,1=Bootloader) │
             └──────────────────────────────┘
```

### 常用引脚功能

| 引脚 | 复用功能 | 说明 |
|------|----------|------|
| PA0–PA7 | ADC0–ADC7, TIM2_CH1–CH4 | 模拟/PWM 输入输出 |
| PA9 | USART1_TX | 串口1发送 |
| PA10 | USART1_RX | 串口1接收 |
| PA11 | USB_DM | USB 数据负 |
| PA12 | USB_DP | USB 数据正 |
| PA13 | SWDIO | SWD 调试数据 |
| PA14 | SWCLK | SWD 调试时钟 |
| PB6 | I2C1_SCL | I²C1 时钟 |
| PB7 | I2C1_SDA | I²C1 数据 |
| PB10 | USART3_TX / I2C2_SCL | 串口3发送 |
| PB11 | USART3_RX / I2C2_SDA | 串口3接收 |
| PC13 | LED（板载） | 板载 LED（低电平点亮） |
| BOOT0 | Bootloader 选择 | 0 = 正常运行；1 = 进入串口 Bootloader |

> **注意**：STM32F103C8T6 的 GPIO 为 3.3 V 电平，**不能直接接 5 V 信号**（部分引脚标注 FT 可容忍 5 V，请查阅数据手册）。

---

## 4. 基础示例

所有示例均位于 [`examples/`](examples/) 目录，基于 STM32 HAL 库（由 STM32CubeMX 生成框架）。

### 01 - LED 闪烁（GPIO 输出）

> 位置：[`examples/01_LED_Blink/`](examples/01_LED_Blink/)

**功能**：每隔 500 ms 翻转一次板载 LED（PC13）。

```c
/* 在 MX_GPIO_Init() 完成后的主循环中 */
while (1)
{
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    HAL_Delay(500);
}
```

**CubeMX 配置要点**：
- PC13 → GPIO_Output，初始电平 High（LED 低电平点亮，初始熄灭）

---

### 02 - 按键输入（GPIO 输入）

> 位置：[`examples/02_Button_Input/`](examples/02_Button_Input/)

**功能**：按下按键（PA0）时点亮 LED，松开时熄灭。

```c
while (1)
{
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
    {
        /* 按键按下（低电平有效，内部上拉） */
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); /* LED 亮 */
    }
    else
    {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   /* LED 灭 */
    }
    HAL_Delay(10); /* 简单消抖 */
}
```

**CubeMX 配置要点**：
- PA0 → GPIO_Input，Pull-up（内部上拉）
- PC13 → GPIO_Output

---

### 03 - 串口通信（UART）

> 位置：[`examples/03_UART/`](examples/03_UART/)

**功能**：每秒通过 USART1 发送 `"Hello STM32!\r\n"`，并将接收到的数据回显。

```c
/* 发送字符串 */
const char *msg = "Hello STM32!\r\n";
HAL_UART_Transmit(&huart1, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
HAL_Delay(1000);
```

```c
/* 接收单字节并回显（轮询模式） */
uint8_t rx_byte;
if (HAL_UART_Receive(&huart1, &rx_byte, 1, 100) == HAL_OK)
{
    HAL_UART_Transmit(&huart1, &rx_byte, 1, HAL_MAX_DELAY);
}
```

**CubeMX 配置要点**：
- USART1 → 异步模式，波特率 115200，8N1
- PA9 → USART1_TX（复用推挽），PA10 → USART1_RX（输入浮空）

---

### 04 - 定时器中断（Timer）

> 位置：[`examples/04_Timer/`](examples/04_Timer/)

**功能**：使用 TIM2 产生 1 s 周期中断，在中断回调中翻转 LED。

```c
/* main.c：启动定时器中断 */
HAL_TIM_Base_Start_IT(&htim2);

/* 中断回调函数（stm32f1xx_it.c 中会调用此函数） */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
    }
}
```

**CubeMX 配置要点**：
- TIM2 → 内部时钟，Prescaler = 7199，Counter Period = 9999
  - 定时周期 = (7199+1) × (9999+1) / 72 000 000 = 1 s
- 开启 TIM2 全局中断（NVIC）

---

## 5. 常用外设速查

| 外设 | HAL 初始化函数 | 常用 API |
|------|---------------|----------|
| GPIO | `MX_GPIO_Init()` | `HAL_GPIO_WritePin`, `HAL_GPIO_ReadPin`, `HAL_GPIO_TogglePin` |
| UART | `MX_USARTx_UART_Init()` | `HAL_UART_Transmit`, `HAL_UART_Receive` |
| SPI | `MX_SPIx_Init()` | `HAL_SPI_Transmit`, `HAL_SPI_Receive`, `HAL_SPI_TransmitReceive` |
| I²C | `MX_I2Cx_Init()` | `HAL_I2C_Master_Transmit`, `HAL_I2C_Master_Receive` |
| TIM（基本） | `MX_TIMx_Init()` | `HAL_TIM_Base_Start_IT`, `HAL_TIM_PeriodElapsedCallback` |
| TIM（PWM） | `MX_TIMx_Init()` | `HAL_TIM_PWM_Start`, `__HAL_TIM_SET_COMPARE` |
| ADC | `MX_ADCx_Init()` | `HAL_ADC_Start`, `HAL_ADC_PollForConversion`, `HAL_ADC_GetValue` |
| DMA | 与外设协同配置 | `HAL_UART_Transmit_DMA`, `HAL_SPI_Transmit_DMA` 等 |

---

## 6. 参考资料

- [STM32F103C8T6 数据手册（ST 官网）](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32F103 参考手册 RM0008（ST 官网）](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [STM32 HAL 库用户手册 UM1850（ST 官网）](https://www.st.com/resource/en/user_manual/um1850-description-of-stm32f1-hal-and-lowlayer-drivers-stmicroelectronics.pdf)
- [STM32CubeIDE 下载](https://www.st.com/en/development-tools/stm32cubeide.html)
- [STM32CubeMX 下载](https://www.st.com/en/development-tools/stm32cubemx.html)
