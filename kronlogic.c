#include "kronlogic.h"

/* =========================================================
 * Contacts
 * ========================================================= */

bool NO_Contact(bool bit) {
    return bit;
}

bool NC_Contact(bool bit) {
    return !bit;
}

bool INV_Contact(bool input) {
    return !input;
}

/* =========================================================
 * Coils
 * ========================================================= */

void Normal_Coil(bool rung, bool *output) {
    *output = rung;
}

void Set_Coil(bool rung, bool *output) {
    if (rung) {
        *output = true;
    }
}

void Reset_Coil(bool rung, bool *output) {
    if (rung) {
        *output = false;
    }
}

/* =========================================================
 * SR Bistable (Set dominant)
 * ========================================================= */

void SR_Call(SR *inst) {
    if (inst->S1) {
        inst->Q1 = true;
    } else if (inst->R) {
        inst->Q1 = false;
    }
}

/* =========================================================
 * RS Bistable (Reset dominant)
 * ========================================================= */

void RS_Call(RS *inst) {
    if (inst->R1) {
        inst->Q1 = false;
    } else if (inst->S) {
        inst->Q1 = true;
    }
}

/* =========================================================
 * Rising Edge Trigger
 * ========================================================= */

void R_TRIG_Call(R_TRIG *inst) {
    inst->Q = inst->CLK && !inst->M;
    inst->M = inst->CLK;
}

/* =========================================================
 * Falling Edge Trigger
 * ========================================================= */

void F_TRIG_Call(F_TRIG *inst) {
    inst->Q = !inst->CLK && inst->M;
    inst->M = inst->CLK;
}
