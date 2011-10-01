#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <map>

#include <SDL.h>
#include <SDL_ttf.h>

#include "buzzer.h"
#include "drawhelper.h"
#include "quiz_interface.h"






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

unsigned int playercounter=0;

quiz_player::player* keyboardPlayers[4]; // increase, later
/*
 * this is no real solution.. I guess, another data structure
 * mapping all input abilities to players (and possibly back)
 * will be needed, later
 */

quiz_player::player* identify_player(
  SDLKey pressedKey, 
  quiz_player::players_t& players
  )
{
  // SDLKey should be SDL_Keycode according to doc
  static std::map<SDLKey,unsigned int> keyPlayerMapping;
  if (keyPlayerMapping.empty()){
    keyPlayerMapping[SDLK_5]=0;
    keyPlayerMapping[SDLK_t]=0;
    keyPlayerMapping[SDLK_g]=0;
    keyPlayerMapping[SDLK_b]=0;
    keyPlayerMapping[SDLK_2]=1;
    keyPlayerMapping[SDLK_w]=1;
    keyPlayerMapping[SDLK_s]=1;
    keyPlayerMapping[SDLK_x]=1;
    keyPlayerMapping[SDLK_3]=2;
    keyPlayerMapping[SDLK_e]=2;
    keyPlayerMapping[SDLK_d]=2;
    keyPlayerMapping[SDLK_c]=2;
    keyPlayerMapping[SDLK_4]=3;
    keyPlayerMapping[SDLK_r]=3;
    keyPlayerMapping[SDLK_f]=3;
    keyPlayerMapping[SDLK_v]=3;
  }
  
  std::map<SDLKey,unsigned int>::const_iterator map_searcher;
  map_searcher=keyPlayerMapping.find(pressedKey);
  if (keyPlayerMapping.end()==map_searcher){
    // key is no player
    return NULL;
  }
  unsigned int keyPlayerIndex = map_searcher->second;
  
  if (NULL!=keyboardPlayers[keyPlayerIndex]){ // player exists
    return keyboardPlayers[keyPlayerIndex];
  }else{ // player must be created
    quiz_player::player* newPlayer = new quiz_player::player;
    std::string newPlayerNumber;
    {
      std::stringstream convertStream;
      convertStream << std::dec << playercounter;
      convertStream >> newPlayerNumber;
      playercounter++;
    }
    newPlayer->playerName = "UnplayedNamer"+newPlayerNumber;
    newPlayer->sumPoints = 0;
    newPlayer->responseTime=0;
    newPlayer->givenAnswer = 0;
    keyboardPlayers[keyPlayerIndex]=newPlayer;
    players.push_back(newPlayer);
    return newPlayer;
  }
}
unsigned int identify_answer(
  SDLKey pressedKey
  )
{
  static std::map<SDLKey,unsigned int> keyAnswerMapping;
  if (keyAnswerMapping.empty()){
    keyAnswerMapping[SDLK_5]=1;
    keyAnswerMapping[SDLK_t]=2;
    keyAnswerMapping[SDLK_g]=3;
    keyAnswerMapping[SDLK_b]=4;
    keyAnswerMapping[SDLK_2]=1;
    keyAnswerMapping[SDLK_w]=2;
    keyAnswerMapping[SDLK_s]=3;
    keyAnswerMapping[SDLK_x]=4;
    keyAnswerMapping[SDLK_3]=1;
    keyAnswerMapping[SDLK_e]=2;
    keyAnswerMapping[SDLK_d]=3;
    keyAnswerMapping[SDLK_c]=4;
    keyAnswerMapping[SDLK_4]=1;
    keyAnswerMapping[SDLK_r]=2;
    keyAnswerMapping[SDLK_f]=3;
    keyAnswerMapping[SDLK_v]=4;
  }
  std::map<SDLKey,unsigned int>::const_iterator map_searcher;
  map_searcher=keyAnswerMapping.find(pressedKey);
  if (keyAnswerMapping.end()==map_searcher){
    // key is no answer
    return 0;
  }else{
    return map_searcher->second;
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
  SDL_Rect fullScreen;
  {
    fullScreen.w=screenWidth;
    fullScreen.h=screenHeight;
    fullScreen.x=0;
    fullScreen.y=0;
  }


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
  SDL_Color color_green = {0x00,0xff,0x00};
  
  
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

  quiz_player::players_t gamePlayers;

  { // admin interface?
    SDL_Surface *adminScreen;
    adminScreen= SDL_SetVideoMode(screenWidth,screenHeight,16,SDL_HWSURFACE ); // |SDL_FULLSCREEN
    if (NULL == adminScreen){
      std::cerr << "Cant set video Mode: " << SDL_GetError() << std::endl;
      exit (1);
    }
    paintRectangleOnSurface(
      adminScreen,
      fullScreen,
      color_red
      );
    /*
     * Admin interface should support:
     * - deleting players 
     * - renaming players
     * - ending the game
     * - continuing the game
     *
     * Note: Adding players is possible in the admin interface but
     * doesn't need to be a direct administrator feature: Players
     * can join by pressing an answer key/button (in admin interface
     * as well as in regular game). The Admin simply has to rename
     * them.
     *
     * For less collision with the keys and buttons,
     * a mouse interface seems sensible.
     *
     * click on players: rename
     * click on space before players: delete
     * special area for quit
     * special area for continue
     *
     * admin interface should also blink up pressed buttons to
     * provide a simple test ability
     *
     * admin interface should be called at the start
     * of the game and between question - when requested
     * via ESC-key.. or perhaps via red buzzer.
     */
    
    SDL_Flip(adminScreen);
    SDL_Delay(3000); // why?
  } // admin interface
  paintRectangleOnSurface(
    screen,
    fullScreen,
    color_black
    );

  // create tiling
  quiz_interface::screenTiling myScreenTiling = 
    quiz_interface::getScreenTiling(
      quiz_interface::SCREEN_LAYOUT_LIST,
      screenWidth,
      screenHeight
      );

  { // question
    std::string questionString = "Fällt Dir gerade eine doofe Frage ein?";
    SDL_Color answerColors[4]={
      color_buzzBlue,
      color_buzzOrange,
      color_buzzGreen,
      color_buzzYellow
      };
      std::string answerString[4]={
        "blau - definitiv blau!",
        "grün - es ist immer grün!",
        "rot sieht viel besser aus!",
        "was spricht gegen orange?"
      };
    writeQuestionAndAnswers(
      screen,
      myScreenTiling,
      font,
      color_black,
      color_white,
      answerColors,
      questionString,
      answerString
      );

  }

  
  #ifdef CREATE_SOME_PLAYERS
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
  
  { // add some random players
    unsigned int currentPlayerIndex;
    for (
      currentPlayerIndex = 0;
      currentPlayerIndex < 0; // 8
      currentPlayerIndex++
      )
    {
      quiz_player::player* newPlayer = new quiz_player::player;
      newPlayer->playerName = newPlayerName+(char)(0x30+currentPlayerIndex);
      newPlayer->sumPoints = currentPlayerIndex*100;
      players.push_back(newPlayer);
    }
  } // add some random players
  #endif // CREATE_SOME_PLAYERS
  
  
  
  
  unsigned int evoliX=320;
  unsigned int evoliY=240;
  drawSprite(screen,eeveepic,evoliX,evoliY);
  unsigned int remain_seconds = 20;

  unsigned int ticks_at_start = SDL_GetTicks();
  SDL_Event myEvent;
  SDL_TimerID my_timer_id = SDL_AddTimer(1000, timeoutCallBack, NULL);
  { // clear all given answers
    quiz_player::players_t::iterator player_iterator;
    for (
      player_iterator  = gamePlayers.begin();
      player_iterator != gamePlayers.end();
      player_iterator++
      )
    {
      (*player_iterator)->givenAnswer = 0;
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
      
      SDL_KeyboardEvent myKeyEvent = myEvent.key;
      SDLKey adminKey = SDLK_ESCAPE;
      if (adminKey == myKeyEvent.keysym.sym){
        cout << "ESC: Terminate (later: admin)!" << endl;
        break;
      }
      quiz_player::player* keyPlayer = identify_player(
        myKeyEvent.keysym.sym,
        gamePlayers
        );
      if (NULL!=keyPlayer){
        unsigned int reaction_time = SDL_GetTicks() - ticks_at_start;
        unsigned int givenAnswer = identify_answer(
          myKeyEvent.keysym.sym
          );
        // if player is valid, answer automatically is
        // we could create a single map-structure instead of two
        if (keyPlayer->givenAnswer==0){ // or re-answering allowed
          keyPlayer->responseTime = reaction_time;
          keyPlayer->givenAnswer = givenAnswer;
          quiz_interface::paintAllPlayers(
            screen,
            myScreenTiling.playerArea,
            font,
            color_black,
            color_white,
            gamePlayers
            );
        }
        SDL_Flip(screen);
      }
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
    }
    if (SDL_USEREVENT == myEvent.type){
      remain_seconds--;
      std::stringstream output;
      output << "noch " << remain_seconds << " Sekunden";
      if (0==remain_seconds){
        SDL_RemoveTimer(my_timer_id);
        output.str("Frage vorbei!");
      }
      quiz_interface::paintRemainingTime(
        screen,
        myScreenTiling.timerArea,
        font,
        output.str(),
        20, // maximum of 20 seconds
        remain_seconds,
        color_black,
        color_white,
        color_red,
        color_green
        );
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


