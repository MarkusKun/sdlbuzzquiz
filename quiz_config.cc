#include <iostream>
#include <fstream>
#include <sstream>

#include <string.h> // strcpy

#include "rapidxml.hpp"

#include "quiz_config.h"



quiz_config::quiz_config(){
  // this is a default config - to be overwritten, later
  randomize_questions = true;
  randomize_answers   = true;
  preshow_answers     = false;
  allow_redecision    = false;
  points_for_correct_after = 1;
  points_for_incorrect_after = 0;
}

void quiz_config::read(std::string xmlfilename){
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
    // jump into config reader TODO
  }
  else{
    std::cerr << "Config file not as expected. Found: " << ((std::string)rootElement->name()) << std::endl;
    return;
  }
}




