/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4_discovery.h"
#include "lis302dl.h"
#include "periph_init.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc_core.h"
#include "usbd_cdc_vcp.h"
#include "usbd_usr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/*
#define TESTRESULT_ADDRESS         0x080FFFFC
#define ALLTEST_PASS               0x00000000
#define ALLTEST_FAIL               0x55555555
*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
#pragma data_alignment = 4
#endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;

extern __I uint32_t SysTime;
extern CDC_IF_Prop_TypeDef  VCP_fops;

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);
//static void getData(void);

/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void) {
    uint8_t buf[PROTOCOL_MAX_LEN];
    int32_t data[3];
    //struct PROTOCOL_Protocol p;
    uint32_t val = PWM_PERIOD;
    Data_get = 0;

    PERIPH_Init_SysTick();
    PERIPH_Init_Leds();
    PERIPH_Init_Timer();
    PERIPH_Init_PWM();
    PERIPH_Init_Spi();
    LIS302DL_Init();

    /* USB configuration */
    USBD_Init(&USB_OTG_dev, USB_OTG_FS_CORE_ID, &USR_desc, &USBD_CDC_cb, &USR_cb);

    while (1) {
        LIS302DL_ReadACC(data);

        val = PWM_PERIOD - val;
        TIM_SetCompare1(TIM4, val);

        //p.num = 1;
        /*p.frames[0].cmd = PROTOCOL_ANGEL_X;
         p.frames[0].type = PROTOCOL_TYPE_INT;
         p.frames[0].data = abs(data[0]);*/

        /*      p.frames[1].cmd = PROTOCOL_ANGEL_X;
         p.frames[1].type = PROTOCOL_TYPE_INT;
         p.frames[1].data = abs(data[1]);*/

        //uint32_t len = PROTOCOL_getBytes(p, 0, buf);
        /*uint32_t len = 10;
        for (uint8_t i = 0; i < len; i++) {
            buf[i] = i + '0';
        }*/

        buf[0] = (uint8_t)abs(PWM_PERIOD+ABS(data[0]));
        buf[1] = (uint8_t)abs(PWM_PERIOD+ABS(data[1]));
        buf[2] = 0;

        APP_FOPS.pIf_DataTx(buf, 2);

        if (Data_get == 1) {
            TIM_SetCompare4(TIM4, 1000);
            //getData();
            VCP_fops.pIf_DataTx(Data_buf, PROTOCOL_MAX_LEN);
            Data_get = 0;
        }
        else {
            TIM_SetCompare4(TIM4, 0);
        }

        Delay(1000);
    }
}

/**
 * @brief  Inserts a delay time.
 * @param  nTime: specifies the delay time length, in 10 ms.
 * @retval None
 */
static void Delay(__IO uint32_t nTime) {
    nTime += SysTime;

    while (nTime > SysTime) {
        __NOP();
    }
}

/*static void getData(void) {
    PROTOCOL_Protocol p;
    PROTOCOL_parseProtocol(Data_buf, &p);
    for (int32_t i = 0; i < p.len; i++) {
        switch (p.frames[i].cmd) {
        case PROTOCOL_MOTOR_1:
            TIM_SetCompare1(TIM4, p.frames[i].data);
            break;
        case PROTOCOL_MOTOR_2:
            TIM_SetCompare2(TIM4, p.frames[i].data);
            break;
        case PROTOCOL_MOTOR_3:
            TIM_SetCompare3(TIM4, p.frames[i].data);
            break;
        case PROTOCOL_MOTOR_4:
            TIM_SetCompare4(TIM4, p.frames[i].data);
            break;
        }
    }
}*/

#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif

/**
 * @}
 */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
