#ifndef _QUIZ_CONFIG_H
#define _QUIZ_CONFIG_H

#include <map>
#include <string>

class quiz_config{
  public: 
    bool randomize_questions;
    bool randomize_answers;
    bool preshow_answers;
    bool allow_redecision;
    std::map<unsigned int, int> points_for_correct;
    std::map<unsigned int, int> points_for_incorrect;
    int points_for_correct_after;
    int points_for_incorrect_after;
    
    quiz_config();
    
    void read(std::string xmlfilename);
    
}; // class quiz_config
  
#endif // _QUIZ_CONFIG_H

