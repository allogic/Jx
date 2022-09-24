#ifndef OPCODE_H
#define OPCODE_H

namespace decompiler
{
  enum instruction_kind
  {
    UNKNOWN,

    // Integer Math
    ADDU, // Add Unsigned Word
    ADDIU, // Add Immediate Unsigned Word
    DADDU,
    DADDIU, // Doubleword Add Immediate Unsigned
    SUBU,
    DSUBU,
    MULT3, // special EE three-operand multiply
    MULTU3,
    DIV,
    DIVU,

    // Stores
    SB,
    SH,
    SW,
    SWC1,
    SD,
    SQ,
    SQC2,

    // Loads
    LB,
    LBU,
    LH,
    LHU,
    LW,
    LWU,
    LWL,
    LWR,
    LWC1,
    LD,
    LDL,
    LDR,
    LQ,
    LQC2,
    LUI,

    // Logical
    AND,
    ANDI,
    OR,
    ORI,
    XOR,
    XORI,
    NOR,

    // Moves
    MOVN,
    MOVZ,
    MFHI,
    MFLO,
    MFLO1,
    MTLO1,
    MFPC,
    MTPC,
    MTC0,
    MFC0,
    MTDAB,
    MTDABM,
    MFC1,
    MTC1,
    QMFC2,
    QMTC2,
    CTC2,
    CFC2,

    // Jumps
    JALR,
    JR,

    // Branch
    BEQ,
    BEQL,
    BNE,
    BNEL,
    BLTZ,
    BLTZL,
    BGTZ,
    BGTZL,
    BGEZ,
    BGEZL,
    BLEZ,
    BGEZAL,

    // Shift
    SLL,
    SLLV,
    SRL,
    SRA,
    DSLL,
    DSLL32,
    DSLLV,
    DSRL,
    DSRL32,
    DSRLV,
    DSRA,
    DSRA32,
    DSRAV,

    // Compare
    SLT,
    SLTI,
    SLTU,
    SLTIU,

    // Weird
    SYSCALL,
    SYNCP,
    SYNCL,
    ERET,
    EI,
    CACHE_DXWBIN,
    PREF,
    MTSAB,
    QFSRV,

    // MMI unsorted
    PSLLW,
    PSRAW,
    PSRAH,
    PLZCW,
    PMFHL_UW,
    PMFHL_LW,
    PMFHL_LH,
    PSLLH,
    PSRLH,

    // MMI 0
    PEXTLW,
    PPACH,
    PSUBW,
    PCGTB,
    PCGTW,
    PEXTLH,
    PEXTLB,
    PMAXH,
    PPACB,
    PADDW,
    PADDH,
    PMAXW,
    PPACW,
    PSUBH,
    PADDB,

    // MMI 1
    PCEQW,
    PEXTUW,
    PMINH,
    PEXTUH,
    PEXTUB,
    PCEQB,
    PMINW,
    PABSW,

    // MMI 2
    PCPYLD,
    PROT3W,
    PAND,
    PMADDH,
    PMULTH,
    PEXEW,
    PXOR,

    // MMI 3
    POR,
    PCPYUD,
    PNOR,
    PCPYH,
    PINTEH,

    // COP1 / FPU
    ADDS,
    SUBS,
    MULS,
    DIVS,
    MINS,
    MAXS,
    ABSS,
    NEGS,
    CVTSW,
    CVTWS,
    CLTS,
    CLES,
    CEQS,
    BC1F,
    BC1T,
    BC1FL,
    BC1TL,
    MULAS,
    MADDAS,
    ADDAS,
    MSUBAS,
    MADDS,
    MSUBS,
    MOVS,
    SQRTS,
    RSQRTS,

    // COP2
    VMOVE,  // first cop2 macro instruction
    VFTOI0,
    VFTOI4,
    VFTOI12,
    VFTOI15,
    VITOF0,
    VITOF12,
    VITOF15,
    VABS,

    VADD,
    VSUB,
    VMUL,
    VMINI,
    VMAX,
    VOPMSUB,
    VMADD,
    VMSUB,

    VADD_BC,
    VSUB_BC,
    VMUL_BC,
    VMULA_BC,
    VMADD_BC,

    VADDA_BC,
    VMADDA_BC,
    VMSUBA_BC,
    VMSUB_BC,
    VMINI_BC,
    VMAX_BC,

    VADDQ,
    VSUBQ,
    VMULQ,
    VMSUBQ,

    VMULA,
    VADDA,
    VMADDA,
    VMSUBA,

    VOPMULA,
    VDIV,
    VCLIP,
    VMULAQ,

    VMTIR,
    VIAND,
    VLQI,
    VIADDI,
    VSQI,

    VRGET,

    VSQRT,
    VRSQRT,

    VRXOR,
    VRNEXT,
    VNOP,
    VWAITQ,
    VMR32,
    VCALLMS,  // last cop2 macro instruction

    EE_OP_MAX
  };
}

#endif