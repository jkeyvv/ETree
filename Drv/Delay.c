#include "Delay.h"

#include "stm32f4xx.h"

//#include "FreeRTOS.h"
//#include "task.h"

/**
 * 设置初始化复位标志
 * 下载程序后的标志位 0xFFFFFFFF 执行软件复位
 * 执行完第一次复位后设置标志为 0xAABBCCDD
 * 软复位一次后不再执行复位操作
 */
static const unsigned int ResetFlag = 0xFFFFFFFF;
static void Download_ResetOneTime(void);

/** 
 * @brief  When Use DWT Delay Need Software Reset
 * @param  void
 * @return void
 */
static void Download_ResetOneTime(void)
{
    /* 延时一段时间后再执行软复位 */
    for(int i = 0; i < 500000; i++);
    if (*(unsigned int*)&ResetFlag == 0xFFFFFFFF)
    {
        FLASH_Unlock();
        FLASH_ProgramWord((unsigned int)&ResetFlag, 0xAABBCCDD);
        FLASH_Lock();
        NVIC_SystemReset();
    }

}

/* ----- DWT 延时 -----	*/
#define  DWT_CR             *(volatile uint32_t *)0xE0001000
#define  DWT_CYCCNT         *(volatile uint32_t *)0xE0001004
#define  DEM_CR             *(volatile uint32_t *)0xE000EDFC
#define  DEM_CR_TRCENA      (1 << 24)
#define  DWT_CR_CYCCNTENA   (1 <<  0)
static unsigned int CPU_ClockFrequency = 0;

/** 
 * @brief  DWT 延时初始化
 * @param  void
 * @return void
 */
void Delay_DWT_Init(void)
{
    CPU_ClockFrequency = SystemCoreClock;
    DEM_CR         |= DEM_CR_TRCENA; 
    DWT_CR         |= DWT_CR_CYCCNTENA;

    Download_ResetOneTime();
}

/** 
 * @brief  DWT 微秒延时
 * @param  US 延时计数
 * @return void
 */
void Delay_DWT_US(unsigned int US)
{
    unsigned int StartTime;
    unsigned int EndTime;
    unsigned int Time;
    
    StartTime = DWT_CYCCNT;
    Time =  US * (CPU_ClockFrequency / (1000000));
    EndTime = StartTime + Time;
    if (EndTime > StartTime)
    {
        while(DWT_CYCCNT < EndTime);
    }
    else
    {
        while(DWT_CYCCNT > EndTime);
        while(DWT_CYCCNT < EndTime);
    }
}

/** 
 * @brief  DWT 毫秒延时
 * @param  MS 延时计数
 * @return void
 */
void Delay_DWT_MS(unsigned int MS)
{
    for(unsigned int i = 0; i < MS; i++)
    {
        Delay_DWT_US(1000);
    }
}







///* ----- 滴答 Tick 延时器 -----	*/
//static unsigned char  Tick_FAC_US = 0;
///** 
// * @brief  滴答定时器初始化
// * @param  clockMHz 时钟频率 MHz
// * @return void
// */
//void Delay_SysTick_Init(unsigned char clockMHz)
//{
//    SysTick->CTRL &= ~(1<<2);
//    Tick_FAC_US = clockMHz / 8;
//}

///** 
// * @brief  滴答定时器微秒延时
// * @param  US 延时计数
// * @return void
// */
//void Delay_SysTick_US(unsigned int US)
//{
//    unsigned int TemplateValue = 0;
//    SysTick->LOAD = US * Tick_FAC_US;
//    SysTick->VAL  = 0x00;
//    SysTick->CTRL = 0x01;
//    do
//    {
//        TemplateValue = SysTick->CTRL;
//    }while((TemplateValue&0x01) && !(TemplateValue & (1<<16)));
//    SysTick->CTRL=0x00;
//    SysTick->VAL =0X00;  
//}

///** 
// * @brief  滴答定时器毫秒延时
// * @param  MS 延时计数
// * @return void
// */
//void Delay_SysTick_MS(unsigned int MS)
//{
//    for(int i = 0; i < MS; i++)
//    {
//        Delay_SysTick_US(1000);
//    }
//}


/* -----系统延时 -----	*/
static unsigned char  Sys_FAC_US = 0;
static unsigned short Sys_FAC_MS = 0;

///** 
// * @brief  系统延时初始化
// * @param  void
// * @return void
// */
//void Delay_System_Init(void)
//{
//    unsigned int ReLoad;
//    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);    //选择外部时钟  HCLK
//    Sys_FAC_US  = SystemCoreClock / 1000000;            //不论是否使用OS,fac_us都需要使用
//    ReLoad  = SystemCoreClock / 1000000;                //每秒钟的计数次数 单位为M  
//    ReLoad *= 1000000 / configTICK_RATE_HZ;             //根据configTICK_RATE_HZ设定溢出时间  //reload为24位寄存器,最大值:16777216,在72M下,约合0.233s左右	
//    Sys_FAC_MS  = 1000 / configTICK_RATE_HZ;            //代表OS可以延时的最少单位	   
//}

///** 
// * @brief  系统微秒延时
// * @param  US 延时计数
// * @return void
// */
//void Delay_System_US(unsigned int US)
//{   
//    unsigned int ticks;
//    unsigned int told,tnow,tcnt = 0;
//    unsigned int ReLoad = SysTick->LOAD;        //LOAD的值  
//    ticks = US * Sys_FAC_US;                    //需要的节拍数 
//    told = SysTick->VAL;                        //刚进入时的计数器值
//    while(1)
//    {
//        tnow = SysTick->VAL;  
//        if(tnow != told)
//        {   
//            if(tnow<told)tcnt += told - tnow;   //这里注意一下SYSTICK是一个递减的计数器就可以了.
//            else tcnt += ReLoad-tnow + told;  
//            told = tnow;
//            if(tcnt >= ticks)break;           //时间超过/等于要延迟的时间,则退出.
//        }
//    };
//}

///** 
// * @brief  系统毫秒延时
// * @param  MS 延时计数
// * @return void
// */
//void Delay_System_MS(unsigned int MS)
//{
//    vTaskDelay(MS / Sys_FAC_MS);            //FreeRTOS延时
//}









