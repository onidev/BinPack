#include <onidev.h>
#include <GL/gl.h>
#include "binpack.h"

struct Object
{
    int x, y, wid, hei;
    unsigned char r, g, b;
};

int main()
{
    od::Window win(512, 512, "OniDev app");
    win.setSynchronization(true);
    
    srand(time(0));
    // Items to binpack
    std::vector<Object> objects;
    for(int i=0; i<7; ++i)
    for(int j=0; j<7; ++j)
    {
        unsigned char r = rand()%255;
        unsigned char g = rand()%255;
        unsigned char b = rand()%255;
        objects.push_back( Object{i*60, j*60, 8 + rand()%48, 8 + rand()%48, r, g, b} );
    }
    
    // Temporary rectangles for binpacking
    std::vector<BinPacker::Rect> boxes;
    for(Object & o: objects)
    {
        boxes.push_back( BinPacker::Rect(o.x, o.y, o.wid, o.hei) );
    }
    
    PowerOfTwoBinPacker packer(win.width(), win.height());
    
    // interpolated render
    std::vector<BinPacker::Rect> temp = boxes;
    float timer = 0;
    
    while(win.open())
    {
        win.ioHandle();
        
        if(od::keyPressed(od::vk_escape))
            break;
        
        od::drawClear();
        win.updateView();
        
        // Binpack
        if(od::keyPressed(od::vk_enter) && timer == 0.f)
        {
            bool ret = packer.process(boxes);
    
            if(ret == false)
                return 1;
            
            timer = 0.001f;
        }
        
        if(timer > 0 && timer < 1.f)
        {
            timer += 0.015f;
            
            if(timer >= 1.f)
            {
                for(size_t i=0; i<objects.size(); ++i)
                {
                    objects[i].x = boxes[i].x;
                    objects[i].y = boxes[i].y;
                }
            }
            else
            {
                for(size_t i=0; i<objects.size(); ++i)
                {
                    objects[i].x = temp[i].x * (1.f - timer) + boxes[i].x * timer;
                    objects[i].y = temp[i].y * (1.f - timer) + boxes[i].y * timer;
                }
            }
        }
        else if(od::keyPressed(od::vk_space))
        {
            timer = 0;
            for(int i=0; i<7; ++i)
            for(int j=0; j<7; ++j)
            {
                auto & o = objects[i + j*7];
                o.x = i*60;
                o.y = j*60;
                o.wid = 8 + rand()%48;
                o.hei = 8 + rand()%48;
                boxes[i + j*7] = BinPacker::Rect(o.x, o.y, o.wid, o.hei);
            }
            temp = boxes;
        }
        
        for(Object & o: objects)
        {
            glColor3ub(o.r, o.g, o.b);
            od::drawRectangle(o.x, o.y, o.x + o.wid, o.y + o.hei, false);
            glColor3ub(o.r * 0.4, o.g * 0.4, o.b * 0.4);
            od::drawRectangle(o.x, o.y, o.x + o.wid, o.y + o.hei, true);
        }
        
        if(timer >= 1.f)
        {
            glColor3f(0, 0, 0);
            od::drawRectangle(0, 0, packer.width(), packer.height(), true);
        }
        
        win.screenRefresh();
    }
    
    return 0;
}
