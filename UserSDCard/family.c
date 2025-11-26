/* metadata:
manufacturer: WCH
*/

#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
// https://github.com/openwch/ch32v307/pull/90
// https://github.com/openwch/ch32v20x/pull/12
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
#endif

#include "ch32v20x.h"
#include "ch32v20x_it.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#define LED_PORT       GPIOA
#define LED_PIN        GPIO_Pin_15
#define LED_STATE_ON   0
#define LED_CLOCK_EN() RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE)
#define LED_MODE       GPIO_Mode_Out_OD


void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));


volatile uint32_t system_ticks = 0;

/**
 * @brief  系统滴答定时器中断服务函数
 * @note   该函数在系统滴答定时器产生中断时被调用
 *         清除中断标志位并递增系统滴答计数器
 * @param  无
 * @retval 无
 */
void SysTick_Handler(void)
{
  // Clear SysTick interrupt flag
  SysTick->SR = 0;
  system_ticks++;
}

/**
 * @brief 配置系统滴答定时器
 *
 * 此函数用于配置CH32V系列MCU的SysTick定时器。它会初始化定时器的各个寄存器，
 * 设置比较值，并启动定时器。
 *
 * @param ticks 定时器的计数周期值。定时器将在计数到(ticks-1)时产生中断
 *
 * @return 总是返回0，表示配置成功
 *
 * @note 函数执行步骤：
 *       1. 使能SysTick中断
 *       2. 清除控制寄存器
 *       3. 清除状态寄存器
 *       4. 清零计数器
 *       5. 设置比较值为(ticks-1)
 *       6. 配置控制寄存器为0xF以启动定时器
 */
static uint32_t SysTick_Config(uint32_t ticks)
{
  NVIC_EnableIRQ(SysTicK_IRQn);
  SysTick->CTLR = 0;
  SysTick->SR = 0;
  SysTick->CNT = 0;
  SysTick->CMP = ticks - 1;
  SysTick->CTLR = 0xF;
  return 0;
}

/**
 * @brief  获取系统运行时间（毫秒）
 * @note   返回自系统启动以来经过的毫秒数
 *         该值由SysTick_Handler中断服务函数每毫秒递增一次
 * @param  无
 * @retval 系统运行时间，单位：毫秒
 */
uint32_t board_millis(void)
{
  return system_ticks;
}

/**
 * @brief  开发板初始化函数
 * @note   此函数用于初始化CH32V208开发板的基本功能
 *         包括中断配置、系统滴答定时器、延时功能和串口通信
 * @param  无
 * @retval 无
 */
void board_init(void)
{
  // 关闭全局中断，防止初始化过程中被中断打断
  __disable_irq();

  // 配置系统滴答定时器，设置为1ms中断一次
  // SystemCoreClock为系统时钟频率，除以1000后得到1ms的计数值
  SysTick_Config(SystemCoreClock / 1000);

  LED_CLOCK_EN();

  GPIO_InitTypeDef GPIO_InitStructure = {
    .GPIO_Pin = LED_PIN,
    .GPIO_Mode = LED_MODE,
    .GPIO_Speed = GPIO_Speed_10MHz,
  };
  GPIO_Init(LED_PORT, &GPIO_InitStructure);
  // 开启全局中断
  __enable_irq();
}


void board_led_write(bool state) {
  GPIO_WriteBit(LED_PORT, LED_PIN, state ? LED_STATE_ON : (1-LED_STATE_ON));
}
