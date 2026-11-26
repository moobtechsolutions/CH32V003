/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2023/12/25
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *Multiprocessor communication mode routine:
 *Master:USART1_Tx(PD5)\USART1_Rx(PD6).
 *This routine demonstrates that USART1 receives the data sent by CH341 and inverts
 *it and sends it (baud rate 115200).
 *
 *Hardware connection:PD5 -- Rx
 *                     PD6 -- Tx
 *
 */

#include "debug.h"

//Variaveis globais 
char receivedData;




void USARTx_CFG(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_USART1, ENABLE);

    /* USART1 TX-->D.5   RX-->D.6 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200; // taxa de transmiss?o 
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}


void USART_SendString(const char *str) {
    while (*str != '\0') {
        // Aguarda o buffer de transmiss?o esvaziar para n?o perder caracteres
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        
        // Envia o caractere atual e avan?a para o pr¨®ximo
        USART_SendData(USART1, (uint8_t)(*str));
        str++;
    }
}

void echoCharacterUSART()
{
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
    {
        // aqui pego o que vem na USART e jogo na variavel receivedData
        //usando o USART1 que no caso desse MCU s¨® tem uma 
        receivedData = (USART_ReceiveData(USART1));

        //aqui mando a variavel que agora te algo receivedData pela USART 
        //USART_SendString("LED invertido!\r\n");

         if (receivedData == '1') {
            GPIO_WriteBit(GPIOD, GPIO_Pin_2, (BitAction)!GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_2));
            USART_SendString("LED1 invertido!\r\n");
        } else if (receivedData == '2') {
            GPIO_WriteBit(GPIOD, GPIO_Pin_3, (BitAction)!GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_3));
            USART_SendString("LED2 invertido!\r\n");
        }
    }
}

int main(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_ResetBits(GPIOD, GPIO_Pin_2 | GPIO_Pin_3);

    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_2;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    
    GPIO_Init(GPIOD, &GPIO_InitStructure);


    USART_Printf_Init(115200);
    USARTx_CFG();

    Delay_Ms(1500);// delay para da tempo de abrir termial quando inicial o MUC 

    printf("CH32V003F4P6 Demo USART\n");


    //loop principal igual ao stm32
    while(1)
    {
        echoCharacterUSART();
       
    }
}
