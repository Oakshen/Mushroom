#include <PS2X_lib.h>  //for v1.6

#define PS2_DAT        13  //14    
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        12  //17

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 ******************************************************************/
#define pressures   true    //按键压感打开
#define rumble      true    //电机振动打开


PS2X ps2x; // 创建ps2x类

int leftmotor1 = 2;
int leftmotor2 =3;
int rightmotor1 = 4 ;
int rightmotor2 = 5 ;
int EN1=6;//左电机使能信号PWM
int EN2=9;//右电机使能信号PWM
int pinBuzzer=7;//定义蜂鸣器控制信号为7号管脚
int light=8;//定义LED控制信号为8号管脚

int error = 0;
byte type = 0;
byte vibrate = 0;

// Reset func 
void (* resetFunc) (void) = 0;

void setup(){
  //定义各管脚的输出状态
  pinMode (leftmotor1, OUTPUT );
  pinMode  (leftmotor2, OUTPUT);
  pinMode (rightmotor1, OUTPUT );
  pinMode ( rightmotor2, OUTPUT );
  pinMode (EN1, OUTPUT );
  pinMode ( EN2, OUTPUT );
  pinMode(pinBuzzer, OUTPUT); 
  pinMode(light,OUTPUT);
 
  Serial.begin(115200);//设置串口波特率为115200
  
  delay(500);  //增加了延迟，使无线ps2模块在配置前有时间启动
  
  error = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
  
  if(error == 0){
    Serial.print("Found Controller, configured successful ");
    Serial.print("pressures = ");
	if (pressures)
	  Serial.println("true ");
	else
	  Serial.println("false");
	Serial.print("rumble = ");
	if (rumble)
	  Serial.println("true)");
	else
	  Serial.println("false");
    Serial.println("Try out all the buttons, X will vibrate the controller, faster as you press harder;");
    Serial.println("holding L1 or R1 will print out the analog stick values.");
    Serial.println("Note: Go to www.billporter.info for updates and to report bugs.");
  }  
  else if(error == 1)
    Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
   
  else if(error == 2)
    Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");

  else if(error == 3)
    Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
  
  type = ps2x.readType(); 
  switch(type) {
    case 0:
      Serial.println("Unknown Controller type found ");
      break;
    case 1:
      Serial.println("DualShock Controller found ");
      break;
    case 2:
      Serial.println("GuitarHero Controller found ");
      break;
	case 3:
      Serial.println("Wireless Sony DualShock Controller found ");
      break;
   }

   analogWrite(EN1,0);
   analogWrite(EN2,0);

}

//定义蜂鸣器函数
void buzzer()
{
  digitalWrite(pinBuzzer,HIGH);
  delay(100);
  digitalWrite(pinBuzzer,LOW);
  delay(100);
  digitalWrite(pinBuzzer,HIGH);
  delay(100);
  digitalWrite(pinBuzzer,LOW);
}

void GoStraight()//前进
{
  digitalWrite ( leftmotor1,LOW );
  digitalWrite ( leftmotor2,HIGH );
  digitalWrite ( rightmotor1,LOW);
  digitalWrite ( rightmotor2,HIGH );
  analogWrite ( EN1,115);
  analogWrite ( EN2,100);  
}

void TurnRight()//右转
{
  digitalWrite ( leftmotor1,LOW );
  digitalWrite ( leftmotor2,HIGH );
  digitalWrite ( rightmotor1,HIGH);
  digitalWrite ( rightmotor2,LOW );
  analogWrite (EN1,125);
  analogWrite (EN2,100); 
}

void TurnLeft()//左转
{
  digitalWrite ( leftmotor1,HIGH );
  digitalWrite ( leftmotor2,LOW );
  digitalWrite ( rightmotor1,LOW);
  digitalWrite ( rightmotor2,HIGH);      
  analogWrite (EN1,125);
  analogWrite (EN2,100);
}
void GoBack()//后退
{
  digitalWrite ( leftmotor1,HIGH );
  digitalWrite ( leftmotor2,LOW );
  digitalWrite ( rightmotor1,HIGH);
  digitalWrite ( rightmotor2,LOW );
  analogWrite ( EN1,180);
  analogWrite(EN2,140);
}



void loop() {
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */  
  if(error == 1){ //skip loop if no controller found
    resetFunc();
  }
  
  if(type == 2){ //Guitar Hero Controller
    ps2x.read_gamepad();          //read controller 
   
    if(ps2x.ButtonPressed(GREEN_FRET))
      Serial.println("Green Fret Pressed");
    if(ps2x.ButtonPressed(RED_FRET))
      Serial.println("Red Fret Pressed");
    if(ps2x.ButtonPressed(YELLOW_FRET))
      Serial.println("Yellow Fret Pressed");
    if(ps2x.ButtonPressed(BLUE_FRET))
      Serial.println("Blue Fret Pressed");
    if(ps2x.ButtonPressed(ORANGE_FRET))
      Serial.println("Orange Fret Pressed"); 

    if(ps2x.ButtonPressed(STAR_POWER))
      Serial.println("Star Power Command");
    
    if(ps2x.Button(UP_STRUM))          //will be TRUE as long as button is pressed
      Serial.println("Up Strum");
    if(ps2x.Button(DOWN_STRUM))
      Serial.println("DOWN Strum");
 
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");
    
    if(ps2x.Button(ORANGE_FRET)) {     // print stick value IF TRUE
      Serial.print("Wammy Bar Position:");
      Serial.println(ps2x.Analog(WHAMMY_BAR), DEC); 
    } 
  }
  else { //DualShock Controller
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))         //will be TRUE as long as button is pressed
      Serial.println("Start is being held");
    if(ps2x.Button(PSB_SELECT))
      Serial.println("Select is being held");      

    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
      Serial.print("Up held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_UP), DEC);
      GoStraight();
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
      Serial.print("Right held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_RIGHT), DEC);
      TurnRight();   
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
      Serial.print("LEFT held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_LEFT), DEC);
      TurnLeft();
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
      Serial.print("DOWN held this hard: ");
      Serial.println(ps2x.Analog(PSAB_PAD_DOWN), DEC);
      GoBack();
    } 

      if(ps2x.Button(PSB_PAD_UP)&&ps2x.Button(PSB_CROSS)) //按下X键加速前进
      {
       digitalWrite ( leftmotor1,LOW );
       digitalWrite ( leftmotor2,HIGH );
       digitalWrite ( rightmotor1,LOW);
       digitalWrite ( rightmotor2,HIGH );
       analogWrite ( EN1,255);
       analogWrite ( EN2,230);
       }

    if(ps2x.Button(PSB_PAD_DOWN)&&ps2x.Button(PSB_R2))//按下X键加速后退
      {
     

      digitalWrite ( leftmotor1,HIGH );
      digitalWrite ( leftmotor2,LOW );
      digitalWrite ( rightmotor1,HIGH);
      digitalWrite ( rightmotor2,LOW );
      analogWrite ( EN1,220);
      analogWrite(EN2,180);
      } 

    

vibrate = ps2x.Analog(PSAB_CROSS);//根据按下X按钮的力度设置大电机振动速度
if (ps2x.NewButtonState()) 
{        //will be TRUE if any button changes state (on to off, or off to on)
   analogWrite(EN1,0);//每次按下按键时让使能信号为0
   analogWrite(EN2,0);
}

if(ps2x.ButtonPressed(PSB_TRIANGLE))
{
  buzzer();
  Serial.println("Triangle pressed");
}
if(ps2x.ButtonPressed(PSB_CIRCLE))
{
  digitalWrite(light,HIGH);
  Serial.println("Triangle pressed");
}
if(ps2x.ButtonPressed(PSB_SQUARE))              
{
  digitalWrite(light,LOW);
}
if(ps2x.ButtonPressed(PSB_L1))              
{
  digitalWrite(pinBuzzer,HIGH);
}
if(ps2x.ButtonPressed(PSB_R1))              
{
  digitalWrite(pinBuzzer,LOW);
}
      
}
  delay(50);  
}
