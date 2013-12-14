#ifndef _QUIZ_INTERFACE
#define _QUIT_INTERFACE

#include <vector>

#include <SDL.h>
#include <SDL_ttf.h>

#include "quiz_player.h"
#include "quiz_question.h"

namespace quiz_interface{
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
  
  void paintPlayer(
    SDL_Surface* target,
    SDL_Rect screenArea,
    TTF_Font* font,
    SDL_Color color,
    SDL_Color color_answer[4],
    quiz_player::player givenPlayer,
    bool showAnswer = false
    );
  
  void paintAllPlayers(
    SDL_Surface* target,
    SDL_Rect screenArea,
    TTF_Font* font_text,
    SDL_Color color_background,
    SDL_Color color_text,
    SDL_Color color_answer[4],
    quiz_player::players_t givenPlayers,
    bool showAnswer = false
    );
  
  enum screenLayout{
    SCREEN_LAYOUT_UNDEF,
    SCREEN_LAYOUT_LIST
  };
  
  screenTiling getScreenTiling(
    const screenLayout desiredLayout,
    const unsigned int screenWidth,
    const unsigned int screenHeight
    ); 
  
  void writeQuestionAndAnswers(
    SDL_Surface* target,
    const screenTiling myScreenTiling,
    TTF_Font* font,
    SDL_Color color_background,
    SDL_Color color_text,
    SDL_Color color_answer[4],
    const std::string& questionString,
    const std::string answerString[4]
    );
  
  void paintRemainingTime(
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
    );
  
  //! do a single question, return false if admin is to be called
  bool callInterface(
    TTF_Font* font,
    SDL_Color color_background,
    SDL_Color color_text,
    SDL_Color answerColors[4],
    quiz_player::players& playerList,
    quiz_question::question currentQuestion
    );
    
  //! callback function for timer - add a user event on every second
  Uint32 timeoutCallBack(Uint32 interval, void *param);    
  
  void showAdminNotify(
    SDL_Surface* target,
    TTF_Font* font,
    SDL_Color color_text
    );
  
  //! store the given answer for this player
  void storePlayerAnswer(
    const quiz_sources::sourceAnswerCombi givenSourceAnswer,
    quiz_player::players& playerList,
    const unsigned int reaction_time
    );
    
} // namespace quiz_interface

#endif // _QUIZ_INTERFACE
