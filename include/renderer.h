#ifndef RENDERER_H
#define RENDERER_H

#include "glad.h"
#include "drawable.h"
#include <vector>

class Renderer {
public:
    Renderer(bool depthTest) {
        if (depthTest) glEnable(GL_DEPTH_TEST);
    };

    void addDrawable(Drawable *obj)
    {
        objects.push_back(obj);
    };

    void render()
    {
        for (auto &obj: objects)
        {
            obj->draw();
        }
    }

private:
    std::vector<Drawable*> objects;
};

#endif
