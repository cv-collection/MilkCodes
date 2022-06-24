#pragma once
#ifndef _NOTCOPYABLE_H
#define _NOTCOPYABLE_H

class NotCopyable
{
protected:
    NotCopyable() {}
    ~NotCopyable() {}

    NotCopyable(const NotCopyable &) = delete;                                    
    NotCopyable &operator=(const NotCopyable &) = delete;
};

#endif

