#include <onidev.h>
#include <GL/gl.h>
#include "binpack.h"

int main()
{
    od::Window win(512, 512, "OniDev app");
    win.setSynchronization(true);
    
    srand(time(0));
    std::vector<BinPacker::Rect> boxes;
    for(int i=0; i<50; ++i)
    {
        boxes.push_back( BinPacker::Rect(0, 0, 8 + rand()%48, 8 + rand()%48) );
    }
    
    PowerOfTwoBinPacker packer(win.width(), win.height());
    
    bool ret = packer.process(boxes);
    
    if(ret == false)
        return 1;
    
    while(win.open())
    {
        win.ioHandle();
        
        if(od::keyPressed(od::vk_escape))
            break;
        
        od::drawClear();
        win.updateView();
        
        glColor3f(0, 0, 0);
        for(BinPacker::Rect & o: boxes)
        {
            od::drawRectangle(o.x, o.y, o.x + o.wid, o.y + o.hei, true);
        }
        od::drawRectangle(0, 0, packer.width(), packer.height(), true);
        
        win.screenRefresh();
    }
    
    return 0;
}
