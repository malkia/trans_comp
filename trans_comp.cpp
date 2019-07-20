// Note we had to declare -DTBB_PREVIEW_CONCURRENT_ORDERED_CONTAINERS=1
// in BUILD.bazel to enable the new concurrent ordered containers
#include <tbb/concurrent_set.h>

#include <string_view>
#include <memory_resource>

// Both the container, and the type it's holding use PMR.
tbb::concurrent_set< 
    std::pmr::string, 
    std::less<>, 
    std::pmr::polymorphic_allocator<std::pmr::string>
> theSet;

int main(int argc, const char *argv[]) { 

    std::string_view a{"One"};

    // Need to convert to type (e.g. std::pmr::string) before inserting
    // theSet.insert(a);

    // But find() just works with std::string_view just fine!
    theSet.find(a);

    return 0; 
}
