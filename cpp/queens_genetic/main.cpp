#include <iostream>
#include <algorithm>
#include <random>
#include <vector>

const int POPULATION = 81;
const int MAX_ITER = 1E+7;

typedef std::vector<int> vec_int;
typedef std::vector<int>::size_type sizetype;
typedef std::uniform_int_distribution<std::mt19937::result_type> Dtype;

void draw(vec_int const& v, int N)
{
    for(auto y = 0; y < N; ++y)
    {
        for(auto x = 0; x < N; ++x)
        {
            if(x == v[y])
                std::cout << 'x';
            else
                std::cout << '.';
        }
        std::cout << std::endl;
    }
}

void printResult(vec_int const& v, int N, int generation)
{
    std::cout << "\n\ngot solution: " << std::endl;
    
    for( auto x : v )
        std::cout << x << ", ";
    
    std::cout << std::endl << "\nfinal generation: " << generation << std::endl;
            
    if(N < 25)
        draw(v, N);
}

int hits(vec_int const& v)
{
    int hits = 0;
    sizetype size = v.size();
    
    for(sizetype i = 0; i < size - 1; ++i) 
        for(sizetype j = i + 1; j < size; ++j) 
            if(j - i == static_cast<sizetype>(abs(v[j] - v[i]))) ++hits;
            
    return hits;
}

std::pair<vec_int, vec_int>
    gemmation(vec_int const& p1, vec_int const& p2, std::mt19937 engine)
{
    auto size = p1.size();
    
    Dtype uds_size(0, size-1);
        
    if(size != p2.size()) throw std::runtime_error("Lengths are not equal.");
    
    vec_int v1(p1), v2(p2);
    
    auto i1 = uds_size(engine);
    auto i2 = uds_size(engine);
    
    auto j1 = std::distance(v2.begin(), std::find(v2.begin(), v2.end(), v1[i1]));
    auto j2 = std::distance(v2.begin(), std::find(v2.begin(), v2.end(), v1[i2]));
    
    std::swap(v1[i1], v2[j2]);
    std::swap(v1[i2], v2[j1]);
        
    return std::make_pair(v1, v2); 
}

int main(int argc, char **argv)
{
    const int N = (argc > 1) ? std::atoi(argv[1]) : 8;
    
    std::vector<std::vector<int>> v(POPULATION, vec_int(N));
    vec_int fit(POPULATION);
    vec_int indices(POPULATION);
    
    std::iota(indices.begin(), indices.end(), (sizetype)0);
    
    unsigned int generation = 1;
    
    std::mt19937 engine;
    std::random_device rd;
    engine.seed(rd());
    Dtype uds100(0, 100);
    
    for(auto i = 0; i < POPULATION; ++i)
    {
        std::iota(v[i].begin(), v[i].end(), (sizetype)0);
        std::shuffle(v[i].begin(), v[i].end(), engine);
        fit[i] = hits(v[i]);
    }
    
    while(generation < MAX_ITER)
    {
        std::sort(indices.begin(), indices.end(), [&](sizetype i, sizetype j) { return fit[i] < fit[j]; });
        
        if(!fit[indices[0]])
        {
            printResult(v[indices[0]], N, generation);
            
            break;
        }
        
        std::cout << "generation: " << generation << "; minimal number of hits: " << fit[indices[0]] << std::endl;
        ++generation;
        
        for(auto i = 0; i < POPULATION / 2; i += 2)
        {
            auto parent1 = indices[i];
            auto parent2 = indices[i+1];
            auto child1 = indices[POPULATION - i - 1];
            auto child2 = indices[POPULATION - i - 2];
            
            auto res = std::move(gemmation(v[parent1], v[parent2], engine));

            // mutate, 10% chance
            if(uds100(engine) <= 10)
                std::shuffle(v[child1].begin(), v[child1].end(), engine);
            else
                v[child1] = std::move(res.first);
            
            v[child2] = std::move(res.second);
        }
        
        for(auto i = POPULATION / 2; i < POPULATION; ++i)
            fit[indices[i]] = hits(v[indices[i]]);
    }
    
    if(generation == MAX_ITER)
        std::cout << "No dice" << std::endl;

    return 0;
}
