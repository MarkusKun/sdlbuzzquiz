#include <sstream>

#include "drawhelper.h"

#include "quiz_interface.h"
#include "quiz_player.h"

#include <math.h> // for sqrt

void quiz_interface::paintPlayer(
  SDL_Surface* target,
  SDL_Rect screenArea,
  TTF_Font* font,
  SDL_Color color,
  quiz_player::player givenPlayer
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
    pointStream << " Points";
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
        target,time,font,color,
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


void quiz_interface::paintAllPlayers(
  SDL_Surface* target,
  SDL_Rect screenArea,
  TTF_Font* font,
  SDL_Color color_background,
  SDL_Color color_text,
  quiz_player::players_t givenPlayers
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
        **player_iterator
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

  
  
  
