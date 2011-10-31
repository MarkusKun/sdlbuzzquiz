#include <iostream> // for cerr and endl
#include <sstream>

#include "admininterface.h"

#include "quiz_player.h"
#include "quiz_config.h"

extern quiz_config myQuizConfig;

void adminInterface::showInterface(
  SDL_Surface* target,
  TTF_Font* font,
  adminInterface::screenTiling myScreenTiling,
  SDL_Color color_background,
  SDL_Color color_text,
  const quiz_player::players_t& playerList,
  const std::map<quiz_player::player*,std::bitset<4> > pressedButtons,
  SDL_Color answerColors[4]
  )
{
  { // print header
    paintRectangleOnSurface(
      target,
      myScreenTiling.headerArea,
      color_text
      );
    writeOnSurfaceCentered(
      target,
      "Admin Interface",
      font,
      color_background,
      myScreenTiling.headerArea
      );
  } // header
  { // small x for closing
    writeOnSurfaceCentered(
      target,
      "X",
      font,
      color_background,
      myScreenTiling.closeArea
      );
  }
  { // all players
    unsigned int playerCount = playerList.size();
    SDL_Rect currentPlayerArea = myScreenTiling.allPlayerArea;
    currentPlayerArea.h = myScreenTiling.allPlayerArea.h / (playerCount+1);
    
    quiz_player::players_t::const_iterator playerIterator;
    for (
      playerIterator  = playerList.begin();
      playerIterator != playerList.end();
      playerIterator++
      )
    {
      paintRectangleOnSurface(
        target,
        currentPlayerArea,
        color_background
        );
      
      playerTiling myPlayerTiling=getPlayerTiling(currentPlayerArea);
      
      writeOnSurfaceVCentered(
        target,
        "[Del]",
        font,
        color_text,
        myPlayerTiling.deleteArea        
        );
      
      std::stringstream pointStream;
      pointStream << std::dec << (*playerIterator)->sumPoints;
      
      std::string playerLabel = (*playerIterator)->playerName + " (" + pointStream.str() + " points)";
      
      writeOnSurfaceVCentered(
        target,
        playerLabel,
        font,
        color_text,
        myPlayerTiling.nameArea
        );
      
      { // buzz colors
        std::map<quiz_player::player*,std::bitset<4> >::const_iterator button_iterator;
        button_iterator = pressedButtons.find(*playerIterator);
        if (pressedButtons.end()==button_iterator){
          // no entry, nothing pressed
        }else{ // we have at least some information
          unsigned int currentBuzzIndex;
          for (
            currentBuzzIndex = 0;
            currentBuzzIndex < 4;
            currentBuzzIndex++
            )
          {
            if ((button_iterator->second)[currentBuzzIndex]){ // button pressed
              paintRectangleOnSurface(
                target,
                myPlayerTiling.colorArea[currentBuzzIndex],
                answerColors[currentBuzzIndex]
                );
            }else{ // not pressed, print background
              paintRectangleOnSurface(
                target,
                myPlayerTiling.colorArea[currentBuzzIndex],
                color_background
                );
            }
          } // for all four buttons
        } // information on buttons exists
      }
      
      currentPlayerArea.y += currentPlayerArea.h;
    }
    { // no real player, but message (inverted)
      paintRectangleOnSurface(
        target,
        currentPlayerArea,
        color_text
        );
      writeOnSurfaceCentered(
        target,
        "Press Button to join",
        font,
        color_background,
        currentPlayerArea
        );
    }
  } // all players
  { // footer
    paintRectangleOnSurface(
      target,
      myScreenTiling.footerArea,
      color_background
      );
    writeOnSurfaceCentered(
      target,
      "Click to Continue",
      font,
      color_text,
      myScreenTiling.footerArea
      );
  }
}
  
adminInterface::screenTiling adminInterface::getScreenTiling(
  const unsigned int screenWidth,
  const unsigned int screenHeight
  )
{
  screenTiling returnTiling;
  unsigned int currentYPos = 0;
  const unsigned int headerFooterSize=20;
  { // define rectangles
    { // header
      returnTiling.headerArea.y = currentYPos;
      returnTiling.headerArea.x = 0;
      returnTiling.headerArea.w = screenWidth;
      returnTiling.headerArea.h = headerFooterSize;
      currentYPos += returnTiling.headerArea.h;
    }
    { // closeArea
      returnTiling.closeArea = returnTiling.headerArea;
      returnTiling.closeArea.x = returnTiling.closeArea.w - headerFooterSize,
      returnTiling.closeArea.w = headerFooterSize;
    }
    { // all player Area
      returnTiling.allPlayerArea.y = currentYPos;
      returnTiling.allPlayerArea.x = 0;
      returnTiling.allPlayerArea.w = screenWidth;
      returnTiling.allPlayerArea.h = 
        screenHeight - currentYPos - headerFooterSize;
      currentYPos += returnTiling.allPlayerArea.h;
    }
    { // footer
      returnTiling.footerArea.y = currentYPos;
      returnTiling.footerArea.x = 0;
      returnTiling.footerArea.w = screenWidth;
      returnTiling.footerArea.h = screenHeight-currentYPos;
      currentYPos += returnTiling.footerArea.h;
    }
  }
  return returnTiling;
}

adminInterface::playerTiling adminInterface::getPlayerTiling(
  SDL_Rect playerArea
  )
{
  const unsigned int deleteWidth = 32;
  const unsigned int spaceBetweenDeleteAndName = 10;
  const unsigned int buzzColorWidth=32;
  playerTiling returnTiling;
  { // deleteArea
    returnTiling.deleteArea = playerArea;
    returnTiling.deleteArea.w = deleteWidth;
  }
  { // nameArea
    returnTiling.nameArea = playerArea;
    returnTiling.nameArea.x = 
      returnTiling.deleteArea.x + returnTiling.deleteArea.w + spaceBetweenDeleteAndName;
    returnTiling.nameArea.w = playerArea.w - returnTiling.nameArea.x - 4*buzzColorWidth;
  }
  { // buzzers
    SDL_Rect currentBuzzArea = playerArea;
    currentBuzzArea.x = returnTiling.nameArea.x + returnTiling.nameArea.w;
    currentBuzzArea.w = buzzColorWidth;
    unsigned int currentBuzzColor;
    for (
      currentBuzzColor  = 0;
      currentBuzzColor  < 4;
      currentBuzzColor++
      )
    {
      returnTiling.colorArea[currentBuzzColor] = currentBuzzArea;
      currentBuzzArea.x+=currentBuzzArea.w;
    }
  }
  return returnTiling;
  
  
  
  
}


bool adminInterface::mouseInRect(
  SDL_Rect givenRect,
  SDL_MouseButtonEvent myMouseEvent
  )
{
  unsigned int mouseX=myMouseEvent.x;
  unsigned int mouseY=myMouseEvent.y;
  if (mouseX < givenRect.x){
    return false;
  }
  if (givenRect.x+givenRect.w < mouseX){
    return false;
  }
  if (mouseY < givenRect.y){
    return false;
  }
  if (givenRect.y+givenRect.h < mouseY){
    return false;
  }
  return true;
}


bool adminInterface::callInterface(
  TTF_Font* font,
  SDL_Color color_background,
  SDL_Color color_text,
  SDL_Color answerColors[4],
  quiz_player::players& playerList
  )
{
  SDL_Surface *adminScreen;
  unsigned int screenWidth = myQuizConfig.screenwidth;
  unsigned int screenHeight = myQuizConfig.screenheight;
  unsigned int bitsPerPixel=16;
  uint32_t screenFlags = SDL_HWSURFACE;
  if (myQuizConfig.fullscreen){
    screenFlags |= SDL_FULLSCREEN;
  }
  
  adminScreen= SDL_SetVideoMode(screenWidth,screenHeight,bitsPerPixel,screenFlags); 
  if (NULL == adminScreen){
    std::cerr << "Cant set video Mode: " << SDL_GetError() << std::endl;
    exit (1);
  }
  screenTiling myScreenTiling=getScreenTiling(screenWidth,screenHeight);

  std::map<quiz_player::player*,std::bitset<4> > pressedButtons;
  quiz_player::players_t playerVector;
  playerVector = playerList.getPlayerList();
  showInterface(
    adminScreen,
    font,
    myScreenTiling,
    color_background,
    color_text,
    playerVector,
    pressedButtons,
    answerColors
    );
  SDL_Flip(adminScreen);
  
  SDL_Event myEvent;
  
  std::string newPlayerName="";
  std::string oldPlayerName="";
  quiz_player::player* selectedPlayer=NULL;
  
  while(true){
    //int event_available = SDL_PollEvent (&myEvent);
    int event_available = SDL_WaitEvent(&myEvent); // saves ressources
    if (0==event_available){
      std::cerr << "Error waiting on event" << std::endl;
      continue;
    }
    if (SDL_MOUSEBUTTONDOWN == myEvent.type){
      SDL_MouseButtonEvent myMouseEvent=myEvent.button;
      if (SDL_BUTTON_LEFT != myMouseEvent.button){
        // only react on left mouse button
        continue;
      }
      
      // find out where user clicked
      if (mouseInRect(myScreenTiling.closeArea,myMouseEvent)){
        return false; // end interface, terminating program
      }
      if (mouseInRect(myScreenTiling.footerArea,myMouseEvent)){
        //std::cout << "ending admin interface by mouse" << std::endl;
        break; // end interface, not terminating program
      }
      { // check the players
        unsigned int playerCount = playerList.size();
        SDL_Rect currentPlayerArea = myScreenTiling.allPlayerArea;
        currentPlayerArea.h = myScreenTiling.allPlayerArea.h / (playerCount+1);
        unsigned int currentPlayerIndex;
        for (
          currentPlayerIndex = 0;
          currentPlayerIndex < playerCount;
          currentPlayerIndex++
          )
        {
          if (mouseInRect(currentPlayerArea,myMouseEvent)){
            break;
          }
          currentPlayerArea.y += currentPlayerArea.h;
        }
        if (currentPlayerIndex < playerCount){ // select this player
          playerTiling subTiling = getPlayerTiling(currentPlayerArea);
          if (mouseInRect(subTiling.nameArea,myMouseEvent)){ // clicked on name
            // so start a namechange
            if (NULL!=selectedPlayer){ // reverse editing
              selectedPlayer->playerName = oldPlayerName;
              selectedPlayer=NULL;
            }
            selectedPlayer = playerVector[currentPlayerIndex];
            oldPlayerName = selectedPlayer->playerName;
            newPlayerName = "";
            selectedPlayer->playerName = "New Name: |";
          }
          if (mouseInRect(subTiling.deleteArea,myMouseEvent)){ // clicked on Del
            // so delete the player
            quiz_player::player* toDelPlayer = playerVector[currentPlayerIndex];
            // if this is the currently edited, cancel editing
            if (toDelPlayer == selectedPlayer){
              selectedPlayer = NULL;
            }
            // no more buttons for this one
            pressedButtons.erase(toDelPlayer);
            // remove the player
            playerList.removePlayer(toDelPlayer);
            
            // and update player Vector
            playerVector = playerList.getPlayerList();
          }
        }
      }
    }
    
    if (SDL_KEYDOWN==myEvent.type){ // user pressed a key
      SDL_KeyboardEvent myKeyEvent = myEvent.key;
      if (NULL!=selectedPlayer){ // we are editing
        bool editingFinished = editString(newPlayerName,myKeyEvent.keysym);
        if (editingFinished){ // enter or escape
          if (newPlayerName.empty()){ // editing was cancelled
            selectedPlayer->playerName = oldPlayerName;
          }else{
            selectedPlayer->playerName = newPlayerName;
          }
          selectedPlayer=NULL;
        }else{ // not yet finished
          std::string displayString = "New Name: " + newPlayerName + "|";
          selectedPlayer->playerName=displayString;
        }
      }else{ // normal key mode
        SDLKey adminKey = SDLK_ESCAPE;
        if (adminKey == myKeyEvent.keysym.sym){ // escape: exit admin screen
          break;
        }
        
        // get source from key.
        quiz_sources::sourceAnswerCombi mySourceAnswerCombi(myKeyEvent.keysym.sym);
        
        setButton(mySourceAnswerCombi,pressedButtons,playerList,true);

        // update player Vector
        playerVector = playerList.getPlayerList();
        
      } // normal key mode - react on players
    } // KEYDOWN
    if (SDL_KEYUP==myEvent.type){ // user pressed a key
      SDL_KeyboardEvent myKeyEvent = myEvent.key;
      if (NULL!=selectedPlayer){ // we are editing
        // don't react on keyup when editing
        continue;
      }
      // get source from key.
      quiz_sources::sourceAnswerCombi mySourceAnswerCombi(myKeyEvent.keysym.sym);
      
      setButton(mySourceAnswerCombi,pressedButtons,playerList,false);
      
      // update player vector - needed??
      //playerVector = playerList.getPlayerList();
      
      
    } // KEYUP
      
    if (SDL_JOYBUTTONDOWN == myEvent.type){ // joystickpressed (buzz)
      SDL_JoyButtonEvent joystickEvent=myEvent.jbutton;
      
      // get source from button
      quiz_sources::sourceAnswerCombi mySourceAnswerCombi(joystickEvent);
      
      setButton(mySourceAnswerCombi,pressedButtons,playerList,true);

      // update player Vector
      playerVector = playerList.getPlayerList();
    }
      
    if (SDL_JOYBUTTONUP == myEvent.type){ // joystickpressed (buzz)
      SDL_JoyButtonEvent joystickEvent=myEvent.jbutton;
      
      // get source from button
      quiz_sources::sourceAnswerCombi mySourceAnswerCombi(joystickEvent);
      
      setButton(mySourceAnswerCombi,pressedButtons,playerList,false);
    }
    
    
    // most actions require a redraw
    showInterface(
      adminScreen,
      font,
      myScreenTiling,
      color_background,
      color_text,
      playerVector,
      pressedButtons,
      answerColors
      );
    SDL_Flip(adminScreen);
  } // while true
  if (NULL!=selectedPlayer){ // reverse editing
    selectedPlayer->playerName = oldPlayerName;
    selectedPlayer=NULL;
  }
  //std::cout << "New Player List:" << std::endl;
  
  //std::cout << playerList.debugPrintKnown();
  SDL_FreeSurface(adminScreen);
  
  return true;
  
}

bool adminInterface::editString(
  std::string& givenString,
  SDL_keysym givenKey
  )
{
  bool shiftPressed=false;
  if (SDLK_ESCAPE == givenKey.sym){
    givenString = "";
    return true;
  }
  if (SDLK_KP_ENTER == givenKey.sym){ // keypad enter
    return true;
  }
  if (SDLK_RETURN == givenKey.sym){ // other return
    return true;
  }
  if (givenKey.mod & KMOD_LSHIFT){
    shiftPressed=true;
  }
  if (givenKey.mod & KMOD_RSHIFT){
    shiftPressed=true;
  }
  if (SDLK_SPACE == givenKey.sym){
    givenString += " ";
    return false;
  }
  if (
    (SDLK_a <= givenKey.sym) &&
    (givenKey.sym <= SDLK_z)
    ) // letter char ('a' - 'z')
  {
    if (shiftPressed){ // capital
      givenString+=(givenKey.sym-0x20);
    }else{
      givenString+=(givenKey.sym);
    }
    return false;
  }
  if (
    (SDLK_0 <= givenKey.sym) &&
    (givenKey.sym <= SDLK_9)
    )
  {
    givenString+=(givenKey.sym);
    return false;
  }
  if (SDLK_MINUS == givenKey.sym){
    givenString+=(givenKey.sym);
    return false;
  }
  
  if (SDLK_BACKSPACE == givenKey.sym){
    if (!givenString.empty()){
      givenString = givenString.substr(0,givenString.size()-1);
    }
    return false;
  }
  // more checks if needed
    
  
  return false;  
}

void adminInterface::setButton(
  const quiz_sources::sourceAnswerCombi givenSourceAnswer,
  std::map<quiz_player::player*,std::bitset<4> >& pressedButtons,
  quiz_player::players& playerList,
  const bool pressed
  )
{
  if(
    (0==givenSourceAnswer.answerIndex) && // no answer given 
    (quiz_sources::PLAYERSOURCETYPE_BUZZ != givenSourceAnswer.sourceController.sourceType) // (can be red on buzzer)
    )
  {
    return; // do nothing - no player!
  }
  quiz_player::player* activePlayer = playerList.getPlayerBySource(givenSourceAnswer.sourceController);
  if (NULL==activePlayer){ // no player found - should never happen
    std::cerr << "adminInterface::setButton: Strange - player could not be found" << std::endl;
    return;
  }
  if (0!=givenSourceAnswer.answerIndex){ // even with buzz, prevent -1
    pressedButtons[activePlayer].set(givenSourceAnswer.answerIndex-1,pressed); // button pressed or not
  }
}
  

