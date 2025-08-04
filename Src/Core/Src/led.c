
#include "main.h"
#include "led.h"


#define WS2812B_0   51          /* 0.3us */
#define WS2812B_1   111         /* 0.65us */

TIM_HandleTypeDef htim8;

const uint32_t led_green128[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        /* reset 1.25us*240 = 300us */
    WS2812B_1,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* G */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* R */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* B */
    0xffffff
};

const uint32_t led_red128[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        /* reset 1.25us*240 = 300us */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* G */
    WS2812B_1,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* R */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* B */
    0xffffffff
};
const uint32_t led_blue128[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        /* reset 1.25us*240 = 300us */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* G */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* R */
    WS2812B_1,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* B */
    0xffffffff
};

const uint32_t led_off[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,                        /* reset 1.25us*240 = 300us */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* G */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* R */
    WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0,WS2812B_0, /* B */
    0xffffffff
};


static void sendLed(const uint32_t *led, uint16_t len)
{
    // The PWM generation timer will not be stopped after the transfer is complete - it will simply be left running.
    // Since the output remains high until the next transfer, this poses no problem.
    // This is intended to reduce interrupts and processing overhead.

    LL_TIM_DisableCounter(TIM8);
    LL_DMA_DisableChannel(DMA2, LL_DMA_CHANNEL_2);
    LL_TIM_EnableAllOutputs(TIM8);
    LL_DMA_ConfigTransfer(DMA2,
                            LL_DMA_CHANNEL_2,
                            LL_DMA_DIRECTION_MEMORY_TO_PERIPH | LL_DMA_PRIORITY_LOW | LL_DMA_MODE_NORMAL |
                            LL_DMA_PERIPH_NOINCREMENT | LL_DMA_MEMORY_INCREMENT |
                            LL_DMA_PDATAALIGN_WORD | LL_DMA_MDATAALIGN_WORD);
    LL_DMA_ConfigAddresses(DMA2,
                            LL_DMA_CHANNEL_2,
                            (uint32_t)led, (uint32_t)&(TIM8->CCR1),
                            LL_DMA_GetDataTransferDirection(DMA2, LL_DMA_CHANNEL_2));

    LL_DMA_SetDataLength(DMA2, LL_DMA_CHANNEL_2, len/4);
    LL_DMA_EnableChannel(DMA2, LL_DMA_CHANNEL_2);
    LL_TIM_ConfigDMABurst(TIM8, LL_TIM_DMABURST_BASEADDR_CCR1, LL_TIM_DMABURST_LENGTH_1TRANSFER);
    LL_TIM_EnableDMAReq_UPDATE(TIM8);
    TIM8->CCR1 = 0;
    LL_TIM_CC_EnableChannel(TIM8, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableCounter(TIM8);
}


void initLed(void)
{


  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 212;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_SET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_TIM_MspPostInit(&htim8);

    HAL_TIM_Base_MspInit(&htim8);
    LL_TIM_CC_EnableChannel(TIM8, LL_TIM_CHANNEL_CH1);
    LL_TIM_EnableARRPreload(TIM8);
    LL_TIM_EnableCounter(TIM8);


    setLed(LED_OFF);
}


void setLed(LED_STATE led)
{
    switch(led){
        case LED_OFF:
            sendLed(led_off, sizeof(led_off));
            break;
        case LED_GREEN128:
            sendLed(led_green128, sizeof(led_green128));
            break;
        case LED_RED128:
            sendLed(led_red128, sizeof(led_red128));
            break;
        case LED_BLUE128:
            sendLed(led_blue128, sizeof(led_blue128));
            break;
    }
}
