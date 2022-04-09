/* 

# 简单的介绍

程序制作者： 蓬莱山望夜

制作者QQ： 3486805897

注意： 该程序仅仅作为交流学习使用，禁止商用（不会真的会用我的Bug吧……），使用请标注制作者！

版本：V1.0 2022-4-9 675H Program Size: data=63.0 xdata=0 code=2335

# 另外想说的话

真的非常开心能完完整整的做完单片机的毕业设计程序！！记得在2020年的时候，我和我的室友参加了强哥的所谓的 “社团” （其实是莫名奇妙进来的）。在他的社团里面，我亲眼看到了单片机的样子，并且利用了曾经所学的知识启动了它！！那时真的特别的开心。之后，我感觉单片机好弱啊，正好那个时候，强哥问我要不要学树莓派，我说好呀！那一段时间，应该算是我对嵌入式开发的启蒙了吧……

后来大三的时候，老师让我们选择毕业设计，毕业设计嘛，有三条路：单片机、PLC、CAD新型设计。出于对嵌入式开发的喜爱，我选择了单片机！当时我对python十分的感兴趣，但是可以运行python的树莓派太贵了，而且开发套件成本高（树莓派一套开发套件 300 多，单片机才 100 多）。

为什么会选择拿单片机测量酸碱呢，因为我前女友是化工学院的，把酸碱和自动控制结合在一起是一件多美的事情啊！但是……害，祝她能找到更好的~

但是，做单片机的路是很难的，不仅要学C语言，还要了解数电，这些在我的路上都是十分大的阻碍啊！！但是，我可以学习嘛！我相信我可以的喵！！

最后的毕业设计的时候，我看到好多人都比我提前交了毕业设计，有些人是买的，有些人是买了套件直接拼装然后程序一股脑下载进来就可以了。看到周围一点一点做单片机的只有我一个，我的心里压力还很大的……

但是，至少现在，我程序写完了！！！原理图写完了，并且是直播写完的！！而且原理图是可以使用的！我突然有了信心，我可以做完的！

那么接下来，就是面包板的组装和焊接了！我相信我可以的喵！！

2022年4月9日 21:55

*/

#include <reg52.h>

/* 
单片机的输入定义的部分
*/

sbit button1 = P3^0;  //按钮1
sbit button2 = P3^1;  //按钮2
sbit button3 = P3^2;  //按钮3
sbit button4 = P3^3;  //按钮4

sbit led_red = P2^6;  //led 红灯
sbit led_green = P2^7;  //led 绿灯
sbit buzzer = P3^7; //蜂鸣器

sbit seg_contrl_1 = P3^4;  //数码管控制管脚 1
sbit seg_contrl_2 = P3^5;  //数码管控制管脚 2
sbit seg_contrl_3  = P3^6;  //数码管控制管脚 3

sbit adc_clk = P2^0;  //数模转换 —— 时钟
sbit adc_sat = P2^1;  //数模转换 —— 开始转换
sbit adc_eoc = P2^2;  //数模转换 —— 转换完成
sbit adc_oe = P2^3;  //数模转换 —— 导出数据
sbit adc_ale = P2^4;  //数模转换 —— 输入锁存

sbit adc_out_0 = P1^0;  //adc 管脚0
sbit adc_out_1 = P1^1;  //adc 管脚1
sbit adc_out_2 = P1^2;  //adc 管脚2
sbit adc_out_3 = P1^3;  //adc 管脚3
sbit adc_out_4 = P1^4;  //adc 管脚4
sbit adc_out_5 = P1^5;  //adc 管脚5
sbit adc_out_6 = P1^6;  //adc 管脚6
sbit adc_out_7 = P1^7;  //adc 管脚7

#define seg_input_num P0  //数码管输入

/*
字符定义部分
*/

typedef unsigned char u8;
typedef unsigned int u16;

/*
变量定义部分
*/

unsigned int PH_value = 7; // 定义报警值
unsigned char mode1_exit = 0;  // 模式1退出
unsigned char mode2_exit = 0;  // 模式2退出
unsigned char mode3_exit = 0;  // 模式3退出

unsigned int adc_translate_result; // Adc_output 输出变量
unsigned int adc_num_0 , adc_num_1 , adc_num_2 , adc_num_3 , adc_num_4 , adc_num_5 , adc_num_6 , adc_num_7; //位输出函数
double PH_num_double; // PH值小数输出
unsigned int PH_num_int;  // PH值整数输出
unsigned int PH_num_show;  // 酸碱模块输出循环值
unsigned char PH_alarm_mode;  // 报警模式

/*
延时函数
*/

void Timedelay(u16 Timedelay_inputTime)
{
    while(Timedelay_inputTime--)
    {

    }
}

/*
模数转换输出函数
*/

int Adc_output()
{
    // 开始调教AD芯片
    adc_oe = 0;
    adc_clk = 0;
    Timedelay(10);

    adc_clk = 1;
    adc_ale = 1;
    Timedelay(10);

    adc_clk = 0;
    adc_sat = 1;
    Timedelay(10);

    adc_clk = 1;
    adc_ale = 0;
    Timedelay(10);

    adc_clk = 0;
    adc_sat = 0;
    Timedelay(10);

    // 等待模数转换芯片输出结果
    while(adc_eoc == 0)
    {
        adc_clk = 1;
        Timedelay(10);
        adc_clk = 0;
        Timedelay(10);
    }

    // 开始输出
    adc_oe = 1;

    // 第0位转换
    if(adc_out_0 == 0)
    {
        adc_num_0 = 0;
    }
    else
    {
        adc_num_0 = 1;
    }

    // 第1位转换
    if(adc_out_1 == 0)
    {
        adc_num_1 = 0;
    }
    else
    {
        adc_num_1 = 1;
    }

    // 第2位转换
    if(adc_out_2 == 0)
    {
        adc_num_2 = 0;
    }
    else
    {
        adc_num_2 = 1;
    }

    // 第3位转换
    if(adc_out_3 == 0)
    {
        adc_num_3 = 0;
    }
    else
    {
        adc_num_3 = 1;
    }

    // 第4位转换
    if(adc_out_4 == 0)
    {
        adc_num_4 = 0;
    }
    else
    {
        adc_num_4 = 1;
    }

    // 第5位转换
    if(adc_out_5 == 0)
    {
        adc_num_5 = 0;
    }
    else
    {
        adc_num_5 = 1;
    }

    // 第6位转换
    if(adc_out_6 == 0)
    {
        adc_num_6 = 0;
    }
    else
    {
        adc_num_6 = 1;
    }

    // 第7位转换
    if(adc_out_7 == 0)
    {
        adc_num_7 = 0;
    }
    else
    {
        adc_num_7 = 1;
    }

    // 将转换得到的结果按照权重求和
    adc_translate_result = adc_num_0 * 128 + adc_num_1 * 64 + adc_num_2 * 32 + adc_num_3 * 16 + adc_num_4 * 8 + adc_num_5 * 4 + adc_num_6 * 2 + adc_num_7 * 1;
    adc_clk = 1;   
    Timedelay(10);

    // 输出PH值 
    return adc_translate_result;
}

/*
按键消抖函数
*/

int Buttoninputget()
{
    //  获取按钮 1 的值
    if (button1 == 0)
    {
        Timedelay(2000);
        if (button1 == 0)
        {
            while(1)
            {
                if (button1 == 1)
                {
                    return 1;
                }
            }
        }
    }

    //  获取按钮 2 的值
    else if (button2 == 0)
    {
        Timedelay(2000);
        if (button2 == 0)
        {
            while(1)
            {
                if (button2 == 1)
                {
                    return 2;
                }
            }            
        }
    }

    //  获取按钮 3 的值
    else if(button3 == 0)
    {
        Timedelay(2000);
        if (button3 == 0)
        {
            while(1)
            {
                if (button3 == 1)
                {
                    return 3;
                }
            }            
        }
    }

    //  检测到按钮没按
    return 0;
}

/*
显示单片机的值
*/

void Led_show(u16 led_show_num_location , u16 Led_show_input_num)
{
    u8 numberList[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
    seg_input_num = numberList[Led_show_input_num];
    switch (led_show_num_location)
    {
        //  位置1
        case 1:
            seg_contrl_1 = 0;
			seg_contrl_2 = 0;
			seg_contrl_3  = 1;
            break;

        //  位置 2
        case 2:
            seg_contrl_1 = 1;
            seg_contrl_2 = 0;
            seg_contrl_3  = 1;
            break;
        
        //  位置 3
        case 3:
            seg_contrl_1 = 0;
            seg_contrl_2 = 1;
            seg_contrl_3  = 1;
            break;
        
        //  位置 4
        case 4:
            seg_contrl_1 = 1;
            seg_contrl_2 = 1;
            seg_contrl_3  = 1;
        
        default:
            break;
    }
    Timedelay(50);
    seg_input_num = 0x00;
    seg_contrl_1 = 1;
    seg_contrl_2 = 1;
    seg_contrl_3  = 1;
    
}

/*
PH阈值设置模式
*/

void PH_alarm_setting()
{
    unsigned char PH_alarm_setting_exit = 0;
    while (1)
    {
        switch (Buttoninputget())
        {
        case 1:
            PH_alarm_setting_exit = 1;
            break;
        
        //  增加数值
        case 2:
            if(PH_value > 0)
            {
                PH_value = PH_value - 1;
                while (Buttoninputget() == 1)
                {

                }
                break;
            }
            else
            {
                break;
            }
        
        //  减小数值
        case 3:
            if(PH_value < 17)
            {
                PH_value = PH_value + 1;
                while (Buttoninputget() == 1)
                {

                }
                break;
            }
            else
            {
                break;
            }
        
        default:
            break;
        }
    
        // 退出模式
        if(PH_alarm_setting_exit == 1)
        {
            break;
        }
        
        // 输出设置的PH值
        Led_show(3 , PH_value / 10);  // 输出十位的值
        Led_show(4  ,PH_value % 10);  // 输出个位的值

        // 输出开头 “B”
        Led_show(1,11);        
    }
    
}

/*
酸碱模块导出模式
*/

void PH_value_mod()
{
    unsigned char PH_value_mod_exit = 0;
    while (1)
    {
        // 设置 循环初始值
        PH_num_show = 50;

        // 导出PH值
        PH_num_double = Adc_output();

        // 将PH值除以255并乘以70得到一个整数的PH值（这个值）
        PH_num_int = PH_num_double / 255 * 170;

        // 展示输出的PH值
        while (PH_num_show )
        {
            PH_num_show = PH_num_show - 1;
            Led_show(2 , PH_num_int / 100);  // 输出百位
            Led_show(3 , PH_num_int % 100 / 10 );  // 输出十位
            Led_show(4 , PH_num_int % 10); // 输出个位
        }

        // 获取按钮输入
        switch (Buttoninputget())
        {
        case 1:
             PH_value_mod_exit = 1;
            break;
        
        default:
            break;
        }

        // 报警设置
        switch (PH_alarm_mode)
        {

        // 不报警
        case 0:
            break;
        
        // 高于值
        case 1:
            if(PH_num_int >= PH_value * 10)
            {
                buzzer = 1;
                buzzer = 0;
                led_red = 1;
            }
            else
            {
                led_red = 0;
            }
            break;
        
        // 低于值
        case 2:
            if(PH_num_int <= PH_value * 10)
            {
                buzzer = 1;
                buzzer = 0;
                buzzer = 1;
                buzzer = 0;
                buzzer = 1;
                buzzer = 0;
                led_red = 1;
            }
            else
            {
                led_red = 0;
            }
            break;
        
        // 其他？？
        default:
            break;
        }

        // 退出确认
        if(PH_value_mod_exit == 1)
        {
            break;
        }
        
    }
}

/*
报警模式设置
*/

void Alarm_mode()
{
    unsigned char Alarm_mode_exit_num = 0;
    while (1)
    {
        //数值显示
        switch (PH_alarm_mode)
        {
        // 显示无模式
        case 0:
            led_green = 0;
            Led_show(2 , 0);
            Led_show(3 , 0);
            Led_show(4 , 0);
            break;
        
        // 显示大于模式
        case 1:
            led_green = 1;
            Led_show(2 , 1);
            Led_show(3 , 1);
            Led_show(4 , 1);
            break;

        // 显示小于模式
        case 2:
            led_green = 1;
            Led_show(2 , 2);
            Led_show(3 , 2);
            Led_show(4 , 2);
            break;
        
        //  应该不会有这种情况吧……
        default:
            break;
        }


        // 按钮获取
        switch (Buttoninputget())
        {
        // 离开报警设置
        case 1:
            Alarm_mode_exit_num = 1;
            break;
        
        // 切换报警模式
        case 2:
            switch (PH_alarm_mode)
            {
            case 0:
                PH_alarm_mode = 1;
                led_green = 1;
                break;
            
            case 1:
                PH_alarm_mode = 2;
                led_green = 1;
                break;
            
            case 2:
                PH_alarm_mode = 0;
                led_green = 0;
                break;
            
            default:
                break;
            }

        default:
            break;
        }

        // 退出模式
        if(Alarm_mode_exit_num == 1)
        {
            Alarm_mode_exit_num = 0;
            break;
        }
    }    
}

/*
主程序
*/

void main()
{
    /*
    变量初始化
    */
    PH_alarm_mode = 0;
    led_red = 0;
    led_green = 0;


    //  主程序部分
    while(1)
    {
        // 检测模式
        while(1)
        {
            Led_show(1,10);  //  显示字母 A

            switch (Buttoninputget())
            {
                case 1:
                    mode1_exit = 1;
                    break;
                case 2:
                    PH_value_mod();
                default:
                    break;
            }
            
            // 退出检测模式
            if (mode1_exit == 1)
            {
                mode1_exit = 0;
                break;
            }
        }

        // 值设定模式
        while(1)
        {
            Led_show(1,11);  //  显示字母 B
            switch (Buttoninputget())
            {
                case 1:
                    mode2_exit = 1;
                    break;
                case 2:
                    PH_alarm_setting();
                    break;
                default:
                    break;
            }

            // 退出值设定模式
            if (mode2_exit == 1)
            {
                mode2_exit = 0;
                break;
            }
        }

        // 设置报警模式
        while (1)
        {
            Led_show(1,12);  // 显示字母 C

            switch (Buttoninputget())
            {
            case 1:
                mode3_exit = 1;
                break;
            case 2:
                Alarm_mode();
                break;
            default:
                break;
            }

            // 退出报警设置
            if(mode3_exit == 1)
            {
                mode3_exit = 0;
                break;
            }
        }
        
    }
}
