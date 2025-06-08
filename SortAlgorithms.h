#ifndef SORT_ALGORITHMS_H
#define SORT_ALGORITHMS_H

#include <algorithm>
#include <vector>

namespace sort
{
//---------------Bubblr Sort----------------------
    //* Two implemantations of the bubble sort algorithm
    template<typename Iterator, typename Compare = std::less<>>
    void bubbleSort(Iterator begin, Iterator end, Compare comp = Compare()) {
        for(Iterator i = begin; i != end; ++i) {
            for(Iterator j = begin; std::next(j) != end; ++j) {
                if(comp(*std::next(j), *j)) {
                    std::iter_swap(std::next(j), j);
                }
            }
        }
    }

    template<typename Container, typename Compare = std::less<>>
    void bubbleSort(Container&& container, Compare comp = Compare()) {
        for(size_t i = 0; i < container.size() - 1; ++i) {
            for(size_t j = 0; j < container.size() - 1; ++j) { 
                if(comp(container[j + 1], container[j])) {
                    std::swap(container[j], container[j + 1]);
                }
            }
        }
        // or just call bubble sort function that has already been written that works with two iterators
        // bubbleSort(std::begin(container), std::end(container));
    }

//---------------------Insert Sort-------------------
    template<typename Iterator, typename Compare = std::less<>>
    void insertSort(Iterator begin, Iterator end, Compare comp = Compare()) {
        for(Iterator i = std::next(begin); i != end; i++) {
            Iterator j = i;
            while (j != begin && comp(*j, *std::prev(j)))
            {
                std::iter_swap(j, std::prev(j));
                j = std::prev(j);
            }
        }
    }

    template<typename Container, typename Compare = std::less<>>
    void insertSort(Container&& container, Compare comp = Compare()) {
        for(size_t i = 1; i < container.size(); ++i) {
            for(size_t j = i; j != 0 && comp(container[j], container[j - 1]); --j) {
                std::swap(container[j], container[j - 1]);
            }
        }
        // or just
        //insertSort(std::begin(container), std::end(container), comp);
    }

//----------------Selection Sort--------------
    template<typename Iterator, typename Compare = std::less<>>
    void selectionSort(Iterator begin, Iterator end, Compare comp = Compare()) {
        for(Iterator i = begin; i != end; ++i) {
            Iterator min = i;
            for(Iterator j = std::next(i); j != end; ++j) {
                if(comp(*j, *min)) {
                    min = j;
                }
            }
            if(min != i) {
                std::iter_swap(min, i);
            }
        }
    }
    template<typename Container, typename Compare = std::less<>>
    void selectionSort(Container&& container, Compare comp = Compare()) {
        for(size_t i = 0; i < container.size(); ++i) {
            auto min = container[i];
            size_t minPosition = i;
            for(size_t j = i + 1; j < container.size(); ++j) {
                if(container[j] < min) {
                    min = container[j];
                    minPosition = j;
                }
            }
            if(i != minPosition) {
                std::swap(container[i], container[minPosition]);
            }
        }
        // or just
        // selectionSort(std::begin(container), std::end(container), comp);
    }

//--------------Heap Sort----------------
    namespace detail /*Heap Sort helper functions*/ {
        // heapify - supports the property of the max/min heap
        template<typename Container, typename Compare>
        void heapify(Container&& container, size_t heapSize, size_t nodeIndex, Compare comp) {
            size_t largestElementIndex = nodeIndex;
            size_t leftHeir  = 2 * nodeIndex + 1;
            size_t rightHeir = leftHeir + 1;  // 2 * heapSize + 2

            if(leftHeir < heapSize && comp(container[leftHeir], container[largestElementIndex])) {
                largestElementIndex = leftHeir;
            }
            if(rightHeir < heapSize && comp(container[rightHeir], container[largestElementIndex])) {
                largestElementIndex = rightHeir;
            }
            if(largestElementIndex != nodeIndex) {
                std::swap(container[largestElementIndex], container[nodeIndex]);
                heapify(container, heapSize, largestElementIndex, comp);
            }
        }
        // builds a heap from the container
        template<typename Container, typename Compare>
        void heapBuilder(Container&& container, Compare comp) {
            size_t containerSize = std::size(container);
            for(int32_t i = container.size() / 2 - 1; i >= 0; --i) {
                detail::heapify(container, containerSize, i, comp);
            }
        }
    }
    // sort function that takes reference to a container and compare funciton
    template<typename Container, typename Compare = std::greater<>>
    void heapSort(Container&& container, Compare comp = Compare()) {
        heapBuilder(container, comp);
        size_t heapSize = container.size() - 1;
        while(heapSize != 0) {
            std::swap(container[heapSize], container[0]);
            detail::heapify(container, heapSize--, 0, comp);
        }
    }


//------------------Quick Sort----------------------
    namespace detail  /*Quick Sort help functions*/ {
        template<typename Iterator, typename Compare>
        Iterator medianOfThree(Iterator first, Iterator middle, Iterator last, Compare comp) {
            if (comp(*first, *middle)) {
                if (comp(*middle, *last)) {
                    return middle;  // first < middle < last
                } else if (comp(*first, *last)) {
                    return last;    // first < last < middle
                } else {
                    return first;   // last < first < middle
                }
            } else {
                if (comp(*first, *last)) {
                    return first;   // middle < first < last
                } else if (comp(*middle, *last)) {
                    return last;    // middle < last < first
                } else {
                    return middle;  // last < middle < first
                }
            }
        }
    }
        // Quick sort functions
        template<typename Iterator, typename Compare = std::less<>>
        void quickSort(Iterator begin, Iterator end, Compare comp = Compare()) {
            if (std::distance(begin, end) <= 1) {
                return;
            }
            
            Iterator last = std::prev(end);
            Iterator middle = begin + (std::distance(begin, end) / 2);
            Iterator pivot = detail::medianOfThree(begin, middle, last, comp);
            
            auto pivotValue = *pivot;
            std::iter_swap(pivot, last);
            
            // Partition up to last (not end), since last holds our pivot
            Iterator partitionIter = std::partition(begin, last, [&](const auto& value) {
                return comp(value, pivotValue);
            });
            
            std::iter_swap(partitionIter, last);
            
            quickSort(begin, partitionIter, comp);
            quickSort(std::next(partitionIter), end, comp);
        }
        
        template<typename Container, typename Compare = std::less<>>
        void quickSort(Container&& container, Compare comp = Compare()) {
            quickSort(std::begin(container), std::end(container), comp);
        }
    

//----------------Merge Sort-----------------------
    namespace detail /*Merge Sort helper functions*/ {
        template<typename Iterator, typename Compare>
        void merge(Iterator begin, Iterator middle, Iterator end, Compare comp) {
            // it helps to get a value of the element the iterator points to create a temp vector that will store values
            std::vector<typename std::iterator_traits<Iterator>::value_type> tempVector;
            tempVector.reserve(std::distance(begin, end)); // to avoid a new memory allocation when the insert function will be used

            Iterator left = begin;
            Iterator mid = middle;

            while(left != middle && mid != end) {
                if(comp(*left, *mid)) {
                    tempVector.emplace_back(*left);
                    ++left;
                } else {
                    tempVector.emplace_back(*mid);
                    ++mid;
                }
            }
            // it is necesary to place elements that left into the tempVector
            tempVector.insert(tempVector.end(), left, middle);
            tempVector.insert(tempVector.end(), mid, end);

            std::move(std::begin(tempVector), std::end(tempVector), begin);
        }
    }
    template<typename Iterator, typename Compare = std::less<>>
    void mergeSort(Iterator begin, Iterator end, Compare comp = Compare()) {
        size_t distance = std::distance(begin, end);
        if(distance <= 1) {
            return ;
        }
        Iterator middle = begin + (distance / 2);

        mergeSort(begin, middle, comp);
        mergeSort(middle, end, comp);
        detail::merge(begin, middle, end, comp);
    }

    template<typename Container, typename Compare = std::less<>>
    void mergeSort(Container&& container, Compare comp = Compare()) {
        mergeSort(std::begin(container), std::end(container), comp);
    }

    
//----------------Radix Sort----------------
    namespace detail/*helper function for Radix Sort algorithm*/ {
        template<typename Iterator>
        void sortByDigits(Iterator begin, Iterator end, int exp) {
            size_t containerSize = std::distance(begin, end);
            std::vector<typename std::iterator_traits<Iterator>::value_type> tempVector(containerSize);
            size_t count[10] = {0};

            for(Iterator i = begin; i != end; ++i) {
                auto digit = (*i / exp) % 10;
                ++count[digit];
            }

            for(size_t i = 1; i < 10; ++i) {
                count[i] += count[i - 1];
            }

            Iterator i = end;
            do {
                --i;
                auto digit = (*i / exp) % 10;
                tempVector[--count[digit]] = *i;
            } while(i != begin);

            std::move(std::begin(tempVector), std::end(tempVector), begin);
        }
    }
    //* template function that implements radix sort algorithm
    template<typename Iterator>
    void radixSort(Iterator begin, Iterator end) {
        using T = typename std::iterator_traits<Iterator>::value_type;
    
        std::vector<T> negatives;
        std::vector<T> positives;

        for(Iterator i = begin; i != end; ++i) {
            if(*i < 0)  negatives.emplace_back(-(*i));
            else        positives.emplace_back(*i);
        }

        if(!positives.empty()) {
            T maxPositiveElement = *std::max_element(std::begin(positives), std::end(positives));
            for(int32_t i = 1; maxPositiveElement / i > 0; i *= 10) {
                sortByDigits(std::begin(positives), std::end(positives), i);
            }
        }

        if(!negatives.empty()) {
            T maxNegativeElement = *std::max_element(std::begin(negatives), std::end(negatives));
            for(int32_t i = 1; maxNegativeElement / i > 0; i *= 10) {
                sortByDigits(std::begin(negatives), std::end(negatives), i);
            }
            for(T& iter : negatives) {
                iter = -iter;
            }
            std::reverse(std::begin(negatives), std::end(negatives));
        }
        auto out = begin;
        out = std::move(std::begin(negatives), std::end(negatives), out);
        std::move(std::begin(positives), std::end(positives), out);
    }
    
    template<typename Container>
    void radixSort(Container&& container) {
        radixSort(std::begin(container), std::end(container));
    }

//--------------------Cocktail Sort----------------------
    //* this template function implamants cocktail sort algorith(alsmost the same as the bubble sort)
    template<typename Iterator, typename Compare = std::less<>>
    void cocktailSort(Iterator begin, Iterator end, Compare comp = Compare()) {
        int32_t left  = 0;
        int32_t right = std::distance(begin, end) - 1;

        while(left <= right) {
            for(Iterator i = begin; std::next(i) != end; ++i) {
                if(comp(*std::next(i), *i)) {
                    std::iter_swap(std::next(i), i);
                }
            }
            ++left;
            for(Iterator j = std::prev(end); std::prev(j) != begin; --j) {
                if(comp(*j, *std::prev(j))) {
                    std::iter_swap(j, std::prev(j));
                }
            }
            --right;
        }
    }

    template<typename Container, typename Compare = std::less<>>
    void cocktailSort(Container&& container, Compare comp = Compare()) {
        cocktailSort(std::begin(container), std::end(container), comp);
    }

//-----------------Comb Sort----------------------------
    //* this template function implements comb Sort algorithm (it is improved bubble sort algorithm)
    template<typename Iterator, typename Compare = std::less<>>
    void combSort(Iterator begin, Iterator end, Compare comp = Compare()) {
        size_t gap = std::distance(begin, end);
        const static double shrink = 1.3;
        bool isSorted {};

        do{
            gap = static_cast<size_t>(gap / shrink);
            if(gap < 1) {
                gap = 1;
            }
            isSorted = (gap == 1);
            for(Iterator i = begin; (i + gap) != end; ++i) {
                if(comp(*std::next(i), *i)) {
                    std::iter_swap(std::next(i), i);
                    isSorted = false;
                }
            }
        }while(!isSorted);
    }
    template<typename Container, typename Compare = std::less<>>
    void combSort(Container&& container, Compare comp = Compare()) {
        combSort(std::begin(container), std::end(container), comp);
    }
} // namespace sort


#endif //SORT_ALGORITHMS_H