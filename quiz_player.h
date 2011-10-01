#ifndef _QUIZ_PLAYER_H
#define _QUIZ_PLAYER_H

#include <string>
#include <vector>
#include <map>

namespace quiz_player{
  struct player{
    std::string playerName;
    unsigned int responseTime; // milliseconds
    unsigned int givenAnswer; // 0: no answer given yet
    unsigned int sumPoints; // sum of points
  };
  
  enum playerSourceType{
    PLAYERSOURCETYPE_UNKNOWN,
    PLAYERSOURCETYPE_KEYBOARD,
    PLAYERSOURCETYPE_BUZZ,
    PLAYERSOURCETYPE_NETWORK // DS players?
  };
  
  struct playerSource{
    playerSourceType  sourceType;
    unsigned int      controllerIndex;
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
      std::map<playerSource,player*> source2playerMap; 
    public:
      players_t getPlayerList() const;
      //! returns an index for this player
      unsigned int getPlayerNumber(player* searchPlayer);
      void addPlayer(playerSource myPlayerSource,player* newPlayer);
      void removePlayer(player* toDelPlayer);
      void removePlayer(playerSource toDelSource);
  }; // class players
  
}; // namespace quiz_player
#endif // _QUIZ_PLAYER_H
