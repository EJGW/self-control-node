
#include "led_pwm.h"

#include <string.h>

RGB_Led_State led;
RGB_Led_State leds[4];

void init_timer(void)
{
    /* TIM1 clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    /* NVIC Configuration */
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the TIM1 global Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel =  TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Compute the prescaler value */
    uint16_t PrescalerValue = (uint16_t) (SystemCoreClock / 1000) - 1;

    /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 10;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

    /* TIM enable counter */
    TIM_Cmd(TIM1, ENABLE);

    /* TIM IT enable */
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

    /*Update Interupt (URS bit) Enable */
    //TIM_UpdateRequestConfig(TIM1, TIM_UpdateSource_Regular); 
}


void initalize_leds(void)
{
    leds[0].mode = 1;
    leds[0].master = 3;
    leds[0].r = 255;
    leds[0].g = 0;
    leds[0].b = 0;
    leds[0].change_r = 5;
    leds[0].change_g = 5;
    leds[0].change_b = 5;
    leds[0].cur_r = 0;
    leds[0].cur_g = 0;
    leds[0].cur_b = 0;
    leds[0].time = 256;
    leds[0].std_time = 50;

    leds[1].mode = 1;
    leds[1].master = 3;
    leds[1].r = 255;
    leds[1].g = 0;
    leds[1].b = 0;
    leds[1].change_r = 5;
    leds[1].change_g = 5;
    leds[1].change_b = 5;
    leds[1].cur_r = 0;
    leds[1].cur_g = 0;
    leds[1].cur_b = 0;
    leds[1].time = 256;
    leds[1].std_time = 50;

    leds[2].mode = 1;
    leds[2].master = 3;
    leds[2].r = 255;
    leds[2].g = 0;
    leds[2].b = 0;
    leds[2].change_r = 5;
    leds[2].change_g = 5;
    leds[2].change_b = 5;
    leds[2].cur_r = 0;
    leds[2].cur_g = 0;
    leds[2].cur_b = 0;
    leds[2].time = 256;
    leds[2].std_time = 50;

    leds[3].mode = 5;
    leds[3].master = 0;
    leds[3].r = 255;
    leds[3].g = 0;
    leds[3].b = 0;
    leds[3].change_r = 5;
    leds[3].change_g = 5;
    leds[3].change_b = 5;
    leds[3].cur_r = 0;
    leds[3].cur_g = 0;
    leds[3].cur_b = 0;
    leds[3].time = 256;
    leds[3].std_time = 50;
}

void enable_PWM(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    //Timer clock
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    // Output clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);


    // ** Output (PWM) Ports definieren **
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    // ** PWM Timer **
    // Time base layout settings: TIM3 works because f = 72KHz 72M divided by 72000000, we subtract 1 and prescaler (0)
    // 72000000 on share for a period of 1 (999 +1) and get some 72000
    TIM_TimeBaseStructure.TIM_Period = PWM_STEPS-1;
    TIM_TimeBaseStructure.TIM_Prescaler = 2;  //fclk = 72M/72M - 1 = 0
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;    //0 = do not share the clock
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // count up mode
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);


    TIM_OCInitTypeDef TIM_OCInitStructure;

    // ** PWM Timer Channel**
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //signal from the timer will be used to control the interrupt controller, so it must be Enable
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // low state

    //TIM2 CN1
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); //PA0  EXT2-4

    //TIM2 CN2
    TIM_OC2Init(TIM2, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); //PA1  EXT2-8

    //TIM2 CN3
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable); //PA2  EXT2-7

    //TIM2 CN4
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); //PA3  EXT2-10

    //TIM3 CN1
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); //PA6  EXT1-14

    //TIM3 CN2
    TIM_OC2Init(TIM3, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); //PA7  EXT1-22

    //TIM3 CN3
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB0  EXT1-21

    //TIM3 CN4
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); //PB1  EXT1-26

    //TIM4 CN1
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); //PB6  EXT1-13

    //TIM4 CN2
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable); //PB7  EXT1-15

    //TIM4 CN3
    TIM_OC3Init(TIM4, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable); //PB8  EXT1-16

    //TIM4 CN4
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable); //PB9  EXT1-17


    // ** Reload TIM ARR Config **
    TIM_ARRPreloadConfig(TIM2,ENABLE);
    TIM_ARRPreloadConfig(TIM3,ENABLE);
    TIM_ARRPreloadConfig(TIM4,ENABLE);

    //  Enable the timer
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
    TIM_Cmd(TIM4, ENABLE);

    //
    initalize_leds();

    // TIM1 Interrupt Config - fürs Farben setzen
    init_timer();
}


// ***************************************************************************************************

// wird bei jedem timer 1 interrupt aufgerufen, setzt neue farben
void led_step(RGB_Led_State *led)
{
    if(led->mode == 1) // ich slave und kopiere farbe von master
    {
        led->r = leds[led->master].r;
        led->g = leds[led->master].g;
        led->b = leds[led->master].b;
    }
    else if(led->mode == 2) //set fixed color
    {
        //pass nothing to do, rgb should be set
    }
    else if(led->mode == 4) //set rnd color
    {
        led->time++;
        if(led->time >= led->std_time)
        {
            led->r = rand()%255;
            led->g = rand()%255;
            led->b = rand()%255;
            led->time = 0;
        }
    }
    else if(led->mode == 5)
    {
        fade_rnd_RGB(led);
    }
    else if(led->mode == 6) //strobe
    {
        led->time++;
        if(led->time == led->std_time)
        {
            led->r = 255;
            led->g = 255;
            led->b = 255;
        }
        else if(led->time >= led->std_time)
        {
            led->r = 0;
            led->g = 0;
            led->b = 0;
            led->time = 0;
        }
    }

}



// hardware fade
void fade_rnd_RGB(RGB_Led_State *led)
{
    #define RAND_VAL 3 + 1

    // aktuellen wert setzen (wird linear zwischen den einzelnen rgb werten gefaded)
    led->r += led->change_r;
    led->g += led->change_g;
    led->b += led->change_b;

    //led->time++;
    //if( led->time >= led->std_time )
    {
        //checken ob ziel innerhalb der werte
        if( led->r >= 255 )
        {
            led->r = 255;
            led->change_r = (float)((rand()% RAND_VAL)*-1)/led->std_time;
        }
        else if( led->r <= 0 )
        {
            led->r = 0;
            led->change_r = (float)((rand()% RAND_VAL))/led->std_time;
        }

        if( led->g >= 255 )
        {
            led->g = 255;
            led->change_g = (float)((rand()% RAND_VAL)*-1)/led->std_time;
        }
        else if( led->g <= 0 )
        {
            led->g = 0;
            led->change_g = (float)((rand()% RAND_VAL))/led->std_time;
        }

        if( led->b >= 255 )
        {
            led->b = 255;
            led->change_b = (float)((rand()% RAND_VAL)*-1)/led->std_time;
        }
        else if( led->b <= 0 )
        {
            led->b = 0;
            led->change_b = (float)((rand()% RAND_VAL))/led->std_time;
        }

        //wieder bei 0 anfangen
        //led->time = 0; // zaehlt hoch bis led->std_time;
    }


/*
    if( led->tmp != NULL )
    {
        char text[50];
        text[0] = ' ';
        ftoc(led->cur_r, text+1);
        text[8] = ' ';
        ftoc(led->target_r, text+9);
        text[16] = ' ';

        ftoc(led->cur_g, text+17);
        text[24] = ' ';
        ftoc(led->target_g, text+25);
        text[32] = ' ';
        
        ftoc(led->cur_b, text+33);
        text[40] = ' ';
        ftoc(led->target_b, text+41);
        
        text[48] = '\n';
        text[49] = 0;
        struct tcp_test_app_state  *s = (struct tcp_test_app_state  *)&(uip_conn->appstate);
        //strcpy(s->outputbuf , "huhu");
        strcpy(s->outputbuf , text);
    }
*/

   // _update_PWM();

}


/* old stuff


void set_RGB(RGB_Led_State *led)
{
    if(led->r <= 255 && led->r >= 0 && led->g <= 255 && led->g >= 0 && led->b <= 255 && led->b >= 0)
        update_PWM();
}


void fade_RGB(RGB_Led_State *led)
{
    if(led->time > 1)
    {
        led->r += led->change_r;
        led->g += led->change_g;
        led->b += led->change_b;

        if( (led->change_r > 0 && led->r > led->target_r) || (led->change_r < 0 && led->r < led->target_r) )
            led->r = led->target_r;

        if( (led->change_g > 0 && led->g > led->target_g) || (led->change_g < 0 && led->g < led->target_g) )
            led->g = led->target_g;

        if( (led->change_b > 0 && led->b > led->target_b) || (led->change_b < 0 && led->b < led->target_b) )
            led->b = led->target_b;
        led->time--;
    }
    else
    {
        led->r = led->target_r;
        led->g = led->target_g;
        led->b = led->target_b;
        led->time = 0;
    }
    update_PWM();
}


void ftoc(float a, char *s)
{
    int t = (int)a;
    s[3] = (char)(t%10 +48);
    t /= 10;
    s[2] = (char)(t%10 +48);
    t /= 10;
    s[1] = (char)(t%10 +48);
    t /= 10;
    s[0] = (char)(t%10 +48);
    s[4] = '.';
    
    t = (a-(int)a)*100;
    s[6] = (char)(t%10 +48);
    t /= 10;
    s[5] = (char)(t%10 +48);

}

int limit(float v, int min, int max)
{
    if(v > max)
        return max;
    else if( v < min)
        return min;

    return (int)v;
}

*/


static int interp(float val)
{
    return LED_PWM_LOOKUP_TABLE[ (int)val ];
    int low = LED_PWM_LOOKUP_TABLE[ (int)val ];
    int up  = LED_PWM_LOOKUP_TABLE[ (int)val+1 ];
    
    return (int)(low + (up-low) * (val-low));
}

void update_PWM(void)
{
    leds[0].cur_r = interp( leds[0].r );
    leds[0].cur_g = interp( leds[0].g );
    leds[0].cur_b = interp( leds[0].b );

    leds[1].cur_r = interp( leds[1].r );
    leds[1].cur_g = interp( leds[1].g );
    leds[1].cur_b = interp( leds[1].b );

    leds[2].cur_r = interp( leds[2].r );
    leds[2].cur_g = interp( leds[2].g );
    leds[2].cur_b = interp( leds[2].b );

    leds[3].cur_r = interp( leds[3].r );
    leds[3].cur_g = interp( leds[3].g );
    leds[3].cur_b = interp( leds[3].b );

    _update_PWM();
}


// set values between 0 and 2047
void _update_PWM(void)
{
    /* Set the Capture Compare Register value */
    TIM2->CCR1 = (int)leds[2].cur_r;
    TIM2->CCR4 = (int)leds[2].cur_g;
    TIM2->CCR2 = (int)leds[2].cur_b;

    TIM3->CCR4 = (int)leds[3].cur_r;
    TIM2->CCR3 = (int)leds[3].cur_g;
    TIM3->CCR3 = (int)leds[3].cur_b;

    TIM4->CCR1 = (int)leds[1].cur_r;
    TIM4->CCR4 = (int)leds[1].cur_g;
    TIM4->CCR2 = (int)leds[1].cur_b;

    TIM3->CCR2 = (int)leds[0].cur_r;
    TIM4->CCR3 = (int)leds[0].cur_b;
    TIM3->CCR1 = (int)leds[0].cur_g;
}



