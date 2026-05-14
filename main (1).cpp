// =====================================================
// TEAM KOMBAT
// MANUAL LCD DRIVER VERSION
// =====================================================

#include "mbed.h"

// =====================================================
// LCD PINS
// =====================================================

DigitalOut rs(D7);
DigitalOut en(D6);

DigitalOut d4(D5);
DigitalOut d5(D4);
DigitalOut d6(D3);
DigitalOut d7(D2);

// =====================================================
// BUTTONS
// =====================================================

DigitalIn btnUp(D10, PullUp);
DigitalIn btnDown(D9, PullUp);
DigitalIn btnSelect(D8, PullUp);
DigitalIn btnChoose(D1, PullUp);

// =====================================================
// LEDS
// =====================================================

DigitalOut red(D13);
DigitalOut blue(D11);
DigitalOut green(D12);
DigitalOut yellow(D0);

// =====================================================
// GAME VARIABLES
// =====================================================

int lives = 3;
int score = 0;

// =====================================================
// LCD LOW LEVEL
// =====================================================

void lcdPulse()
{
    en = 1;

    wait_us(1);

    en = 0;

    wait_us(100);
}

void lcdWrite4(int value)
{
    d4 = value & 1;
    d5 = (value >> 1) & 1;
    d6 = (value >> 2) & 1;
    d7 = (value >> 3) & 1;

    lcdPulse();
}

void lcdSend(int value, int mode)
{
    rs = mode;

    lcdWrite4(value >> 4);
    lcdWrite4(value);

    thread_sleep_for(2);
}

void lcdCommand(int command)
{
    lcdSend(command, 0);
}

void lcdData(int data)
{
    lcdSend(data, 1);
}

void lcdClear()
{
    lcdCommand(0x01);

    thread_sleep_for(2);
}

void lcdLocate(int col, int row)
{
    if(row == 0)
    {
        lcdCommand(0x80 + col);
    }
    else
    {
        lcdCommand(0xC0 + col);
    }
}

void lcdPrint(const char* text)
{
    while(*text)
    {
        lcdData(*text);

        text++;
    }
}

void lcdPrintNumber(int n)
{
    if(n >= 10)
    {
        lcdData((n / 10) + '0');
    }

    lcdData((n % 10) + '0');
}

// =====================================================
// LCD INIT
// =====================================================

void lcdInit()
{
    thread_sleep_for(50);

    rs = 0;
    en = 0;

    for(int i=0;i<3;i++)
    {
        lcdWrite4(0x3);

        thread_sleep_for(5);
    }

    lcdWrite4(0x2);

    lcdCommand(0x28);
    lcdCommand(0x0C);
    lcdCommand(0x06);

    lcdClear();
}

// =====================================================
// CUSTOM HEART
// =====================================================

char heart[8] =
{
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000
};

void createHeart()
{
    lcdCommand(0x40);

    for(int i=0;i<8;i++)
    {
        lcdData(heart[i]);
    }
}

// =====================================================
// QUESTIONS
// =====================================================

struct Question
{
    const char* q;

    const char* A;
    const char* B;
    const char* C;
    const char* D;

    int correct;
};

Question mathQ[25] =
{
    {"5+3=?",   "6",  "8",  "9",  "10", 1},
    {"9-4=?",   "3",  "4",  "5",  "6",  2},
    {"6x2=?",   "10", "11", "12", "14", 2},
    {"15/3=?",  "3",  "4",  "5",  "6",  2},
    {"7+8=?",   "13", "14", "15", "16", 2},

    {"10-6=?",  "2",  "3",  "4",  "5",  2},
    {"9x3=?",   "18", "21", "24", "27", 3},
    {"20/5=?",  "2",  "3",  "4",  "5",  2},
    {"11+9=?",  "18", "19", "20", "21", 2},
    {"14-7=?",  "5",  "6",  "7",  "8",  2},

    {"8x4=?",   "28", "30", "32", "36", 2},
    {"18/2=?",  "7",  "8",  "9",  "10", 2},
    {"13+6=?",  "17", "18", "19", "20", 2},
    {"16-9=?",  "5",  "6",  "7",  "8",  2},
    {"3x5=?",   "10", "12", "15", "18", 2},

    {"25/5=?",  "3",  "4",  "5",  "6",  2},
    {"17+2=?",  "18", "19", "20", "21", 1},
    {"12-8=?",  "2",  "3",  "4",  "5",  2},
    {"4x4=?",   "12", "14", "16", "18", 2},
    {"21/7=?",  "2",  "3",  "4",  "5",  1},

    {"30-10=?", "15", "20", "25", "30", 1},
    {"2x9=?",   "16", "18", "20", "22", 1},
    {"45/5=?",  "7",  "8",  "9",  "10", 2},
    {"50-25=?", "20", "25", "30", "35", 1},
    {"11x2=?",  "20", "21", "22", "24", 2}
};

// =====================================================
// HELPERS
// =====================================================

void ledsOff()
{
    red = 0;
    blue = 0;
    green = 0;
    yellow = 0;
}

void setLed(int n)
{
    ledsOff();

    if(n == 0) red = 1;
    if(n == 1) blue = 1;
    if(n == 2) green = 1;
    if(n == 3) yellow = 1;
}

void blinkLed(int n)
{
    for(int i=0;i<3;i++)
    {
        setLed(n);

        thread_sleep_for(150);

        ledsOff();

        thread_sleep_for(150);
    }
}

void waitRelease()
{
    while(!btnUp || !btnDown || !btnSelect || !btnChoose)
    {
        thread_sleep_for(40);
    }
}

void drawLives()
{
    lcdLocate(13,0);

    for(int i=0;i<lives;i++)
    {
        lcdData(0);
    }

    for(int i=lives;i<3;i++)
    {
        lcdPrint(" ");
    }
}

// =====================================================
// START SCREEN
// =====================================================

void startScreen()
{
    lcdClear();

    lcdLocate(7,0);
    lcdPrint("K");

    red = 1;
    thread_sleep_for(300);
    red = 0;

    lcdLocate(7,0);
    lcdPrint("O");

    blue = 1;
    thread_sleep_for(300);
    blue = 0;

    lcdLocate(7,0);
    lcdPrint("M");

    green = 1;
    thread_sleep_for(300);
    green = 0;

    lcdLocate(7,0);
    lcdPrint("B");

    yellow = 1;
    thread_sleep_for(300);
    yellow = 0;

    lcdLocate(7,0);
    lcdPrint("A");

    red = 1;
    thread_sleep_for(300);
    red = 0;

    lcdLocate(7,0);
    lcdPrint("T");

    blue = 1;
    thread_sleep_for(300);
    blue = 0;

    lcdClear();

    lcdLocate(2,0);
    lcdPrint("Team Kombat");

    while(true)
    {
        red = 1;
        blue = 1;
        green = 1;
        yellow = 1;

        thread_sleep_for(120);

        ledsOff();

        thread_sleep_for(120);

        if(!btnUp || !btnDown || !btnSelect || !btnChoose)
        {
            waitRelease();

            break;
        }
    }
}

// =====================================================
// PLAYER SELECT
// =====================================================

void selectPlayers()
{
    int players = 1;

    while(true)
    {
        lcdClear();

        lcdLocate(0,0);
        lcdPrint("Players");

        lcdLocate(0,1);
        lcdPrintNumber(players);

        setLed(players - 1);

        if(!btnUp)
        {
            if(players < 4)
            {
                players++;
            }

            waitRelease();
        }

        if(!btnDown)
        {
            if(players > 1)
            {
                players--;
            }

            waitRelease();
        }

        if(!btnSelect)
        {
            waitRelease();

            ledsOff();

            break;
        }

        thread_sleep_for(60);
    }
}

// =====================================================
// TOPIC SELECT
// =====================================================

void selectTopic()
{
    const char* topics[3] =
    {
        "Memory",
        "Math",
        "General"
    };

    int pos = 0;

    while(true)
    {
        setLed(pos);

        lcdClear();

        lcdLocate(0,0);
        lcdPrint("Choose Topic");

        lcdLocate(0,1);
        lcdPrint(topics[pos]);

        if(!btnDown)
        {
            pos++;

            if(pos > 2)
            {
                pos = 0;
            }

            waitRelease();
        }

        if(!btnUp)
        {
            pos--;

            if(pos < 0)
            {
                pos = 2;
            }

            waitRelease();
        }

        if(!btnSelect)
        {
            waitRelease();

            if(pos == 1)
            {
                ledsOff();

                break;
            }
        }

        thread_sleep_for(60);
    }
}

// =====================================================
// ANSWER SELECT
// =====================================================

int chooseAnswer(Question q)
{
    int pos = 0;

    while(true)
    {
        setLed(pos);

        lcdClear();

        lcdLocate(0,0);
        lcdPrint(q.q);

        drawLives();

        lcdLocate(0,1);

        if(pos == 0) lcdPrint(q.A);
        if(pos == 1) lcdPrint(q.B);
        if(pos == 2) lcdPrint(q.C);
        if(pos == 3) lcdPrint(q.D);

        if(!btnDown)
        {
            pos++;

            if(pos > 3)
            {
                pos = 0;
            }

            waitRelease();
        }

        if(!btnUp)
        {
            pos--;

            if(pos < 0)
            {
                pos = 3;
            }

            waitRelease();
        }

        if(!btnChoose)
        {
            waitRelease();

            blinkLed(pos);

            return pos;
        }

        thread_sleep_for(60);
    }
}

// =====================================================
// PLAY MATH
// =====================================================

void playMath()
{
    lives = 3;
    score = 0;

    for(int i=0;i<25;i++)
    {
        int ans = chooseAnswer(mathQ[i]);

        lcdClear();

        if(ans == mathQ[i].correct)
        {
            lcdLocate(0,0);
            lcdPrint("Correct");

            green = 1;

            score++;
        }
        else
        {
            lcdLocate(0,0);
            lcdPrint("Wrong");

            red = 1;

            lives--;
        }

        thread_sleep_for(1000);

        ledsOff();

        if(lives <= 0)
        {
            lcdClear();

            lcdLocate(0,0);
            lcdPrint("GAME OVER");

            lcdLocate(0,1);
            lcdPrint("Score:");
            lcdPrintNumber(score);

            while(true)
            {
            }
        }
    }

    lcdClear();

    lcdLocate(0,0);
    lcdPrint("YOU WIN");

    lcdLocate(0,1);
    lcdPrintNumber(score);
}

// =====================================================
// MAIN
// =====================================================

int main()
{
    lcdInit();

    createHeart();

    ledsOff();

    startScreen();

    selectPlayers();

    selectTopic();

    playMath();

    while(true)
    {
    }
}