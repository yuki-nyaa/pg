#include<yuki/Allocator.hpp>

namespace yuki::pg{
template<typename T,typename A=yuki::Allocator<T>,typename A::size_type dynamic_threshold_p=(sizeof(typename A::pointer)+sizeof(typename A::size_type))/sizeof(T)>
struct Tokens : private A{
    static_assert(std::is_trivial_v<T>);
    static_assert(std::is_same_v<T,typename A::value_type>);
    static_assert(std::is_same_v<T*,typename A::pointer>);
    static_assert(std::is_same_v<const T*,typename A::const_pointer>);
    static_assert(dynamic_threshold_p>=(sizeof(typename A::pointer)+sizeof(typename A::size_type))/sizeof(T));

    typedef T value_type;
    typedef A allocator_type;
    using typename A::pointer;
    using typename A::const_pointer;
    using typename A::size_type;
    using typename A::difference_type;

    static constexpr size_type dynamic_threshold = dynamic_threshold_p;
  private:
    union{
    struct{T arr[dynamic_threshold];} static_;
    struct{pointer ptr; size_type cap;} dynamic_;
    };
    size_type size_=0;
  public:
    Tokens() noexcept = default;

    Tokens(const Tokens& other) noexcept :
        size_(other.size_)
    {
        if(size_>dynamic_threshold){
            dynamic_.cap = size_;
            dynamic_.ptr = A::allocate(size_);
            yuki::copy_no_overlap(dynamic_.ptr,other.dynamic_.ptr,size_);
        }else{
            yuki::copy_no_overlap(static_.arr,other.static_.arr,size_);
        }
    }

    Tokens(Tokens&& other) noexcept :
        size_(other.size_)
    {
        if(size_>dynamic_threshold){
            dynamic_.ptr = other.dynamic_.ptr;
            dynamic_.cap = other.dynamic_.cap;
            other.dynamic_.ptr = nullptr;
            other.dynamic_.cap = 0;
            other.size_=0;
        }else{
            yuki::copy_no_overlap(static_.arr,other.static_.arr,size_);
        }
    }

    ~Tokens() noexcept {
        if(size_>dynamic_threshold)
            A::deallocate(dynamic_.ptr,dynamic_.cap);
    }

    Tokens& operator=(const Tokens&) = delete;
    Tokens& operator=(Tokens&&) = delete;

    template<typename It>
    Tokens(It it,size_t count,T(*const kv)(const decltype(*it)&)) noexcept {
        if(count>dynamic_threshold){
            dynamic_.ptr=A::allocate(count);
            dynamic_.cap=count;
            for(;count!=0;++it,--count){
                assert(size_==0 || dynamic_.ptr[size_]<*it);
                dynamic_.ptr[size_]=kv(*it);
                ++size_;
            }
        }else{
            for(;count!=0;++it,--count){
                assert(size_==0 || static_.arr[size_]<*it);
                static_.arr[size_]=kv(*it);
                ++size_;
            }
        }
    }

    bool empty() const {return size_==0;};
    size_type size() const {return size_;}

    void clear(){
        if(size_>dynamic_threshold)
            A::deallocate(dynamic_.ptr,dynamic_.cap);
        size_=0;
    }

    typedef pointer iterator;
    typedef const_pointer const_iterator;

    iterator begin(){
        return size_>dynamic_threshold ? dynamic_.ptr : static_.arr;
    }
    iterator end() {return begin()+size_;}
    const_iterator begin() const {
        return size_>dynamic_threshold ? dynamic_.ptr : static_.arr;
    }
    const_iterator end() const {return begin()+size_;}

    T& operator[](const size_type i) {assert(i<size_); return begin()[i];}
    const T& operator[](const size_type i) const {assert(i<size_); return begin()[i];}
    T& front() {assert(!empty()); return *begin();}
    const T& front() const {assert(!empty()); return *begin();}
    T& back() {assert(!empty()); return *(end()-1);}
    const T& back() const {assert(!empty()); return *(end()-1);}

    void push_back(const T& t){
        if(size_>dynamic_threshold){
            if(size_==dynamic_.cap){
                const pointer ptr_new = A::allocate(2*dynamic_.cap);
                yuki::move_no_overlap(ptr_new,dynamic_.ptr,size_);
                A::deallocate(dynamic_.ptr,dynamic_.cap);
                dynamic_.ptr=ptr_new;
                dynamic_.cap*=2;
            }
            dynamic_.ptr[size_]=t;
        }else{
            if(size_==dynamic_threshold){
                const pointer ptr_new = A::allocate(2*dynamic_threshold);
                yuki::move_no_overlap(ptr_new,static_.arr,dynamic_threshold);
                dynamic_.ptr=ptr_new;
                dynamic_.cap=2*dynamic_threshold;
                dynamic_.ptr[size_]=t;
            }else
                static_.arr[size_]=t;
        }
        ++size_;
    }

    friend bool operator==(const Tokens& lhs,const Tokens& rhs) noexcept {
        if(lhs.size_!=rhs.size_)
            return false;
        if constexpr(std::has_unique_object_representations_v<T>)
            return memcmp(lhs.begin(),rhs.begin(),lhs.size_*sizeof(T))==0;
        else{
            const_iterator lb = lhs.begin();
            const_iterator rb = rhs.begin();
            const const_iterator le = lhs.end();
            for(;lb!=le;++lb,++rb)
                if(*lb!=*rb)
                    return false;
            return true;
        }
    }
}; // struct Tokens<T,A,dt>
} // namespace yuki::pg

size_t sum=0;

int main(){
    for(size_t i=0;i<100000000;++i){
        //std::basic_string<unsigned char> str;
        yuki::pg::Tokens<unsigned char> str;
        str.push_back(2);
        str.push_back(3);
        str.push_back(5);
        str.push_back(7);
        str.push_back(11);
        str.push_back(13);
        str.push_back(17);
        str.push_back(19);
        sum=0;
        for(const auto s : str)
            sum+=s;
    }
    //{
    //yuki::pg::Tokens<unsigned> str;
    //str.push_back(2);
    //str.push_back(3);
    //str.push_back(5);
    //yuki::pg::Tokens<unsigned> str1 = str;
    //for(const auto s:str1)
    //    printf("%u ",(unsigned)s);
    //fputc(static_cast<unsigned char>('\n'),stdout);
    //yuki::pg::Tokens<unsigned> str2 = std::move(str);
    //for(const auto s:str2)
    //    printf("%u ",(unsigned)s);
    //fputc(static_cast<unsigned char>('\n'),stdout);
    //str.push_back(7);
    //str.push_back(11);
    //yuki::pg::Tokens<unsigned> str3 = str;
    //for(const auto s:str3)
    //    printf("%u ",(unsigned)s);
    //fputc(static_cast<unsigned char>('\n'),stdout);
    //printf("%u\n",unsigned(str3==str));
    //yuki::pg::Tokens<unsigned> str4 = std::move(str);
    //for(const auto s:str4)
    //    printf("%u ",(unsigned)s);
    //fputc(static_cast<unsigned char>('\n'),stdout);
    //printf("%u\n",unsigned(str4==str));
    //for(const auto s:str)
    //    printf("%u ",(unsigned)s);
    //fputc(static_cast<unsigned char>('\n'),stdout);
    //}
}
