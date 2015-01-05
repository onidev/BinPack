#ifndef ONI_BINPACK_H_INCLUDED
#define ONI_BINPACK_H_INCLUDED

#include <vector>
#include <utility>

class BinPacker
{
public:
    struct Rect
    {
        int x, y, wid, hei;
        Rect(){}
        Rect(int x, int y, int w, int h):x(x),y(y),wid(w),hei(h){}
    };
    
    BinPacker();
    
    virtual bool process(std::vector<Rect> & rects) = 0;
    
protected:
    bool binpack(std::vector<Rect> & rects, int wid, int hei) const;
    bool preprocess(std::vector<Rect> & rects, int & minWid, int & minHei, int maxWid, int maxHei);
    
    std::vector<int> index;
};

class PowerOfTwoBinPacker : public BinPacker
{
    int maxWid;
    int maxHei;
    int m_width;
    int m_height;
    
public:
    PowerOfTwoBinPacker();
    PowerOfTwoBinPacker(int maxWid, int maxHei);
    
    bool process(std::vector<Rect> & rects);
    int width() const;
    int height() const;
};

#endif
