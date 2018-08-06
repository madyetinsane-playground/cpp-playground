#include <iostream>
#include <memory>

class Variant
{
public:
    Variant() : object(nullptr) {}
    template<class T>
    Variant(T const& t) : object( new Type<T>(t) ) {}
    
    template<class T>
    Variant& operator=(T const& t)
    {
        typedef Type<T> type;
        object.reset(new type(t));
        
        return *this;
    }
    void swap(Variant& other) noexcept
    {
        object.swap(other.object);
    }    
    template<class T>
    operator T() const
    {
        typedef Type<T> assign_type;
        assign_type& type = dynamic_cast<assign_type&>(*object);
        
        return type.get();
    }
    
private:
    class Base
    {
    public:
        virtual ~Base() {}
    };
    
    template<class T>
    class Type : public Base
    {
    public:
        Type(T const& t) : object(t) {}
        T get() const { return object; }
    private:
        T object;
    };
    
    std::unique_ptr<Base> object;
};

int main(int argc, char **argv)
{
    Variant v1(5), v2, v3;
    
    v2 = std::string("string");
    v3 = 3.14;
    
    v1.swap(v2);
    std::string s = v1;
    int k = v2;
    double d = v3;
    
    std::cout << k << "\n\"" << s << "\"\n" << d << std::endl;
    
    return 0;
}
