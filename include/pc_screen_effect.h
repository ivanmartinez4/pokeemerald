#ifndef GUARD_PC_SCREEN_EFFECT_H
#define GUARD_PC_SCREEN_EFFECT_H

struct PCScreenEffectStruct
{
    /*0x00*/ u16 tileTag;
    /*0x02*/ u16 paletteTag;
    /*0x10*/ u32 selectedPalettes;
};

#endif //GUARD_PC_SCREEN_EFFECT_H
