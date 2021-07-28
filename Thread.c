/**
  ******************************************************************************
  * @file    Templates/Src/Thread.c
  * @author  MCD Application Team
  * @brief   Fichero que trata los distintas funciones de los hilos que se
	*					 que se ejecutan en el RTOS. En este caso se encuentra el hilo app_main
	*					 que lanza el hilo rebotes donde se manejan las acciones correspondientes
	*					 a las pulsaciones y se gestionan los rebotes.
	*					 Con las pulsaciones UP y DOWN se aumenta o disminuye la intensidad 
	*					 del LED RGB, con las pulsaciones LEFT y RIGHT se cambia el color y
	*					 con la pulsación central se enciende y se apaga.
	*
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */


#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "cmsis_os2.h"  
#include "USART.h"
#include "joystick.h"
#include "RGB.h"



#define SIGLEFT    0x001
#define SIGDOWN    0x002
#define SIGRIGHT   0x004
#define SIGUP 		 0x008
#define SIGCENTER  0x010
#define SIGBAJADAL 0X020
#define SIGBAJADAR 0X040
#define SIGBAJADAU 0X080
#define SIGBAJADAD 0X100
#define SIGBAJADAC 0X200

int UP = 0;
int DOWN = 0;
int RIGHT = 0;
int LEFT = 0;
int CENTER = 0;

int contUp = 0;
int contDown = 0;
int contRight = 0;
int contLeft = 0;
int contCenter = 0;

int modo = 0;
int inten = 30000;
int encender = 0;

__NO_RETURN static void rebotes (void *arg); 
osThreadId_t tid_rebotes;    

#define APP_MAIN_STK_SZ (1024U)
static uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};



/**
  * @brief Hilo main donde se crea el hilo responsable de la gestión de los rebotes
	* @param arg
  * @retval None
  */
__NO_RETURN void app_main (void *arg) {
	
	 tid_rebotes = osThreadNew (rebotes, NULL, NULL);
	
	 osThreadExit();
}
/**
  * @brief Hilo de gestión de los rebotes donde se realiza las acciones corespondientes en cada pulsación.
	*				 En este caso se realiza el aumento del contador de pulsación y se envía el mensaje al terminal 
	*				 a traves de la USART.
	* @param arg
  * @retval None
  */
static __NO_RETURN void rebotes (void *arg) {
	
	uint32_t flag;
	char buf[100];
	int size = 0;
	

  while (1) {
		
		/*Se espera al envío de la señal desde la función de callback de las interrupciones del joystick*/
		flag = osThreadFlagsWait (0x3FF, osFlagsWaitAny, osWaitForever);

		
		/*Se recibe señal de interrupción en el flanco de subida de la pulsación LEFT*/
		if(flag == SIGLEFT){
			/*Se realiza un delay de 20 ms para evitar los rebotes*/
			osDelay(20);
			/*Se activan las interrupciones por flanco de bajada en la pulsacion LEFT*/
			IRQ_Fall_Enable(5);
			/*Se limpia el flag generado por la señal de interrupción en el flanco de subida de la pulsación LEFT*/
			osThreadFlagsClear(SIGLEFT);
		}
		
		/*Se recibe señal de interrupción en el flanco de subida de la pulsación RIGHT*/
		if(flag == SIGRIGHT){
			/*Se realiza un delay de 20 ms para evitar los rebotes*/
			osDelay(20);
			/*Se activan la interrupciones por flanco de bajada en la pulsacion RIGHT*/
			IRQ_Fall_Enable(10);
			/*Se limpia el flag generado por la señal de interrupción en el flanco de subida de la pulsación RIGHT*/
			osThreadFlagsClear(SIGRIGHT);			
		}
		
		/*Se recibe señal de interrupción en el flanco de subida de la pulsación DOWN*/
		if(flag == SIGDOWN){
			/*Se realiza un delay de 20 ms para evitar los rebotes*/
			osDelay(20);	
			/*Se activan las interrupciones por flanco de bajada en la pulsacion DOWN*/
			IRQ_Fall_Enable(3);
			/*Se limpia el flag generado por la señal de interrupción en el flanco de subida de la pulsación DOWN*/
			osThreadFlagsClear(SIGDOWN);
		}
		
		/*Se recibe señal de interrupción en el flanco de subida de la pulsación UP*/
		if(flag == SIGUP){						
			/*Se realiza un delay de 20 ms para evitar los rebotes*/
			osDelay(20);						
			/*Se activan las interrupciones por flanco de bajada en la pulsacion UP*/
			IRQ_Fall_Enable(2);			
			/*Se limpia el flag generado por la señal de interrupción en el flanco de subida de la pulsación UP*/
			osThreadFlagsClear(SIGUP);
		}
		
		/*Se recibe señal de interrupción en el flanco de subida de la pulsación CENTER*/
		if(flag == SIGCENTER){						
			/*Se realiza un delay de 20 ms para evitar los rebotes*/
			osDelay(20);						
			/*Se activan las interrupciones por flanco de bajada en la pulsacion CENTER*/
			IRQ_Fall_Enable(14);			
			/*Se limpia el flag generado por la señal de interrupción en el flanco de subida de la pulsación LEFT*/
			osThreadFlagsClear(SIGCENTER);
		}
		
		/*Se recibe señal de interrupción en el flanco de bajada de la pulsación LEFT*/
		if(flag == SIGBAJADAL){					
			/*Se activan las interrupciones por flanco de subida en la pulsacion LEFT*/
			IRQ_Rise_Enable(5);			
			/*Se limpia el flag generado por la señal de interrupción en el flanco de bajada de la pulsación LEFT*/
			osThreadFlagsClear(SIGBAJADAL);	
			
			/*Si esta encendido se realiza el cambio de color del LED RGB*/
			if (encender == 1){
				if (modo == 0){
					apagar_LED_rojo();
					encender_LED_verde(inten);
					/*Se envía mensaje al terminal a traves de la USART indicando el color que se enciende*/
					size = sprintf(buf,"\r Pulsación izquierda: Se enciende LED verde\n");
					tx_USART(buf, size);
					modo = 2;
				}
				else if (modo == 1){
					apagar_LED_azul();
					encender_LED_rojo(inten);
					/*Se envía mensaje al terminal a traves de la USART indicando el color que se enciende*/
					size = sprintf(buf,"\r Pulsación izquierda: Se enciende LED rojo\n");
					tx_USART(buf, size);
					modo = 0;
				}
				else if (modo == 2){
					apagar_LED_verde();
					encender_LED_azul(inten);
					/*Se envía mensaje al terminal a traves de la USART indicando el color que se enciende*/
					size = sprintf(buf,"\r Pulsación izquierda: Se enciende LED azul\n");
					tx_USART(buf, size);
					modo = 1;
				}	
			}			 
			
		}
				
		/*Se recibe señal de interrupción en el flanco de bajada de la pulsación RIGHT*/
		if(flag == SIGBAJADAR){		
			/*Se activan las interrupciones por flanco de subida en la pulsacion RIGHT*/
			IRQ_Rise_Enable(10);		
			/*Se limpia el flag generado por la señal de interrupción en el flanco de bajada de la pulsación RIGHT*/
			osThreadFlagsClear(SIGBAJADAR);					
			
			/*Si esta encendido se realiza el cambio de color del LED RGB*/
			if (encender == 1){
				if (modo == 0){
					apagar_LED_verde();
					encender_LED_rojo(inten);
					modo = 1;
					/*Se envía mensaje al terminal a traves de la USART indicando el color que se enciende*/
					size = sprintf(buf,"\r Pulsación derecha: Se enciende LED rojo\n");
					tx_USART(buf, size);
				}
				else if (modo == 1){
					apagar_LED_rojo();
					encender_LED_azul(inten);
					/*Se envía mensaje al terminal a traves de la USART indicando el color que se enciende*/
					size = sprintf(buf,"\r Pulsación derecha: Se enciende LED azul\n");
					tx_USART(buf, size);
					modo = 2;
				}
				else if (modo == 2){
					apagar_LED_azul();
					encender_LED_verde(inten);
					/*Se envía mensaje al terminal a traves de la USART indicando el color que se enciende*/
					size = sprintf(buf,"\r Pulsación derecha: Se enciende LED verde\n");
					tx_USART(buf, size);
					modo = 0;
				}	
			}			
		}
				
		/*Se recibe señal de interrupción en el flanco de bajada de la pulsación UP*/
		if(flag == SIGBAJADAU){								
			/*Se activan las interrupciones por flanco de subida en la pulsacion UP*/
			IRQ_Rise_Enable(2);			
			/*Se limpia el flag generado por la señal de interrupción en el flanco de bajada de la pulsación UP*/
			osThreadFlagsClear(SIGBAJADAU);	
			
			/*Se cambia la intensidad del LED RGB*/
			if (inten < 2000)
				inten = 60010;
			else
				inten = inten - 20000;
			if (modo == 0){
				intensidad_LED_verde(inten);				
			}
			else if (modo == 1){
				intensidad_LED_rojo(inten);
			}
			else if (modo == 2) { 
				intensidad_LED_azul(inten);
			}
			
			/*Se envía mensaje al terminal a traves de la USART indicando que se aumenta la intensidad*/
			size = sprintf(buf,"\r Pulsacion UP: Se aumenta la intensidad \n");
				tx_USART(buf, size);			
		}
				
		/*Se recibe señal de interrupción en el flanco de bajada de la pulsación DOWN*/
		if(flag == SIGBAJADAD){						
			/*Se activan las interrupciones por flanco de subida en la pulsacion DOWN*/
			IRQ_Rise_Enable(3);			
			/*Se limpia el flag generado por la señal de interrupción en el flanco de bajada de la pulsación DOWN*/
			osThreadFlagsClear(SIGBAJADAD);			
			
			/*Se cambia la intensidad del LED RGB*/
			if (inten > 60000)
				inten = 10;
			else
				inten = inten + 20000;
			if (modo == 0){
				intensidad_LED_verde(inten);
			}
			else if (modo == 1){
				intensidad_LED_rojo(inten);
			}
			else if (modo == 2){ 
				intensidad_LED_azul(inten);
			}		
			
			/*Se envía mensaje al terminal a traves de la USART indicando que se disminuye la intensidad*/
			size = sprintf(buf,"\r Pulsacion DOWN: Se disminuye la intensidad \n");
			tx_USART(buf, size);
		}
				
		/*Se recibe señal de interrupción en el flanco de bajada de la pulsación CENTER*/
		if(flag == SIGBAJADAC){						
			/*Se activan las interrupciones por flanco de subida en la pulsacion CENTER*/
			IRQ_Rise_Enable(14);	
			/*Se limpia el flag generado por la señal de interrupción en el flanco de bajada de la pulsación CENTER*/
			osThreadFlagsClear(SIGBAJADAC);									
					
			/*Se enciende/apaga el LED RGB*/
			if (encender == 0){
				encender = 1;
				encender_LED_verde(inten);
				/*Se envía mensaje al terminal a traves de la USART indicando que se ennciende el RGB*/
				size = sprintf(buf,"\r Pulsacion Central: Se enciende el RGB \n");
				tx_USART(buf, size);
			}
			else {
				encender = 0;
				apagar_LED_verde();
				apagar_LED_azul();
				apagar_LED_rojo();
				/*Se envía mensaje al terminal a traves de la USART indicando que se apaga el RGB*/
				size = sprintf(buf,"\r Pulsacion Central: Se apaga el RGB \n");
				tx_USART(buf, size);
			}
		}
  }
}

/**
  * @brief Función de callback de las lineas de interrupción. En este caso, con la pulsación
	*				 del joystick se envía una señal para manejar los rebotes.
	* @param GPIO_Pin: Pin que provoca la interrupción
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/*Gestión de la pulsación UP*/
	if( GPIO_Pin == GPIO_PIN_2){
		/*Se deshabilitan las interrupciones de la pulsación UP*/
		HAL_NVIC_DisableIRQ(EXTI2_IRQn);
		if(UP == 0)
		{
			UP = 1;
			osThreadFlagsSet (tid_rebotes, SIGUP);
		}	
		else {
			UP = 0;
			osThreadFlagsSet (tid_rebotes, SIGBAJADAU);
		}		
}
	if( GPIO_Pin == GPIO_PIN_3){
		/*Se deshabilitan las interrupciones de la pulsación DOWN*/
		HAL_NVIC_DisableIRQ(EXTI3_IRQn);
		if(DOWN == 0)
		{
			DOWN = 1;
			osThreadFlagsSet (tid_rebotes, SIGDOWN);
		}	
		else {
			DOWN = 0;
			osThreadFlagsSet (tid_rebotes, SIGBAJADAD);
		}		
}
	if( GPIO_Pin == GPIO_PIN_5){
	 /*Se deshabilitan las interrupciones de la pulsación LEFT*/
	 HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
		if(LEFT == 0)
		{
			LEFT = 1;
			osThreadFlagsSet (tid_rebotes, SIGLEFT);
		}	
		else {
			LEFT = 0;
			osThreadFlagsSet (tid_rebotes, SIGBAJADAL);
		}		
}
if( GPIO_Pin == GPIO_PIN_14){
	 /*Se deshabilitan las interrupciones de la pulsación CENTER*/
	 HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		if(CENTER == 0)
		{
			CENTER = 1;
			osThreadFlagsSet (tid_rebotes, SIGCENTER);
		}	
		else {
			CENTER = 0;
			osThreadFlagsSet (tid_rebotes, SIGBAJADAC);
		}		
}
if( GPIO_Pin == GPIO_PIN_10){
	 /*Se deshabilitan las interrupciones de la pulsación RIGHT*/
	 HAL_NVIC_DisableIRQ(EXTI15_10_IRQn);
		if(RIGHT == 0)
		{
			RIGHT = 1;
			osThreadFlagsSet (tid_rebotes, SIGRIGHT);
		}	
		else {
			RIGHT = 0;
			osThreadFlagsSet (tid_rebotes, SIGBAJADAR);
		}		
}
}
