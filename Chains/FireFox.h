#ifndef FIREFOX_H
#define FIREFOX_H

#include "Chain.h"

//Do a fully invincible edge stall
class FireFox : public Chain
{

public:

    FireFox();
    ~FireFox();
    //Determine what buttons to press in order to execute our tactic
    void PressButtons();
    bool IsInterruptible();
    std::string ToString(){return "FireFox";};

  private:
      bool m_isRightEdge;
      bool m_hasUpBd;

};

#endif
