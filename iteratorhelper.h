#pragma once
#include <iterator>
#include <algorithm>

namespace IteratorHelper {

template <typename I>
bool circularIncrement(I &toIncrement, const I &begin, const I &end)
{
    bool cycleComplete = false;

    ++toIncrement;

    if (toIncrement == end)
    {
        toIncrement = begin;
        cycleComplete = true;
    }

    return cycleComplete;
}

template <typename I>
bool circularDecrement(I &toDecrement, const I &begin, const I &end)
{
    bool cycleComplete = false;

    if (toDecrement == begin)
    {
        toDecrement = end;
        cycleComplete = true;
    }

    --toDecrement;

    return cycleComplete;
}

template <typename I>
void circularAdvance(I &toAdvance, const I &begin, const I &end, int step)
{
    /*int size = std::distance(begin, end);

    if (step >= 0)
    {
        int distance = std::distance(toAdvance, end);

        if (distance > step)
        {
            std::advance(toAdvance, step);
        }
        else
        {
            circularAdvance(toAdvance, begin, end, step - distance);
        }

    }
    else
    {
        int distance = std::distance(begin, toAdvance);

        if (distance >= -step)
        {
            std::advance(toAdvance, step);
        }
        else
        {
            std::advance(toAdvance, )
        }
    }*/

    bool (*foo)(I &, const I &, const I &);

    if (step > 0)
    {
        foo = circularIncrement;
    }
    else
    {
        step = -step;
        foo = circularDecrement;
    }

    for (unsigned i = 0; i < (unsigned)step; ++i)
    {
        foo(toAdvance, begin, end);
    }

}

template<typename I>
I circularNext(const I &iter, const I &begin, const I &end, int step = 1)
{
    I temp = iter;
    circularAdvance(temp, begin, end, step);
    return temp;
}

}
