#ifndef PANTARGET_H
#define PANTARGET_H

#pragma once

class PanTarget {
public:
    virtual void pan(vec3f amount) = 0;
};

#endif
