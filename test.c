#include <stdio.h>
#include "kronlogic.h"

static int pass_count = 0;
static int fail_count = 0;

static void check(const char *name, bool condition) {
    if (condition) {
        printf("  [PASS] %s\n", name);
        pass_count++;
    } else {
        printf("  [FAIL] %s\n", name);
        fail_count++;
    }
}

/* ─────────────────────────────────────────────
 * Contacts
 * ───────────────────────────────────────────── */
static void test_contacts(void) {
    printf("\n--- NO_Contact (Normally Open) ---\n");
    check("NO passes true when bit=true",  NO_Contact(true)  == true);
    check("NO blocks false when bit=false", NO_Contact(false) == false);

    printf("\n--- NC_Contact (Normally Closed) ---\n");
    check("NC passes true when bit=false", NC_Contact(false) == true);
    check("NC blocks true when bit=true",  NC_Contact(true)  == false);

    printf("\n--- INV_Contact (Invert) ---\n");
    check("INV inverts true to false",  INV_Contact(true)  == false);
    check("INV inverts false to true",  INV_Contact(false) == true);

    printf("\n--- Contact chaining ---\n");
    bool a = true, b = false;
    bool rung = NO_Contact(a) && NC_Contact(b);
    check("NO(true) AND NC(false) = true", rung == true);

    rung = NO_Contact(a) && NC_Contact(a);
    check("NO(true) AND NC(true) = false", rung == false);

    rung = INV_Contact(NO_Contact(a));
    check("INV(NO(true)) = false", rung == false);
}

/* ─────────────────────────────────────────────
 * Coils
 * ───────────────────────────────────────────── */
static void test_coils(void) {
    printf("\n--- Normal_Coil ---\n");
    bool out = false;
    Normal_Coil(true, &out);
    check("Normal coil sets output true when rung=true",  out == true);
    Normal_Coil(false, &out);
    check("Normal coil sets output false when rung=false", out == false);

    printf("\n--- Set_Coil ---\n");
    out = false;
    Set_Coil(false, &out);
    check("Set coil does not set when rung=false", out == false);
    Set_Coil(true, &out);
    check("Set coil latches true when rung=true", out == true);
    Set_Coil(false, &out);
    check("Set coil retains true after rung goes false", out == true);

    printf("\n--- Reset_Coil ---\n");
    out = true;
    Reset_Coil(false, &out);
    check("Reset coil does not reset when rung=false", out == true);
    Reset_Coil(true, &out);
    check("Reset coil latches false when rung=true", out == false);
    Reset_Coil(false, &out);
    check("Reset coil retains false after rung goes false", out == false);

    printf("\n--- Set + Reset interaction ---\n");
    out = false;
    Set_Coil(true, &out);
    check("Set: out = true", out == true);
    Reset_Coil(true, &out);
    check("Reset after Set: out = false", out == false);
    Reset_Coil(true, &out);
    check("Reset again: out stays false", out == false);
}

/* ─────────────────────────────────────────────
 * SR Bistable
 * ───────────────────────────────────────────── */
static void test_SR(void) {
    printf("\n--- SR (Set Dominant Bistable) ---\n");
    SR sr = {0};

    sr.S1 = true; sr.R = false;
    SR_Call(&sr);
    check("Q1 set when S1=1, R=0", sr.Q1 == true);

    sr.S1 = false; sr.R = true;
    SR_Call(&sr);
    check("Q1 reset when S1=0, R=1", sr.Q1 == false);

    sr.S1 = false; sr.R = false;
    SR_Call(&sr);
    check("Q1 retains false when S1=0, R=0", sr.Q1 == false);

    sr.S1 = true; sr.R = false;
    SR_Call(&sr);
    sr.S1 = false; sr.R = false;
    SR_Call(&sr);
    check("Q1 retains true when S1=0, R=0 after set", sr.Q1 == true);

    sr.S1 = true; sr.R = true;
    SR_Call(&sr);
    check("Q1 true when S1=1, R=1 (Set dominant)", sr.Q1 == true);
}

/* ─────────────────────────────────────────────
 * RS Bistable
 * ───────────────────────────────────────────── */
static void test_RS(void) {
    printf("\n--- RS (Reset Dominant Bistable) ---\n");
    RS rs = {0};

    rs.S = true; rs.R1 = false;
    RS_Call(&rs);
    check("Q1 set when S=1, R1=0", rs.Q1 == true);

    rs.S = false; rs.R1 = true;
    RS_Call(&rs);
    check("Q1 reset when S=0, R1=1", rs.Q1 == false);

    rs.S = false; rs.R1 = false;
    RS_Call(&rs);
    check("Q1 retains false when S=0, R1=0", rs.Q1 == false);

    rs.S = true; rs.R1 = false;
    RS_Call(&rs);
    rs.S = false; rs.R1 = false;
    RS_Call(&rs);
    check("Q1 retains true when S=0, R1=0 after set", rs.Q1 == true);

    rs.S = true; rs.R1 = true;
    RS_Call(&rs);
    check("Q1 false when S=1, R1=1 (Reset dominant)", rs.Q1 == false);
}

/* ─────────────────────────────────────────────
 * R_TRIG
 * ───────────────────────────────────────────── */
static void test_R_TRIG(void) {
    printf("\n--- R_TRIG (Rising Edge Trigger) ---\n");
    R_TRIG rt = {0};

    rt.CLK = false;
    R_TRIG_Call(&rt);
    check("Q false when CLK stays low", rt.Q == false);

    rt.CLK = true;
    R_TRIG_Call(&rt);
    check("Q true on rising edge (scan 1)", rt.Q == true);

    R_TRIG_Call(&rt);
    check("Q false on second scan while CLK stays high", rt.Q == false);

    rt.CLK = false;
    R_TRIG_Call(&rt);
    check("Q false on falling edge", rt.Q == false);

    rt.CLK = true;
    R_TRIG_Call(&rt);
    check("Q true on next rising edge", rt.Q == true);

    R_TRIG_Call(&rt);
    check("Q false again after one scan", rt.Q == false);
}

/* ─────────────────────────────────────────────
 * F_TRIG
 * ───────────────────────────────────────────── */
static void test_F_TRIG(void) {
    printf("\n--- F_TRIG (Falling Edge Trigger) ---\n");
    F_TRIG ft = {0};

    ft.CLK = true;
    F_TRIG_Call(&ft);
    check("Q false on rising edge (no prior fall)", ft.Q == false);

    F_TRIG_Call(&ft);
    check("Q false while CLK stays high", ft.Q == false);

    ft.CLK = false;
    F_TRIG_Call(&ft);
    check("Q true on falling edge (scan 1)", ft.Q == true);

    F_TRIG_Call(&ft);
    check("Q false on second scan while CLK stays low", ft.Q == false);

    ft.CLK = true;
    F_TRIG_Call(&ft);
    check("Q false on rising edge", ft.Q == false);

    ft.CLK = false;
    F_TRIG_Call(&ft);
    check("Q true on next falling edge", ft.Q == true);

    F_TRIG_Call(&ft);
    check("Q false again after one scan", ft.Q == false);
}

/* ─────────────────────────────────────────────
 * Ladder logic simulation: interlock example
 *   Start button (NO) AND Not-fault (NC) → motor coil
 *   Emergency stop (NC) gates everything
 * ───────────────────────────────────────────── */
static void test_ladder_simulation(void) {
    printf("\n--- Ladder simulation: motor start/stop interlock ---\n");

    bool start_btn  = false;
    bool fault_bit  = false;
    bool estop_bit  = false; /* E-stop = true means pressed (open circuit) */
    bool motor_run  = false;
    bool fault_latch = false;
    SR   motor_sr   = {0};
    R_TRIG start_edge = {0};

    /* scan 1: all off, motor should be off */
    bool rung_estop = NC_Contact(estop_bit);
    bool rung_start = rung_estop && NO_Contact(start_btn) && NC_Contact(fault_bit);
    bool rung_fault_reset = rung_estop && NO_Contact(false);
    start_edge.CLK = rung_start;
    R_TRIG_Call(&start_edge);
    motor_sr.S1 = start_edge.Q;
    motor_sr.R  = fault_latch || !rung_estop;
    SR_Call(&motor_sr);
    Normal_Coil(motor_sr.Q1, &motor_run);
    check("Motor off at startup", motor_run == false);

    /* scan 2: press start button, no fault */
    start_btn = true;
    rung_estop = NC_Contact(estop_bit);
    rung_start = rung_estop && NO_Contact(start_btn) && NC_Contact(fault_bit);
    start_edge.CLK = rung_start;
    R_TRIG_Call(&start_edge);
    motor_sr.S1 = start_edge.Q;
    motor_sr.R  = fault_latch || !rung_estop;
    SR_Call(&motor_sr);
    Normal_Coil(motor_sr.Q1, &motor_run);
    check("Motor starts on rising edge of Start button", motor_run == true);

    /* scan 3: button released, motor should stay on (latched) */
    start_btn = false;
    rung_estop = NC_Contact(estop_bit);
    rung_start = rung_estop && NO_Contact(start_btn) && NC_Contact(fault_bit);
    start_edge.CLK = rung_start;
    R_TRIG_Call(&start_edge);
    motor_sr.S1 = start_edge.Q;
    motor_sr.R  = fault_latch || !rung_estop;
    SR_Call(&motor_sr);
    Normal_Coil(motor_sr.Q1, &motor_run);
    check("Motor stays on after button released (SR latch)", motor_run == true);

    /* scan 4: E-stop pressed */
    estop_bit = true;
    rung_estop = NC_Contact(estop_bit);
    motor_sr.S1 = false;
    motor_sr.R  = fault_latch || !rung_estop;
    SR_Call(&motor_sr);
    Normal_Coil(motor_sr.Q1, &motor_run);
    check("Motor stops when E-stop pressed", motor_run == false);
}

/* ─────────────────────────────────────────────
 * main
 * ───────────────────────────────────────────── */
int main(void) {
    printf("========================================\n");
    printf("  KronLogic Unit Tests\n");
    printf("========================================\n");

    test_contacts();
    test_coils();
    test_SR();
    test_RS();
    test_R_TRIG();
    test_F_TRIG();
    test_ladder_simulation();

    printf("\n========================================\n");
    printf("  Results: %d passed, %d failed\n", pass_count, fail_count);
    printf("========================================\n");

    return (fail_count == 0) ? 0 : 1;
}
