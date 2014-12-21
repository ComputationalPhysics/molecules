#ifndef ATOM_TYPES_H
#define ATOM_TYPES_H



#define ARGON  18
#define FIXED 1
#define FROZEN 0

class AtomType {
public:
    enum AtomTypeList {
        Frozen = 0,
        Fixed = 1,
        Argon = 18,
        Argon2 = 181
    };
};

#endif
