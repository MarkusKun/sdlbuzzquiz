#ifndef _QUIZ_SOURCES_H
#define _QUIZ_SOURCES_H

#include <SDL.h>

namespace quiz_sources{
  enum sourceType_t{
    PLAYERSOURCETYPE_UNKNOWN,
    PLAYERSOURCETYPE_KEYBOARD,
    PLAYERSOURCETYPE_BUZZ,
    PLAYERSOURCETYPE_NETWORK // DS players?
  };
  
  struct playerSource{
    sourceType_t      sourceType;
    unsigned int      controllerIndex;
    bool operator<(const playerSource& vsPlayerSource)const;
    
    playerSource(
      sourceType_t     newSourceType = PLAYERSOURCETYPE_UNKNOWN,
      unsigned int     newControllerIndex = 0
      );

  };
  
  struct controllerAnswerCombi{
    unsigned int sourceControllerIndex;
    unsigned int givenAnswer;
    controllerAnswerCombi(
      unsigned int newSCI=0,
      unsigned int newGA=0
      );
  };
  
  struct sourceAnswerCombi{
    playerSource sourceController;
    unsigned int answerIndex;
    sourceAnswerCombi(
      playerSource newPS=playerSource(), 
      unsigned int newAI=0
      );
    /*!
     * \brief create an answercombi by pressed key.
     * 
     * (not found) is indicated by a "givenAnswer" of zero
     */
    sourceAnswerCombi(SDLKey pressedKey);
    sourceAnswerCombi(SDL_JoyButtonEvent joystickEvent);
  };
  
}; // namespace quiz_sources



#endif // _QUIZ_SOURCES_H
