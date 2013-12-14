#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h> // strcpy


#include "quiz_config.h"

#define DEBUG_CONFIGREADER


quiz_config::quiz_config(){
  // this is a default config - to be overwritten, later
  randomize_questions = true;
  randomize_answers   = true;
  preshow_answers     = false;
  allow_redecision    = false;
  points_for_correct_after = 1;
  points_for_incorrect_after = 0;
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
    else if("import" == configName){
      read(configValue);
    }
    else{
      std::cerr << "Unknown Configuration Option: " << configName << std::endl;
    }
  }
  
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
    // jump into question reader TODO
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




