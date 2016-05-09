#define MOTOR_GO_FORWARD  {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}    //车体前进
#define MOTOR_GO_BACK   {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}    //车体前进
#define MOTOR_GO_RIGHT    {digitalWrite(INPUT1,HIGH);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,HIGH);}    //车体前进
#define MOTOR_GO_LEFT   {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,HIGH);digitalWrite(INPUT3,HIGH);digitalWrite(INPUT4,LOW);}    //车体前进
#define MOTOR_GO_STOP   {digitalWrite(INPUT1,LOW);digitalWrite(INPUT2,LOW);digitalWrite(INPUT3,LOW);digitalWrite(INPUT4,LOW);}    //车体前进

int ledpin = 13;//设置系统启动指示灯
int ENA = 5;//L298使能A
int ENB = 6;//L298使能B
int INPUT2 = 7;//电机接口1
int INPUT1 = 8;//电机接口2
int INPUT3 = 12;//电机接口3
int INPUT4 = 13;//电机接口4

int Left_Speed_Hold = 150;//定义左侧速度变量
int Right_Speed_Hold = 150;//定义右侧速度变量
int Left_Speed[11]={0,50,100,150,165,180,195,210,225,240,255};//左侧速度档位值
int Right_Speed[11]={0,50,100,150,165,180,195,210,225,240,255};//右侧速度档位值

int rec_flag;   //串口接收标志位
int buffer[3];  //串口接收数据缓存
int serial_data; //串口接收的数据
unsigned long cost_time; //串口超时计数

/*
*********************************************************************************************************
** 函数名称 ：init_system()
** 函数功能 ：系统初始化（串口、电机、指示灯等初始化）
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/

void  init_system()
{
    int i;
    for(i=0;i<20;i++) // 延迟40秒等WIFI模块启动完毕
    {
        digitalWrite(ledpin,LOW);
        delay(1000);
        digitalWrite(ledpin,HIGH);
        delay(1000);
    }

    analogWrite(ENB,Left_Speed_Hold);//给L298使能端B赋值
    analogWrite(ENA,Right_Speed_Hold);//给L298使能端A赋值
    Serial.begin(9600);//串口波特率设置为9600 bps
    digitalWrite(ledpin,LOW);
}

/*
*********************************************************************************************************
** 函数名称 ：setup()
** 函数功能 ：初始函数
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/

void setup()
{
    pinMode(ledpin,OUTPUT);
    pinMode(ENA,OUTPUT);
    pinMode(ENB,OUTPUT);
    pinMode(INPUT1,OUTPUT);
    pinMode(INPUT2,OUTPUT);
    pinMode(INPUT3,OUTPUT);
    pinMode(INPUT4,OUTPUT);

    init_system();
}

/*
*********************************************************************************************************
** 函数名称 ：loop()
** 函数功能 ：主函数
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/

void loop()
{
  while(1)
  {
      get_uart_data();
      uart_timeout_check();
   }
}

/*
*********************************************************************************************************
** 函数名称 ：get_uart_data()
** 函数功能 ：读取串口命令
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/
void get_uart_data()
{
    static int i;

    if (Serial.available() > 0) //判断串口缓冲器是否有数据装入
    {
        serial_data = Serial.read();//读取串口
        if(rec_flag==0)
        {
            if(serial_data==0xff)
            {
                rec_flag = 1;
                i = 0;
               cost_time = 0;
            }
        }
        else
        {
            if(serial_data==0xff)
            {
                rec_flag = 0;
                if(i==3)
                {
                    communication_decode();
                }
                i = 0;
            }
            else
            {
                buffer[i]=serial_data;
                i++;
            }
        }
    }
}

/*
*********************************************************************************************************
** 函数名称 ：uart_timeout_check()
** 函数功能 ：串口超时检测
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/

void uart_timeout_check(void)
{
  if(rec_flag == 1)
  {
    cost_time++;
    if(cost_time == 100000)
    {
      rec_flag = 0;
    }
  }
}

/*
*********************************************************************************************************
** 函数名称 ：communication_decode()
** 函数功能 ：串口命令解码
** 入口参数 ：无
** 出口参数 ：无
*********************************************************************************************************
*/

void communication_decode()
{
    if(buffer[0]==0x00) // 电机命令
    {
        switch(buffer[1])
        {
          case 0x00:
            MOTOR_GO_STOP;
            break;
          case 0x01:
            MOTOR_GO_FORWARD;
            break;
          case 0x02:
            MOTOR_GO_BACK;
            break;
          case 0x03:
            MOTOR_GO_LEFT;
            break;
          case 0x04:
            MOTOR_GO_RIGHT;
            break;
          default:
            break;
        }
    }
    else if(buffer[0]==0x01) // 调速命令
    {
      int i = buffer[2]; //
      if(i < 10) { // 速度档位值范围0-10
        switch(buffer[1])
        {
          case 0x00: // 两侧调档
            Right_Speed_Hold=Right_Speed[i] ;
            Left_Speed_Hold=Left_Speed[i] ;
            analogWrite(ENA,Right_Speed_Hold);
            analogWrite(ENB,Left_Speed_Hold);
            break;
          case 0x01: // 左侧调档
            Left_Speed_Hold=Left_Speed[i] ;
            analogWrite(ENB,Left_Speed_Hold);
            break;
          case 0x02: // 右侧调档
            Right_Speed_Hold=Right_Speed[i];
            analogWrite(ENA,Right_Speed_Hold);
            break;
          default:
            break;
        }
      }
    }
    else if(buffer[0]==0x02) // 继电器命令
    {
      /* TODO
      int i = buffer[1]; // 第i号继电器
      switch(buffer[2])
      {
          case 0x00:
            open_relay(i);
            break;
          case 0x01:
            close_relay(i);
            break;
          default:
            break;
      }
      */
    }
}
