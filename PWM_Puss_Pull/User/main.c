#include "ch32v00x.h"

#define PWM_PERIOD 2400U   /* 48 MHz / 2400 = 20 kHz (ARR = 2399) */
#define HALF_COUNT 1200U   /* Equivale a 50% de 2400 */

void PWM_PushPull_Test_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = {0};
    TIM_OCInitTypeDef TIM_OCInitStructure = {0};

    /* Habilita clocks de GPIOD, GPIOA e TIM1 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD | RCC_APB2Periph_TIM1, ENABLE);

    /* PD2 -> TIM1_CH1 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* PA1 -> TIM1_CH2 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Base de tempo de 20 kHz */
    TIM_TimeBaseInitStructure.TIM_Period = PWM_PERIOD - 1;
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    /* Configura CH1 e CH2 em modo PWM1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    /* TIM1 exige CtrlPWMOutputs para liberar os pinos de sa¨ªda */
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    TIM_Cmd(TIM1, ENABLE);
}

/* controle_0_a_100 vai de 0 (0%) a 1000 (100,0%) com 0,1% de resolu??o */
void Atualizar_Faixas_PWM(uint16_t controle_0_a_1000)
{
    uint16_t ccr1 = 0; /* Faixa 0% a 50% -> PD2 */
    uint16_t ccr2 = 0; /* Faixa 50% a 100% -> PA1 */

    if (controle_0_a_1000 > 1000U)
    {
        controle_0_a_1000 = 1000U;
    }

    if (controle_0_a_1000 <= 500U)
    {
        /* Mapeia 0..50% do controle para 0..50% do per¨ªodo do PWM (0..1200) */
        ccr1 = (uint16_t)(((uint32_t)controle_0_a_1000 * HALF_COUNT) / 500U);
        ccr2 = 0;
    }
    else
    {
        /* Mant¨¦m CH1 em 50% (1200) e sobe CH2 de 0..50% (0..1200) na faixa 50..100% */
        ccr1 = HALF_COUNT;
        ccr2 = (uint16_t)(((uint32_t)(controle_0_a_1000 - 500U) * HALF_COUNT) / 500U);
    }

    TIM_SetCompare1(TIM1, ccr1);
    TIM_SetCompare2(TIM1, ccr2);
}

int main(void)
{
    uint16_t nivel = 0;
    int8_t passo = 5;

    SystemCoreClockUpdate();
    PWM_PushPull_Test_Init();

    while (1)
    {
        Atualizar_Faixas_PWM(nivel);

        if ((int32_t)nivel + passo >= 1000)
        {
            nivel = 1000;
            passo = -passo;
        }
        else if ((int32_t)nivel + passo <= 0)
        {
            nivel = 0;
            passo = -passo;
        }
        else
        {
            nivel = (uint16_t)((int32_t)nivel + passo);
        }

        Delay_Ms(5);
    }
}