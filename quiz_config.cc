#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h> // strcpy


#include "quiz_config.h"

//#define DEBUG_CONFIGREADER


quiz_config::quiz_config(){
  // this is a default config - to be overwritten, later
  randomize_questions = true;
  randomize_answers   = true;
  preshow_answers     = false;
  allow_redecision    = false;
  reuse_questions     = true;
  points_for_correct.push_back(0);
  points_for_incorrect.push_back(0);
  points_for_nothing = 0;
  points_decay = 0;
  
  questionTime = 15;
  answerTime = 6;
  earlyFinish         = true;
  
}

int convertInteger(std::string textValue){
  int returnValue;
  std::stringstream convertStream;
  convertStream << textValue;
  convertStream >> std::dec >> returnValue;
  return returnValue;
}

bool convertBoolean(std::string textValue){
  if ("true" == textValue){
    return true;
  }
  if ("false" == textValue){
    return false;
  }
  std::cerr << "Value " << textValue << " not accepted as boolean" << std::endl;
  return false;
}

void quiz_config::extractScoring(rapidxml::xml_node<>* scoringNode){
  // before reading new scoring, delete the old one
  points_for_correct.clear();
  points_for_incorrect.clear();
  points_for_nothing = 0;
  rapidxml::xml_node<>* scoreElement;
  for (
    scoreElement  = scoringNode->first_node();
    scoreElement != NULL;
    scoreElement  = scoreElement->next_sibling()
    )
  {
    std::string configName = scoreElement->name();
    std::string configValue = scoreElement->value();
    if ("correct" == configName){
      points_for_correct.push_back(convertInteger(configValue));
    }
    else if ("incorrect" == configName){
      points_for_incorrect.push_back(convertInteger(configValue));
    }
    else if ("nothing" == configName){
      points_for_nothing = convertInteger(configValue);
    }
    else if ("decay" == configName){
      points_decay = convertInteger(configValue);
    }
    else{
      std::cerr << "Unknown scoring option: " << configName << std::endl;
    }
  }
  // if no scoring is given, assume zero points
  if (points_for_correct.empty()){
    points_for_correct.push_back(0);
  }
  if (points_for_incorrect.empty()){
    points_for_incorrect.push_back(0);
  }
}
void quiz_config::extractTiming(rapidxml::xml_node<>* configNode){
  rapidxml::xml_node<>* configElement;
  for (
    configElement  = configNode->first_node();
    configElement != NULL;
    configElement  = configElement->next_sibling()
    )
  {
    std::string configName = configElement->name();
    std::string configValue = configElement->value();
    #ifdef DEBUG_CONFIGREADER
    std::cout << configName << " -> " << configValue << std::endl;
    #endif
    if ("questionTime" == configName){
      questionTime = convertInteger(configValue);
    }
    else if("answerTime" == configName){
      answerTime = convertInteger(configValue);
    }
    else if("earlyFinish" == configName){
      earlyFinish = convertBoolean(configValue);
    }
    else{
      std::cerr << "Unknown Configuration Option: " << configName << std::endl;
    }
  }
} // function extractTiming

void quiz_config::extractGraphics(rapidxml::xml_node<>* configNode){
  rapidxml::xml_node<>* configElement;
  for (
    configElement  = configNode->first_node();
    configElement != NULL;
    configElement  = configElement->next_sibling()
    )
  {
    std::string configName = configElement->name();
    std::string configValue = configElement->value();
    #ifdef DEBUG_CONFIGREADER
    std::cout << configName << " -> " << configValue << std::endl;
    #endif
    if ("screenwidth" == configName){
      screenwidth = convertInteger(configValue);
    }
    else if("screenheight" == configName){
      screenheight = convertInteger(configValue);
    }
    else if("fullscreen" == configName){
      fullscreen = convertBoolean(configValue);
    }
    else if("fontsize" == configName){
      fontsize = convertInteger(configValue);
    }
    else{
      std::cerr << "Unknown Configuration Option: " << configName << std::endl;
    }
  }
} // function extractTiming

void quiz_config::extractConfig(rapidxml::xml_node<>* configNode){
  rapidxml::xml_node<>* configElement;
  for (
    configElement  = configNode->first_node();
    configElement != NULL;
    configElement  = configElement->next_sibling()
    )
  {
    std::string configName = configElement->name();
    std::string configValue = configElement->value();
    #ifdef DEBUG_CONFIGREADER
    std::cout << configName << " -> " << configValue << std::endl;
    #endif
    if ("randomize_questions" == configName){
      randomize_questions = convertBoolean(configValue);
    }
    else if("randomize_answers" == configName){
      randomize_answers = convertBoolean(configValue);
    }
    else if("preshow_answers" == configName){
      preshow_answers = convertBoolean(configValue);
    }
    else if("allow_redecision" == configName){
      allow_redecision = convertBoolean(configValue);
    }
    else if("reuse_questions" == configName){
      reuse_questions = convertBoolean(configValue);
    }
    else if("import" == configName){
      read(configValue);
    }
    else if ("scoring" == configName){
      extractScoring(configElement);
    }
    else if ("timing" == configName){
      extractTiming(configElement);
    }
    else if ("graphics" == configName){
      extractGraphics(configElement);
    }
    else{
      std::cerr << "Unknown Configuration Option: " << configName << std::endl;
    }
  }
  
}

void quiz_config::extractQuestions(rapidxml::xml_node<>* configNode){
  using namespace rapidxml;
  xml_node<>* questionElement;
  for (
    questionElement  = configNode->first_node("question");
    questionElement != NULL;
    questionElement  = questionElement->next_sibling("question")
    )
  {
    quiz_question::question newQuestion;
    
    xml_node<>* questionStringElem = questionElement->first_node("questionString");
    if (NULL == questionStringElem){
      std::cerr << "Question had no question" << std::endl;
      return;
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
    quizQuestions.push_back(newQuestion);
  } // for all "question" element
}


void quiz_config::read(std::string xmlfilename){
  #ifdef DEBUG_CONFIGREADER
  std::cout << "Reading " << xmlfilename << " as config" << std::endl;
  #endif
  using namespace rapidxml;
  xml_document<> doc; // character type is char (default
  
  { // read the file into doc
    std::stringstream fileText;
    { // read file into text
      std::fstream inFile;
      inFile.open(xmlfilename.c_str());
      if (!inFile.is_open()){
        std::cerr << "Error on opening file " << xmlfilename << std::endl;
        return;
      }
      while (!inFile.eof()){
        std::string readLine;
        std::getline(inFile,readLine);
        fileText << readLine;
      }
      inFile.close();
    }
    /*
     * rapidxml needs work on a non-const c-string for maximul
     * potential
     */
    char* fileText_c = new char[fileText.str().size()+1];
    // so copy from stl-string to c-string
    strcpy(fileText_c,fileText.str().c_str());
    doc.parse<0>(fileText_c); // 0 means using default flags
  }
  xml_node<>* rootElement = doc.first_node();
  if ("questions" == (std::string)(rootElement->name())){
    extractQuestions(rootElement);
  }
  
  else if ("gameconfig" == (std::string)(rootElement->name())){
    #ifdef DEBUG_CONFIGREADER
    std::cout << "gameconfig-rootelem found" << std::endl;
    #endif
    extractConfig(rootElement);
  }
  else{
    std::cerr << "Config file not as expected. Found: " << ((std::string)rootElement->name()) << std::endl;
    return;
  }
}




