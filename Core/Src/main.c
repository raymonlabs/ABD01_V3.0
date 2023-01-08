/* USER CODE BEGIN Header */

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
typedef enum eLedStatus
{
    led_off = GPIO_PIN_SET,
    led_on = GPIO_PIN_RESET
} eLedStatus_t;
typedef enum eLedColor
{
    color_none = 0,      // LED All OFF
    color_red,      // LED Color Red
    color_green,    // LED Color Green
    color_blue,     // LED Color Blue
    color_yellow,   // LED Color Yellow
    color_magenta,  // LED Color Magenta
    color_cyan,     // LED Color Cyan
    color_white     // LED Color White
} eLedColor_t;

typedef enum eBuzMode
{
	buzz_off,
	buzz_on,
	buzz_long,
	buzz_short
} eBuzMode_t;

#define low_vol     2048
#define ampOn  GPIO_PIN_SET
#define ampOff GPIO_PIN_RESET
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;

/* USER CODE BEGIN PV */
uint8_t ampModeCount = 0;
uint8_t voltIn = 0;
uint8_t chgStat = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM14_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */
void pwmDutySet(const int duty);
void buzzerControl(const eBuzMode_t mdoe, const int count);
void ledColor(eLedColor_t ledColor);
void ledControl(eLedStatus_t led_r, eLedStatus_t led_g, eLedStatus_t led_b);
void boostEnable(GPIO_PinState bstStatus);

void deviceInit(void);
void readSwitchStatus(int	* const status_count,
					  bool	* const mode_change,
					  int	* const befor_mode);
void ampControl(const int status);
void chargeDetect(void);
void fanControl(int     * const status,
                bool    * const mode_status);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* Driver *****************************************************************************/
// PWM Duty Set
void pwmDutySet(const int duty)
{
    if((duty >= 0) && (duty <= 100))
    {
        if((duty == 0) || (duty == 100))
        {
            TIM14->CCR1 = ((100 / 100) * duty);
        }
        else
        {
            TIM14->CCR1 = ((100 / 100) * duty) - 1;
        }
    }
    else
    {
        TIM14->CCR1 = 0;
    }
}

// Buzzer Control
void buzzerControl(const eBuzMode_t mode, const int count)
{
	switch(mode)
	{
		case buzz_off: TIM3->CCR1 = 0; break;
		case buzz_on: TIM3->CCR1 = 125; break;
		case buzz_long:
			for(int idx = 0; idx < count; idx++)
			{
				TIM3->CCR1 = 125;
				HAL_Delay(2000);
				TIM3->CCR1 = 0;
				HAL_Delay(10);
			}

		break;
		case buzz_short:
			for(int idx = 0; idx < count; idx++)
			{
				TIM3->CCR1 = 125;
				HAL_Delay(300);
				TIM3->CCR1 = 0;
				HAL_Delay(100);
			}
		break;
		default: TIM3->CCR1 = 0; break;
	}
}

// LED Color Set
void ledColor(eLedColor_t ledColor)
{
    switch(ledColor)
    {
        case color_none:
            ledControl(led_off, led_off, led_off); break;
        case color_red:
            ledControl(led_on, led_off, led_off); break;
        case color_green:
            ledControl(led_off, led_on, led_off); break;
        case color_blue:
            ledControl(led_off, led_off, led_on); break;
        case color_yellow:
            ledControl(led_on, led_on, led_off); break;
        case color_magenta:
            ledControl(led_on, led_off, led_on); break;
        case color_cyan:
            ledControl(led_off, led_on, led_on); break;
        case color_white:
            ledControl(led_on, led_on, led_on); break;
        default:
            ledControl(led_off, led_off, led_off); break;
    }
}

// LED Control
void ledControl(eLedStatus_t led_r, eLedStatus_t led_g, eLedStatus_t led_b)
{
    HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, led_r);
    HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, led_g);
    HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, led_b);
}

void boostEnable(GPIO_PinState bstStatus)
{
    HAL_GPIO_WritePin(BST_EN_GPIO_Port, BST_EN_Pin, bstStatus);
}
/**************************************************************************************/

/* Application ************************************************************************/
// LED ROTATE 100ms
void deviceInit(void)
{
    // GPIO ALL OFF
    HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_RESET); //AMP OFF
    HAL_GPIO_WritePin(BST_EN_GPIO_Port, BST_EN_Pin, GPIO_PIN_RESET); // BOOSTER OFF
    HAL_GPIO_WritePin(LED_CHG_GPIO_Port, LED_CHG_Pin, GPIO_PIN_RESET); // CHG LED OFF

    // PWM START
    HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);    // Buzzer PWM
    HAL_TIM_PWM_Start(&htim14, TIM_CHANNEL_1);	// FAN PWM

    // PWM Init
    pwmDutySet(0);
    buzzerControl(buzz_off, 0);

    // LED Initial
    // RED
    ledColor(color_red);
    HAL_Delay(250);
    // GRN
    ledColor(color_green);
    HAL_Delay(250);
    // BLU
    ledColor(color_blue);
    HAL_Delay(250);
    // YEL
    ledColor(color_yellow);
    HAL_Delay(250);
    // MAGENTA
    ledColor(color_magenta);
    HAL_Delay(250);
    // CYAN
    ledColor(color_cyan);
    HAL_Delay(250);
    // WHT
    ledColor(color_white);
    HAL_Delay(250);
    // LED ALL OFF
    ledColor(color_none);
}

void readSwitchStatus(int	* const status_count,
					  bool	* const mode_change,
					  int	* const befor_mode)
{
    int time_count = 0;

    // Status Count doc
    // status_count = 0 : System Standby Mode   (FAN OFF, )
    // status_count = 1 : System Active Mode    (FAN 0%, )
    // sratus_count = 2 : System Active Mode    (FAN 65%, )
    // sratus_count = 3 : System Active Mode    (FAN 80%, )
    // sratus_count = 4 : System Active Mode    (FAN 100%, )

    // Switch Detect
    if(HAL_GPIO_ReadPin(FAN_SW_GPIO_Port, FAN_SW_Pin) == 0)
    {
        // Switch Detect Time Count
        while(HAL_GPIO_ReadPin(FAN_SW_GPIO_Port, FAN_SW_Pin) == 0)
        {
            time_count++;
            HAL_Delay(100);
        }
    }

    // Switch Detect Time Check
    // System Mode Cheange Detect
    if(time_count >= 15)
    {
        // if System StandBy Status
        if(*status_count == 0)
        {
            *status_count = 1; // Mode Change
        }
        // System Active Status
        else
        {
            *status_count = 0; // Mode Change
        }
    }
    // System Status Cheange Detect
    else if((time_count < 20) && (time_count > 0))
    {
        if(*status_count >= 4)
        {
            *status_count = 2;
        }
        else
        {
            *status_count+=1;
        }
    }
    // Nothing
    else
    {
        /* none */
    }

    if(*befor_mode != *status_count)
    {
    	*mode_change = true;
    	*befor_mode = *status_count;
    }
}

// FAN Control
void fanControl(int     * const status,
				bool	* const mode_status)
{
    switch (*status)
    {
        case 0: // Device Off
        	ledColor(color_none);
        	boostEnable(GPIO_PIN_RESET);
        	HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_RESET);
        	pwmDutySet(0);
        	if(*mode_status == true)
        	{
        		buzzerControl(buzz_long, 1);
        		*mode_status = false;
        	}
        	break;
        case 1: // Device Start
            ledColor(color_green);
            if(*mode_status == true)
            {
                buzzerControl(buzz_long, 1);
                *mode_status = false;
            }
            *status+=1;
            break;
        case 2: // PWM 30%
            ledColor(color_green);
            boostEnable(GPIO_PIN_SET);
            HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_SET);
            pwmDutySet(50);
            if(*mode_status == true)
            {
            	buzzerControl(buzz_short, 1);
            	*mode_status = false;
            }
            break;
        case 3: // PWM 50%
            ledColor(color_green);
            boostEnable(GPIO_PIN_SET);
            HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_SET);
            pwmDutySet(80);
            if(*mode_status == true)
            {
            	buzzerControl(buzz_short, 2);
            	*mode_status = false;
            }
            break;
        case 4: // PWM 80%
            ledColor(color_green);
            boostEnable(GPIO_PIN_SET);
            HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_SET);
            pwmDutySet(100);
            if(*mode_status == true)
            {
            	buzzerControl(buzz_short, 3);
            	*mode_status = false;
            }
            break;
    }
}

// Amp Control
void ampControl(const int status)
{
	int ampSWstat = HAL_GPIO_ReadPin(AMP_SW_GPIO_Port,AMP_SW_Pin);

    if(status > 0)
    {
        if(ampSWstat == 0)
        {
            while(ampSWstat == 0);
            ampModeCount++;
            if(ampModeCount >= 2) ampModeCount = 0;
        }

        switch (ampModeCount)
        {
            case 0:
                HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, ampOn);
            break;

            case 1:
                HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, ampOff);
            break;
        }
    }
    else
    {
        ampModeCount = 1;
        HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, ampOff);
    }
}

// Charge Detect
void chargeDetect(void)
{
    voltIn = HAL_GPIO_ReadPin(VBUS_DET_GPIO_Port, VBUS_DET_Pin);
    chgStat = HAL_GPIO_ReadPin(CHG_STAT_GPIO_Port, CHG_STAT_Pin);

    if(voltIn == 1)
    {
        if (chgStat == 0)
        {
            HAL_GPIO_WritePin(LED_CHG_GPIO_Port, LED_CHG_Pin, GPIO_PIN_SET);
        } else
        {
            HAL_GPIO_WritePin(LED_CHG_GPIO_Port, LED_CHG_Pin, GPIO_PIN_RESET);
        }
    }
    else
    {
        HAL_GPIO_WritePin(LED_CHG_GPIO_Port,LED_CHG_Pin,GPIO_PIN_RESET);
    }
}
/**************************************************************************************/

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  int system_status = 0; // System Status Variable
  bool mode_status = false;
  int befor_mode = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_TIM14_Init();
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */

  deviceInit(); // Device Initial
  HAL_Delay(50);
//  HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(LED_CHG_GPIO_Port, LED_CHG_Pin, GPIO_PIN_SET);
//  HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, ampOn);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        chargeDetect();
        readSwitchStatus(&system_status,
        				 &mode_status,
						 &befor_mode);
        fanControl(&system_status, &mode_status);
        ampControl(system_status);
    	boostEnable(GPIO_PIN_SET);
//    	HAL_GPIO_TogglePin(AMP_SD_GPIO_Port, AMP_SD_Pin);
//    	HAL_Delay(2500);
    }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC_Init(void)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc.Instance = ADC1;
  hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc.Init.LowPowerAutoWait = DISABLE;
  hadc.Init.LowPowerAutoPowerOff = DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.DiscontinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc.Init.DMAContinuousRequests = DISABLE;
  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(&hadc) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 9;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 251;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 125;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 3;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 99;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_CHG_GPIO_Port, LED_CHG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(AMP_SD_GPIO_Port, AMP_SD_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, BST_EN_Pin|LED_G_Pin|LED_R_Pin|LED_B_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : FAN_SW_Pin */
  GPIO_InitStruct.Pin = FAN_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FAN_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_CHG_Pin */
  GPIO_InitStruct.Pin = LED_CHG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_CHG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : CHG_STAT_Pin */
  GPIO_InitStruct.Pin = CHG_STAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(CHG_STAT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : AMP_SD_Pin BST_EN_Pin LED_G_Pin LED_R_Pin
                           LED_B_Pin */
  GPIO_InitStruct.Pin = AMP_SD_Pin|BST_EN_Pin|LED_G_Pin|LED_R_Pin
                          |LED_B_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : AMP_SW_Pin */
  GPIO_InitStruct.Pin = AMP_SW_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(AMP_SW_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : VBUS_DET_Pin */
  GPIO_InitStruct.Pin = VBUS_DET_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(VBUS_DET_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1)
    {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
