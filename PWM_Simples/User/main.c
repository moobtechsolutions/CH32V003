#include "ch32v00x.h"


//criando metodo para configrar o pwm no PD3
//pwm em 50% e frequncia de 20KHz 
void PWM_TIM2_CH2_PD3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    /* 1. Habilita os clocks do GPIOD e do TIM2 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* 2. Configura PD3 como sa¨ªda alternativa push-pull (50 MHz) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* 3. Configura a base de tempo do TIM2 para 20 kHz (48 MHz / 2400) */
    TIM_TimeBaseInitStructure.TIM_Period = 2400 - 1;            // ARR = 2399
    TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;            // PSC = 0
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

    /* 4. Configura o canal 2 (CH2 -> PD3) em modo PWM1 com 50% de duty cycle */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1200;                       // CCR2 = 1200 (50%)
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);

    /* 5. Habilita preload e inicia o contador */
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    TIM_CtrlPWMOutputs(TIM2, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

int main(void)
{
    SystemCoreClockUpdate();

    //Chama o metodo 
    PWM_TIM2_CH2_PD3_Init();

    while (1)
    {
        /* O sinal PWM de 20 kHz / 50% ¨¦ gerado por hardware no PD3 */
    }
}