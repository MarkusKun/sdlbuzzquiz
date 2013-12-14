#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>

#include <math.h> // for sqrt
#include <SDL.h>
#include <SDL_ttf.h>

#include "buzzer.h"

struct player{
  std::string playerName;
  unsigned int responseTime; // milliseconds
  unsigned int givenAnswer; // 0: no answer given yet
  unsigned int sumPoints; // sum of points
};


/*
 * 0,0     ->      x    ->   screenWidth
 *
 *  |
 *  v  
 *
 *  y
 *  
 *  |
 *  v
 *
 * screenHeight
 */

// todo: map of enums to SDL_Rect?
struct screenTiling{
  SDL_Rect questionArea;
  SDL_Rect answerArea[4];
  SDL_Rect answerColorArea[4];
  SDL_Rect imageArea;
  SDL_Rect timerArea;
  SDL_Rect playerArea;
};


void buzz_identify(int givenButton){
  using std::cout; using std::endl;
  unsigned int buzzer_number =  givenButton/5;
  unsigned int buzzer_button =  givenButton%5;
  
  cout << givenButton << " -> Buzzer: " << buzzer_number << " ";
  switch (buzzer_button){
    case BUZZ_RED:
      {
        cout << "Big Red Button on top";
        break;
      }
    case BUZZ_YELLOW:
      {
        cout << "Yellow";
        break;
      }
    case BUZZ_GREEN:
      {
        cout << "Green";
        break;
      }
    case BUZZ_ORANGE:
      {
        cout << "Orange";
        break;
      }
    case BUZZ_BLUE:
      {
        cout << "Blue";
        break;
      }
    default:
      {
        cout << "SOMETHING WRONG!!!";
        break;
      }
  } // switch
  cout << endl;
  
}

void drawSprite(SDL_Surface* target, SDL_Surface* sprite, unsigned int x, unsigned int y){
  { // draw eevee on screen?
    SDL_Rect DestR;

    { // destination / where to draw
      DestR.x = x;
      DestR.y = y;
    }

    SDL_Rect SrcR;
    { // source rectangle - if not all should be printed

      SrcR.x = 5;
      SrcR.y = 5;
      SrcR.w = 5;
      SrcR.h = 5;
    }
    
    SDL_BlitSurface(sprite, NULL, target, &DestR);
  }
}

Uint32 timeoutCallBack(Uint32 interval, void *param){
  static unsigned int passedSeconds=0;
  passedSeconds++;
  //std::cout << "A Second has passed!" << std::endl;

  SDL_Event event;
  SDL_UserEvent userevent;
  
  /* In this example, our callback pushes an SDL_USEREVENT event
  into the queue, and causes ourself to be called again at the
  same interval: */
  
  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = NULL;
  userevent.data2 = NULL;
  
  event.type = SDL_USEREVENT;
  event.user = userevent;
  
  SDL_PushEvent(&event);
 
 
 
  return(interval);

}

void printJoystickInformation(std::ostream& outStream){
  using std::endl;
  int numOfJoys = SDL_NumJoysticks ();
  outStream << "number of Joysticks: " << numOfJoys << endl;
  {
    unsigned int joy_iterator;
    for (
      joy_iterator  = 0;
      joy_iterator  < numOfJoys;
      joy_iterator++
      )
    {

      outStream << "Joystick " << joy_iterator << ": ";
      SDL_Joystick* myJoystick =SDL_JoystickOpen (joy_iterator);
      
      if (NULL == myJoystick){
        outStream << "Cant open joystick: " << SDL_GetError() << endl;
        exit (1);
      }
      
      outStream << SDL_JoystickName (joy_iterator) << endl;
      outStream << "Knoepfe: " << SDL_JoystickNumButtons (myJoystick) << endl;
      outStream << "Achsen: " << SDL_JoystickNumAxes (myJoystick) << endl;
      
      // SDL_JoystickClose (myJoystick);
      outStream << endl;
    }

  } // print all joys
}

void printTTFVersion(std::ostream& outStream){
  using std::endl;
  { // print ttf-version
    SDL_version compile_version;
    const SDL_version *link_version=TTF_Linked_Version();
    SDL_TTF_VERSION(&compile_version);
    outStream << "compiled with SDL_ttf version: ";
    outStream << (int)compile_version.major << ".";
    outStream << (int)compile_version.minor << ".";
    outStream << (int)compile_version.patch << std::endl;
    
    
    outStream << "running with SDL_ttf version: ";
    outStream << (int)link_version->major << ".";
    outStream << (int)link_version->minor << "."; 
    outStream << (int)link_version->patch << std::endl;
  }
}  

void writeOnSurface(
  SDL_Surface* target,
  const std::string& writeString,
  TTF_Font* font,
  SDL_Color color,
  unsigned int x, unsigned int y
  )
{
  
  SDL_Surface *text_surface;
  if(!(text_surface=TTF_RenderUTF8_Solid(font,writeString.c_str(),color))) {
    //handle error here, perhaps print TTF_GetError at least
  } else {

    SDL_Rect DestR;

    { // destination / where to draw
      DestR.x = x;
      DestR.y = y;
    }
    SDL_BlitSurface(text_surface,NULL,target,&DestR);
    //perhaps we can reuse it, but I assume not for simplicity.
    SDL_FreeSurface(text_surface);
  }
}

void writeOnSurfaceCentered(
  SDL_Surface* target,
  const std::string writeString,
  TTF_Font* font,
  SDL_Color color,
  SDL_Rect screenArea
  )
{
  int renderedWidth;
  int renderedHeight;
    
  TTF_SizeUTF8(font, writeString.c_str(), &renderedWidth, &renderedHeight);
  
  int emptyWidth  = (screenArea.w - renderedWidth);
  int emptyHeight = (screenArea.h - renderedHeight);
  
  int outputStartLine = screenArea.y + (emptyHeight/ 2);
  int outputStartColumn = screenArea.x + (emptyWidth / 2);
  writeOnSurface(
    target, writeString,font,color,
    outputStartColumn,outputStartLine
    );
}
void writeOnSurfaceVCentered(
  SDL_Surface* target,
  const std::string writeString,
  TTF_Font* font,
  SDL_Color color,
  SDL_Rect screenArea
  )
{
  int renderedHeight;
    
  TTF_SizeUTF8(font, writeString.c_str(), NULL, &renderedHeight);
  
  int emptyHeight = (screenArea.h - renderedHeight);
  
  int outputStartLine = screenArea.y + (emptyHeight / 2);
  int outputStartColumn = screenArea.x;
  writeOnSurface(
    target, writeString,font,color,
    outputStartColumn,outputStartLine
    );
}

void paintRectangleOnSurface(
  SDL_Surface* target,
  SDL_Rect screenArea,
  SDL_Color color
  )
{
  uint32_t mappedColor = SDL_MapRGB(target->format,color.r,color.g,color.b);
  SDL_FillRect(target,&screenArea,mappedColor);
}


void paintPlayer(
  SDL_Surface* target,
  SDL_Rect screenArea,
  TTF_Font* font,
  SDL_Color color,
  player givenPlayer
  )
{
  SDL_Rect pointRect=screenArea;
  SDL_Rect nameRect =screenArea;
  SDL_Rect timeRect =screenArea;
  { // create subDivision
    unsigned int subFieldHeight=screenArea.h/4;
    pointRect.h = subFieldHeight;
    nameRect.h = subFieldHeight;
    timeRect.h = subFieldHeight;
    pointRect.y = screenArea.y + 1*subFieldHeight;
    nameRect.y = screenArea.y + 2*subFieldHeight;
    timeRect.y = screenArea.y + 3*subFieldHeight;
    // x and w can stay the same - for this layout
  }
    
  { // points
    std::stringstream pointStream;
    pointStream << std::dec << givenPlayer.sumPoints;
    writeOnSurfaceCentered(
      target,pointStream.str(),font,color,
      pointRect
      );
  }
  writeOnSurfaceCentered(
    target,givenPlayer.playerName,font,color,
    nameRect
    );
  { // time - if already an answer was given
    if (0!=givenPlayer.givenAnswer){
      // todo: time in other format
      writeOnSurfaceCentered(
        target,"te",font,color,
        timeRect
        );
    }
  }
}




using namespace std;
int main(void){
  
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_TIMER )== -1){
    cout << "Cant Init SDL: " << SDL_GetError() << endl;
    exit (1);
  }
  atexit(SDL_Quit);
  
  printTTFVersion(std::cout);
  
  if(TTF_Init()==-1) {
      printf("TTF_Init: %s\n", TTF_GetError());
      exit(2);
  }

  const unsigned int screenWidth  = 800;
  const unsigned int screenHeight = 600;


  SDL_Surface *screen;
  screen= SDL_SetVideoMode(screenWidth,screenHeight,16,SDL_HWSURFACE ); // |SDL_FULLSCREEN
  if (NULL == screen){
    cout << "Cant set video Mode: " << SDL_GetError() << endl;
    exit (1);
  }

  TTF_Font *font;
  {
    // load font.ttf at size 16 into font
    font=TTF_OpenFont("LiberationSans-Regular.ttf", 16);
    if(!font) {
      printf("TTF_OpenFont: %s\n", TTF_GetError());
      // handle error
    }
  }
  
  SDL_Color color_buzzRed     = {0xff,0x00,0x00};
  SDL_Color color_buzzBlue    = {0x00,0x00,0xff};
  SDL_Color color_buzzOrange  = {0xff,0x70,0x00};
  SDL_Color color_buzzGreen   = {0x00,0xff,0x00};
  SDL_Color color_buzzYellow  = {0xff,0xff,0x00};
  
  
  SDL_Color color_black = {0x00,0x00,0x00};
  SDL_Color color_white = {0xff,0xff,0xff};
  SDL_Color color_red   = {0xff,0x00,0x00};
  
  
  SDL_Surface* eeveepic = NULL;
  { // try to load pic  
    SDL_Surface *eeveetemppic;
    if((eeveetemppic = SDL_LoadBMP("eevee.bmp")) == NULL) {//Error!
      std::cerr << "Picture could not be loaded" << std::endl;
    }
    
    eeveepic = SDL_DisplayFormat(eeveetemppic);
    SDL_FreeSurface(eeveetemppic);    

    SDL_SetColorKey(eeveepic, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(eeveepic->format, 0x60, 0x98, 0x80));
  }
  
  printJoystickInformation(std::cout);

  
  screenTiling myScreenTiling;
  { // create screen Tiling
    unsigned int currentYPos = 0; 
    // again, note: x is column, y is line, w is width (number of columns), h is height (number of lines
    { // question has upper quarter of screen
      myScreenTiling.questionArea.x = 0;
      myScreenTiling.questionArea.y = currentYPos;
      myScreenTiling.questionArea.w = screenWidth;
      myScreenTiling.questionArea.h = screenHeight / 4;
      currentYPos+=myScreenTiling.questionArea.h;
    }
    SDL_Rect answerArea;
    const unsigned int colorAreaWidth = screenHeight / 2 / 4;
    const unsigned int spaceBetweenColorAndAnswer=10; 
    const unsigned int timerHeight = 30;
    { // generic area for answers: half of the screen,  below the questions
      answerArea.y = currentYPos;
      answerArea.x = colorAreaWidth + spaceBetweenColorAndAnswer;
      answerArea.w = screenWidth / 2;
      answerArea.h = screenHeight / 2;
      currentYPos += answerArea.h;
    }
    { // small answer areas, divide generic answer area
      unsigned int currentAnswerIndex;
      unsigned int singleAnswerHeight = answerArea.h / 4;
      for (
        currentAnswerIndex  = 0;
        currentAnswerIndex  < 4;
        currentAnswerIndex++
        )
      {
        myScreenTiling.answerArea[currentAnswerIndex].y = 
          answerArea.y +
          singleAnswerHeight*currentAnswerIndex;
        myScreenTiling.answerArea[currentAnswerIndex].x = answerArea.x;
        myScreenTiling.answerArea[currentAnswerIndex].w = answerArea.w;
        myScreenTiling.answerArea[currentAnswerIndex].h = singleAnswerHeight;
        
        // only thing to change is the x value and width
        myScreenTiling.answerColorArea[currentAnswerIndex]=
          myScreenTiling.answerArea[currentAnswerIndex];
        myScreenTiling.answerColorArea[currentAnswerIndex].x = 0;
        myScreenTiling.answerColorArea[currentAnswerIndex].w = colorAreaWidth;
        
      } // for all the small areas
    }
    { // image area - right from the answer area
      myScreenTiling.imageArea.y = answerArea.y;
      myScreenTiling.imageArea.x = 
        answerArea.x + 
        answerArea.w;
      myScreenTiling.imageArea.w = screenWidth - answerArea.w;
      myScreenTiling.imageArea.h = answerArea.h;
    }
    { // timer area: below the answers
      myScreenTiling.timerArea.y = currentYPos;
      myScreenTiling.timerArea.x = 0;
      myScreenTiling.timerArea.w = screenWidth;
      myScreenTiling.timerArea.h = timerHeight;
      currentYPos += myScreenTiling.timerArea.h;
    }
    { // player area: remainder at the bottom
      myScreenTiling.playerArea.y = currentYPos;
      myScreenTiling.playerArea.x = 0;
      myScreenTiling.playerArea.w = screenWidth;
      myScreenTiling.playerArea.h  =
        screenHeight - 
        currentYPos; // rest of screen height
      currentYPos += myScreenTiling.playerArea.h;
    }
    
  } // create tiling

  
  std::string questionString = "Fällt Dir gerade eine doofe Frage ein?";
  { // question
    
    writeOnSurfaceCentered(
      screen,questionString,font,color_white,
      myScreenTiling.questionArea
      );
    paintRectangleOnSurface(screen,myScreenTiling.answerColorArea[0],color_buzzBlue);
    writeOnSurfaceVCentered(
      screen,"Antwort A",font,color_white,
      myScreenTiling.answerArea[0]
      );
    paintRectangleOnSurface(screen,myScreenTiling.answerColorArea[1],color_buzzOrange);
    writeOnSurfaceVCentered(
      screen,"Antwort B",font,color_white,
      myScreenTiling.answerArea[1]
      );
    paintRectangleOnSurface(screen,myScreenTiling.answerColorArea[2],color_buzzGreen);
    writeOnSurfaceVCentered(
      screen,"Antwort C",font,color_white,
      myScreenTiling.answerArea[2]
      );
    paintRectangleOnSurface(screen,myScreenTiling.answerColorArea[3],color_buzzYellow);
    writeOnSurfaceVCentered(
      screen,"Antwort D",font,color_white,
      myScreenTiling.answerArea[3]
      );
  }
  
  std::string newPlayerName="";
  { // getting a player name // this has to be changed
    
    writeOnSurfaceCentered(
      screen,"Please enter player name in console!",font,color_white,
      myScreenTiling.playerArea
      );
    SDL_Flip(screen);
  
    
    { // getting a player name
      cout << "Enter Player name: ";
      cin >> newPlayerName;
      cout << endl;
    }
    
    while (""==newPlayerName){ // wait until new player is given
      sleep(1);
    }
    paintRectangleOnSurface(screen,myScreenTiling.playerArea,color_black);
    writeOnSurfaceCentered(
      screen,newPlayerName,font,color_white,
      myScreenTiling.playerArea
      );
    SDL_Flip(screen);    
  }
  
  std::vector < player > players;
  
  { // add some random players
    unsigned int currentPlayerIndex;
    for (
      currentPlayerIndex = 0;
      currentPlayerIndex < 8;
      currentPlayerIndex++
      )
    {
      player newPlayer;
      newPlayer.playerName = newPlayerName+(char)(0x30+currentPlayerIndex);
      newPlayer.sumPoints = currentPlayerIndex*100;
      players.push_back(newPlayer);
    }
  } // add some random players
    
  unsigned int evoliX=320;
  unsigned int evoliY=240;
  drawSprite(screen,eeveepic,evoliX,evoliY);
  unsigned int remain_seconds = 20;

  unsigned int ticks_at_start = SDL_GetTicks();
  SDL_Event myEvent;
  SDL_TimerID my_timer_id = SDL_AddTimer(1000, timeoutCallBack, NULL);
  { // clear all given answers
    std::vector<player>::iterator player_iterator;
    for (
      player_iterator  = players.begin();
      player_iterator != players.end();
      player_iterator++
      )
    {
      player_iterator->givenAnswer = 0;
    }
  }
   
  
  SDL_Flip(screen);
  while(true){
    //int event_available = SDL_PollEvent (&myEvent);
    int event_available = SDL_WaitEvent(&myEvent); // saves ressources
    if (0==event_available){
      std::cerr << "Error waiting on event" << std::endl;
      continue;
    }
    if (SDL_KEYDOWN==myEvent.type){
      cout << "KeyDown";
      
      
      SDL_KeyboardEvent myKeyEvent = myEvent.key;
      if (SDLK_q == myKeyEvent.keysym.sym){
        cout << "Q: Terminate!" << endl;
        break;
      }
      cout << endl;
    }
    if (SDL_JOYBUTTONDOWN == myEvent.type){
      unsigned int reaction_time = SDL_GetTicks() - ticks_at_start;
      
      cout << reaction_time << ": JoyButton,";
      SDL_JoyButtonEvent joystickEvent=myEvent.jbutton;
      cout << "Joystick " << (int) joystickEvent.which;
      #define IDENTIFY_BUZZ
      #ifdef IDENTIFY_BUZZ
      {
        buzz_identify(joystickEvent.button);
      }
      #endif
      #ifndef IDENTIFY_BUZZ
      { 
        cout << "Button " << (int) joystickEvent.button;
        cout << endl;
      }
      #endif
      if (joystickEvent.button%5==0){
        SDL_Rect blackRectangle;
        blackRectangle.x = evoliX;
        blackRectangle.y = evoliY;
        blackRectangle.w = 32;
        blackRectangle.h = 32;
        
        SDL_FillRect(screen,&blackRectangle,SDL_MapRGB(screen->format,0,0,0));
        
        switch (joystickEvent.button/5){
          case 0: evoliX+=5; break;
          case 1: evoliX-=5; break;
          case 2: evoliY+=5; break;
          case 3: evoliY-=5; break;
          default: break;// nothing
        }
        drawSprite(screen,eeveepic,evoliX,evoliY);
      }
      { // color rectangle somewhere
        SDL_Rect colorRectangle;
        colorRectangle.x = 0;
        colorRectangle.y = 0;
        colorRectangle.w = 64;
        colorRectangle.h = 64;
        
        SDL_Color myColor;
        switch (joystickEvent.button%5){
          case BUZZ_YELLOW:  myColor=color_buzzYellow; break;
          case BUZZ_GREEN:  myColor=color_buzzGreen ; break;
          case BUZZ_ORANGE:  myColor=color_buzzOrange; break;
          case BUZZ_BLUE:  myColor=color_buzzBlue  ; break;
          default: myColor=color_buzzRed   ; break;
        } // switch
          
        paintRectangleOnSurface(screen,colorRectangle,myColor);
        SDL_Flip(screen);
      } // color rectangle
          

      
      
    }
    if (SDL_MOUSEBUTTONDOWN == myEvent.type){
      { // print players - this function could be moved 1:1
        unsigned int playerCount = players.size();
        if (0==playerCount){ // prevent div by zero
          paintRectangleOnSurface(
            screen,
            myScreenTiling.playerArea,
            color_red
            );
          continue;
        }
        
        // clear area
        paintRectangleOnSurface(
          screen,
          myScreenTiling.playerArea,
          color_black
          );
        /*
         * Reality Check:
         * 1 Player -> 1 row (1-1)/4+1 = 1
         * 4 Players -> 1 row (4-1)/4+1 = 1
         * 5 players -> 2 rows
         * 8 players -> 2 rows
         *
         * we could make the "4" customizable, later
         */
        unsigned int playersPerRow=4;
         
        SDL_Rect currentPlayerRect;
        { // prepare rectangle
          unsigned int numberOfLines = (playerCount-1) / playersPerRow +1;
          currentPlayerRect.x = myScreenTiling.playerArea.x;
          currentPlayerRect.y = myScreenTiling.playerArea.y;
          currentPlayerRect.w = myScreenTiling.playerArea.w / playersPerRow;;
          currentPlayerRect.h = myScreenTiling.playerArea.h / numberOfLines;
        }
        
        std::vector<player>::const_iterator player_iterator;
        for (
          player_iterator  = players.begin();
          player_iterator != players.end();
          player_iterator++ 
          )
        {
          paintPlayer(screen,currentPlayerRect,font,color_white,*player_iterator);
          currentPlayerRect.x += currentPlayerRect.w; // move to right
          if (currentPlayerRect.x >= myScreenTiling.playerArea.x+myScreenTiling.playerArea.w){ // overflow
            currentPlayerRect.y += currentPlayerRect.h;
            currentPlayerRect.x = myScreenTiling.playerArea.x;
          }
        } // for all players
      }
      SDL_Flip(screen);
    }
    if (SDL_USEREVENT == myEvent.type){
      remain_seconds--;
      std::stringstream output;
      output << "noch " << remain_seconds << " Sekunden";
      paintRectangleOnSurface(
        screen,
        myScreenTiling.timerArea,
        color_red
        );
      SDL_Rect remainRect = myScreenTiling.timerArea;
      remainRect.w = remainRect.w * remain_seconds/20;
      paintRectangleOnSurface(
        screen,
        remainRect,
        color_buzzGreen
        );
      
      
      if (0<remain_seconds){
        writeOnSurface(
          screen,
          output.str(),
          font,
          color_white,
          0,myScreenTiling.timerArea.y);
      } else{// oder schon alle beantwortet
        writeOnSurface(
          screen,
          "Frage vorbei!",
          font,
          color_white,
          0,myScreenTiling.timerArea.y);
          SDL_RemoveTimer(my_timer_id);
      }
      SDL_Flip(screen);
    }
    if (remain_seconds == 0){
      break;
    }
  }
  SDL_Delay(3000); // why?
  SDL_Quit();
  TTF_Quit();
  return 0;
  
}


