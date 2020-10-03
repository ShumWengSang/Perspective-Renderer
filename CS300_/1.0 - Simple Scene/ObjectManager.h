//
// Created by roland on 10/1/20.
//

#ifndef SIMPLE_SCENE_OBJECTMANAGER_H
#define SIMPLE_SCENE_OBJECTMANAGER_H

class Object;

class ObjectManager {
    std::vector<SharedPtr<Object>> Objects;
public:
    SharedPtr<Object> CreateObject();
    // Reminder: NEVER CREATE AN UPDATE FOR ALL OBJECTS HERE
    // need to rethink update if you do.

    void RenderAllObject();
};


#endif //SIMPLE_SCENE_OBJECTMANAGER_H
