#ifndef _QUIZ_CONFIG_H
#define _QUIZ_CONFIG_H

#include <vector>
#include <string>


#include "rapidxml.hpp"

#include "quiz_question.h"

class quiz_config{
  public: 
    bool randomize_questions;
    bool randomize_answers;
    bool preshow_answers;
    bool allow_redecision;
    bool reuse_questions;
    std::vector<int> points_for_correct;
    std::vector<int> points_for_incorrect;
    int points_for_nothing;
    unsigned int points_decay;
    
    
    quiz_question::questions quizQuestions; // vector of question
    
    unsigned int questionTime;
    unsigned int answerTime;
    bool earlyFinish;
    
    
    unsigned int screenwidth;
    unsigned int screenheight;
    bool fullscreen;
    unsigned int fontsize;
    
    quiz_config();
    
    void read(std::string xmlfilename);
    void extractConfig(rapidxml::xml_node<>* configNode);
    void extractQuestions(rapidxml::xml_node<>* configNode);
    void extractScoring(rapidxml::xml_node<>* scoringNode);
    void extractTiming(rapidxml::xml_node<>* configNode);
    void extractGraphics(rapidxml::xml_node<>* configNode);
    
}; // class quiz_config
bool convertBoolean(std::string textValue);
int convertInteger(std::string textValue);
  
#endif // _QUIZ_CONFIG_H

