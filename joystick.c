/**
  ******************************************************************************
  * @file    Templates/Src/joystick.c
  * @author  MCD Application Team
  * @brief   Fichero de inicialización del joystick de la tarjeta de aplicaciones.	
	*					 Pines del joystick:
	*					 PIN UP-> 	 PF2	
	*					 PIN DOWN->	 PF3	
	*					 PIN CENTER->PF14	
	*					 PIN LEFT->	 PF5
	*					 PIN RIGHT-> PF10	
  *
	*					 Se utiliza la librería mbedAppBoard_PINOUT.h donde se encuentran los
	*					 pines correspondientes a la mbed application shield. En este caso se 
	*					 emplea el pin PF2 en lugar del pin PC3 para la pulsación UP debido 
	*					 a que el la línea de interrupción para el pin 3 ya se encuentra ocupada
	*					 para la pulsación DOWN
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */

#include "joystick.h"
#include "mbedAppBoard_PINOUT.h"

/**
  * @brief Funcion de inicializacion del los pulsadores del joystick y de 
	*				 habilitación de las interrupciones por falnco de subida.
  * @param None
  * @retval None
  */
void Init_GPIO(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, SW_UP.IO | SW_RIGHT.IO | SW_CENTER.IO | SW_LEFT.IO | SW_DOWN.IO, GPIO_PIN_RESET);

  /*Configure GPIO pins : UP_Pin DOWN_Pin LEFT_Pin RIGHT_Pin
                           CENTER_Pin */
  GPIO_InitStruct.Pin = SW_UP.IO|SW_RIGHT.IO|SW_LEFT.IO|SW_CENTER.IO
                          |SW_DOWN.IO;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /* Habilitación de las interrupciones*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

void IRQ_Rise_Enable(int pin){
		
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if (pin == 5){		
		/*Se activan las interrupciones por flanco de bajada en la pulsacion LEFT*/
		GPIO_InitStruct.Pin = SW_LEFT.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	}
	
	if (pin == 10){
		/*Se activan las interrupciones por flanco de bajada en la pulsacion RIGHT*/
		GPIO_InitStruct.Pin = SW_RIGHT.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
	
	if(pin == 3){
		/*Se activan las interrupciones por flanco de bajada en la pulsacion RIGHT*/
		GPIO_InitStruct.Pin = SW_DOWN.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	}
	if(pin == 2){				
		/*Se activan las interrupciones por flanco de bajada en la pulsacion UP*/
		GPIO_InitStruct.Pin = SW_UP.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI2_IRQn);		
	}
	if(pin == 14){
		/*Se activan las interrupciones por flanco de bajada en la pulsacion CENTER*/
		GPIO_InitStruct.Pin = SW_CENTER.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}

void IRQ_Fall_Enable(int pin){
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	if (pin == 5){
		/*Se activan las interrupciones por flanco de bajada en la pulsacion LEFT*/
		GPIO_InitStruct.Pin = SW_LEFT.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
	}
	
	if (pin == 10){
		/*Se activan las interrupciones por flanco de bajada en la pulsacion RIGHT*/
		GPIO_InitStruct.Pin = SW_RIGHT.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
	
	if(pin == 3){
		/*Se activan las interrupciones por flanco de bajada en la pulsacion RIGHT*/
		GPIO_InitStruct.Pin = SW_DOWN.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
	}
	if(pin == 2){				
		/*Se activan las interrupciones por flanco de bajada en la pulsacion UP*/
		GPIO_InitStruct.Pin = SW_UP.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI2_IRQn);		
	}
	if(pin == 14){
		/*Se activan las interrupciones por flanco de bajada en la pulsacion CENTER*/
		GPIO_InitStruct.Pin = SW_CENTER.IO;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}