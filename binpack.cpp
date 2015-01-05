#include "binpack.h"
#include <algorithm>

static int pow2ceil(int x)
{
    if (x < 0)
        return 0;
    --x;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x+1;
}

namespace
{

struct Node
{
    Node * child[2];
    BinPacker::Rect rc;
    bool full;
    
    Node() : full(false) { child[0] = child[1] = 0; }
    ~Node() { delete child[0]; delete child[1]; }
    Node * insert(BinPacker::Rect & rect);
    
    bool leaf() const { return child[0] == 0 && child[1] == 0; }
};

Node * Node::insert(BinPacker::Rect & rect)
{
    if(!leaf())
    {
        Node * newNode = child[0]->insert(rect);
        if(newNode)
            return newNode;
        else
            return child[1]->insert(rect);
    }
    else
    {
        if(full)
        {
            return 0;
        }
        
        // On regarde si le restangle rentre
        if(rect.wid > rc.wid || rect.hei > rc.hei)
        {
            return 0;
        }
        
        // Rentre parfaitement
        if(rect.wid == rc.wid && rect.hei == rc.hei)
        {
            full = true;
            rect.x = rc.x;
            rect.y = rc.y;
            return this;
        }
        
        // Sinon, on coupe la zone en deux parties
        // on regarde dans si l'on doit couper horizontalement ou verticalement
        int dw = rc.wid - rect.wid;
        int dh = rc.hei - rect.hei;
        
        if(dw > dh)
        {
            child[0] = new Node;
            child[0]->rc = BinPacker::Rect(rc.x, rc.y, rect.wid, rc.hei);
            child[1] = new Node;
            child[1]->rc = BinPacker::Rect(rc.x + rect.wid, rc.y, dw, rc.hei);
        }
        else
        {
            child[0] = new Node;
            child[0]->rc = BinPacker::Rect(rc.x, rc.y, rc.wid, rect.hei);
            child[1] = new Node;
            child[1]->rc = BinPacker::Rect(rc.x, rc.y + rect.hei, rc.wid, dh);
        }
        
        return child[0]->insert(rect);
    }
}

} // namespace



// BinPacker base
BinPacker::BinPacker():
    index()
{
}

bool BinPacker::binpack(std::vector<Rect> & rects, int wid, int hei) const
{
    Node root;
    root.rc = Rect(0, 0, wid, hei);
    
    for(size_t i=0; i<index.size(); ++i)
    {
        if(!root.insert(rects[index[i]]))
            return false;
    }
    
    return true;
}

bool BinPacker::preprocess(std::vector<Rect> & rects, int & minWid, int & minHei, int maxWid, int maxHei)
{
    // Check minimum area size
    minWid = 0;
    minHei = 0;
    for(Rect & r: rects)
    {
        if(r.wid > minWid) minWid = r.wid;
        if(r.hei > minHei) minHei = r.hei;
    }
    
    // if limited size
    if(maxWid != 0 && maxHei != 0 && (minWid > maxWid || minHei > maxHei))
        return false;
    
    index.resize(rects.size());
    for(size_t i=0; i<rects.size(); ++i)
    {
        index[i] = i;
    }
    
    std::sort(index.begin(), index.end(), [&rects](int i, int j)
      {
          const Rect & a = rects[i];
          const Rect & b = rects[j];
          
          if(a.wid > a.hei)
          {
              return (a.wid > b.wid && a.wid > b.hei);
          }
          return (a.hei > b.wid && a.hei > b.hei);
      });
    
    return true;
}



PowerOfTwoBinPacker::PowerOfTwoBinPacker():
    maxWid(0), maxHei(0)
{
}

PowerOfTwoBinPacker::PowerOfTwoBinPacker(int maxWid, int maxHei):
    maxWid(maxWid), maxHei(maxHei)
{
}

bool PowerOfTwoBinPacker::process(std::vector<Rect> & rects)
{
    int minWid, minHei;
    if(!preprocess(rects, minWid, minHei, maxWid, maxHei))
        return false;
    
    minWid = pow2ceil(minWid);
    minHei = pow2ceil(minHei)>>1;
    bool swap = false;
    
    while(true)
    {
        if(swap)
            minWid <<= 1;
        else
            minHei <<= 1;
        swap ^= 1;
        
        if(binpack(rects, minWid, minHei))
        {
            m_width = minWid;
            m_height = minHei;
            break;
        }
    }
    
    return true;
}

int PowerOfTwoBinPacker::width() const
{
    return m_width;
}
int PowerOfTwoBinPacker::height() const
{
    return m_height;
}
