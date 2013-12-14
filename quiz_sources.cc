#include <map>

#include "quiz_sources.h"

#include "buzzer.h"

bool quiz_sources::playerSource::operator<(
  const quiz_sources::playerSource& vsPlayerSource
  )const
{
  //std::cout << "compare " << sourceType << "." << controllerIndex;
  //std::cout << " vs" << vsPlayerSource.sourceType << "." << vsPlayerSource.controllerIndex;
  if (sourceType < vsPlayerSource.sourceType){
    //std::cout << " smaller sourceType" << std::endl;
    return true;
  }
  // this "equals"-check is necessary, as
  if (sourceType == vsPlayerSource.sourceType){
    if (controllerIndex < vsPlayerSource.controllerIndex){
      //std::cout << " smaller Index" << std::endl;
      return true;
    }
  }
  //std::cout << " not smaller" << std::endl;
  return false;
}

quiz_sources::playerSource::playerSource(
  sourceType_t     newSourceType,
  unsigned int     newControllerIndex
  )
: sourceType(newSourceType), controllerIndex(newControllerIndex)
{
  // no function body needed - I like initlists
}

quiz_sources::controllerAnswerCombi::controllerAnswerCombi(
  unsigned int newSCI,
  unsigned int newGA
  )
: sourceControllerIndex(newSCI), givenAnswer(newGA)
{
  // again, no body needed
}


quiz_sources::sourceAnswerCombi::sourceAnswerCombi(
  playerSource newPS, 
  unsigned int newAI
  )
: sourceController(newPS), answerIndex(newAI)
{
  // all done in initlist
}

quiz_sources::sourceAnswerCombi::sourceAnswerCombi(SDLKey pressedKey){
  static std::map<SDLKey,controllerAnswerCombi> keyPlayerMapping;
  if (keyPlayerMapping.empty()){ // if it doesn't exist, create it
    keyPlayerMapping[SDLK_2]=controllerAnswerCombi(0,1);
    keyPlayerMapping[SDLK_w]=controllerAnswerCombi(0,2);
    keyPlayerMapping[SDLK_s]=controllerAnswerCombi(0,3);
    keyPlayerMapping[SDLK_x]=controllerAnswerCombi(0,4);
    keyPlayerMapping[SDLK_3]=controllerAnswerCombi(1,1);
    keyPlayerMapping[SDLK_e]=controllerAnswerCombi(1,2);
    keyPlayerMapping[SDLK_d]=controllerAnswerCombi(1,3);
    keyPlayerMapping[SDLK_c]=controllerAnswerCombi(1,4);
    keyPlayerMapping[SDLK_4]=controllerAnswerCombi(2,1);
    keyPlayerMapping[SDLK_r]=controllerAnswerCombi(2,2);
    keyPlayerMapping[SDLK_f]=controllerAnswerCombi(2,3);
    keyPlayerMapping[SDLK_v]=controllerAnswerCombi(2,4);
    keyPlayerMapping[SDLK_5]=controllerAnswerCombi(3,1);
    keyPlayerMapping[SDLK_t]=controllerAnswerCombi(3,2);
    keyPlayerMapping[SDLK_g]=controllerAnswerCombi(3,3);
    keyPlayerMapping[SDLK_b]=controllerAnswerCombi(3,4);
  }
  std::map<SDLKey,controllerAnswerCombi>::const_iterator map_searcher;
  map_searcher=keyPlayerMapping.find(pressedKey);
  if (keyPlayerMapping.end()==map_searcher){
    // key is no player
    (*this)=sourceAnswerCombi();
  }
  sourceController.sourceType=PLAYERSOURCETYPE_KEYBOARD;
  sourceController.controllerIndex= map_searcher->second.sourceControllerIndex;
  answerIndex = map_searcher->second.givenAnswer;
}

quiz_sources::sourceAnswerCombi::sourceAnswerCombi(SDL_JoyButtonEvent joystickEvent){
  /*
   * this always assumes that a joystick is a buzzer. We might want to add some special
   * handling for non-buzzers, later
   */
  // four buzzers exist on one joystick. This is the joystick number (so 4 can have it)
  unsigned int buzzerJoystickIndex = (int) joystickEvent.which;
  
  sourceController.sourceType=PLAYERSOURCETYPE_BUZZ;
  sourceController.controllerIndex = buzzerJoystickIndex*4+buzz_identify_buzzer(joystickEvent.button);
  switch (buzz_identify_button(joystickEvent.button)){
    case (BUZZ_RED): // no real answer - but we might need this for calling admin.
      {
        answerIndex = 0;
        break;
      }
    case (BUZZ_YELLOW):
      {
        answerIndex=4;
        break;
      }
      
    case (BUZZ_GREEN):
      {
        answerIndex=3;
        break;
      }
    case (BUZZ_ORANGE):
      { 
        answerIndex=2;
        break;
      }
    case (BUZZ_BLUE):
      {
        answerIndex=1;
        break;
      }
    default:
      {
        answerIndex=0;
        break;
      }
  }
  
}

