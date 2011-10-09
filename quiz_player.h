#ifndef _QUIZ_PLAYER_H
#define _QUIZ_PLAYER_H

#include <string>
#include <vector>
#include <map>

#include <SDL.h> // for SDLKey

#include "quiz_sources.h"

namespace quiz_player{
  struct player{
    std::string playerName;
    unsigned int responseTime; // milliseconds
    unsigned int givenAnswer; // 0: no answer given yet
    int sumPoints; // sum of points
    int plusPoints; // new points for this question
    player(std::string newPlayerName = "NoName");
  };
  
  
  /*
   * controllerIndex can be
   *  index of keyboardPlayers
   * or 
   *  buzz_major(number of usb-controller)*4+
   *  buzz_minor(buzzer on this usb)
   * or 
   *  network index.. (if this is adapted at some point
   */

  typedef std::vector<player*> players_t;
  
  
  
  class players{
    private:
      std::map<quiz_sources::playerSource,player*> source2playerMap; 
    public:
      players_t getPlayerList() const;
      //! returns an index for this player
      unsigned int getPlayerNumber(player* searchPlayer)const;
      //void addPlayer(quiz_sources::playerSource myPlayerSource,player* newPlayer);
      //! create with zero points and without response
      void addPlayer(quiz_sources::playerSource myPlayerSource,std::string newPlayerName);
      
      /*!
       * \brief remove the given player
       *
       * This function removes the player from the map (if she exists in map)
       * and also deletes the player object. Afterwards, the pointer is 
       * no longer valid!
       */
      void removePlayer(player* toDelPlayer);
      //! set given answers and newpoints to 0 (call before a question)
      void clearAnswers();
      //! number of players
      unsigned int size()const;
      //! calculate points for all players
      void calculatePoints(const unsigned int correctAnswerIndex);
      //! award points to all players who deserve them
      void awardPoints();
      
      std::string debugPrintKnown();
      /*!
       * this function is authorized to create the player if needed.
       */
      player* getPlayerBySource(
        quiz_sources::playerSource myPlayerSource
        );
  }; // class players
  
}; // namespace quiz_player
#endif // _QUIZ_PLAYER_H
