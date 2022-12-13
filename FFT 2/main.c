#include "gd32vf103.h"
#include "lcd.h"
#include "adc.h"
#include "stdint.h"
#include "fft.h"
#include "cordic-math.h"

#define WavePoints 128 
//#define FrequencyOne 32000 
//#define FrequencyTwo 6400000
static const uint32_t Alpha = (2*3.14*256);

void delay_01us(uint32_t count);

int main(void){
  
  ADC3powerUpInit(1);                       
  Lcd_SetType(LCD_INVERTED);                
  Lcd_Init();                               
  LCD_Clear(BLACK);                         
  
  int adcValues = 0;
  Complex waveSample[WavePoints];
  
  for (int i = 0; i < WavePoints; i++){
    waveSample[i].real=0;
    waveSample[i].imag=0;
  }
  /* 
  cordic_sin(to_degree(Alpha * FrequencyOne * adcValues / (WavePoints))) + 
  cordic_sin(to_degree(Alpha * FrequencyTwo * adcValues / (WavePoints))); 
  */

  while(1){
    if(adc_flag_get(ADC0,ADC_FLAG_EOC)==SET){        
        waveSample[adcValues].real = ((((adc_regular_data_read(ADC0))/5) <<8)/1241);
        waveSample[adcValues].imag = 0;
        adcValues++;
        delay_01us(15);
      if (adcValues >= WavePoints){
        fft(waveSample, WavePoints);
        for (int i = 0; i < WavePoints/2; i++){
          LCD_DrawLine(i*2, 70, i*2, (70-(cordic_hypotenuse(((waveSample[i].imag * 2) / WavePoints), ((waveSample[i].real * 2) / WavePoints)))), GREEN);
          LCD_Wait_On_Queue();
        }
        LCD_Clear(BLACK);
        adcValues = 0;
      }
      adc_software_trigger_enable(ADC0,ADC_REGULAR_CHANNEL);
    }
  }
}

/***************DELAY FUNCTION*******************************************/
void delay_01us(uint32_t count)
{
	uint64_t start_mtime, delta_mtime;

	// Don't start measuruing until we see an mtime tick
	uint64_t tmp = get_timer_value();
	do{
	    start_mtime = get_timer_value();
	}while (start_mtime == tmp);

	do{
	    delta_mtime = get_timer_value() - start_mtime;
	}while(delta_mtime <(SystemCoreClock/4000000.0 *count ));
} 