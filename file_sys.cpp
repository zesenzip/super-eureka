// $Id: file_sys.cpp,v 1.7 2019-07-09 14:05:44-07 - - $

#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <iomanip>

using namespace std;

#include "debug.h"
#include "file_sys.h"

int inode::next_inode_nr {1};

struct file_type_hash {
   size_t operator() (file_type type) const {
      return static_cast<size_t> (type);
   }
};

ostream& operator<< (ostream& out, file_type type) {
   static unordered_map<file_type,string,file_type_hash> hash {
      {file_type::PLAIN_TYPE, "PLAIN_TYPE"},
      {file_type::DIRECTORY_TYPE, "DIRECTORY_TYPE"},
   };
   return out << hash[type];
}

inode_state::inode_state() {
   root = make_shared<inode>(file_type::DIRECTORY_TYPE);
   cwd = root;
   root->contents->addEntry(".", root);
   root->contents->addEntry("..", root);
   DEBUGF ('i', "root = " << root << ", cwd = " << cwd
          << ", prompt = \"" << prompt() << "\"");
}

inode_state::~inode_state(){
   root->get_contents()->harakiri();
   cwd = nullptr;
}
const string& inode_state::prompt() const { return prompt_; }

ostream& operator<< (ostream& out, const inode_state& state) {
   out << "inode_state: root = " << state.root
       << ", cwd = " << state.cwd;
   return out;
}

inode::inode(file_type type): inode_nr (next_inode_nr++) {
   switch (type) {
      case file_type::PLAIN_TYPE:
           contents = make_shared<plain_file>();
           break;
      case file_type::DIRECTORY_TYPE:
           contents = make_shared<directory>();
           break;
   }
   DEBUGF ('i', "inode " << inode_nr << ", type = " << type);
}

int inode::get_inode_nr() const {
   DEBUGF ('i', "inode = " << inode_nr);
   return inode_nr;
}


file_error::file_error (const string& what):
            runtime_error (what) {
}
// plain only
const wordvec& base_file::readfile() const {
   throw file_error ("is a " + error_file_type());
}
// plain only
void base_file::writefile (const wordvec&) {
   throw file_error ("is a " + error_file_type());
}
// directory only
void base_file::wipe(){
   throw file_error ("is a " + error_file_type());
}
// directory only
void base_file::remove (const string&) {
   throw file_error ("is a " + error_file_type());
}
// directory only
inode_ptr base_file::mkdir (const string&) {
   throw file_error ("is a " + error_file_type());
}
// directory only
inode_ptr base_file::mkfile (const string&) {
   throw file_error ("is a " + error_file_type());
}
// directory only
void base_file::addEntry(
   const string&, const inode_ptr) {
      throw file_error ("is a " + error_file_type());
}
// directory only
void base_file::harakiri( ){
   throw file_error ("is a " + error_file_type());
}
// directory only
inode_ptr base_file::find(const string&){
   throw file_error ("is a " + error_file_type());
}
// directory only
string base_file::find_string(const inode_ptr ){
   throw file_error ("is a " + error_file_type());
}
void base_file::print_dir(){
   throw file_error ("is a " + error_file_type());
}
void base_file::megaprint(const string){
   throw file_error ("is a " + error_file_type());
}


size_t plain_file::size() const {
   size_t size {0};
   if (data.size() == 0)
      return size;
   for (auto rit = data.begin(); rit != data.end(); ++rit){
      size += (*rit).size();
   }
   size_t spaces {data.size()-1};
   size += spaces;
   DEBUGF ('i', "size = " << size);
   return size;
}

const wordvec& plain_file::readfile() const {
   DEBUGF ('i', data);
   return data;
}

void plain_file::writefile (const wordvec& words) {
   DEBUGF ('i', words);
   data = words;
}

size_t directory::size() const {
   size_t size {0};
   DEBUGF ('i', "size = " << size);
   size = dirents.size();
   return size;
}
void directory::wipe(){
   dirents.clear();
}
void directory::remove (const string& filename) {
   DEBUGF ('i', filename);
   if(filename == "." || filename == ".."){
      return;
   }
   inode_ptr node;
   base_file_ptr contents;
   auto rit = dirents.find(filename);
   if(rit != dirents.end()){
      node = (*rit).second;
      contents = node->get_contents();
   }
   else
      return;
   contents = node->get_contents();
   shared_ptr<directory> ptr
      = dynamic_pointer_cast<directory>(contents);
   if(ptr != nullptr ){
      if(ptr->size() <= 2){
         ptr->wipe();
         dirents.erase(rit);
      }
   }
   else{
      dirents.erase(rit);
   }   
}
// return nullptr if key already exists
// maps allow multiple items with the same key.
inode_ptr directory::mkdir (const string& dirname) {
   DEBUGF ('i', dirname);
   auto rit = dirents.find(dirname);
   if(rit == dirents.end()){
      inode_ptr new_node = 
         make_shared<inode>(file_type::DIRECTORY_TYPE);
      addEntry(dirname, new_node);
      base_file_ptr new_dir = new_node->get_contents();
      new_dir->addEntry(".", new_node);
      // assumes parent has correct "."
      new_dir->addEntry("..", dirents["."]);
      return new_node;
   }
   return nullptr;
}

inode_ptr directory::mkfile (const string& filename) {
   DEBUGF ('i', filename);
   auto rit = dirents.find(filename);
   if(rit == dirents.end()){
      inode_ptr new_node = 
         make_shared<inode>(file_type::PLAIN_TYPE);
      addEntry(filename, new_node);
      DEBUGF('i', new_node);
      return new_node;
   }
   return nullptr;
}

void directory::addEntry(
   const string& filename, const inode_ptr node) {
   //dirents.insert(std::pair<string,inode_ptr>(filename,node));
   dirents[filename] = node;
   DEBUGF ('i', filename << ":" << node);
   return;
}
void directory::harakiri(){
   for (auto rit = dirents.begin(); rit != dirents.end(); ++rit){
      if((*rit).first == "." || (*rit).first == "..")
         continue;
      inode_ptr node = (*rit).second;
      base_file_ptr contents = node->get_contents();

      shared_ptr<directory> ptr = 
         dynamic_pointer_cast<directory>(contents);
      if(ptr != nullptr)
         ptr->harakiri();
   }
   wipe();
   return;
}
// return nullptr if not found
inode_ptr directory::find(const string& foldername){
   auto rit = dirents.find(foldername);
   if(rit != dirents.end()){
      return rit->second;
      DEBUGF('i', rit->second);
   }
   else
      return nullptr;
}
// finds string base on inode
string directory::find_string(const inode_ptr node){
   auto rit = dirents.begin();
   while(rit != dirents.end()){
      if(rit->second == node)
         return rit->first;
      else
         rit++;
   }
   return "";
}

void directory::print_dir(){
   for (auto rit = dirents.begin(); rit != dirents.end(); ++rit){
      inode_ptr node = (*rit).second;
      string filename = (*rit).first;
      shared_ptr<directory> ptr;

      cout << setw(6) << node->get_inode_nr();
      base_file_ptr contents = node->get_contents();
      cout << setw(6) << contents->size();

      ptr = dynamic_pointer_cast<directory>(contents);
      // directories need a trailing slash
      // excludes . and ..
      if(ptr != nullptr){
         filename = filename + "/";
      }
      cout << " " << filename << endl;
   } 
}

void directory::megaprint(const string path){
   cout << path << ':' << endl;
   print_dir();
   for (auto rit = dirents.begin(); rit != dirents.end(); ++rit){
      if((*rit).first == "." || (*rit).first == "..")
         continue;
      inode_ptr node = (*rit).second;
      base_file_ptr contents = node->get_contents();

      shared_ptr<directory> ptr = 
         dynamic_pointer_cast<directory>(contents);
      if(ptr != nullptr)
         ptr->megaprint( path + "/" +(*rit).first);
   }
   return;
}

