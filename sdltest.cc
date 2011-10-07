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
#include "quiz_question.h"
#include "admininterface.h"

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
  
  /*
  SDL_Rect fullScreen;
  {
    fullScreen.w=screenWidth;
    fullScreen.h=screenHeight;
    fullScreen.x=0;
    fullScreen.y=0;
  }
  */

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
  SDL_Color answerColors[4]={
    color_buzzBlue,
    color_buzzOrange,
    color_buzzGreen,
    color_buzzYellow
    };
  
  
  SDL_Color color_black = {0x00,0x00,0x00};
  SDL_Color color_white = {0xff,0xff,0xff};
  SDL_Color color_red   = {0xff,0x00,0x00};
  SDL_Color color_green = {0x00,0xff,0x00};
  
  
  #ifdef TEST_SDL_SPRITES
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
  #endif // TEST_SDL_SPRITES
  
  printJoystickInformation(std::cout);

  quiz_player::players gamePlayers;

  #define CREATE_SOME_PLAYERS
  
  #ifdef CREATE_SOME_PLAYERS
  std::string newPlayerName = "noPlayer";
  
  { // add some random players
    unsigned int currentPlayerIndex;
    for (
      currentPlayerIndex = 0;
      currentPlayerIndex < 2; // 8
      currentPlayerIndex++
      )
    {
      quiz_sources::playerSource newPlayerSource(
        quiz_sources::PLAYERSOURCETYPE_UNKNOWN,
        currentPlayerIndex
        );
      std::string currentPlayerName = (newPlayerName+(char)(0x30+currentPlayerIndex));
      gamePlayers.addPlayer(newPlayerSource,currentPlayerName);
      
      quiz_player::player* newPlayer = gamePlayers.getPlayerBySource(newPlayerSource);
      newPlayer->sumPoints = currentPlayerIndex*100;
    }
  } // add some random players
  #endif // CREATE_SOME_PLAYERS
  
  bool terminateGame=false;
  bool callAdmin = true;
  
  quiz_question::questions myQuestions = quiz_question::readFile("./xml/questions.xml");
  
  quiz_question::questions::const_iterator question_iterator;
  
  srand((unsigned)time(0)); // seed the RNG
    
  while (!myQuestions.empty()){
    unsigned int currentQuestionIndex=0;
    //#define QUESTIONS_RANDOM_ORDER
    #ifdef QUESTIONS_RANDOM_ORDER
    {
      currentQuestionIndex = (rand()*1.0)/RAND_MAX * myQuestions.size();
    }
    #endif // QUESTIONS_RANDOM_ORDER
    
    // todo: choose question randomly
    if (callAdmin){ // admin interface?
      if (adminInterface::callInterface(
        screenWidth,
        screenHeight,
        font,
        color_black,
        color_white,
        answerColors,
        gamePlayers
        )
        ==false)
      { // terminate
        terminateGame=true;
        break;
      }
      callAdmin=false;
    } // admin interface
    quiz_question::question currentQuestion = myQuestions[currentQuestionIndex];
    #ifdef STATIC_QUESTION
    { // create question
      currentQuestion.questionString = "Welche Zahl kommt Pi am nächsten?";
      currentQuestion.answerStrings[0]="3,141592653589793238562643383279502884197169399"; // wrong
      currentQuestion.answerStrings[1]="3,141592653589793238462643383279502884197169399";
      currentQuestion.answerStrings[2]="3,141592653589793238462643384279502884197169399";
      currentQuestion.answerStrings[3]="3,141592663589793238462643383279502884197169399";
      currentQuestion.correctAnswerIndex = 2;
    }
    #endif // STATIC_QUESTION
    
    
    { // do a question 
      if(quiz_interface::callInterface(
        screenWidth,
        screenHeight,
        font,
        color_black,
        color_white,
        answerColors,
        gamePlayers,
        currentQuestion
        )
        ==false)
      { // call admin
        callAdmin=true;
      }
    }
    
    { // erase question from game
      quiz_question::questions::iterator del_iterator = myQuestions.begin();
      del_iterator += currentQuestionIndex;
      myQuestions.erase(del_iterator);
    }
    
  } // while not terminate game
      
  SDL_Delay(500); // why?
  { // print final scores
    
    // TODO: Move to a graphical part.. perhaps start admin interface one last time.
    
    quiz_player::players_t playerList = gamePlayers.getPlayerList();
    
    cout << "Game had " << playerList.size() << " players at the end" << endl;
    
    quiz_player::players_t::const_iterator player_iterator;
    for (
      player_iterator  = playerList.begin();
      player_iterator != playerList.end();
      player_iterator++
      )
    {
      cout << ((*player_iterator)->playerName) << " : " << ((*player_iterator)->sumPoints) << endl;
    } // for all players
  }
  SDL_Quit();
  TTF_Quit();
  return 0;
  
  
  
  
  #ifdef TEST_SDL_SPRITES
  unsigned int evoliX=320;
  unsigned int evoliY=240;
  drawSprite(questionScreen,eeveepic,evoliX,evoliY);
  #endif // TEST_SDL_SPRITES
  
  SDL_Event myEvent;
   
  
  while(!terminateGame){
    if (SDL_JOYBUTTONDOWN == myEvent.type){
      
      #ifdef TEST_SDL_SPRITES // move evoli
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
        drawSprite(questionScreen,eeveepic,evoliX,evoliY);
      }
      #endif // TEST_SDL_SPRITES
    }
  }
  SDL_Delay(500); // why?
  SDL_Quit();
  TTF_Quit();
  return 0;
  
}


