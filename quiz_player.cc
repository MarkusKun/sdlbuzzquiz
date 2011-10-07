#include <sstream>
#include <iostream>

#include <utility> // for pair
#include "quiz_player.h"

void quiz_player::players::clearAnswers(){
  std::map<quiz_sources::playerSource,player*>::iterator player_iterator;
  for (
    player_iterator  = source2playerMap.begin();
    player_iterator != source2playerMap.end();
    player_iterator++
    )
  {
    player_iterator->second->givenAnswer = 0;
    player_iterator->second->plusPoints  = 0;
  }
}

unsigned int quiz_player::players::size()const{
  return source2playerMap.size();
}

quiz_player::players_t quiz_player::players::getPlayerList() const{
  players_t returnVector;
  std::map<quiz_sources::playerSource,player*>::const_iterator player_iterator;
  for (
    player_iterator  = source2playerMap.begin();
    player_iterator != source2playerMap.end();
    player_iterator++
    )
  {
    returnVector.push_back(player_iterator->second);
  }
  return returnVector;
}

void quiz_player::players::addPlayer(quiz_sources::playerSource myPlayerSource,std::string newPlayerName){
  // normally, we should check that there is no collision.
  player* newPlayer = new player(newPlayerName);
  source2playerMap[myPlayerSource]=newPlayer;
}


void quiz_player::players::removePlayer(quiz_player::player* toDelPlayer){
  std::map<quiz_sources::playerSource,player*>::iterator mapIterator;
  for (
    mapIterator  = source2playerMap.begin();
    mapIterator != source2playerMap.end();
    mapIterator++
    )
  {
    if (toDelPlayer == mapIterator->second){
      source2playerMap.erase(mapIterator);
      return;
    }
  }
  delete toDelPlayer;
}

quiz_player::player* quiz_player::players::getPlayerBySource(
  quiz_sources::playerSource myPlayerSource
  )
{
  std::map<quiz_sources::playerSource,player*>::const_iterator player_finder;
  player_finder = source2playerMap.find(myPlayerSource);
  if (source2playerMap.end() != player_finder){ // found something
    return player_finder->second;
  }else{ // found nothing -> so create
    std::string newPlayerName;
    { // take number from controller index
      std::stringstream convertStream;
      convertStream << std::dec << myPlayerSource.controllerIndex;
      convertStream >> newPlayerName;
    }
    switch (myPlayerSource.sourceType){
      case (quiz_sources::PLAYERSOURCETYPE_KEYBOARD):
        {
          newPlayerName = "KeyboardPlayer"+newPlayerName;
          break;
        }
      case (quiz_sources::PLAYERSOURCETYPE_BUZZ):
        {
          newPlayerName = "BuzzPlayer"+newPlayerName;
          break;
        }
      default:
        {
          newPlayerName = "NoPlayer"+newPlayerName;
          break;
        }
    } // switch
    addPlayer(myPlayerSource,newPlayerName);
    return (getPlayerBySource(myPlayerSource));
  }
  return NULL; // should never be reached
}

std::string quiz_player::players::debugPrintKnown(){
  std::stringstream infoStream;
  std::map<quiz_sources::playerSource,player*>::const_iterator player_iterator;
  for (
    player_iterator  = source2playerMap.begin();
    player_iterator != source2playerMap.end();
    player_iterator++
    )
  {
    infoStream << "Source " << player_iterator->first.sourceType;
    infoStream << "." << player_iterator->first.controllerIndex;
    infoStream << " -> " << player_iterator->second << std::endl;
  }
  return infoStream.str();
}
  
quiz_player::player::player(std::string newPlayerName) 
: playerName(newPlayerName), responseTime(0), givenAnswer(0), sumPoints(0)
{
  // again, no constructor body needed
}

void quiz_player::players::calculatePoints(const unsigned int correctAnswerIndex){
  // a real point distribution still has to be found
  std::multimap<unsigned int, player*> timeSortedCorrectPlayers;
  std::multimap<unsigned int, player*> timeSortedInCorrectPlayers;
  { // create timesorted list of players
    std::map<quiz_sources::playerSource,player*>::const_iterator player_iterator;
    for (
      player_iterator  = source2playerMap.begin();
      player_iterator != source2playerMap.end();
      player_iterator++
      )
    {
      player* currentPlayer = player_iterator->second;
      if (correctAnswerIndex == currentPlayer->givenAnswer){ // player has answered, correctly
        timeSortedCorrectPlayers.insert (std::pair<unsigned int, player*>(currentPlayer->responseTime,currentPlayer));
      }else{ // not correct answer
        if (0==currentPlayer->givenAnswer){ // no answer given
          // award no negative points
        }else{ // wrong answer given
          timeSortedInCorrectPlayers.insert (std::pair<unsigned int, player*>(currentPlayer->responseTime,currentPlayer));
        }
      } // not correct answer
    } // for all players
  }
  { // now award points according to this list
    std::multimap<unsigned int, player*>::const_iterator player_iterator;
    int player_points = 1000;
    for (
      player_iterator  = timeSortedCorrectPlayers.begin();
      player_iterator != timeSortedCorrectPlayers.end();
      player_iterator++
      )
    {
      player* currentPlayer = player_iterator->second;
      // this number has to get time/order-dependent
      currentPlayer->plusPoints = player_points;
      player_points /= 2;
    }
    player_points=1; // no penalty for incorrect answer
    for (
      player_iterator  = timeSortedInCorrectPlayers.begin();
      player_iterator != timeSortedInCorrectPlayers.end();
      player_iterator++
      )
    {
      player* currentPlayer = player_iterator->second;
      // this number may get configurable
      currentPlayer->plusPoints = player_points;
    }
  }
}
void quiz_player::players::awardPoints(){
  std::map<quiz_sources::playerSource,player*>::const_iterator player_iterator;
  for (
    player_iterator  = source2playerMap.begin();
    player_iterator != source2playerMap.end();
    player_iterator++
    )
  {
    player* currentPlayer = player_iterator->second;
    currentPlayer->sumPoints += currentPlayer->plusPoints;
    currentPlayer->plusPoints = 0;
  }
}


