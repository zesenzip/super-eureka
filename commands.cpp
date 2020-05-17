// $Id: commands.cpp,v 1.18 2019-10-08 13:55:31-07 - - $

#include "commands.h"
#include "debug.h"

inode_ptr find_parent(inode_state& state ,const string& path);
command_hash cmd_hash {
   {"cat"   , fn_cat   },
   {"cd"    , fn_cd    },
   {"echo"  , fn_echo  },
   {"exit"  , fn_exit  },
   {"ls"    , fn_ls    },
   {"lsr"   , fn_lsr   },
   {"make"  , fn_make  },
   {"mkdir" , fn_mkdir },
   {"prompt", fn_prompt},
   {"pwd"   , fn_pwd   },
   {"rm"    , fn_rm    },
   {"rmr"   , fn_rmr   },
};

command_fn find_command_fn (const string& cmd) {
   // Note: value_type is pair<const key_type, mapped_type>
   // So: iterator->first is key_type (string)
   // So: iterator->second is mapped_type (command_fn)
   DEBUGF ('c', "[" << cmd << "]");
   const auto result = cmd_hash.find (cmd);
   if (result == cmd_hash.end()) {
      throw command_error (cmd + ": no such function");
   }
   return result->second;
}

command_error::command_error (const string& what):
            runtime_error (what) {
}

int exit_status_message() {
   int status = exec::status();
   cout << exec::execname() << ": exit(" << status << ")" << endl;
   return status;
}
void fn_cat (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   base_file_ptr parent_dir;
   if(words.size() < 2){
      cerr << words.at(0) << ": missing operand" << endl;
      return exec::status(1);
   }
   // has to print files for multiple file paths
   for (auto rit = words.begin()+1; rit != words.end(); ++rit){
      inode_ptr parent_node = find_parent(state, *rit);
      // checks if parent is found
      inode_ptr node;
      if(split(*rit,"/").size() == 0){
         node = state.get_root();
         parent_node = state.get_root();
      }
      else if(parent_node != nullptr){
         string filename = split(*rit,"/").back();
         parent_dir = parent_node->get_contents();
         node = parent_dir->find(filename);
      }
      // checks if node is found   
      if(node == nullptr || parent_node == nullptr){
         cerr << words.at(0) << ": ";
         cerr << *rit << ": No such file or directory" << endl;
         return exec::status(1); 
      }
      base_file_ptr base = node->get_contents();
      shared_ptr<plain_file> ptr = 
         dynamic_pointer_cast<plain_file>(base);
      //checks if node is plainfile
      if (ptr != nullptr){
         cout << base->readfile() << endl;
      }
      else{
         cerr << words.at(0) << ": ";
         cerr << *rit << ": This a directory" << endl;
         return exec::status(1); 
      }
   }
}
// split "/"
// split " "
void fn_cd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   base_file_ptr parent_dir;
   inode_ptr next_node;
   string foldername;
   if(words.size() == 1){
      state.set_cwd(state.get_root());
   }
   else{
      inode_ptr parent_node = find_parent(state, words.at(1));
      
      if(split(words.at(1),"/").size() == 0){
         next_node = state.get_root();
         parent_node = state.get_root();
      }
      else if (parent_node != nullptr){
         foldername = split(words.at(1),"/").back();
         parent_dir = parent_node->get_contents();
         next_node = parent_dir->find(foldername);
      }
      if(parent_node == nullptr || next_node == nullptr){
         cerr << words.at(0) << ": ";
         cerr << words.at(1) << ": No such file or directory" << endl;
         return exec::status(1);
      }
      base_file_ptr next_dir = next_node->get_contents();
      shared_ptr<directory> ptr = 
         dynamic_pointer_cast<directory>(next_dir);
      if(ptr == nullptr){
         cerr << words.at(0) << ": ";
         cerr << foldername << ": Not a directory" << endl;
         return exec::status(1);
      }
      state.set_cwd(next_node);
      return;
   }
   DEBUGF ('c', state);
}

void fn_echo (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   cout << word_range (words.cbegin() + 1, words.cend()) << endl;
}


void fn_exit (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   if(words.size() > 1){
      try{
         int exit_code = std::stoi(words.at(1));
         exec::status(exit_code);
      }catch (invalid_argument& error){
         exec::status(127);
      }
   }
   throw ysh_exit();
}

void fn_ls (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string foldername;
   inode_ptr parent_node;
   base_file_ptr parent_dir = nullptr;
   inode_ptr node = nullptr;
   shared_ptr<directory> ptr;
   base_file_ptr dir;
   if(words.size() == 1){
      node = state.get_cwd();
      dir = node->get_contents();
      parent_node = dir->find("..");
      parent_dir = parent_node->get_contents();
      foldername = parent_dir->find_string(node);
      ptr = dynamic_pointer_cast<directory>(dir);
      if(node == state.get_root())
         foldername = "";
      cout << '/' <<  foldername << ':' <<endl; 
      ptr->print_dir();
      return;
   }
   else{
      for (auto rit = words.begin()+1; rit != words.end(); ++rit){
         parent_node = find_parent(state, *rit);
         DEBUGF('c', parent_node);
         if(split(*rit,"/").size() == 0){
           node = state.get_root();
           parent_node = state.get_root();
         }
         else if(parent_node != nullptr){
            foldername = split(*rit,"/").back();
            parent_dir = parent_node->get_contents();
            node = parent_dir->find(foldername);
         }
         if(parent_node == nullptr || node == nullptr){
            cerr << words.at(0) << ": ";
            cerr << *rit << ": No such file or directory" << endl;
            return exec::status(1);
         }
         dir = node->get_contents();
         ptr = dynamic_pointer_cast<directory>(dir);
         if(ptr != nullptr){
            if(node == state.get_root())
               foldername = "";
            cout << '/' << foldername << ':' <<endl; 
            ptr->print_dir();
         }
         else {
            cerr << words.at(0) << ": ";
            cerr << foldername << ": Is not a directory" << endl;
         }
      }
   }
}


void fn_lsr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   string foldername;
   inode_ptr parent_node;
   base_file_ptr parent_dir = nullptr;
   inode_ptr node = nullptr;
   shared_ptr<directory> ptr;
   base_file_ptr dir;
   if(words.size() == 1){
      node = state.get_cwd();
      dir = node->get_contents();
      parent_node = dir->find("..");
      parent_dir = parent_node->get_contents();
      foldername = parent_dir->find_string(node);
      ptr = dynamic_pointer_cast<directory>(dir);
      if(node == state.get_root()){
         cout << "/";
         foldername = "";
      }
      ptr->megaprint(foldername);
      return;
   }
   else{
      for (auto rit = words.begin()+1; rit != words.end(); ++rit){
         parent_node = find_parent(state, *rit);
         DEBUGF('c', parent_node);
         if(split(*rit,"/").size() == 0){
           node = state.get_root();
           parent_node = state.get_root();
         }
         else if(parent_node != nullptr){
            foldername = split(*rit,"/").back();
            parent_dir = parent_node->get_contents();
            node = parent_dir->find(foldername);
         }
         if(parent_node == nullptr || node == nullptr){
            cerr << words.at(0) << ": ";
            cerr << *rit << ": No such file or directory" << endl;
            return exec::status(1);
         }
         dir = node->get_contents();
         ptr = dynamic_pointer_cast<directory>(dir);
         if(ptr != nullptr){
            if(node == state.get_root()){
               foldername = "";
            }
            cout << "/" << foldername;
            ptr->megaprint("");
         }
         else {
            cerr << words.at(0) << ": ";
            cerr << foldername << ": Is not a directory" << endl;
         }
      }
   }
}

void fn_make (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   base_file_ptr parent_dir;
   inode_ptr node;
   string filename;
   if(words.size() > 1){
      inode_ptr parent_node = find_parent(state, words.at(1));
      // found parent

      if(split(words.at(1),"/").size() == 0){
         node = state.get_root();
         parent_node = state.get_root();
      }
      else if(parent_node != nullptr){
         filename = split(words.at(1),"/").back();
         parent_dir = parent_node->get_contents();
         node = parent_dir->find(filename);
         DEBUGF('c', node);
      }
      else{
          //no path
         cerr << words.at(0) << ": ";
         cerr << words.at(1) << ": No such file or directory" << endl;
         return exec::status(1); 
      }
      // not found
      if(node == nullptr){
         parent_dir->mkfile(filename);
         node = parent_dir->find(filename);
      }
      base_file_ptr base = node->get_contents();
      shared_ptr<plain_file> ptr = 
         dynamic_pointer_cast<plain_file>(base);
      DEBUGF('c', filename);
      if (ptr != nullptr){
         // file found or created
         auto rit = words.begin()+2;
         wordvec text;
         while(rit != words.end()){
            text.push_back(*rit);
            rit++;
         }
         ptr->writefile(text); 
      }
      else{ //found a directory
         cerr << words.at(0) << ": ";
         cerr << words.at(1) << ": Is a directory" << endl;
         return exec::status(1); 
      }
   }
   else{
      cerr << words.at(0) << ": missing operand" << endl;
      return exec::status(1);
   }
}

void fn_mkdir (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string foldername;
   inode_ptr parent_node;
   inode_ptr new_node;
   if(words.size() > 1){
      parent_node = find_parent(state, words.at(1));
   }
   else{
      cerr << words.at(0) << ": missing operand" << endl;
      return exec::status(1);
   }
   if(split(words.at(1),"/").size() == 0){
      new_node = nullptr;
      parent_node = state.get_root();
      foldername = "/";
   }
   else if(parent_node != nullptr){
      foldername = split(words.at(1),"/").back();
      base_file_ptr parent_dir = parent_node->get_contents();
      new_node = parent_dir->mkdir(foldername);
   }
   else{
      cerr << words.at(0) << ": ";
      cerr << words.at(1) << ": No such file or directory" << endl;
      return exec::status(1);
   }
   if(new_node == nullptr){
      cerr << words.at(0) << ": ";
      cerr << "cannot create directory '" << foldername;
      cerr << "': File exists" << endl;
      return exec::status(1); 
   }
}


void fn_prompt (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   DEBUGF ('c', words.size())
   if(words.size() > 1){
      string prompt;
      for (auto rit = words.begin()+1; rit != words.end(); ++rit)
      {
         prompt = prompt + *rit + " ";
      }
      return state.set_prompt(prompt);
   }
   cerr << words.at(0) << ": missing operand" << endl;
   return exec::status(1);
}
// terminal ignores any operands passed to pwd
void fn_pwd (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
   inode_ptr cwd = state.get_cwd();
   base_file_ptr cwd_dir = cwd->get_contents();
   inode_ptr root = state.get_root();
   base_file_ptr root_dir = root->get_contents();

   if(cwd == root){
      cout << "/" << endl;
      return;
   }
   inode_ptr parent_node = cwd_dir->find("..");
   base_file_ptr parent_dir = parent_node->get_contents();
   wordvec path;
   // parent_dir contains names.
   while(cwd_dir != parent_dir){
      path.push_back( "/" + parent_dir->find_string(cwd));

      cwd = parent_node;
      cwd_dir = parent_dir;
      parent_node = cwd_dir->find("..");
      parent_dir = parent_node->get_contents();
   }
   for (auto rit = path.crbegin();rit != path.crend(); ++rit)
      cout<< *rit;
   cout << endl;
}

void fn_rm (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);

   string foldername;
   inode_ptr parent_node;
   inode_ptr node;
   base_file_ptr parent_dir;
   if(words.size() > 1){
      parent_node = find_parent(state, words.at(1));
   }
   else{
      cerr << words.at(0) << ": missing operand" << endl;
      return exec::status(1);
   }
   if(split(words.at(1),"/").size() == 0){
      node = state.get_root();
      parent_node = state.get_root();
      foldername = "";
      parent_dir = parent_node->get_contents();
      node = parent_dir->find(foldername);
   }
   else if(parent_node != nullptr){
      foldername = split(words.at(1),"/").back();
      parent_dir = parent_node->get_contents();
      node = parent_dir->find(foldername);
   }
   if(node == nullptr || parent_node == nullptr){
      cerr << words.at(0) << ": ";
      cerr << foldername << ": No such file or directory" << endl;
      return exec::status(1);
   }
   parent_dir->remove(foldername);
   node = parent_dir->find(foldername);
   if(node != nullptr){
      cerr << words.at(0) << ": ";
      cerr << foldername << ": Directory not empty" << endl;
      return exec::status(1);
   }
}

void fn_rmr (inode_state& state, const wordvec& words){
   DEBUGF ('c', state);
   DEBUGF ('c', words);
      string foldername;
   inode_ptr parent_node;
   inode_ptr node;
   base_file_ptr parent_dir;
   base_file_ptr dir;
   shared_ptr<directory> ptr;

   if(words.size() > 1){
      parent_node = find_parent(state, words.at(1));
   }
   else{
      cerr << words.at(0) << ": missing operand" << endl;
      return exec::status(1);
   }
   if(split(words.at(1),"/").size() == 0){
      node = state.get_root();
      parent_node = state.get_root();
      foldername = "";
      parent_dir = parent_node->get_contents();
      node = parent_dir->find(foldername);
   }
   else if(parent_node != nullptr){
      foldername = split(words.at(1),"/").back();
      parent_dir = parent_node->get_contents();
      node = parent_dir->find(foldername);
   }
   if(node == nullptr || parent_node == nullptr){
      cerr << words.at(0) << ": ";
      cerr << foldername << ": No such file or directory" << endl;
      return exec::status(1);
   }
   dir = node->get_contents();
   ptr = dynamic_pointer_cast<directory>(dir);

   if(ptr != nullptr){
      ptr->harakiri();
      parent_dir->remove(foldername);
   }
   else {
      cerr << words.at(0) << ": ";
      cerr << foldername << ": Is not a directory" << endl;
   }

}
// pass in only the file path
// '/' means start at root
// do not use for mkdir
// mkdir 
inode_ptr find_parent(inode_state& state ,const string& path){
   inode_ptr parent_node;

   if (path[0] == '/') {
      parent_node = state.get_root();
   }
   else 
      parent_node = state.get_cwd();
   wordvec words = split(path,"/");
   if(words.size() >= 1)
      words.pop_back();

   auto rit = words.begin();
   DEBUGF('c', words);
   

   base_file_ptr parent_dir;
   shared_ptr<directory> ptr;

   while(rit != words.end()){
      parent_dir = parent_node->get_contents();
      ptr = dynamic_pointer_cast<directory>(parent_dir);
      if (ptr != nullptr) {
         parent_node = ptr->find(*rit);
         // cant find
         if(parent_node == nullptr)
            return nullptr;
      } else {
         // found plain file
         return nullptr;
      }
      DEBUGF('c', *rit);
      rit++;
   }
   DEBUGF('c', parent_node);
   parent_dir = parent_node->get_contents();
   ptr = dynamic_pointer_cast<directory>(parent_dir);
   if(ptr == nullptr)
      return nullptr;
   return parent_node;
}
