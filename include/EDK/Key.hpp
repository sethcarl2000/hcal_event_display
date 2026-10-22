#ifndef EDK_Key_hpp
#define EDK_Key_hpp

namespace EDK 
{

template<typename KeyOwner> class Key {
private:
    friend KeyOwner;
    Key() = default; 
};

}

#endif