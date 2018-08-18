/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#ifndef _GDISP_LLD_BOARD_H
#define _GDISP_LLD_BOARD_H

//#include STM32_HAL_H
#include "py/mphal.h"
//#include "pin.h"
//#include "genhdr/pins.h"
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/PWM.h>


//SPI_HandleTypeDef ili_spi;
SPI_Handle spi_h;
PWM_Handle pwm_h;

SPI_Transaction trans;
#define DMA_BUFF_LEN 100
uint8_t dma_buffer[DMA_BUFF_LEN*2];
uint16_t dma_buffer_i;

extern orientation_t blit_rotation;

/*
static void pwm_timer_init(void)
{
	__TIM17_CLK_ENABLE();
	TIM_HandleTypeDef htim;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	TIM_OC_InitTypeDef sConfigOC;

	htim.Instance = TIM17;
	htim.Init.Prescaler = 2000;
	htim.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim.Init.Period = 100;
	htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim.Init.RepetitionCounter = 0;
	HAL_TIM_Base_Init(&htim);

	HAL_TIM_PWM_Init(&htim);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim, &sBreakDeadTimeConfig);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 100;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&htim, &sConfigOC, TIM_CHANNEL_1);//MICROPY_HW_UGFX_TIMER_CHANNEL);
	HAL_TIM_PWM_Start(&htim, TIM_CHANNEL_1);

}*/

static GFXINLINE void change_spi_speed(uint32_t clk) {
    SPI_close(spi_h);
    SPI_Params params;
    SPI_Params_init(&params);
    params.bitRate = clk;
    params.dataSize = 8;
    params.frameFormat = 0;
    //params.transferMode = SPI_MODE_CALLBACK;
    spi_h = SPI_open(MICROPY_HW_UGFX_SPI, &params);   
}


static GFXINLINE void init_board(GDisplay *g) {
	// As we are not using multiple displays we set g->board to NULL as we don't use it.
/*
	GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
*/


    SPI_Params params;
    SPI_Params_init(&params);
    params.bitRate = 10000000;
    params.dataSize = 8;
    params.frameFormat = 0;
    //params.transferMode = SPI_MODE_CALLBACK;


    spi_h = SPI_open(MICROPY_HW_UGFX_SPI, &params);
    
#ifdef MICROPY_HW_UGFX_BL_PWM
    PWM_Params pwmParams;    
    PWM_Params_init(&pwmParams);
    pwmParams.idleLevel = PWM_IDLE_LOW;      // Output low when PWM is not running
    pwmParams.periodUnits = PWM_PERIOD_HZ;   // Period is in Hz
    pwmParams.periodValue = 10000;           // 10kHz
    pwmParams.dutyUnits = PWM_DUTY_FRACTION; // Duty is in fractional percentage
    pwmParams.dutyValue = PWM_DUTY_FRACTION_MAX/2; // 50% initial duty cycle
    
    pwm_h = PWM_open(MICROPY_HW_UGFX_BL_PWM, &pwmParams);
    
    PWM_start(pwm_h);
#endif

    /*

	ili_spi = MICROPY_HW_UGFX_SPI;

	// init the SPI bus
    ili_spi.Init.Mode = SPI_MODE_MASTER;

    // compute the baudrate prescaler from the desired baudrate
    // select a prescaler that yields at most the desired baudrate
    uint spi_clock;
    if ((ili_spi.Instance == SPI2) || (ili_spi.Instance == SPI3)) {
        // SPI2/3 is on APB1
        spi_clock = HAL_RCC_GetPCLK1Freq();
    } else {
        // SPI1 and SPI4 are on APB2
        spi_clock = HAL_RCC_GetPCLK2Freq();
    }
    uint br_prescale = spi_clock / 10000000;
    if (br_prescale <= 2) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; }
    else if (br_prescale <= 4) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; }
    else if (br_prescale <= 8) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8; }
    else if (br_prescale <= 16) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; }
    else if (br_prescale <= 32) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32; }
    else if (br_prescale <= 64) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64; }
    else if (br_prescale <= 128) { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128; }
    else { ili_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; }

    // data is sent bigendian, latches on rising clock
    ili_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
    ili_spi.Init.CLKPhase = SPI_PHASE_1EDGE;
    ili_spi.Init.Direction = SPI_DIRECTION_2LINES;
    ili_spi.Init.DataSize = SPI_DATASIZE_8BIT;
    ili_spi.Init.NSS = SPI_NSS_SOFT;
    ili_spi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    ili_spi.Init.TIMode = SPI_TIMODE_DISABLED;
    ili_spi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    ili_spi.Init.CRCPolynomial = 0;


    // init the SPI bus
    spi_init(&ili_spi, false);

    // set the pins to default values
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS);
	GPIO_set_pin(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0);

    // init the pins to be push/pull outputs
    GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_CS;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_CS, &GPIO_InitStructure);

	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_A0;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_A0, &GPIO_InitStructure);
*/


    //GPIODirModeSet(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS, GPIO_DIR_MODE_OUT);
    //GPIODirModeSet(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST, GPIO_DIR_MODE_OUT);
    //GPIODirModeSet(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0, GPIO_DIR_MODE_OUT);

    //GPIOPinWrite(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS, MICROPY_HW_UGFX_PIN_CS);
    //GPIOPinWrite(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST, MICROPY_HW_UGFX_PIN_RST);
    //GPIOPinWrite(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_RST, MICROPY_HW_UGFX_PIN_A0);
    GPIO_write(MICROPY_HW_UGFX_PIN_CS, 1);
    GPIO_write(MICROPY_HW_UGFX_PIN_RST, 1);
    GPIO_write(MICROPY_HW_UGFX_PIN_A0, 1);

/*
	//configure the RST and backlight pins
    GPIO_set_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);

	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_RST;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_RST, &GPIO_InitStructure);

	#ifdef MICROPY_HW_UGFX_PIN_MODE
	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_MODE;
	HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_MODE, &GPIO_InitStructure);
	MICROPY_HW_UGFX_SET_MODE;
	#endif

	GPIO_InitStructure.Pin = MICROPY_HW_UGFX_PIN_BL;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_LOW;
	GPIO_InitStructure.Alternate = GPIO_AF14_TIM17;
    HAL_GPIO_Init(MICROPY_HW_UGFX_PORT_BL, &GPIO_InitStructure);
	pwm_timer_init();
*/

}

static GFXINLINE void post_init_board(GDisplay *g) {
	(void) g;
}

static inline void set_blit_rotation(orientation_t o){
    blit_rotation = o;
}

static GFXINLINE void setpin_reset(GDisplay *g, bool_t state) {
	(void) g;
	if(state) {
		// reset lcd
		//GPIO_clear_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
        //GPIOPinWrite(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST, 0);
        GPIO_write(MICROPY_HW_UGFX_PIN_RST, 0);
	} else {
		//GPIO_set_pin(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST);
        //GPIOPinWrite(MICROPY_HW_UGFX_PORT_RST, MICROPY_HW_UGFX_PIN_RST, MICROPY_HW_UGFX_PIN_RST);
        GPIO_write(MICROPY_HW_UGFX_PIN_RST, 1);
	}
}


static GFXINLINE void set_backlight(GDisplay *g, uint8_t percent) {
	(void) g;
#ifdef MICROPY_HW_UGFX_BL_PWM
   PWM_setDuty(pwm_h,(PWM_DUTY_FRACTION_MAX / 100)*percent);
#endif
}

static GFXINLINE void acquire_bus(GDisplay *g) {
	(void) g;
}


static GFXINLINE void release_bus(GDisplay *g) {
	(void) g;
	//GPIOPinWrite(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS, MICROPY_HW_UGFX_PIN_CS);  //CS high
    GPIO_write(MICROPY_HW_UGFX_PIN_CS, 1);
}
static GFXINLINE void write_index(GDisplay *g, uint16_t index) {
	(void) g;

	//GPIOPinWrite(MICROPY_HW_UGFX_PORT_CS, MICROPY_HW_UGFX_PIN_CS, 0);  //CS low
	//GPIOPinWrite(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0, 0);  //CMD low
    GPIO_write(MICROPY_HW_UGFX_PIN_CS, 0);
    GPIO_write(MICROPY_HW_UGFX_PIN_A0, 0);

	//HAL_SPI_Transmit(&ili_spi, &index, 1, 1000);


    trans.txBuf = &index;
    trans.rxBuf = NULL;
    trans.count = 1;

    (void)SPI_transfer(spi_h, &trans);
	//GPIOPinWrite(MICROPY_HW_UGFX_PORT_A0, MICROPY_HW_UGFX_PIN_A0, MICROPY_HW_UGFX_PIN_A0);  //CMD high
    GPIO_write(MICROPY_HW_UGFX_PIN_A0, 1);
    GPIO_write(MICROPY_HW_UGFX_PIN_CS, 1);
}
static GFXINLINE void write_data(GDisplay *g, uint16_t data) {
	(void) g;
	//HAL_SPI_Transmit(&ili_spi, &data, 1, 1000);
GPIO_write(MICROPY_HW_UGFX_PIN_CS, 0);
    trans.txBuf = &data;
    trans.rxBuf = NULL;
    trans.count = 1;
    (void)SPI_transfer(spi_h, &trans);
    //GPIO_write(MICROPY_HW_UGFX_PIN_CS, 1);
}
   
static GFXINLINE void write_data16_block_flush(GDisplay *g) {
   (void) g;
      
   GPIO_write(MICROPY_HW_UGFX_PIN_CS, 0);
   
   trans.txBuf = &dma_buffer;
   trans.rxBuf = NULL;           
   trans.count = dma_buffer_i;
   (void)SPI_transfer(spi_h, &trans);
   
   dma_buffer_i = 0;
   
   //GPIO_write(MICROPY_HW_UGFX_PIN_CS, 1);
}

static GFXINLINE void write_data16_block(GDisplay *g, uint16_t data) {
      
   dma_buffer[dma_buffer_i++] = data>>8;
   dma_buffer[dma_buffer_i++] = data&0xFF;
   
   if (dma_buffer_i >= DMA_BUFF_LEN*2)
      write_data16_block_flush(g);
}

static GFXINLINE void write_data16_repeated(GDisplay *g, uint16_t data, uint32_t cnt) {
	(void) g;
   
   GPIO_write(MICROPY_HW_UGFX_PIN_CS, 0);   
   
   uint8_t c_low = data&0xff;
   uint8_t c_high = data>>8;
   for (int i = 0; i < DMA_BUFF_LEN*2;){
      dma_buffer[i] = c_high;
      i++;
      dma_buffer[i] = c_low;
      i++;
   }
   
   trans.txBuf = &dma_buffer;
   trans.rxBuf = NULL;
   
   while(cnt >= DMA_BUFF_LEN){         
      trans.count = DMA_BUFF_LEN*2;
      (void)SPI_transfer(spi_h, &trans);
      cnt -= DMA_BUFF_LEN;
   }
   trans.count = cnt*2;
   (void)SPI_transfer(spi_h, &trans);

   
   //GPIO_write(MICROPY_HW_UGFX_PIN_CS, 1);
   
   
}

static GFXINLINE uint16_t read_data(GDisplay *g) {
	(void) g;
	uint8_t d;
	//HAL_SPI_Receive(&ili_spi, &d, 1, 1000);
    SPI_Transaction trans;
    trans.txBuf = NULL;
    trans.rxBuf = &d;
    trans.count = 1;
    (void)SPI_transfer(spi_h, &trans);
	return d;
}


static GFXINLINE void setreadmode(GDisplay *g) {
	(void) g;
}

static GFXINLINE void setwritemode(GDisplay *g) {
	(void) g;
}




#endif /* _GDISP_LLD_BOARD_H */
