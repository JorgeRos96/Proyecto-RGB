/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Proyecto para el control del led RGB de la tarjeta de aplicaciones.
	*					 Se utiliza el joystick de la tarjeta de aplicaciones para subir y  
	*					 bajar su intensidad, cambiar el color del LED y encender o apagarlo.
	*					 Se utiliza la USART para enviar por el terminal un mensaje cada vez
	*					 que se realiza una pulsación, indicando la acción que se realiza.
	*					 Se utiliza el sistem operativo RTOSv2 para realizar este proyecto
	*					 ya que se maneja la gestión de los rebotes a traves de un hilo.
	*					
	*					 Para configurar el LED RGB se utiliza una señal PWM generada a partir
	*					 del canal de un Timer por cada color del RGB. La distribución de los 
	*					 pines y los timers que se han utilizado son los siguientes:
	*
	*					 - LED RGB rojo: Timer 1 Canal 2 pin PE11
	*					 - LED RGB verde: Timer 4 Canal 4 pin PD15
	*					 - LED RGB azul: Timer 1 Canal 3 pin PE13
	*
	*					 En este proyecto se ha utilizado el conexionado de la mbed application
	*					 shield para seleccionar los pines. SIn embargo, debido a que el pin
	*					 del color azul (PF12) no podía generar la señal PWM se cammbió por el 
	*					 pin PE13 que si dispone de Timer para generar la señal PWM.
	*					 Las frecuencias de las señales PWM que se generan tienen una frecuencia 
	*					 de 1.37 kHz y se modifica la intensidad al variar el ciclo de trabajo
	*					 de la señal PWM. Para modificar el ciclo de trabajo se modifica el valor 
	*					 del registro CCRx, donde x es el canal utilizado para generar la señal.
	*
	*					 Se ha configurado la frecuencia del sistema a 180 MHz utilizando el PLL 
	*					 con el HSI como fuente de reloj.
	*
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "RGB.h"
#include "joystick.h"
#include "USART.h"
#include "Watchdog.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif
#ifdef RTE_CMSIS_RTOS2                  // when RTE component CMSIS RTOS2 is used
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#endif

#ifdef RTE_CMSIS_RTOS2_RTX5
/**
  * Override default HAL_GetTick function
  */
uint32_t HAL_GetTick (void) {
  static uint32_t ticks = 0U;
         uint32_t i;

  if (osKernelGetState () == osKernelRunning) {
    return ((uint32_t)osKernelGetTickCount ());
  }

  /* If Kernel is not running wait approximately 1 ms then increment 
     and return auxiliary tick counter value */
  for (i = (SystemCoreClock >> 14U); i > 0U; i--) {
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
  }
  return ++ticks;
}

#endif





/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(int fallo);


/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	/*Inicialización del IWDG*/
	if (init_Watchdog() != 0)
			Error_Handler(5);
  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  if (HAL_Init() != HAL_OK)
		Error_Handler(0);

  /* Configure the system clock to 180 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();

	/*Inicialización del joystick*/
	Init_GPIO();
	
	/*Inicialización del RGB*/
	if (initRGB() != HAL_OK)
		Error_Handler(4);
	
	/* Inicialización de la USART a traves de la función init_USART de la libreria USART
	*	 y habilitación de la transmisión
	*							- Baudrate = 9600 baud
	*							- Word length = 8 bits
	*							- Un bit de stop
	*							- Sin bit de paridad
	*							- Sin control de flujo
	*/
	if (init_USART() != 0)
		Error_Handler(2);

#ifdef RTE_CMSIS_RTOS2
  /* Initialize CMSIS-RTOS2 */
  osKernelInitialize ();

	osThreadNew(app_main, NULL, &app_main_attr); 

  /* Start thread execution */
  osKernelStart();
#endif



  /* Infinite loop */
  while (1)
  {
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 8
  *            PLL_N                          = 180
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
  /** Se configura el HSI como fuente de reloj del PLL y se configuran
	* 	los parametros del PLL para ajusta la frecuencia a 180 MHz con una
	* 	frecuencia del HSI de 16 MHZ (por defecto).
	* 	SYSCLK =[(16MHz(frecuencia HSI)/8(PLLM))*180 (PLLN)]/2 (PLLP) = 180 MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se activa el modo de Over Drive para poder alcanzar los 180 MHz
	* 	como frecuencia del sistema
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se selecciona el PLL como fuente de reloj del sistema y se configuran los parametros
	*		para configurar el HCLK, PCLK1 y PCLK2. La frecuencia máxima del HCLK es 180 MHZ, la 
	*		frecuencia máxima del PCLK1 es de 45 MHZ y la frecuencia máxima del PCLK2 es de 90 MHz
	*		HCLK = SYSCK/AHB = 180 MHz / 1 = 180 MHz
	*		PCLK1 = HCLK/APB1 = 180 MHz / 4 = 45 MHZ
	*		PCLK2 = HCLK/APB2 = 180 MHz / 2 = 90 MHZ
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler(1);
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(int fallo)
{
	char buf [100];
	
  if(fallo == 0)
		/* Mensaje si se ha producido un error en la inicializacón de la librería HAL*/
		printf(buf,"\r Se ha producido un error al inicializar la librería HAL\n");
	else if (fallo == 1)
		/* Mensaje si se ha producido un error en la inicializacón del reloj del sistema*/
		printf(buf,"\r Se ha producido un error al inicializar el reloj del sistema\n");
	else if(fallo == 2)
		/* Mensaje si se ha producido un error en la inicializacón de la USART*/
		printf(buf,"\r Se ha producido un error al inicializar la USART\n");
	else if (fallo == 3)
		/* Mensaje si se ha producido un error en el envío de datos de la USART*/
		printf(buf,"\r Se ha producido un error al enviar datos por la USART\n");
	else if (fallo == 4)
		/* Mensaje si se ha producido un error en la inicialización del RGB*/
		printf(buf,"\r Se ha producido un error al inicializar el RGB\n");
	else if (fallo == 5)
		/* Mensaje si se ha producido un error en la inicialización del Watchdog*/
		printf(buf,"\r Se ha producido un error al inicializar el Watchdog\n");
 
  while(1)
  {
  }
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
