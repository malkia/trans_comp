// Note we had to declare -DTBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS=1
// in BUILD.bazel to enable the new concurrent ordered containers
#include <tbb/concurrent_set.h>
#include <tbb/concurrent_map.h>
#include <tbb/concurrent_unordered_set.h>

#include <string_view>
#include <memory_resource>

// Both the container, and the type it's holding use PMR.
tbb::concurrent_set< 
    std::pmr::string, 
    std::less<>, 
    std::pmr::polymorphic_allocator<std::pmr::string>
> theSet;

// For some reason polymorphic_allocator does not have the "rebind" functionality
// needed by some of the containers. This one adds.
// Somewhat related: https://github.com/intel/tbb/issues/38
// Hopefully this would work with Microsoft's ConCRT (The concurrency framework)
template<typename T>
class polymorphic_allocator_with_rebind : public std::pmr::polymorphic_allocator<T>
{
public:
    polymorphic_allocator_with_rebind() noexcept 
        : std::pmr::polymorphic_allocator<T>()
    {}
    
    polymorphic_allocator_with_rebind( const polymorphic_allocator_with_rebind& t ) noexcept
        : std::pmr::polymorphic_allocator<T>( t )
    {}

    template<typename U> 
    polymorphic_allocator_with_rebind(const polymorphic_allocator_with_rebind<U>& u) noexcept
        : std::pmr::polymorphic_allocator<T>( u )
    {}

    polymorphic_allocator_with_rebind( std::pmr::memory_resource* r ) noexcept
        : std::pmr::polymorphic_allocator<T>( r )
    {
    }

// Would this be needed? So far not with TBB, but might be needed with ConCRT
//    template<typename U> struct rebind {
//        typedef polymorphic_allocator_with_rebind<U> other;
//    };
};

/*
 */

// Both the container, and the type it's holding use PMR.
tbb::concurrent_map< 
    std::pmr::string 
    ,std::pmr::string 
    ,std::less<>
    ,polymorphic_allocator_with_rebind<std::pair<std::pmr::string, std::pmr::string>>
> theMap;

// Still working out how we are going to use transparent comparator here.
// Though at least the polymorphic allocator can be used.
tbb::concurrent_unordered_set<
    std::pmr::string
    ,std::hash<std::pmr::string>
    ,std::equal_to<std::pmr::string>
    ,polymorphic_allocator_with_rebind<std::pmr::string>   
> theUSet;

using pmr_cset = tbb::concurrent_unordered_set<
    std::pmr::string
    ,std::hash<std::pmr::string>
    ,std::equal_to<std::pmr::string>
    ,polymorphic_allocator_with_rebind<std::pmr::string>   
>;

int main(int argc, const char *argv[]) { 

    std::string_view a{"One"};
    pmr_cset cset(std::pmr::get_default_resource());

    // Need to convert to type (e.g. std::pmr::string) before inserting
    // theSet.insert(a);

    // But find() just works with std::string_view just fine!
    theSet.find(a);
    theMap.find(a);
    theUSet.insert("test");

    return 0; 
}
