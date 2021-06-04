#include "main.h"
#include "stm32f1xx_hal.h"
#include "fatfs.h"
#include "fatfs_sd.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define BUFFER_SIZE1 10
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi1;
char str[BUFFER_SIZE1];
int realaltitude1;
int realaxis1;
int realexp;
uint8_t takenaltitude = 0 , takenyaxisacc = 0;
double realaltitude   = 0 , realyaxisacc  = 0;
uint8_t takebuff[3] = {0};
uint8_t breakout = 0;
uint16_t num;
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
void tostring(char [], int);
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

int bufsize (char *buf)
{
	int i=0;
	while (*buf++ != '\0') i++;
	return i;
}
FATFS fs;
FIL fil;
FILINFO fno;
FRESULT fresult;
UINT br, bw;

int main(void)
{	
  HAL_Init();
  MX_GPIO_Init();
	SystemClock_Config();
  MX_FATFS_Init();
  MX_SPI1_Init();
	MX_SPI2_Init();
	fresult = f_mount(&fs, "/", 1);
  fresult = f_open(&fil, "Atalay_Roket_Takimi.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	f_puts("Atalay Mini Sensor Datas", &fil);
 	f_puts("\n", &fil);
	f_puts("Designed by Erdem",&fil);
	f_puts("\n", &fil);
	f_puts("\n", &fil);
	fresult = f_close(&fil);	
  while (1)
  {
	  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	  HAL_SPI_Receive(&hspi2, takebuff , sizeof(takebuff) , 1000);
	  takenyaxisacc = takebuff[1] ;
	  takenaltitude = takebuff[2] ;
	  realaltitude = takenaltitude*4;
	  breakout = takebuff[0];
    realyaxisacc = takenyaxisacc;
		realexp = breakout;
		realaltitude1 = realaltitude;
		realaxis1 = realyaxisacc;
		if(realaxis1>10)(realaxis1=0);
		fresult = f_open(&fil, "Atalay_Roket_Takimi.txt", FA_OPEN_EXISTING | FA_READ | FA_WRITE);
		fresult = f_lseek(&fil, f_size(&fil));
		f_puts("		", &fil);
		f_puts("\n", &fil);
		f_puts("-------",&fil);
		num=realaltitude1;
		if(num<0 || num==0){
		f_puts("0",&fil);
		f_puts(".",&fil);}
		else{
		tostring(str, num);
		f_write(&fil, str, bufsize(str), &bw);
		f_puts(".",&fil);}
		num=realaxis1;	
		if(num<0 || num==0){
		f_puts("0",&fil);
		f_puts(".",&fil);}
		else{
		tostring(str, num);
		f_write(&fil, str, bufsize(str), &bw);
		f_puts(".",&fil);}
		num=realexp;
		if(num<0 || num==0){
		f_puts("0",&fil);
		f_puts("-----------",&fil);}
		else{
		tostring(str, num);
		f_write(&fil, str, bufsize(str), &bw);
		f_puts("-----------",&fil);}
    f_close (&fil);		
}
}
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

   
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

static void MX_SPI1_Init(void)
{

  
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

static void MX_SPI2_Init(void)
{

  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_SLAVE;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }


}

static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

 
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_12|GPIO_PIN_14 
                          |GPIO_PIN_4, GPIO_PIN_RESET);


  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_12|GPIO_PIN_14 
                          |GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void _Error_Handler(char *file, int line)
{
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{ 

}
#endif 