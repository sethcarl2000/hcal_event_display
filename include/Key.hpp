#ifndef Key_hpp
#define Key_hpp

template<typename KeyOwner> class Key {
private:
    friend KeyOwner;
    Key() = default; 
};

#endif