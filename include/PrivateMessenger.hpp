#ifndef PrivateMessenger_hpp
#define PrivateMessenger_hpp

#include <string>
class TCanvas; 
class UserWindow; 

//this class is designed to pass messages to 'EventDisplayKernel', which the user cannot access
class PrivateMessenger {
private:    
    //put classes here which need access to the ptr 
    friend class UserWindow; 
    friend class UserApp; 

    static void SetCanvas(TCanvas* canv); 
    static void SetUserWindow(UserWindow* window); 
    static void SetObjDrawFunctionName(std::string name); 
    static void SetAppDrawFunctionName(std::string name); 

}; 

#endif