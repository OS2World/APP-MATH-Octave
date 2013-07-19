/* written by Stefan Mueller stm@fgan.de 1996 */

/* scaleimg is a filter */
/* the input on stdin is a output of pnmtops image postscript-file */
/* the ouput on stdout is a eps file with scaled image */
#include <stdio.h>
#define TEXT_LENGTH 256
#define SCALE_FONT_SIZE 10 

const char * h1="%%!PS-Adobe-2.0 EPSF-2.0\n";
const char * h2="%%%%Creator: scaleimg stm@fgan.de 1996\n";
const char * h3="%%%%DocumentFonts: Helvetica Times-Roman\n";
const char * h4="%%%%BoundingBox: %d %d %d %d\n";
const char * h5="%%%%EndComments\n";
const char * h6="/readstring {\n";
const char * h7="currentfile exch readhexstring pop\n";
const char * h8="} bind def\n";
const char * h9="/picstr %d string def\n";
const char * h10="/rpicstr %d string def\n";
const char * h11="/gpicstr %d string def\n";
const char * h12="/bpicstr %d string def\n";
const char * h13="%%%%EndProlog\n";
const char * mt="%d %d moveto\n";
const char * mtf="%1.1f %1.1f moveto\n";
const char * rmt="%d %d rmoveto\n";
const char * lt="%d %d lineto\n";
const char * rlt="%d %d rlineto\n";
const char * np="newpath\n";
const char * cp="closepath\n";
const char * sh="(%s) show\n";
const char * shc="(%s) dup stringwidth pop dup 2 div sub neg 0 rmoveto show\n";
const char * shr="(%s) dup stringwidth pop neg 0 rmoveto show\n";
const char * slw="%d setlinewidth\n";
const char * st="stroke\n";
const char * sp="showpage\n";
const char * gs="gsave\n";
const char * gr="grestore\n";
const char * sc="%1.2f %1.2f scale\n";
const char * tl="%d %d translate\n";
const char * rt="%d rotate\n";
const char * f1="/Helvetica findfont %d scalefont setfont\n";
const char * f2="/Times-Roman findfont %d scalefont setfont\n";
const char *suchScale="cale\n";
const char *suchTranslate="late\n";
const char *suchGrestore="tore\n";

void error()
  {
  fprintf(stderr,"Usage: scaleimg <options>\n");
  fprintf(stderr,"options:\n");
  fprintf(stderr,"    -fw <size>            frame size on the westside of image\n");
  fprintf(stderr,"    -fe <size>            frame size on the eastside of image\n");
  fprintf(stderr,"    -fn <size>            frame size on the northside of image\n");
  fprintf(stderr,"    -fs <size>            frame size on the southside of image\n");
  fprintf(stderr,"    -ix <position>        x position of sw-corner of image\n");
  fprintf(stderr,"    -iy <position>        y position of sw-corner of image\n");
  fprintf(stderr,"    -iw <size>            width of image\n");
  fprintf(stderr,"    -ih <size>            height image\n");
  fprintf(stderr,"    -x0 <value>           start value of x scale\n");
  fprintf(stderr,"    -xn <value>           end value of x scale\n");
  fprintf(stderr,"    -y0 <value>           start value of y scale\n");
  fprintf(stderr,"    -yn <value>           end value of y scale\n");
  fprintf(stderr,"    -ls <text>            text of south-label (x-axis)\n");
  fprintf(stderr,"    -lw <text>            text of west-label (y-axis)\n");
  fprintf(stderr,"    -ln <text>            text of north-label\n");
  fprintf(stderr,"    -le <text>            text of east-label\n");
  fprintf(stderr,"    -lt <text>            text of title-label\n");
  fprintf(stderr,"    -ll <text>            text of legend-label\n");
  exit(0);
  }

void drawFrame(int x,int y,int width,int height,int lineWidth)
  {
  printf(np);
  printf(mt,x,y);
  printf(rlt,width,0);
  printf(rlt,0,height);
  printf(rlt,-width,0);
  printf(rlt,0,-height);
  printf(cp);
  printf(slw,lineWidth);
  printf(st);
  }

void getTicDistance(float axisLength,short maxTics,float *size) 
{
float currentTics; 
float factor[3]; 
float exponent; 
int i;

if (maxTics<2) maxTics=2;
factor[0]=5.0;
factor[1]=2.0;
factor[2]=1.0;
exponent=1.0;
if (axisLength<0) axisLength*=-1.0;
while (axisLength>=1.0)
  {
  axisLength/=10.0;
  exponent*=10.0;
  }
i=2;
do
  {
  *size=factor[i];
  i++;
  if (i>2) i=0;
  factor[i]/=10.0;
  currentTics=axisLength/factor[i];
  }
while (currentTics<=maxTics);
*size*=exponent;
}

void drawSouthScale(int x,int y,int length,float
  startValue,float endValue,int xValueFormat)
  {
  float xFac;
  float sign;
  float xValue;
  float deltaXTic;
  float deltaXLabel;
  float startEndDiff;
  float currentX;
  float currentY;
  char text[TEXT_LENGTH];
  char form[TEXT_LENGTH];
  int i;
  
  startEndDiff=endValue-startValue;
  if (startEndDiff<0) sign=-1.0;
    else sign=1.0;
  getTicDistance(sign*startEndDiff,7,&deltaXLabel); 
  xFac=length/startEndDiff;
  deltaXTic=sign*deltaXLabel/5;
  xValue=startValue;
  i=0;
  currentY=y;
  while (sign*xValue<sign*(endValue+deltaXTic/10))
    {
    currentX=x+xFac*(xValue-startValue);
    if (i%5)
      {
      /* short tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,0,-5);
      printf(slw,1);
      printf(st);
      }
    else 
      {
      /* long tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,0,-10);
      printf(slw,1);
      printf(rmt,0,-11);
      printf(f1,SCALE_FONT_SIZE);
      sprintf(form,"%%1.%df",xValueFormat);
      sprintf(text,form,xValue);
      printf(shc,text);
      printf(st);
      }
      
    xValue+=deltaXTic;
    i++;
    }
  }

void drawNorthScale(int x,int y,int length,
  float startValue,float endValue,int xValueFormat)
  {
  float xFac;
  float sign;
  float xValue;
  float deltaXTic;
  float deltaXLabel;
  float startEndDiff;
  float currentX;
  float currentY;
  char text[TEXT_LENGTH];
  char form[TEXT_LENGTH];
  int i;
  
  startEndDiff=endValue-startValue;
  if (startEndDiff<0) sign=-1.0;
    else sign=1.0;
  getTicDistance(sign*startEndDiff,7,&deltaXLabel); 
  xFac=length/startEndDiff;
  deltaXTic=sign*deltaXLabel/5;
  xValue=startValue;
  i=0;
  currentY=y;
  while (sign*xValue<sign*(endValue+deltaXTic/10))
    {
    currentX=x+xFac*(xValue-startValue);
    if (i%5)
      {
      /* short tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,0,5);
      printf(slw,1);
      printf(st);
      }
    else 
      {
      /* long tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,0,10);
      printf(slw,1);
      printf(rmt,0,3);
      printf(f1,SCALE_FONT_SIZE);
      sprintf(form,"%%1.%df",xValueFormat);
      sprintf(text,form,xValue);
      printf(shc,text);
      printf(st);
      }
      
    xValue+=deltaXTic;
    i++;
    }
  }

void drawWestScale(int x,int y,int length,
  float startValue,float endValue,int yValueFormat)
  {
  float yFac;
  float sign;
  float yValue;
  float deltaYTic;
  float deltaYLabel;
  float startEndDiff;
  float currentX;
  float currentY;
  char text[TEXT_LENGTH];
  char form[TEXT_LENGTH];
  int i;
  
  startEndDiff=endValue-startValue;
  if (startEndDiff<0) sign=-1.0;
    else sign=1.0;
  getTicDistance(sign*startEndDiff,7,&deltaYLabel); 
  yFac=length/startEndDiff;
  deltaYTic=sign*deltaYLabel/5;
  yValue=startValue;
  i=0;
  currentX=x;
  while (sign*yValue<sign*(endValue+deltaYTic/10))
    {
    currentY=y+yFac*(yValue-startValue);
    if (i%5)
      {
      /* short tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,-5,0);
      printf(slw,1);
      printf(st);
      }
    else 
      {
      /* long tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,-10,0);
      printf(slw,1);
      printf(rmt,-3,-3); 
      printf(f1,SCALE_FONT_SIZE);
      sprintf(form,"%%1.%df",yValueFormat);
      sprintf(text,form,yValue);
      printf(shr,text); 
      printf(st);
      }
      
    yValue+=deltaYTic;
    i++;
    }
  }

void drawEastScale(int x,int y,int length,
  float startValue,float endValue,int yValueFormat)
  {
  float yFac;
  float sign;
  float yValue;
  float deltaYTic;
  float deltaYLabel;
  float startEndDiff;
  float currentX;
  float currentY;
  char text[TEXT_LENGTH];
  char form[TEXT_LENGTH];
  int i;
  
  startEndDiff=endValue-startValue;
  if (startEndDiff<0) sign=-1.0;
    else sign=1.0;
  getTicDistance(sign*startEndDiff,7,&deltaYLabel); 
  yFac=length/startEndDiff;
  deltaYTic=sign*deltaYLabel/5;
  yValue=startValue;
  i=0;
  currentX=x;
  while (sign*yValue<sign*(endValue+deltaYTic/10))
    {
    currentY=y+yFac*(yValue-startValue);
    if (i%5)
      {
      /* short tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,5,0);
      printf(slw,1);
      printf(st);
      }
    else 
      {
      /* long tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,10,0);
      printf(slw,1);
      printf(rmt,3,-3);
      printf(f1,SCALE_FONT_SIZE);
      sprintf(form,"%%1.%df",yValueFormat);
      sprintf(text,form,yValue);
      printf(sh,text);
      printf(st);
      }
      
    yValue+=deltaYTic;
    i++;
    }
  }

void drawLegendScale(int x,int y,int length,
  int nColor,float startValue,float endValue,int yValueFormat)
  {
  float yFac;
  float sign;
  float yValue;
  float deltaYTic;
  float deltaYLabel;
  float startEndDiff;
  float currentX;
  float currentY;
  char text[TEXT_LENGTH];
  char form[TEXT_LENGTH];
  int i;
  
  startEndDiff=endValue-startValue;
  if (startEndDiff<0) sign=-1.0;
    else sign=1.0;
  getTicDistance(sign*startEndDiff,7,&deltaYLabel); 
  deltaYLabel=sign*startEndDiff/nColor;
  deltaYTic=sign*deltaYLabel/2;
  yFac=length/startEndDiff;
  yValue=startValue;
  i=0;
  currentX=x;
  while (sign*yValue<sign*(endValue+deltaYTic/10))
    {
    currentY=y+yFac*(yValue-startValue);
    if (i%2)
      {
      /* label */
      printf(mtf,currentX,currentY);
      printf(rmt,3,-3);
      printf(f1,SCALE_FONT_SIZE);
      if (sign>0) sprintf(form,">%%1.%df",yValueFormat);
      else        sprintf(form,"<%%1.%df",yValueFormat); 
      sprintf(text,form,yValue-deltaYTic);
      printf(sh,text);
      }
    else 
      {
      /* long tics */
      printf(np);
      printf(mtf,currentX,currentY);
      printf(rlt,10,0);
      printf(slw,1);
      printf(st);
      }
      
    yValue+=deltaYTic;
    i++;
    }
  }
      
void  printLabel(int x,int y,int rotation,char *text,int size)
  {
  printf(mt,x,y);
  printf(f2,size);
  printf(rt,rotation); 
  printf(shc,text);
  printf(st);
  printf(rt,-rotation); 
  }
  
  

main(int argc,char **argv)
  {
  int frameSizeWest=0;
  int frameSizeEast=0;
  int frameSizeNorth=0;
  int frameSizeSouth=0;
  int imagePosX=55;
  int imagePosY=200;
  int imageWidth=350;
  int imageHeight=350;
  float xStartValue=-100.0;
  float xEndValue=100.0;
  int xValueFormat;
  float yStartValue=-100.0;
  float yEndValue=100.0;
  int yValueFormat;
  char labelSouth[TEXT_LENGTH]="";
  char labelWest[TEXT_LENGTH]="";
  char labelEast[TEXT_LENGTH]="";
  char labelNorth[TEXT_LENGTH]="";
  char labelTitle[TEXT_LENGTH]="";
  char labelLegend[TEXT_LENGTH]="";
  int imageNCols;
  int imageNRows;
  int imageDepth;
  char c;
  int i,j;
  int scan;
  if (argc==1) error();
  if (argc%2==0) error();  
  for (i=1;i<argc;i+=2) {
    if (!strcmp("-fw",argv[i])) 
      if (1!=sscanf(argv[i+1],"%d",&frameSizeWest)) error();
      else;
    else
      if (!strcmp("-fe",argv[i])) 
        if (1!=sscanf(argv[i+1],"%d",&frameSizeEast)) error();
        else;
      else
        if (!strcmp("-fn",argv[i]))
          if (1!=sscanf(argv[i+1],"%d",&frameSizeNorth)) error();
          else;
        else
          if (!strcmp("-fs",argv[i]))
            if (1!=sscanf(argv[i+1],"%d",&frameSizeSouth)) error();
            else;
          else
            if (!strcmp("-ix",argv[i])) 
              if (1!=sscanf(argv[i+1],"%d",&imagePosX)) error();
              else;
            else
              if (!strcmp("-iy",argv[i])) 
                if (1!=sscanf(argv[i+1],"%d",&imagePosY)) error();
                else;
              else
                if (!strcmp("-iw",argv[i]))
                  if (1!=sscanf(argv[i+1],"%d",&imageWidth)) error();
                  else;
                else
                  if (!strcmp("-ih",argv[i]))
                    if (1!=sscanf(argv[i+1],"%d",&imageHeight)) error();
                    else;
                   else
                     if (!strcmp("-x0",argv[i]))
                       if (1!=sscanf(argv[i+1],"%f",&xStartValue)) error();
                       else {
                         j=0;
                         while((argv[i+1][j]!='.')&&(argv[i+1][j]!='\0'))
                           j++;
                         xValueFormat=0;
                         if (argv[i+1][j]=='.') j++;
                         while(argv[i+1][j]!='\0') {
                           xValueFormat++;
                           j++;
                           }
                         } 
                     else
                       if (!strcmp("-xn",argv[i]))
                         if (1!=sscanf(argv[i+1],"%f",&xEndValue)) error();
                         else;
                       else
                         if (!strcmp("-y0",argv[i]))
                           if (1!=sscanf(argv[i+1],"%f",&yStartValue)) error();
                           else {
                             j=0;
                             while((argv[i+1][j]!='.')&&(argv[i+1][j]!='\0'))
                               j++;
                             yValueFormat=0;
                             if (argv[i+1][j]=='.') j++;
                             while(argv[i+1][j]!='\0') {
                               yValueFormat++;
                               j++;
                               }
                             } 
                         else
                           if (!strcmp("-yn",argv[i]))
                             if (1!=sscanf(argv[i+1],"%f",&yEndValue)) error();
                             else; 
                           else
                             if (!strcmp("-lw",argv[i]))
                               strcpy(labelWest,argv[i+1]);
                             else
                               if (!strcmp("-le",argv[i]))
                                 strcpy(labelEast,argv[i+1]);
                               else
                                 if (!strcmp("-ln",argv[i]))
                                   strcpy(labelNorth,argv[i+1]);
                                 else
                                   if (!strcmp("-ls",argv[i]))
                                     strcpy(labelSouth,argv[i+1]);
                                   else
                                     if (!strcmp("-lt",argv[i]))
                                       strcpy(labelTitle,argv[i+1]);
                                     else
                                       if (!strcmp("-ll",argv[i]))
                                         strcpy(labelLegend,argv[i+1]);
    } /* of for */

  scan=1;

  while (scan)
    {
    c=getchar(); 
    if(c==suchScale[0]) {
      c=getchar(); 
      if(c==suchScale[1]) {
        c=getchar(); 
        if(c==suchScale[2]) {
          c=getchar(); 
          if(c==suchScale[3]) {
            c=getchar(); 
            if(c==suchScale[4]) {
              scan=0;
              }
            }
          }
        }
      }
    }      
  scanf("%d",&imageNCols);
  if (imageNCols==0)
    {
    do c=getchar(); while (c!='\n');
    scanf("%d %d %d\n",&imageNCols,&imageNRows,&imageDepth);
    }
  else
    {
    scanf(" %d %d\n",&imageNRows,&imageDepth);
    }
  /* print head */
  printf(h1);
  printf(h2);
  printf(h4,imagePosX-frameSizeWest,imagePosY-frameSizeSouth,
   (int)(imagePosX+imageWidth+frameSizeEast),
   (int)(imagePosY+imageHeight+frameSizeNorth));
  printf(h5);
  printf(h6);
  printf(h7);
  printf(h8);
  printf(h9,imageNCols);
  printf(h10,imageNCols);
  printf(h11,imageNCols);
  printf(h12,imageNCols);
  printf(h13);
  printf(gs);
  printf(tl,imagePosX,imagePosY);
  printf(sc,(float)imageWidth,(float)imageHeight);
  printf("%d %d %d\n",imageNCols,imageNRows,imageDepth);

  scan=1;
  while (scan)
    {
    c=getchar(); 
    putchar(c);
    if(c==suchGrestore[0]) {
      c=getchar(); 
      putchar(c);
      if(c==suchGrestore[1]) {
        c=getchar(); 
        putchar(c);
        if(c==suchGrestore[2]) {
          c=getchar(); 
          putchar(c);
          if(c==suchGrestore[3]) {
            c=getchar(); 
            putchar(c);
            if(c==suchGrestore[4]) {
              scan=0;
              }
            }
          }
        }
      }
    }      
  drawFrame(imagePosX,imagePosY,imageWidth,imageHeight,1);
  if (strcmp(labelSouth,""))
    {
    drawSouthScale(imagePosX,imagePosY,imageWidth,
      xStartValue,xEndValue,xValueFormat);
    if (strcmp(labelSouth,"noLabel"))
      {
      printLabel(imagePosX+imageWidth/2,imagePosY-35,
        0,labelSouth,12);
      }
    }
  if (strcmp(labelNorth,""))
    {
    drawNorthScale(imagePosX,imagePosY+imageHeight,imageWidth,
      xStartValue,xEndValue,xValueFormat);
    if (strcmp(labelNorth,"noLabel"))
      {
      printLabel(imagePosX+imageWidth/2,imagePosY+imageHeight+25,
        0,labelNorth,12);
      }
    }
  if (strcmp(labelWest,""))
    {
    drawWestScale(imagePosX,imagePosY,imageHeight,
      yStartValue,yEndValue,yValueFormat);
    if (strcmp(labelWest,"noLabel"))
      {
      printLabel(imagePosX-40,imagePosY+imageHeight/2,
        90,labelWest,12);
      }
     }
  if (strcmp(labelEast,""))
    {
    drawEastScale(imagePosX+imageWidth,imagePosY,imageHeight,
      yStartValue,yEndValue,yValueFormat);
    if (strcmp(labelEast,"noLabel"))
      {
      printLabel(imagePosX+imageWidth+47,imagePosY+imageHeight/2,
        90,labelEast,12);
      }
    }
  if (strcmp(labelLegend,""))
    {
    drawLegendScale(imagePosX+imageWidth,imagePosY,imageHeight,
      imageNRows,yStartValue,yEndValue,yValueFormat);
    printLabel(imagePosX+imageWidth+45,imagePosY+imageHeight/2,
      90,labelLegend,12);
     }
  if (strcmp(labelTitle,""))
    {
    printLabel(imagePosX+imageWidth/2,imagePosY+imageHeight+40,
      0,labelTitle,16);
    }
  }
