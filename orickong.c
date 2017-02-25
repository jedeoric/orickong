/*
  This is a port to C of Bertrand Raver's Oric Kong 
  published in Tile magazine.

  Vangelis Rozos Nov. 2012
*/

#define SPEED1 12
#define SPEED2 40
#define SPEED3 30
#define SPEED4 30

#define DEAD  -16
#define COMPLETE 1

#ifdef CC65
#include <telestrat.h>
#include "../oric-common/include/telemon.h"
#define poke(addr,val)     (*(unsigned char*) (addr) = (val))
#include <stdio.h>
#include <string.h>

int fin(void);
#else
#include <lib.h>
#endif

// Declare the assembly code function
void AdvancedPrint(char x_pos,char y_pos,const char *ptr_message);

# define PLOTS(x,y,str) AdvancedPrint( (char) (x), (char) y+1, str)



// Global variables
unsigned char* screen=(unsigned char*)0xBB80;
int nb;                         // Number of lifes
int le;                         // Level
int a1;                         // Jump direction
int a;                          // Direction to move, 1 right, -1 left
int b;                          // Vertical direction to move
int x,y;                        // Coordinates of Mario
int xb, yb;
char bh;                        // Sprite of Mario
char ap;                        // What Mario is infront
char ab;
int ht;
int bo;                         // Bonus
int sc=0;                       // Score
int tt;                         // Number of barels on screen
int ho;
int li;                         // The piste number
int np;                         // Number of removed rivets
int in;
int st;
int t;
char const A[] = "TILT & V.ROZOS sz ORIC KONG";
char hs[9][20]= { "", "MIRTILLE", "+ OLIVIER", "GREGORY PAIC","ALAIN HOQUET",
                   "VINCENTI", "GALIBERT", "VANG", ""};
int  hsi[9]   =  { 0, 6000, 5000, 3000, 2000, 2100, 1000, 100, -1};
char strng[40];                 // Used to convert integers to strings
int rseed=3;

/******************************************************************************
                              WAIT FUNCTION
*******************************************************************************/
void WAIT(int n)
{
    int i_; 
    for(i_=0;i_<n*10;++i_);
}

/******************************************************************************
                              PLOT FUNCTION 
*******************************************************************************/
void PLOT(int x,int y, char car) 
{ 
    screen[x+(y+1)*40]=car;
}

/******************************************************************************
                              SCRN FUNCTION 
*******************************************************************************/
unsigned char SCRN(int x,int y)
{
  return screen[x+(y+1)*40];
}


/******************************************************************************
                                 TIME OUT          
*******************************************************************************/
int timeout()
{
    bo-=100; if (bo<1000) ping();
    sprintf(strng, "%d", bo);
    PLOTS(3,1,"     "); PLOTS(3,1,strng);
    if (bo<0) return DEAD;
    return 0;
}



/******************************************************************************
                              CHAP, PAR, SAC
*******************************************************************************/
void chaparsac()
{
    int i;

    play(7,0,0,0);
    for (i=20; i<=50; ++i) sound(1,i,9);
    play(0,0,0,0);
    PLOTS(x-1,y,"500"); WAIT(50); 
    PLOTS(x-1,y,"   "); sc+=500;
    ap=' ';
    if (li==5000 && st=='p') {PLOT(11,19,'h'); PLOT(13,19,'h');}
}


/******************************************************************************
                                  FALL MUSIC    
*******************************************************************************/
void fallmusic()
{
    int wa,tn;

    tn=5;
    sc+=bo; play(7,0,0,0);
    for (wa=50; wa<=25; wa-=25)
    {
        music(1,tn,1,7); WAIT(wa);
        music(1,tn,5,8); WAIT(wa);
        music(1,tn,8,9); WAIT(wa);
        tn=3;
    }
    play(0,0,0,0); music(1,1,1,0); play(7,0,0,0);
    WAIT(150);

}


/******************************************************************************
                                  PERTE   
*******************************************************************************/
void perte()
{
    int l,i,j=0;
    int cl;

    play(7,0,0,0);
    for (i=11; i>=1; --i)
    {
        music(1,4,i,7); WAIT(10);
        if (j==1) {WAIT(20); j=0; continue; break;}
        j=1;
    }
    WAIT(20); shoot(); WAIT(100);
    a1=0;a=0;b=0;t=0;bh='d';
    --nb; if(nb==0) PLOTS(15,12,"GAME OVER"); 
    else return;
    WAIT(100); 
    if (sc>hsi[7])
    {
        cls(); ink(2);
        for (t=1; t<=7; ++t) if (sc>hsi[t]) break;
        for (l=7; l>=t; --l)
        {
          sprintf(hs[l+1], "%s", hs[l]);
          hsi[l+1]=hsi[l];
        }
        hsi[t]=sc;
        printf("\n\nVOUS VENEZ DE REALISER UN\nDE 7 MEILLEURS SCORES");
        printf("\n\nVOTRE NOM S.V.P:"); gets(strng);
        strng[15]=0;
#ifdef CC65
	    strncpy(hs[t], strng,strlen(strng));
#else
        strncpy(hs[t], strng);
#endif	
    }
    cls(); ink(4);
    printf("%c\n%cN%cQ          --- ORIC KONG ---\n%c\n\n",4,27,27,4);
    cl=1;
    for(i=1; i<=7; ++i) printf("- %d -  %d\n\n", i, hsi[i]);
    for(i=4; i<=16; i+=2) {PLOT(1,i,cl); PLOTS(17,i,hs[cl]); ++cl; }
#ifdef TELEMON
key();
#else
    getchar();
#endif
    nb=6; le=1; li=100; sc=0; t=sc;

}

/******************************************************************************
                             RANDOM NUMBERS GENERATOR
*******************************************************************************/
int rnd(char n)
{
   int rn;

   rn=(52*rseed+11) % 255;
   rseed=rn;
   rn= rn >> n;
   return rn;
}


/******************************************************************************
                                REMOVE RIVET
*******************************************************************************/
int rivet()
{
    int i;

    PLOT(x,y+1,' '); play(7,0,0,0);
    for (i=20; i<=50; ++i) sound(1,i,9); play(0,0,0,0);
    if (in==1) {x+=a1; in=0;};
    PLOT(x,y,ap); x+=a1; ap=SCRN(x,y);
    sc+=100; ++np; if (np==8) { fallmusic(); fin(); return COMPLETE;}

    return 0; 
}


/******************************************************************************
                                      FALL       
*******************************************************************************/
void fall()
{
    int i;

    play(7,0,0,0);
    for(i=y; i<=25; ++i) 
    {
        music(1,0,1,10);
        st=SCRN(x,i+1);
        if (st!=32 && st>7) break;
        PLOT(x,i,'a'); WAIT(5); PLOT(x,i,' ');
    }
    explode();
    PLOT(x,i,'a'); WAIT(100); 

}

/******************************************************************************
                                    JUMP3
*******************************************************************************/
int jump3()
{
    char s, as;
    int i;

    if(bh=='a') return 0;
    if (x==16 && y==13) {x=20; ap='h';}
    else
    {
        PLOT(x,y,ap); as=SCRN(x+a1, y-1);
        if(x==24 && y==14) {ap='h'; x=26; y=15;}
        else
        { 
            PLOT(x+a1,y-1,'a');
            play(7,0,0,0); for (i=40; i<=60; ++i) sound(1,i,8); play(0,0,0,0);
            PLOT(x+a1,y-1,as); x+=2*a1; --y;
            ap=SCRN(x,y);
        } 
    }
    PLOT (x,y,bh); s=SCRN(x,y+1);
    if(s=='b' || s=='h') return 0;
    for (i=0; i<=1; ++i )
    {
        PLOT(x,y,ap); ap=SCRN(x,y+1); ++y, PLOT(x,y,bh); s=SCRN(x,y+1);
        if(s=='b' || s=='h') return 0;
    }
    fall();
    return DEAD; 

}


/******************************************************************************
                                    JUMP   
*******************************************************************************/
int jump()
{
    char as;
    char s;
    char sd;
    int zx;
    int i;

    as=SCRN(x+a1, y-1);
    PLOT(x,y,ap);
    if (li==3000) { zx=x+2*a1; if (zx<6 || zx>29) return 0;}
    PLOT(x+a1,y-1,'a'); WAIT(10);
    play(7,0,0,0); for (i=40; i<=60; ++i) sound(1,i,8); play(0,0,0,0);
    PLOT(x+a1,y-1,as); x+=2*a1;
    s=SCRN(y,x); if (s=='r' || s=='t' || s=='f') return DEAD;
    if (s=='i' || s=='p' || s=='q') {chaparsac(); return 0;}
    ap=SCRN(x,y); PLOT(x,y,bh);
    sd=SCRN(x-a1,y);
    if (sd=='f') {PLOTS(x,y,"100");WAIT(50); PLOTS(x,y,"   "); sc+=100;}
    if (sd=='r' || sd=='t') chaparsac();
    if (SCRN(x-a1,y+1)=='g') {x-=a1; in=1; return rivet();}

    return 0;
}

/******************************************************************************
                                  GET KEY
*******************************************************************************/
int readkeybrd()
{
    switch (key())
    {
      case 'I': return jump();
      case '[': 
#ifdef TELEMON
key();
#else
		getchar();
#endif	
      case 'P': a=1; bh='d'; break;
      case 'O': a=-1; bh='e'; break;
      case 'Q': if (SCRN(x, y-1)=='h') b=-4; break;
      case 'A': if (SCRN(x, y+1)=='h') b= 4; break;
      default: return 0;
    }
    PLOT(x,y,ap);
    x+=a; y+=b; ap=SCRN(x,y);
    if (a!=0) a1=a;
    a=0; b=0;

    return 0;
}


/******************************************************************************
                               GENERATE CHARACTERS
*******************************************************************************/
void generatecharacters()
{
    int i;
    char AD[]={ 12,45,45,63,12,12,18,33, 63,63,4,10,17,32,63,63,
              63,63,49,32,32,49,63,63, 12,12,8,29,46,12,10,17,
              12,12,4,46,29,12,20,34, 0,0,0,28,58,54,46,28,
              63,63,45,45,45,45,63,63, 63,33,63,33,63,33,63,33,
              4,14,31,21,4,4,20,28, 0,0,1,7,15,31,31,28,
              0,63,45,63,51,45,63,51, 0,0,32,56,60,62,62,14,
              12,7,7,3,7,7,7,15, 33,33,51,63,51,33,0,0,
              12,56,56,48,56,56,56,60, 0,2,5,2,4,12,30,63,
              0,12,18,18,30,51,51,63, 0,0,0,4,12,18,63,30,
              12,30,31,31,15,7,3,1, 0,8,42,54,54,34,54,28,
              12,18,35,37,41,49,18,12, 14,8,28,46,12,28,62,20,
              63,63,24,24,24,24,63,63, 32,32,32,32,32,32,32,32,
              63,12,18,33,33,18,12,63, 12,30,62,62,60,56,48,32};

    for (i=0; i<208; ++i) poke(i+46856, AD[i]); 

}


/******************************************************************************
                           PRINT SCORE, LEVEL LIFES 
*******************************************************************************/
void printbonscorliv()
{
    cls(); ink(7);
    PLOTS(3,0,"BONUS:"); PLOTS(16,0,"SCORE:");
    PLOTS(33,0,"L="); sprintf(strng, "%d", le); PLOTS(35,0, strng);
    ///printf("LE=%d LE=%s\n", le, strng); // XXX
    sprintf(strng, "%d", bo);PLOTS(3,1, strng);
    sprintf(strng, "%d", sc);PLOTS(16,1, strng);
}


/******************************************************************************
                             HOW HIGH CAN YOU GET?
*******************************************************************************/
void howhigh()
{
    int i,j;
    int ch,ha;

    cls();ink(1);i=15;play(7,0,0,0);ch=4;
    if (li==11000) {ink(4);ch=3;}
    for (j=1; j<ho+1; ++j)
    {
       ha=j*25;sprintf(strng, "%d", ha);
       PLOTS(16,i,"mno");PLOTS(16,i-1,"jkl");
       PLOTS(20,i,strng);PLOT(24,i,'M');
       i-=2;
    }
    PLOT(1,22,ch);PLOTS(9,22,"JUSQU'OU IREZ-VOUS ?");
    WAIT(30);music(1,4,2,7);WAIT(50);
    music(1,4,7,7);WAIT(25);
    music(1,4,6,7);WAIT(50);
    music(1,4,9,7);WAIT(25);
    music(1,4,7,7);WAIT(25);
    music(1,4,11,7);WAIT(30);
    music(1,5,2,7);WAIT(55);
    ping();
    //WAIT(300);
}

/******************************************************************************
                                  GORILA KILLED
*******************************************************************************/
int fin(void)
{
    int i;

    play(7,0,0,0);
    for(i=10; i<=25;++i) {PLOTS(13,i,"           ");PLOT(12,i,1);PLOT(25,i,4);}
    for(i=23; i<=25;++i) PLOTS(13,i,"cccccccccccx");
    for(i=9; i<=21;++i)
    {
        PLOTS(17,i,"   ");PLOTS(17,i-1,"   ");
        music(1,0,1,10);
        PLOTS(17,i,"jkl");PLOTS(17,i+1,"mno");WAIT(20);
    }
    explode();
    PLOT(20,5,'e'); PLOT(1,3,1);
    for(i=1; i<=5; ++i) {PLOTS(19,3,"  "); WAIT(50); PLOTS(19,3,"sz");WAIT(50);}
    play(7,0,0,0);
    music(1,4,8,9);WAIT(64);
    music(1,4,7,9);WAIT(42);
    music(1,4,2,9);WAIT(20);
    music(1,4,5,9);WAIT(42);
    music(1,4,3,9);WAIT(42);
    music(1,4,1,9);WAIT(42);
    music(1,3,10,9);WAIT(42);
    music(1,3,8,9);WAIT(42);
    music(1,3,12,9);WAIT(20);
    music(1,3,10,9);WAIT(12);
    music(1,3,12,9);WAIT(18);
    music(1,3,10,9);WAIT(50);
    music(1,3,8,9);WAIT(100);
    ping();WAIT(100);++le;t=0;

    return 0;
}

/******************************************************************************
                                  TABLEAU 4
*******************************************************************************/
int tableau4()
{
    int i;
    int c;
    int co=SPEED4;
    char sd, s;
    char ab_[5];
    char xb_[5], yb_[5];

    howhigh(); printbonscorliv(); bo=3000; np=0;
    ink(3); PLOT(1,0,7);
    PLOT(33,3,'d'); PLOT(35,3,nb);
    PLOTS(12,6,"cccccccccccccx"); 
    PLOTS(9,10,"cccgcccccccccccgccc");
    PLOTS(8,14,"ccccgcccccccccccgcccc");
    PLOTS(7,18,"cccccgcccccccccccgccccc");
    PLOTS(6,22,"ccccccgcccccccccccgcccccc");
    for(i=5; i<=32; ++i) PLOT(i,26,'c'); PLOT(32,26,'x');
    for(i=6; i<=26; i+=4) PLOT(1,i,4);
    PLOT(1,5,5); PLOT(18,5,'v');
    for(i=7; i<=9; ++i) {PLOT(1,i,1); PLOT(14,i,'x'); PLOT(23,i,'x');}
    PLOTS(17,9,"mno"); PLOTS(17,8,"jkl");
    for(i=10; i<=13; ++i) {PLOT(9,i,'h'); PLOT(13,i,'h'); PLOT(23,i,'h');
        PLOT(27,i,'h');}
    for(i=14; i<=17; ++i) {PLOT(8,i,'h'); PLOT(18,i,'h'); PLOT(28,i,'h');}
    for(i=18; i<=21; ++i) {PLOT(7,i,'h'); PLOT(14,i,'h'); PLOT(22,i,'h');
        PLOT(29,i,'h');}
    for(i=22; i<=25; ++i) {PLOT(6,i,'h'); PLOT(18,i,'h'); PLOT(30,i,'h');}
    PLOT(9,9,'i'); PLOT(22,25,'q'); PLOT(27,21,'p');
    x=12; y=25; ap=' '; for(i=1; i<=4; ++i) ab_[i]=' ';
    yb_[1]=9; yb_[2]=13; yb_[3]=17; yb_[4]=21;
    bh='d'; PLOT(x,y,bh);
    for (i=1; i<=4; ++i) xb_[i]=21; 
action4:
    if (readkeybrd()==DEAD) return DEAD; ++c; if(c==8){c=0; timeout();}
    s=SCRN(x,y);
    if(s=='i' || s=='p' || s=='q') chaparsac();
    sd=SCRN(x,y+1);
    if(sd==' ') {fall(); return DEAD;}
    if(s=='t') return DEAD;
    if(sd!='g')
    {
        PLOT(x,y,bh);
        --co;
        for(i=1; co==0 && i<=4; ++i)
        {
            s=rnd(7); if(s==0) s=-1; //fire
            PLOT(xb_[i],yb_[i],ab_[i]);
            xb_[i]+=s;
            if (SCRN(xb_[i],yb_[i]+1)==' ') xb_[i]-=s; 
            else
            {
                ab_[i]=SCRN(xb_[i], yb_[i]);
                if (ab_[i]=='d' || ab_[i]=='e') return DEAD;
            }
            PLOT(xb_[i], yb_[i], 't');
        }
        if (co==0) co=SPEED4;
        sprintf(strng, "%d", sc); PLOTS(16,1, strng);
        goto action4;
    }
    PLOT(x,y+1, ' '); play(7,0,0,0);
    for(i=20; i<=50; ++i) sound(1,i,9); play(0,0,0,0);
    if (in==1) {x+=a1; in=0;}
    PLOT(x,y,ap); x+=a1; ap=SCRN(s,y);
    sc+=100; ++np; if(np==8) {fallmusic(); return COMPLETE;}
    goto action4;

}


/******************************************************************************
                                  TABLEAU 3
*******************************************************************************/
int tableau3()
{
    char ar,dO,du,s,sb;
    int co=SPEED3;            // How many scan keyboard times until move rest
    int yr;
    int i;
    int xa[4];
    int ya[4];
    char pressedkey;
    char const pl[]="bbb";
    char const b2[]="bb";
    char const ba[]="ccx";

    x=4;y=22;howhigh();bo=2000;xb=16;yb=5;
    bh='d'; ap=' '; yr=5; ar=' '; ab=' ';
    for (i=1; i<=3; ++i) xa[i]=15;
    ya[1]=14; ya[2]=18; ya[3]=22;
    printbonscorliv();
    PLOT(33,3,'d'); PLOT(35,3,nb);
    ink(3); PLOT(1,0,7);
    for (i=8; i<=24; ++i) {PLOT(7,i,1); PLOT(10,i,3); PLOT(14,i,1);}
    PLOT(1,26,1);
    for(i=4; i<=30; ++i) PLOT(i,26,'b');
    PLOT(1,23,1);
    PLOTS(4,23,pl); PLOTS(18,23,pl);
    PLOT(1,12,1); PLOT(1,18,1);
    PLOTS(4,12,pl); PLOTS(4,18,pl);
    for(i=4; i<=22; ++i) PLOT(i,6,'b'); PLOT(1,6,1);
    for(i=6; i<=9; ++i) PLOT(23,i,'h');
    PLOTS(11,12,"hbh"); PLOTS(11,20,pl); PLOT(10,20,1);
    for(i=13; i<=19; ++i) PLOTS(11,i,"h h"); 
    PLOT(1,11,1); PLOTS(19,11,pl); PLOTS(28,20,pl); 
    for(i=11; i<=13; ++i) PLOT(20,i,'h');
    PLOTS(22,22,b2); PLOTS(25,21,b2); PLOTS(23,15,b2); PLOTS(20,14,b2); 
    PLOTS(23,10,b2);
    PLOTS(26,9,b2); PLOTS(29,8,b2); PLOTS(26,16,b2); PLOTS(29,17,b2);
    for(i=8; i<=12; ++i) PLOT(29,i,'h');
    PLOTS(26,13,"bbbbb");
    for(i=13; i<=15; ++i) PLOT(26,i,'h');
    for(i=18; i<=22; ++i) PLOT(5,i,'h');
    for(i=12; i<=17; ++i) PLOT(6,i,'h');
    for(i=17; i<=19; ++i) PLOT(29,i,'h');
    PLOT(28,8,1); PLOT(25,9,1); PLOT(22,10,1); PLOT(25,13,1); PLOT(25,16,1);
    PLOT(28,17,1);
    PLOT(21,22,1); PLOT(24,21,1); PLOT(22,15,1); PLOT(19,14,1);
    for(i=9; i<=11; ++i) PLOT(28,i,3);
    PLOT(19,12,3);
    PLOT(25,15,3); PLOT(25,14,3);
    PLOTS(8,7,ba); PLOTS(15,7,ba); PLOTS(8,25,ba); PLOTS(15,25,ba);
    PLOTS(6,4,"jkl"); PLOTS(6,5,"mno");
    for(i=3; i<=5;++i) PLOTS(11,i,"h h");
    PLOT(19,4,'h'); PLOT(19,5,'h');
    PLOTS(14,3,"bbbbbh");
    PLOT(12,3,1);
    PLOT(29,7,'q'); PLOT(4,11,'i'); PLOT(12,19,'p');
    PLOT(16,2,'v'); PLOT(x,y,bh);
    PLOT(16,2,'v'); PLOT(x,y,bh);
    for(i=8; i<=24; ++i) {PLOT(9,i,'x'); PLOT(16,i,'x');}
    PLOT(22,8,3); PLOT(22,9,3);
    PLOT(25,18,3); PLOT(25,19,3);
    PLOT(19,13,3); PLOT(22,13,1);
    PLOT(10,12,1); 
action3: 
    dO=SCRN(x,y+1); du=SCRN(x,y-1);
    pressedkey=key();
    switch (pressedkey)
    {
      case '[': {
#ifdef TELEMON
	key();
#else		  
		  getchar();
#endif	  
		  break;}
      case 'P': {if(dO!='h' || ap!='h') {a=1; bh='d';} break;}
      case 'O': {if(dO!='h' || ap!='h') {a=-1; bh='e';} break;}
      case 'Q': {if(du=='h' || ap=='h') {b=-1; bh='a';} break;} 
      case 'A': {if(dO=='h') {b=1; bh='a';} break;}
      case 'I': if(jump3()==DEAD) return DEAD; 
    }
    //PLOT(x,y,ap); 
    if (pressedkey!=0 || x==8 || x==9 || x==15 || x==16) {PLOT(x,y,ap); 
    x+=a; y+=b; ap=SCRN(x,y);}
    if(y==5 && x<15) x=15;
    if(a!=0) a1=a;
    a=0; b=0;
    st=SCRN(x,y); if(st=='i' || st=='p' || st=='q') chaparsac();
    if(st=='t' || st=='y') return DEAD;
    if (pressedkey!=0 || x==8 || x==9 || x==15 || x==16) {PLOT(x,y,bh); 
    st=SCRN(x,y+1);}
    //PLOT(x,y,bh); st=SCRN(x,y+1);
    if(SCRN(x,y-1)=='c') return DEAD;
    if(st==' ' || st<7) {fall(); return DEAD;}
    if(y==2) {fallmusic(); ++ho; li=11000; return COMPLETE;}
    --co; if(co>0) goto action3;
    else co=SPEED3;

    for(i=1; i<=3; ++i) 
    {
        PLOTS(xa[i], ya[i], " x");
        if(xa[i]==8) --ya[i]; else ++ya[i];
        if(ya[i]==8) xa[i]=15;
        if(ya[i]==24 && xa[i]==15) xa[i]=8;
        PLOTS(xa[i], ya[i], "bb");
    }
    if ( (x==15 || x==16) && y>7) {PLOT(x,y,ap); ++y;}
    if (x==8 || x==9) --y;
    if (y>24) return DEAD;
    if (yr==20) {
        yr=5; PLOT(26,20, ' ');
        if (SCRN(23,5)!=' ') {PLOT(23,5,'y'); return DEAD; }
    }
    if (yr==5) {PLOT(23,5,'y'); if (timeout()==DEAD) return DEAD; 
        PLOT(23,5, ' ');}
    PLOT(26,yr,ar); ++yr; ar=SCRN(26,yr);
    PLOT(26,yr,'y');
    if(ar=='a' || ar=='d' || ar=='e') return DEAD;
    sprintf(strng, "%d", sc);
    PLOTS(16,1,strng);

    s=rnd(7); if(s==0) s=-1; //fire
    PLOT(xb,yb,ab);
    xb+=s; if(xb<15 || xb>22) xb-=s;
    sb=SCRN(xb,yb); if(sb!=' ' && sb!='h') return DEAD;
    ab=SCRN(xb,yb);
    PLOT(xb,yb,'t');
    goto action3; 

}


/******************************************************************************
                                  TABLEAU 2
*******************************************************************************/
int tableau2()
{
    int dp, yp;                               // Tarte
    int iy; 
    char sb, s;
    int i;
    int cb=0;                                 // Counter to reduce bonus
    int co=SPEED2;                            // Game speed

    dp=6; yp=21; howhigh();printbonscorliv(); yb=17; xb=17;ab=' ';
    bo=3000;
    PLOT(33,3,'d'); PLOT(35,3,nb);
    ink(7);
    PLOT(1,26,1); PLOT(1,18,1);
    for (i=6; i<=29; ++i) {PLOT(i,26,'b'); PLOT(i,18,'b');}
    PLOT(1,10,3);PLOT(1,14,3);PLOT(1,22,3);
    for(i=6;i<=29;++i) {PLOT(i,10,'w');PLOT(i,14,'w');PLOT(i,22,'w');} 
    PLOT(11,18,' ');PLOT(23,18,' ');
    for(i=14; i<=17; ++i) {PLOT(14,i,'h'); PLOT(21,i,'h');
        PLOT(8,i,'h');PLOT(27,i,'h');}
    for(i=22; i<=25; ++i) {PLOT(14,i,'h');PLOT(21,i,'h');
        PLOT(8,i,'h'); PLOT(27,i,'h');}
    PLOTS(16,14,"u==u"); PLOT(5,14,'u'); PLOT(30,14,'u'); PLOT(5,22,'u');
    PLOT(30,22,'u');
    for(i=18;i<=20;++i) {PLOT(12,i,'h'); PLOT(22,i,'h');}
    PLOT(17,25,'q');PLOT(12,17,'p');PLOT(29,17,'i');
    for(i=11;i<=12;++i) {PLOT(7,i,'h'); PLOT(28,i,'h');}
    PLOT(1,8,1); PLOT(1,9,1); PLOTS(8,8,"jkl"); PLOTS(8,9,"mno");
    PLOT(11,8,7);PLOT(11,9,7);
    for(i=6; i<=9; ++i) {PLOT(12,i,'h'); PLOT(14,i,'h');}
    PLOT(1,5,1); PLOTS(12,5,"b b");
    for(i=8; i<=9; ++i) PLOT(20,i,'h');
    PLOT(13,7,1);PLOTS(15,7,"bbbbbb");
    PLOT(17,6,'v'); PLOTS(17,13,"tt");
    x=10;y=25;bh='d'; PLOT(x,y,bh); ap=' ';
action2: 
    if (readkeybrd()==DEAD) return DEAD;
    s=SCRN(x,y);
    if (s=='i' ||s=='p' || s=='q') chaparsac();
    if (y==9) {fallmusic(); li=5000; return COMPLETE;};
    if (x>29) x=29;
    if (x<6) x=6;
    if (SCRN(x,y+1)==' ') {fall(); return DEAD;}
    if (s=='t') return DEAD;
    PLOT(x,y,bh);
    --co; if (co>0) goto action2;
    co=SPEED2;
    if (y==21 && iy==1) {PLOT(x,y,' '); ++x; iy=0;}
    else if (y==13 && x>18) {PLOT(x,y,' '); --x; iy=0;}
    else if (y==13 && x<17) {PLOT(x,y,' '); ++x; iy=1;}
    else iy=1;
    if (dp>29) {dp=6; yp=21; PLOT(30,21,' ');}
    PLOT(dp,yp,' '); ++dp;
    if (SCRN(dp,yp)!=' ') return DEAD;
    PLOT(dp,yp,'r');
    s=rnd(7); if(s==0) s=-1;
    PLOT(xb,yb,ab);
    xb+=s; if(xb<12 || xb>22) xb-=s;
    sb=SCRN(xb,yb); if(sb!=' ' && sb!='h' && sb!='p') return DEAD;
    ab=SCRN(xb,yb);
    PLOT(xb,yb,'t');
    cb++; if(cb==5) {cb=0; if (timeout()<0) return DEAD;}
    sprintf(strng, "%d", sc);
    PLOTS(16,1, strng);
    goto action2;
}

/******************************************************************************
                                  TABLEAU 1
*******************************************************************************/
int tableau1()
{
  int Xt[4];                // X of barels
  int Yt[4];                // Y of barels
  int At[4];                // Horizontal move step of barels 
  int cp,s;
  int co=SPEED1;            // How many scan keyboard times until move barels
  int i;

    cls; if(le>2) ht=0; else ht=4;
    if(le==1) bo=2000; else bo=3000;
    ho=1; howhigh();x=9;y=25;ap=' ';tt=0;
    printbonscorliv();
    PLOT(x,y,bh);

    for (i=26; i>=6; i-=4)
    {
      PLOT(2,i,1);PLOT(2,i-1,2);PLOT(2,i-2,2);PLOT(2,i-3,2);
      PLOTS(6,i,"bbbbbbbbbbbbbbbbbbbbbbbbb");
    }
    PLOT(2,4,3);PLOT(2,5,3);PLOTS(6,4,"jkl");
    PLOTS(6,5,"mno"); 
    PLOT(9,4,2);PLOT(9,5,2);
    for (i=22; i<26; ++i) PLOT(28,i,'h');
    for (i=18; i<22; ++i) { PLOT(8,i,'h');PLOT(18,i,'h');}
    for (i=14; i<18; ++i) { PLOT(21,i,'h');PLOT(28,i,'h');}
    for (i=10; i<14; ++i) { PLOT(8,i,'h');PLOT(14,i,'h');}
    for (i=6; i<10; ++i) PLOT(28,i,'h');
    for (i=3; i<6; ++i) { PLOT(10,i,'h'); PLOT(12,i,'h');}
    for (i=3; i<6; ++i) PLOT(20,i,'h');
    PLOT(9,2,1);PLOTS(10,2,"bbbbbbbbbbh");
    PLOT(33,3,'d');sprintf(strng,"%d",nb);PLOTS(35,3,strng);

lowlanders:
    PLOT(28,9,'h');PLOT(8,13,'h');PLOT(14,13,'h');PLOT(21,17,'h');
    PLOT(28,17,'h');
    PLOT(8,21,'h');PLOT(18,21,'h');PLOT(28,25,'h');PLOT(20,5,'h');
    if (le>1) {PLOT(21,10,' ');PLOT(17,14,' ');}
action1:
    PLOT(x,y,bh);
    PLOT(14,1,'v');
    sprintf(strng, "%d", sc); PLOTS(16,1,strng);
    a=0;b=0;
    if(tt<3) goto toneaux;
    if (readkeybrd()==DEAD || ap=='f') return DEAD; 
    if (y==5 && x<16) x=16;
    if (SCRN(x,y+1)==32) {fall(); return DEAD; }
    if (x==20 && y==1) 
    {
      fallmusic();
      ho+=1; cls(); ink(7);
      li=3000;
      return COMPLETE; //tableau2();
    }
    if (a!=0) a1=a;
    a=0;b=0;co-=1;
    if(co>0) goto lowlanders;
    else co=SPEED1;

toneaux:
    if (tt==0) 
    {
      //s=(char) (rand(1)*8) + 8; 
      for (t=1; t<=3; ++t) {Xt[t] =15; Yt[t]=5; At[t]=2;}
      tt=1;
    }
    for (t=1; t<=tt; ++t)
    {
      PLOT(x,y,bh);
      PLOT(Xt[t],Yt[t],' ');
      Xt[t]+=At[t];
      PLOT(Xt[t],Yt[t],'f');
      if (Xt[t]==x && Yt[t]==y) return DEAD;
      if( (Yt[t]==25&&Xt[t]<6) || Yt[t]>(y+ht) ) 
      {
          PLOT(Xt[t],Yt[t],' ');Yt[t]=5;Xt[t]=15+rnd(5);At[t]=1;
      }
      if (Xt[t]<6 || Xt[t]>30)
      {
        PLOT(Xt[t],Yt[t],' ');Yt[t]+=4;
        if(timeout()<0) return DEAD;
      }
      if (Xt[t]<6) At[t]=1;
      if (Xt[t]>30) At[t]=-1; 
      if (SCRN(Xt[t],Yt[t]+1)=='h') 
      {
        s=rnd(6); 
        if (s==2) 
        {
          At[t]=-At[t];
          PLOT(Xt[t],Yt[t],' ');Yt[t]+=4;
        }
      }
    }
    if (tt<3) 
    {
      cp+=1;
      if (cp>s) {cp=0; tt+=1; }
    }

    goto action1;
}

/******************************************************************************
                                    MAIN
*******************************************************************************/
void main()
{
    int i;

    x=9; y=25; bh='d'; nb=3; le=1;
    cls();
	paper(0); 
	ink(7);
	zap();
#ifdef TELEMON
#else
    poke(618,10);
#endif	
	printf("\n\n");
#ifdef TELEMON
#else	
	poke(0x20C,255);
#endif
    play(7,0,0,0);
    printf("%c\n", 4);
    printf("\n");
#ifdef TELEMON
#else		
    for (i=0xbb80; i<48040; ++i) poke(i, 20);
#endif
    for (i=0; i<strlen(A); ++i) poke(i+48005, A[i]);
    poke(0xbb93,1); poke(48022,7);
    printf("\n\n\n\n\n\n\n");
    printf("%cN%cC               ORIC KONG", 27, 27);
    printf("%c",4);
    printf("\n\n\n\n\n\n\n\n\n\n");
    PLOTS(2,23,"UN INSTANT S.V.P..."); 
    generatecharacters();
    PLOTS(2,23,"PRESSEZ UNE TOUCHE..."); 
    //for (i=0; i<10; ++i) printf("%d ", (rnd(5))); 
#ifdef TELEMON
	key();
#else		
    getchar();
#endif	
	cls();
    PLOT(1,1,3); PLOTS(11,1,"--- ORIC KONG ---"); ink(4);
    PLOT(1,9,1); 
    li=100;
loop:
    if (li==100 && tableau1()==DEAD) {perte(); goto loop;}
    if (li==3000 && tableau2()==DEAD) {perte(); goto loop;}
    if (li==5000 && tableau3()==DEAD) {perte(); goto loop;}
    if (li==11000) 
    {
       if(tableau4()==DEAD) perte();
       else {fin(); li=100;}
       goto loop;
    }
}