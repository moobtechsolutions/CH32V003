#include "ch32v00x.h"

void PWM_TIM2_CH2_PD3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* PD3 configurado como sa¨ªda alternativa push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Base de tempo de 20 kHz (48 MHz / 2400) */
    TIM_TimeBaseInitStructure.TIM_Period = 2400 - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    /* Canal 2 (PD3) em modo PWM1 iniciando em 0% */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

int main(void)
{
    uint16_t duty = 0;
    int8_t step = 12; /* Passo de ~0,5% por atualiza??o (12 de 2400) */

    SystemCoreClockUpdate();
    PWM_TIM2_CH2_PD3_Init();

    while (1)
    {
        TIM_SetCompare2(TIM2, duty);

        if ((duty + step) >= 2400)
        {
            duty = 2400;
            step = -step;
        }
        else if ((int16_t)duty + step <= 0)
        {
            duty = 0;
            step = -step;
        }
        else
        {
            duty = (uint16_t)((int16_t)duty + step);
        }

        Delay_Ms(1);
    }
}