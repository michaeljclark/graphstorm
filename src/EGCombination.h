/*
 *  EGCombination.h
 *
 *  Copyright (c) 2008 - 2013, Michael Clark <michael@earthbuzz.com>, EarthBuzz Software
 */

#ifndef EGCombination_H
#define EGCombination_H


/* EGCombination */

class EGCombination
{
    EGint *a;
    EGint n;
    EGint r;
    
public:
    EGCombination(EGint n, EGint r) : n(n), r(r)
    {
        if (n < 1) {
            n = 1;
        }
        if (r > n) {
            r = n;
        }
        a = new EGint[r];
        reset();
    }
    
    ~EGCombination()
    {
        delete [] a;
    }
    
    void reset()
    {
        for (EGint i = 0; i < r; i++) {
            a[i] = n - r + i;
        }
        a[0] = -1;;
    }
    
    
    EGbool hasNext()
    {
        return (a[0] < n - r);
    }
    
    
    EGint* getNext()
    {
        int i = r - 1;
        while (a[i] == n - r + i) {
            i--;
        }
        
        a[i] = a[i] + 1;
        for (EGint j = i + 1; j < r; j++) {
            a[j] = a[i] + j - i;
        }
        
        return a;
    }
    
    std::vector<EGint> getNextVector()
    {
        std::vector<EGint> v;
        getNext();
        for (EGint i = 0; i < r; i++) {
            v.push_back(a[i]);
        }
        return v;
    }
};

#endif
