#pragma once
#include "sorter.h"

namespace yas {
    class IntroSorter :public Sorter
    {
    
    public:
        IntroSorter()=default;
        ~IntroSorter()=default;
        void sort(int from, int to) override;
    private:
        int partition(int from,int to);
        int median_of_three(int from ,int mid,int to);
        void quick_sort(int from, int to, int depth);
    };
}