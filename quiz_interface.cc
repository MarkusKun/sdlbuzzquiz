#include <sstream>
#include <iostream>

#include "drawhelper.h"

#include "quiz_interface.h"
#include "quiz_player.h"

#include <math.h> // for sqrt

void quiz_interface::paintPlayer(
  SDL_Surface* target,
  SDL_Rect screenArea,
  TTF_Font* font,
  SDL_Color color_text,
  SDL_Color color_answer[4],
  quiz_player::player givenPlayer,
  bool showAnswer
  )
{
  SDL_Rect pointRect=screenArea;
  SDL_Rect nameRect =screenArea;
  SDL_Rect timeRect =screenArea;
  SDL_Rect answerRect1 = screenArea;
  SDL_Rect answerRect2 = screenArea;
  { // create subDivision
    unsigned int subFieldHeight=screenArea.h/4;
    pointRect.h = subFieldHeight;
    nameRect.h = subFieldHeight;
    timeRect.h = subFieldHeight;
    pointRect.y = screenArea.y + 1*subFieldHeight;
    nameRect.y  = screenArea.y + 2*subFieldHeight;
    timeRect.y  = screenArea.y + 3*subFieldHeight;
    // x and w can stay the same - for this layout
    
    answerRect1.h = answerRect1.h/2;
    answerRect1.w = answerRect1.h;
    answerRect1.y = answerRect1.y + answerRect1.h/2;
    
    answerRect2 = answerRect1;
    answerRect2.x = answerRect2.x + screenArea.w - answerRect2.w;
    
    answerRect1.x+=answerRect1.h;
    answerRect2.x-=answerRect2.h;
  }
  
  { // given Answer
    if (showAnswer){ // if answers are to be shown
      if (givenPlayer.givenAnswer != 0){ // if player answered
        paintRectangleOnSurface(
          target,
          answerRect1,
          color_answer[givenPlayer.givenAnswer-1]
          );
        paintRectangleOnSurface(
          target,
          answerRect2,
          color_answer[givenPlayer.givenAnswer-1]
          );
      }
    }
  }
        
  { // points
    std::stringstream pointStream;
    pointStream << std::dec << givenPlayer.sumPoints;
    if (showAnswer){
      pointStream << " + " << givenPlayer.plusPoints;
    }
    pointStream << " Points";
    writeOnSurfaceCentered(
      target,pointStream.str(),font,color_text,
      pointRect
      );
  }
  writeOnSurfaceCentered(
    target,givenPlayer.playerName,font,color_text,
    nameRect
    );
  { // time - if already an answer was given
    if (0!=givenPlayer.givenAnswer){
      std::string time;
      { // convert time
        std::stringstream convertStream;
        // full seconds
        convertStream << std::dec << givenPlayer.responseTime/1000;
        convertStream << ".";
        // millis
        convertStream << std::dec << givenPlayer.responseTime%1000;
        time = convertStream.str();
      }
      writeOnSurfaceCentered(
        target,time,font,color_text,
        timeRect
        );
    }
  }
}


quiz_interface::screenTiling quiz_interface::getScreenTiling(
  const screenLayout desiredLayout,
  const unsigned int screenWidth,
  const unsigned int screenHeight
  )
{
  screenTiling myScreenTiling;
  switch (desiredLayout){
    case SCREEN_LAYOUT_LIST:
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
        break;
      } // case SCREEN_LAYOUT_LIST
    
  } // switch
  return myScreenTiling;
}

void quiz_interface::writeQuestionAndAnswers(
  SDL_Surface* target,
  const screenTiling myScreenTiling,
  TTF_Font* font,
  SDL_Color color_background,
  SDL_Color color_text,
  SDL_Color color_answer[4],
  const std::string& questionString,
  const std::string answerString[4]
  )
{
  { // question
    paintRectangleOnSurface(
      target,
      myScreenTiling.questionArea,
      color_background
      ); // clear area
    writeOnSurfaceCentered(
      target,questionString,font,color_text,
      myScreenTiling.questionArea
      );
  }
  unsigned int currentAnswer;
  for (
    currentAnswer = 0;
    currentAnswer < 4;
    currentAnswer++
    )
  {
    // color rectangle
    paintRectangleOnSurface(
      target,
      myScreenTiling.answerColorArea[currentAnswer],
      color_answer[currentAnswer]
      );
    
    // write a small number into the colored square
    std::stringstream answerIndexStream;
    answerIndexStream << (currentAnswer+1);
    writeOnSurfaceCentered(
      target,
      answerIndexStream.str(),
      font,
      color_background,
      myScreenTiling.answerColorArea[currentAnswer]
      );
    
    // clear text area
    paintRectangleOnSurface(
      target,
      myScreenTiling.answerArea[currentAnswer],
      color_background
      );
    
    // answer
    writeOnSurfaceVCentered(
      target,answerString[currentAnswer],font,color_text,
      myScreenTiling.answerArea[currentAnswer]
      );
  } // for all four answers
}
  
void quiz_interface::paintRemainingTime(
  SDL_Surface* target,
  SDL_Rect screenArea,
  TTF_Font* font,
  const std::string& timeAsString,
  unsigned int maxRemainTime,
  unsigned int currentRemainTime,
  SDL_Color color_background,
  SDL_Color color_text,
  SDL_Color color_passed,
  SDL_Color color_remain
  )
{
  { // clear area
    paintRectangleOnSurface(
      target,
      screenArea,
      color_background
      );
  }
  { // print "percentual" - if wanted
    // set maxRemainTime to zero to not print
    if (maxRemainTime > 0){
      SDL_Rect remainRect = screenArea;
      remainRect.w = remainRect.w * currentRemainTime / maxRemainTime;
      
      // print (completely) with passed color
      paintRectangleOnSurface(
        target,
        screenArea,
        color_passed
        );
      
      // paint (partially) with remaining color
      paintRectangleOnSurface(
        target,
        remainRect,
        color_remain
        );
    }
  }
  { // write text - if wanted
    // set text to empty to not print
    if (! timeAsString.empty()){
      writeOnSurfaceVCentered(
        target,timeAsString,font,color_text,
        screenArea
        );
    }
  }
}
void quiz_interface::showAdminNotify(
  SDL_Surface* target,
  TTF_Font* font,
  SDL_Color color_text
  )
{
  SDL_Rect adminRect;
  {
    adminRect.x=0;
    adminRect.y=0;
    adminRect.w=0; // ignored
    adminRect.h=30;
  }
  writeOnSurfaceVCentered(
    target,
    "Admin requested",
    font,
    color_text,
    adminRect
    );
}


void quiz_interface::paintAllPlayers(
  SDL_Surface* target,
  SDL_Rect screenArea,
  TTF_Font* font,
  SDL_Color color_background,
  SDL_Color color_text,
  SDL_Color color_answer[4],
  quiz_player::players_t givenPlayers,
  bool showAnswer
  )
{
  unsigned int playerCount = givenPlayers.size();
  if (0==playerCount){ // invite to play
    const std::string noPlayerMessage = "No Players, press answer to join!";
    paintRectangleOnSurface(
      target,
      screenArea,
      color_text
      );
    writeOnSurfaceCentered(
      target,
      noPlayerMessage,
      font,color_background,
      screenArea
      );
  }else{ // we have to print the players
    // clear area
    paintRectangleOnSurface(
      target,
      screenArea,
      color_background
      );
    
    //unsigned int playersPerRow=4;
    
    /*
     * try a square root:
     * 2 players -> 2 per row
     * 3 players -> 2 per row
     * 4 players -> 2 per row (2x2)
     * 5-9 players -> 3 per row (3x3)
     *
     * reality check:
     * sqrt(1-1)+1=sqrt(0)+1=0+1
     * sqrt(2-1)+1=sqrt(1)+1=1+1
     * sqrt(4-1)+1=2
     * sqrt(5-1)+1=3
     *...
     */
    unsigned int playersPerRow=
      sqrt(playerCount-1)+1;

      
    SDL_Rect currentPlayerRect;
    { // prepare rectangle
      /*
       * Reality Check:
       * 1 Player -> 1 row (1-1)/4+1 = 1
       * 4 Players -> 1 row (4-1)/4+1 = 1
       * 5 players -> 2 rows
       * 8 players -> 2 rows
       *
       * we could make the "4" customizable, later (done with sqrt)
       * Although sqrt does not seem to be the best solution,
       * as the playerarea is not square. Some calculations
       * with the aspect ratio of the playerarea may be a good ide.
       */
      unsigned int numberOfLines = (playerCount-1) / playersPerRow +1;
      currentPlayerRect.x = screenArea.x;
      currentPlayerRect.y = screenArea.y;
      currentPlayerRect.w = screenArea.w / playersPerRow;;
      currentPlayerRect.h = screenArea.h / numberOfLines;
    }
    quiz_player::players_t::const_iterator player_iterator;
    for (
      player_iterator  = givenPlayers.begin();
      player_iterator != givenPlayers.end();
      player_iterator++ 
      )
    {
      quiz_interface::paintPlayer(
        target,
        currentPlayerRect,
        font,
        color_text,
        color_answer,
        **player_iterator,
        showAnswer
        );
      currentPlayerRect.x += currentPlayerRect.w; // move to right
      if (
        (currentPlayerRect.x+currentPlayerRect.h) >= 
        (screenArea.x+screenArea.w)
        ) // overflow
      { 
        currentPlayerRect.y += currentPlayerRect.h;
        currentPlayerRect.x = screenArea.x;
      }
    } // for all players
  } // print the players
}

Uint32 quiz_interface::timeoutCallBack(Uint32 interval, void *param){
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


bool quiz_interface::callInterface(
  unsigned int screenWidth,
  unsigned int screenHeight,
  TTF_Font* font,
  SDL_Color color_background,
  SDL_Color color_text,
  SDL_Color answerColors[4],
  quiz_player::players& playerList,
  quiz_question::question currentQuestion
  )
{
  SDL_Surface *questionScreen;
  questionScreen= SDL_SetVideoMode(screenWidth,screenHeight,16,SDL_HWSURFACE  |SDL_FULLSCREEN); // |SDL_FULLSCREEN
  if (NULL == questionScreen){
    std::cerr << "Cant set video Mode: " << SDL_GetError() << std::endl;
    exit (1);
  }
  screenTiling myScreenTiling=getScreenTiling(SCREEN_LAYOUT_LIST,screenWidth,screenHeight);
  { // write the question and available answers
    writeQuestionAndAnswers(
      questionScreen,
      myScreenTiling,
      font,
      color_background,
      color_text,
      answerColors,
      currentQuestion.questionString,
      currentQuestion.answerStrings
      );
  }
  const unsigned int maximumTime = 15;
  
  // these definitions probably should get their own argument
  SDL_Color color_red   = {0xff,0x00,0x00};
  SDL_Color color_green = {0x00,0xff,0x00};

  { // print first time bar (full)
    std::stringstream output;
    output << "noch " << maximumTime << " Sekunden";
    quiz_interface::paintRemainingTime(
      questionScreen,
      myScreenTiling.timerArea,
      font,
      output.str(),
      maximumTime, // nothing gone
      maximumTime, 
      color_text,
      color_background,
      color_red,
      color_green
      );
  }
  { // clear all given answers
    playerList.clearAnswers();
  }
  { // paint the players
    quiz_interface::paintAllPlayers(
      questionScreen,
      myScreenTiling.playerArea,
      font,
      color_background,
      color_text,
      answerColors,
      playerList.getPlayerList(),
      false
      );
  }
  // finally, show the screen
  SDL_Flip(questionScreen);

  // and start counting
  unsigned int remain_seconds = maximumTime;
  unsigned int ticks_at_start = SDL_GetTicks();
  bool adminRequested = false;
  SDL_Event myEvent;
  SDL_TimerID my_timer_id = SDL_AddTimer(1000, timeoutCallBack, NULL);
  
  while(true){
    //int event_available = SDL_PollEvent (&myEvent);
    int event_available = SDL_WaitEvent(&myEvent); // saves ressources
    if (0==event_available){
      std::cerr << "Error waiting on event" << std::endl;
      continue;
    }
    if (SDL_KEYDOWN==myEvent.type){ // key pressed
      unsigned int reaction_time = SDL_GetTicks() - ticks_at_start;
      
      SDL_KeyboardEvent myKeyEvent = myEvent.key;
      SDLKey adminKey = SDLK_ESCAPE;
      if (adminKey == myKeyEvent.keysym.sym){
        adminRequested=true; 
        showAdminNotify(
          questionScreen,
          font,
          color_red
          );
        
        
      }
      
      quiz_sources::sourceAnswerCombi mySourceAnswerCombi(myKeyEvent.keysym.sym);
      
      storePlayerAnswer(mySourceAnswerCombi,playerList,reaction_time);
    }
    if (SDL_JOYBUTTONDOWN == myEvent.type){ // joystickpressed (buzz)
      unsigned int reaction_time = SDL_GetTicks() - ticks_at_start;
      SDL_JoyButtonEvent joystickEvent=myEvent.jbutton;
      
      // get source from button
      quiz_sources::sourceAnswerCombi mySourceAnswerCombi(joystickEvent);
      
      if (mySourceAnswerCombi.answerIndex==0){ // red button - call admin
        adminRequested=true; 
        showAdminNotify(
          questionScreen,
          font,
          color_red
          );
      }
      
      storePlayerAnswer(mySourceAnswerCombi,playerList,reaction_time);
    }
    if (SDL_USEREVENT == myEvent.type){ // userevent - a second passed
      remain_seconds--;
      std::stringstream output;
      output << "noch " << remain_seconds << " Sekunden";
      if (0==remain_seconds){
        SDL_RemoveTimer(my_timer_id);
        output.str("Frage vorbei!");
      }
      // update the timer bar
      quiz_interface::paintRemainingTime(
        questionScreen,
        myScreenTiling.timerArea,
        font,
        output.str(),
        maximumTime, 
        remain_seconds,
        color_text,
        color_background,
        color_red,
        color_green
        );
      SDL_Flip(questionScreen);
    }
    if (remain_seconds == 0){
      break;
    }
        
    { // update the players
      quiz_interface::paintAllPlayers(
        questionScreen,
        myScreenTiling.playerArea,
        font,
        color_background,
        color_text,
        answerColors,
        playerList.getPlayerList(),
        false
        );
    }
    // finally, show the screen
    SDL_Flip(questionScreen);
  } // while (true)

  { // at this point, we should present the correct answer
    SDL_Color correctColors[4];
    unsigned int answerIndex;
    for (
      answerIndex  = 1;
      answerIndex <= 4;
      answerIndex++
      )
    {
      SDL_Color thisColor = {0x00,0x00,0x00};
      if (answerIndex == currentQuestion.correctAnswerIndex){
        // answerColors go 0 -> 3, answerIndex fo 1->4, so subtract 1
        thisColor = answerColors[answerIndex-1];
      }
      correctColors[answerIndex-1] = thisColor;
    }
    writeQuestionAndAnswers(
      questionScreen,
      myScreenTiling,
      font,
      color_background,
      color_text,
      correctColors,
      currentQuestion.questionString,
      currentQuestion.answerStrings
      );
  } // present correct answer
  
  { // award points
    playerList.calculatePoints(currentQuestion.correctAnswerIndex);    
  }
  
  { // and show who answered what
    quiz_interface::paintAllPlayers(
      questionScreen,
      myScreenTiling.playerArea,
      font,
      color_background,
      color_text,
      answerColors,
      playerList.getPlayerList(),
      true
      );
  }
  SDL_Flip(questionScreen);
  SDL_Delay(6000); // so players have time to wonder
  
  { // afterwards, really add the points
    playerList.awardPoints();
  }
  
  // return false to call admin, true to simply continue
  return !adminRequested;
}

void quiz_interface::storePlayerAnswer(
  const quiz_sources::sourceAnswerCombi givenSourceAnswer,
  quiz_player::players& playerList,
  const unsigned int reaction_time
  )
{
  if (0==givenSourceAnswer.answerIndex){ // no answer given
    // nothing to do
    return;
  }
  quiz_player::player* activePlayer = playerList.getPlayerBySource(givenSourceAnswer.sourceController);
  if (NULL==activePlayer){ // no player found 
    std::cerr << "quiz_interface::storePlayerAnswer: Strange - player could not be found" << std::endl;
    return;
  }
  if (activePlayer->givenAnswer==0){ // has not yet answered
    // we could allow re-answering in another mode
    activePlayer->responseTime = reaction_time;
    activePlayer->givenAnswer = givenSourceAnswer.answerIndex;
  }
}

  
  
