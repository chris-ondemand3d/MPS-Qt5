#ifndef SINGLETON_H
#define SINGLETON_H


template <typename T>
class Singleton
{
public:
    static T* instance()
    {
        static T* instance = new T();
        return instance;
    }
};

#endif // SINGLETON_H
