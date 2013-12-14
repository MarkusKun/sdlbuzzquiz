#include <fstream>
#include <iostream>
#include <sstream>

#include <string.h>

#include "quiz_question.h"

#include "rapidxml.hpp"

quiz_question::questions quiz_question::readFile(std::string questionFile){
  questions returnVector;

  using namespace rapidxml;
  xml_document<> doc;    // character type defaults to char
  
  { // read file into doc
    std::stringstream fileText;
    { // read file into text
      std::fstream inFile;
      inFile.open(questionFile.c_str());
      if (!inFile.is_open()){
        std::cerr << "Error on opening file" << std::endl;
        return returnVector;
      }
      while (!inFile.eof()){
        std::string readLine;
        std::getline(inFile,readLine);
        fileText << readLine;
      }
      inFile.close();
    }
    char* fileText_c = new char[fileText.str().size()+1];
    strcpy(fileText_c,fileText.str().c_str());
    doc.parse<0>(fileText_c);    // 0 means default parse flags
  }
  xml_node<>* rootElement = doc.first_node();
  if ("questions" != (std::string)(rootElement->name())){
    std::cerr << "XML document malformed" << std::endl;
    return returnVector;
  }
  xml_node<>* questionElement;
  for (
    questionElement  = rootElement->first_node("question");
    questionElement != NULL;
    questionElement  = questionElement->next_sibling("question")
    )
  {
    question newQuestion;
    
    xml_node<>* questionStringElem = questionElement->first_node("questionString");
    if (NULL == questionStringElem){
      std::cerr << "XML document malformed" << std::endl;
      return returnVector;
    }
    newQuestion.questionString = questionStringElem->value();
    unsigned int answerIndex=1;
    xml_node<>* answerStringElem;
    for (
      answerStringElem  = questionElement->first_node("answerString");
      answerStringElem != NULL;
      answerStringElem  = answerStringElem->next_sibling("answerString"), answerIndex++
      )
    {
      if (answerIndex > 4){
        std::cerr << "too many answers" << std::endl;
        break;
      }
      newQuestion.answerStrings[answerIndex-1]=answerStringElem->value();
      if (NULL != answerStringElem->first_attribute("correctAnswer")){
        newQuestion.correctAnswerIndex = answerIndex;
      }
    }
    returnVector.push_back(newQuestion);
  } // for all "question" element

  
  return returnVector;
}

