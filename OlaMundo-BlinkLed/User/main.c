// Inclui a biblioteca principal do SDK da WCH para o CH32V003.
// Ela j芍 traz defini??es dos registradores, fun??es de GPIO, clock e a fun??o Delay_Ms().
#include "debug.h"

// Fun??o principal onde a execu??o do programa sempre come?a.
int main(void) {
    
    // Configura como o controlador de interrup??es (NVIC) divide os n赤veis de prioridade.
    // ? uma inicializa??o padr?o exigida pelo SDK da WCH.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    
    // L那 as configura??es de hardware e atualiza a vari芍vel global que informa
    // a frequ那ncia atual do processador (ex: 24 MHz ou 48 MHz).
    SystemCoreClockUpdate();
    
    // Inicializa o temporizador interno (SysTick) usando a frequ那ncia do sistema
    // para permitir pausas exatas em milissegundos e microssegundos.
    Delay_Init();

    // Liga o sinal de clock (energia l車gica) para o barramento APB2 na Porta D (GPIOD).
    // Sem ativar o clock do perif谷rico primeiro, nenhum pino da Porta D responde.
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);

    // Cria uma estrutura (um bloco de configura??es) na mem車ria para definir
    // como o pino vai funcionar, inicializando todos os campos com zero.
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    
    // Seleciona o pino n迆mero 3 da porta (neste caso, ser芍 o pino PD3).
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    
    // Define o modo de opera??o como Sa赤da Push-Pull (Out_PP).
    // Isso permite que o pino envie tanto 3.3V (n赤vel alto) quanto 0V/GND (n赤vel baixo).
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    
    // Define a velocidade m芍xima de chaveamento (troca de estado) do pino para 30 MHz.
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    
    // Aplica efetivamente todas as configura??es acima nos registradores f赤sicos da Porta D.
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // Inicia um loop infinito (enquanto 1 for verdadeiro, repete para sempre).
    // Em microcontroladores, o programa nunca deve terminar e sair da fun??o main.
    while (1) {
        
        // Coloca o pino PD0 em n赤vel l車gico ALTO (Bit_SET = 3.3V), acendendo o LED.
        GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_SET);
        
        // Pausa a execu??o do c車digo por 500 milissegundos (0,5 segundo).
        Delay_Ms(500);
        
        // Coloca o pino PD0 em n赤vel l車gico BAIXO (Bit_RESET = 0V), apagando o LED.
        GPIO_WriteBit(GPIOD, GPIO_Pin_3, Bit_RESET);
        
        // Pausa novamente por 500 milissegundos antes de recome?ar o loop.
        Delay_Ms(500);
    }
}