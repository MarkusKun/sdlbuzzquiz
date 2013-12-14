#ifndef _ADMININTERFACE_H
#define _ADMININTERFACE_H

#include <bitset> 
#include <map>
#include <SDL.h>
#include <SDL_ttf.h>

#include "drawhelper.h"
#include "quiz_player.h"
#include "quiz_sources.h"

namespace adminInterface{
  
  /*!
   * \brief show the admin interface and manipulate players
   *
   * This function shows the administrator interface and let's
   * the user change player data. Changes are directly done to the
   * blablabla
   *
   * return false  if program is to terminate
   */
  bool callInterface(
    TTF_Font* font,
    SDL_Color color_background,
    SDL_Color color_text,
    SDL_Color answerColors[4],
    quiz_player::players& playerList
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
     
     
  struct screenTiling{
    SDL_Rect headerArea;
    SDL_Rect allPlayerArea;
    SDL_Rect footerArea;
    SDL_Rect closeArea;
  };
  
  struct playerTiling{
    SDL_Rect deleteArea;
    SDL_Rect nameArea;
    SDL_Rect colorArea[4];
  };
  
  screenTiling getScreenTiling(
    const unsigned int screenWidth,
    const unsigned int screenHeight
    ); 
  
  playerTiling getPlayerTiling(
    SDL_Rect playerArea
    );
  
  void showInterface(
    SDL_Surface* target,
    TTF_Font* font,
    adminInterface::screenTiling myScreenTiling,
    SDL_Color color_background,
    SDL_Color color_text,
    const quiz_player::players_t& playerList,
    const std::map<quiz_player::player*,std::bitset<4> > pressedButtons,
    SDL_Color answerColors[4]
    );
  
  bool mouseInRect(
    SDL_Rect givenRect,
    SDL_MouseButtonEvent myMouseEvent
    );
  
  /*!
   * return value: editing finished
   *
   * if cancelled (by ESC), string will be cleared
   */
  bool editString(
    std::string& givenString,
    SDL_keysym givenKey
    );
  
  //! marks a button as pressed or not pressed
  void setButton(
    const quiz_sources::sourceAnswerCombi givenSourceAnswer,
    std::map<quiz_player::player*,std::bitset<4> >& pressedButtons,
    quiz_player::players& playerList,
    const bool pressed = true
    );
    
  
}; // namespace





#endif // _ADMININTERFACE_H
