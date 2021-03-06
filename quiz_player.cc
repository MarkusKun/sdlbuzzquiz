#include <sstream>
#include <iostream>

#include <utility> // for pair
#include <set>

#include "quiz_config.h"
#include "quiz_player.h"

extern quiz_config myQuizConfig;

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

bool quiz_player::players::empty()const{
  return source2playerMap.empty();
}  

bool quiz_player::players::hasEveryoneAnswered(){
  std::map<quiz_sources::playerSource,player*>::iterator player_iterator;
  for (
    player_iterator  = source2playerMap.begin();
    player_iterator != source2playerMap.end();
    player_iterator++
    )
  {
    if (0==player_iterator->second->givenAnswer){ // has not yet answered
      return false;
    }
  }
  // no player has not yet answered:
  return true;
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
: playerName(newPlayerName), responseTime(0), givenAnswer(0), sumPoints(0), plusPoints(0)
{
  // again, no constructor body needed
}

void quiz_player::players::calculatePoints(const unsigned int correctAnswerIndex){
  // a real point distribution still has to be found
  std::multimap<unsigned int, player*> timeSortedCorrectPlayers;
  std::multimap<unsigned int, player*> timeSortedInCorrectPlayers;
  std::set<player*> nonAnswerPlayers;
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
          nonAnswerPlayers.insert(currentPlayer);
          // award no negative points
        }else{ // wrong answer given
          timeSortedInCorrectPlayers.insert (std::pair<unsigned int, player*>(currentPlayer->responseTime,currentPlayer));
        }
      } // not correct answer
    } // for all players
  }
  { // now award points according to this list
    std::multimap<unsigned int, player*>::const_iterator player_iterator;
    std::vector<int>::const_iterator score_iterator;
    
    { // points for correct
      score_iterator = myQuizConfig.points_for_correct.begin();
      for (
        player_iterator  = timeSortedCorrectPlayers.begin();
        player_iterator != timeSortedCorrectPlayers.end();
        player_iterator++,score_iterator++
        )
      {
        if (myQuizConfig.points_for_correct.end()==score_iterator){
          score_iterator--;
        }
        player* currentPlayer = player_iterator->second;
        
        currentPlayer->plusPoints = (*score_iterator);
      }
    }
    { // points for incorrect
      score_iterator = myQuizConfig.points_for_incorrect.begin();
      for (
        player_iterator  = timeSortedInCorrectPlayers.begin();
        player_iterator != timeSortedInCorrectPlayers.end();
        player_iterator++,score_iterator++
        )
      {
        if (myQuizConfig.points_for_incorrect.end()==score_iterator){
          score_iterator--;
        }
        player* currentPlayer = player_iterator->second;
        // this number may get configurable
        currentPlayer->plusPoints = (*score_iterator);
      }
    } // points for incorrect
    { // points for noanswer
      std::set<player*>::const_iterator player_iterator;
      for(
        player_iterator  = nonAnswerPlayers.begin();
        player_iterator != nonAnswerPlayers.end();
        player_iterator++
        )
      {
        (*player_iterator)->plusPoints = myQuizConfig.points_for_nothing;
      }
    } // no answer
  } // award the points
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
    int currentPlayerPoints = currentPlayer->sumPoints;
    { // calculate new point number
      // points may decay
      currentPlayerPoints -= (currentPlayerPoints * myQuizConfig.points_decay) / 100;
      // add new points
      currentPlayerPoints += currentPlayer->plusPoints;
    }
    currentPlayer->sumPoints = currentPlayerPoints;
    currentPlayer->plusPoints = 0;
  }
}


