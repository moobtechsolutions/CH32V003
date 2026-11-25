#include "ch32v00x.h"

/*
 * SYSCLK = 48 MHz
 * Modo CenterAligned1: f_PWM = SYSCLK / (2 * ARR)
 * Para 20 kHz: 48.000.000 / (2 * 1200) = 20.000 Hz (per¨ªodo = 50 us, meio per¨ªodo = 25 us)
 * Cada contagem do timer equivale a 1 / 48 MHz = 20,833 ns
 */
#define PWM_HALF_PERIOD 1200U  /* ARR = 1200 */
#define MAX_DUTY_COUNTS  540U  /* 45% de 1200 = 540 contagens (90% somado no transformador) */

void CH32V00F4P6_PushPull_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};
    TIM_BDTRInitTypeDef TIM_BDTRInitStructure = {0};

    /* Habilita clocks de GPIOA, GPIOD e TIM1 no CH32V00F4P6 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);

    /* PD2 -> TIM1_CH1 (Semiciclo A) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* PA1 -> TIM1_CH2 (Semiciclo B, defasado em 180¡ã) */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Base de tempo em 20 kHz (Center-Aligned) */
    TIM_TimeBaseInitStructure.TIM_Period = PWM_HALF_PERIOD;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    /* CH1 (PD2) em PWM1: pulso pr¨®ximo ao in¨ªcio/fim do ciclo */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);

    /* CH2 (PA1) em PWM2: pulso no centro do ciclo (defasado em 180¡ã) */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
    TIM_OCInitStructure.TIM_Pulse = PWM_HALF_PERIOD;
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    /*
     * Configura Dead Time no registrador BDTR do TIM1:
     * Com 48 MHz (TDTS = 20,83 ns) e TDG = 1:
     * 120 contagens * 20,833 ns ¡Ö 2,5 us de tempo morto
     */
    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_DeadTime = 120;
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_Low;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    /* Libera sa¨ªdas avan?adas do TIM1 e inicia a contagem */
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

/* duty_0_a_450: 0 = 0,0% | 450 = 45,0% em cada canal */
void CH32V00F4P6_SetDuty(uint16_t duty_0_a_450)
{
    uint16_t pulso;

    if (duty_0_a_450 > 450U)
    {
        duty_0_a_450 = 450U;
    }

    pulso = (uint16_t)(((uint32_t)duty_0_a_450 * MAX_DUTY_COUNTS) / 450U);

    TIM_SetCompare1(TIM1, pulso);
    TIM_SetCompare2(TIM1, PWM_HALF_PERIOD - pulso);
}

int main(void)
{
    uint16_t duty = 0;

    SystemCoreClockUpdate();
    CH32V00F4P6_PushPull_Init();

    /* Soft-start gradual de 0% at¨¦ 45% por canal */
    while (1)
    {
        CH32V00F4P6_SetDuty(duty);

        if (duty < 450U)
        {
            duty++;
        }

        Delay_Ms(2);
    }
}