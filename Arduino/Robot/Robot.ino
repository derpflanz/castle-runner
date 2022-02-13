const int Motor1_Speed = 3; ///<Motor1 Speed
const int Motor2_Speed = 11;///<Motor2 Speed
const int Motor3_Speed = 5; ///<Motor3 Speed
const int Motor4_Speed = 6; ///<Motor4 Speed

const int Motor1_Direction = 4; ///<Motor1 Direction
const int Motor2_Direction = 12;///<Motor2 Direction
const int Motor3_Direction = 8; ///<Motor3 Direction
const int Motor4_Direction = 7; ///<Motor4 Direction

#define FORWARD     LOW
#define BACKWARD    HIGH

void M1_advance(char Speed) ///<Motor1 Advance
{
    digitalWrite(Motor1_Direction, FORWARD);
    analogWrite(Motor1_Speed, Speed);
}

void M4_advance(char Speed) ///<Motor1 Advance
{
    digitalWrite(Motor4_Direction, FORWARD);
    analogWrite(Motor4_Speed, Speed);
}

void setup() {
    for(int i=3;i<9;i++)
        pinMode(i,OUTPUT);
    for(int i=11;i<13;i++)
        pinMode(i,OUTPUT);
}

void loop() {
    M4_advance(100);

    delay(2000);

    M4_advance(0);

    delay(2000);
}