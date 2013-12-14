#ifndef _QUIZ_QUESTION_H
#define _QUIZ_QUESTION_H

#include <string>
#include <vector>

namespace quiz_question{
  struct question{
    std::string questionString;
    std::string answerStrings[4];
    unsigned int correctAnswerIndex; // 1->4 to check against given answers
  };
  
  typedef std::vector<question> questions;
  
  questions readFile(std::string questionFile);
  
  
}; // namespace





#endif // _QUIZ_QUESTION
