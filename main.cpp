#include "mbed.h"
#include "TextLCD.h"
#include <cstring>



TextLCD 
    lcd(D7, D6, D5, D4, D3, D2, TextLCD::LCD16x2);


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
        thread_sleep_for(170);
        ledsOff();
        thread_sleep_for(170);
    }
}

void waitRelease()
{
    while(!btnUp || !btnDown || !btnSelect || !btnChoose)
        thread_sleep_for(40);
}


// =====================================================
// PAGE ENGINE
// =====================================================

struct Page
{
    char line1[17];
    char line2[17];
};

Page pages[20];
int totalPages = 0;

void clearLine(char* s)
{
    for(int i=0;i<16;i++) s[i] = ' ';
    s[16] = '\0';
}

void buildPages(const char* txt)
{
    totalPages = 0;

    int i = 0;
    int len = strlen(txt);

    while(i < len && totalPages < 20)
    {
        clearLine(pages[totalPages].line1);
        clearLine(pages[totalPages].line2);

        for(int row=0; row<2; row++)
        {
            int col = 0;

            while(i < len)
            {
                while(txt[i] == ' ') i++;

                if(i >= len) break;

                char word[20];
                int w = 0;

                int temp = i;

                while(txt[temp] != ' ' && txt[temp] != '\0')
                {
                    word[w++] = txt[temp++];
                }

                word[w] = '\0';

                if(col + w + (col>0 ? 1:0) > 16)
                    break;

                i = temp;

                char* line =
                (row == 0) ?
                pages[totalPages].line1 :
                pages[totalPages].line2;

                if(col > 0)
                    line[col++] = ' ';

                for(int k=0;k<w;k++)
                    line[col++] = word[k];
            }
        }

        totalPages++;
    }
}

void drawPage(int p)
{
    lcd.cls();

    lcd.locate(0,0);
    lcd.printf("%s", pages[p].line1);

    lcd.locate(0,1);
    lcd.printf("%s", pages[p].line2);
}

// SELECT = exit
// CHOOSE = choose answer
bool readText(const char* txt, bool chooseAllowed)
{
    buildPages(txt);

    int page = 0;

    waitRelease();

    while(true)
    {
        drawPage(page);

        if(!btnDown)
        {
            if(page < totalPages - 1) page++;
            waitRelease();
        }

        if(!btnUp)
        {
            if(page > 0) page--;
            waitRelease();
        }

        if(!btnSelect)
        {
            waitRelease();
            return false;
        }

        if(chooseAllowed && !btnChoose)
        {
            waitRelease();
            return true;
        }

        thread_sleep_for(60);
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
    int correct;
};

Question iq[3] =
{
    {
        "Which city is considered the capital city of the United Kingdom today?",
        "London is the capital city of the United Kingdom.",
        "Paris is the capital city of France and famous worldwide.",
        "Rome is the capital city of Italy with ancient history.",
        0
    },

    {
        "Which animal is the largest land mammal alive in the world now?",
        "Tiger is strong but not the largest land mammal.",
        "Elephant is the largest land mammal alive in the world.",
        "Horse is useful but not the largest animal.",
        1
    },

    {
        "Which colour usually appears in the sky during a clear sunny day?",
        "Blue colour appears in the daytime sky naturally.",
        "Black colour usually appears during night time.",
        "Orange colour appears mostly near sunset hours.",
        0
    }
};


// =====================================================
// FLOW
// =====================================================

void startScreen()
{
    lcd.cls();
    thread_sleep_for(5);

    lcd.locate(7,0);
    lcd.printf("K");
    red = 1;
    thread_sleep_for(1000);
    red = 0;
    
    lcd.locate(7,0);
    lcd.printf("O");
    blue = 1;
    thread_sleep_for(1000);
    blue = 0;
    
    lcd.locate(7,0);
    lcd.printf("M");
    green=1;
    thread_sleep_for(1000);
    green = 0;
    
    lcd.locate(7,0);
    lcd.printf("B");
    yellow = 1;
    thread_sleep_for(1000);
    yellow = 0;
   
    lcd.locate(7,0);
    lcd.printf("A");
    red = 1;
    thread_sleep_for(1000);
    red = 0;

    lcd.locate(7,0);
    lcd.printf("T");
    blue = 1;
    thread_sleep_for(1000);
    blue = 0;
  
    lcd.locate(2,0);
    lcd.printf("Team Kombat");
    
while(true)
{
    red=!red;
    blue=!blue;
    green=!green;
    yellow=!yellow;
    thread_sleep_for(50);

    if(!btnUp || !btnDown || !btnSelect || !btnChoose)
    {
        waitRelease();
        ledsOff();
        break;
    }

    thread_sleep_for(220);
    
}
}

void selectPlayers()
{
    int p = 1;

    while(true)
    {
        lcd.cls();

        lcd.locate(0,0);
        lcd.printf("Players:");

        lcd.locate(0,1);
        lcd.printf("%d", p);

        if(!btnUp)
        {
            if(p<5) p++;
            waitRelease();
        }

        if(!btnDown)
        {
            if(p>1) p--;
            waitRelease();
        }

        if(!btnSelect)
        {
            waitRelease();
            break;
        }

        thread_sleep_for(60);
    }
}

int selectTopic()
{
    const char* topics[5] =
    {
        "Math",
        "Science",
        "History",
        "Sports",
        "IQ"
    };

    int pos = 0;

    while(true)
    {
        setLed(pos % 4);

        lcd.cls();

        lcd.locate(0,0);
        lcd.printf("Choose Topic");

        lcd.locate(0,1);
        lcd.printf("%s", topics[pos]);

        if(!btnDown)
        {
            pos=(pos+1)%5;
            waitRelease();
        }

        if(!btnUp)
        {
            pos=(pos+4)%5;
            waitRelease();
        }

        if(!btnSelect)
        {
            waitRelease();
            ledsOff();
            return pos;
        }

        thread_sleep_for(60);
    }
}


// =====================================================
// ANSWERS
// =====================================================

int chooseAnswer(Question q)
{
    int pos = 0;

    while(true)
    {
        setLed(pos);

        lcd.cls();

        if(pos==0) lcd.printf("A) Sel Read");
        if(pos==1) lcd.printf("B) Sel Read");
        if(pos==2) lcd.printf("C) Sel Read");

        lcd.locate(0,1);
        lcd.printf("D1 Choose");

        if(!btnDown)
        {
            pos=(pos+1)%3;
            waitRelease();
        }

        if(!btnUp)
        {
            pos=(pos+2)%3;
            waitRelease();
        }

        if(!btnSelect)
        {
            waitRelease();

            if(pos==0) readText(q.A,true);
            if(pos==1) readText(q.B,true);
            if(pos==2) readText(q.C,true);
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
// GAME
// =====================================================

void playIQ()
{
    int score = 0;

    for(int i=0;i<3;i++)
    {
        lcd.cls();

        lcd.locate(0,0);
        lcd.printf("Question %d", i+1);

        lcd.locate(0,1);
        lcd.printf("Sel Read");

        while(true)
        {
            if(!btnSelect)
            {
                waitRelease();
                readText(iq[i].q,false);
                break;
            }

            thread_sleep_for(60);
        }

        int ans = chooseAnswer(iq[i]);

        lcd.cls();

        if(ans == iq[i].correct)
        {
            lcd.printf("Correct!");
            score++;
        }
        else
        {
            lcd.printf("Wrong!");
        }

        thread_sleep_for(1400);
    }

    lcd.cls();

    lcd.locate(0,0);
    lcd.printf("Final Score");

    lcd.locate(0,1);
    lcd.printf("%d / 3", score);
}


// =====================================================
// MAIN
// =====================================================

int main()
{
    
    lcd.setCursor(TextLCD::CurOff_BlkOff);
    
    ledsOff();

    startScreen();

    selectPlayers();

    int topic = selectTopic();

    if(topic == 4)
        playIQ();
    else
    {
        lcd.cls();
        lcd.printf("Topic Empty");
    }


    while(true)
    {
    }
}