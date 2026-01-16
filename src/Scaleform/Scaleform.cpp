#include "Scaleform/Scaleform.h"

#include "Scaleform/FloatingDamageMenu.h"

namespace Scaleform
{
    void Register()
    {
        FloatingDamageMenu::Register();
        logger::info("Registered scaleform menu");
    }
}

