#include <string.h>
#include "anzar_io.h"
#include "anzar_config.h"
#include "main.h"

/* USART2 is the ST-LINK virtual COM port on Nucleo-64 boards; LD2 is the user LED. */
extern UART_HandleTypeDef huart2;

void anzar_io_write(const char *s, uint16_t len)
{
    HAL_UART_Transmit(&huart2, (uint8_t *)s, len, 100);
}

void anzar_io_print(const char *s)
{
    anzar_io_write(s, (uint16_t)strlen(s));
}

void anzar_io_led(bool on)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, on ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void anzar_io_buzz(uint32_t ms)
{
#if ANZAR_USE_BUZZER
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
    HAL_Delay(ms);
    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
#else
    (void)ms;
#endif
}

void anzar_io_fatal(const char *why)
{
    anzar_io_print("# ERROR ");
    anzar_io_print(why);
    anzar_io_print("\r\n");
    for (;;) {
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
        HAL_Delay(100);
    }
}
