// sub_130ba38 @ 0140ba38 size=616
// why: direct:0x0140ba38
// callers: sub_c49776@00d49776, sub_c7c3f2@00d7c3f2
// callees: sub_153da8, sub_14e69b, sub_14e70a, sub_162008, sub_121aa3a, sub_1215012, sub_1234712, sub_121d00a, sub_120b9ee, __cxa_guard_acquire@plt, __cxa_atexit@plt, __cxa_guard_release@plt
// lit 0140ba3f	free@plt
// lit 0140ba43	[assembly:/repository/pro.repo](fullgame,dynamic).{0}_repo
// lit 0140ba51	free@plt
// lit 0140ba58	[assembly:/repository/pro.repo](fullgame,dynamic).{0}_repo
// lit 0140ba87	rodata
// lit 0140baa4	s.1
// lit 0140bb3f	rodata
// lit 0140bb3f	PATH@
// lit 0140bb43	E:/p4/compile_del05/del05/code/_out/ps5/Retail/engine.elf
// lit 0140bb48	free@plt
// lit 0140bb48	PATH@
// lit 0140bb54	ta.1
// lit 0140bb58	E:/p4/compile_del05/del05/code/_out/ps5/Retail/engine.elf
// lit 0140bb5b	data
// lit 0140bb7b	e@plt
// lit 0140bb80	exattr_init@plt
// lit 0140bb80	PATH@
// lit 0140bb8b	rodata
// lit 0140bb8b	e@plt
// lit 0140bb92	rodata
// lit 0140bb92	e@plt
// lit 0140bba0	rodata
// lit 0140bba4	plt
// lit 0140bba8	free@plt
// lit 0140bba8	.shstrtab
// lit 0140bba8	lt
// lit 0140bc40	free@plt
// lit 0140bc64	ps5

void sub_130ba38(void)

{
  uint uVar1;
  int *piVar2;
  int iVar3;
  undefined8 uVar4;
  undefined8 *puVar5;
  uint uVar6;
  long lVar7;
  undefined8 *local_58;
  undefined4 *local_50;
  uint local_48 [2];
  long local_40;
  undefined8 local_38;
  uint local_30;
  uint uStack_2c;
  long lStack_28;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  
  local_38 = CONCAT44(local_38._4_4_,0x8000003a);
  DAT_051fb818 = 1;
  local_30 = 0x1d348bc;
  uStack_2c = 0;
  if ((DAT_032f6058 == '\0') && (iVar3 = __cxa_guard_acquire_plt(&DAT_032f6058), iVar3 != 0)) {
    DAT_032f6060 = 0x80000003;
    DAT_032f6068 = &DAT_01d93e2e;
    __cxa_atexit_plt(sub_14e65a,&DAT_032f6060,&DAT_0267c010);
    __cxa_guard_release_plt(&DAT_032f6058);
  }
  local_58 = &DAT_0267c968;
  local_50 = &DAT_032f6060;
  sub_153da8(local_48,&local_38,&local_58,&DAT_032fa560,&DAT_032fa560,&DAT_032fa560,&DAT_032fa560,
             &DAT_032fa560,&DAT_032fa560,&DAT_032fa560);
  if ((uint)local_38 < 0x40000000) {
    lVar7 = CONCAT44(uStack_2c,local_30);
    uVar4 = sub_14e69b();
    sub_14e70a(uVar4,lVar7 + -0x10);
  }
  sub_162008(&local_38,local_48);
  puVar5 = (undefined8 *)sub_121aa3a(&local_38);
  local_58 = puVar5;
  if ((uint)local_38 < 0x40000000) {
    lVar7 = CONCAT44(uStack_2c,local_30);
    uVar4 = sub_14e69b();
    sub_14e70a(uVar4,lVar7 + -0x10);
  }
  piVar2 = DAT_051e8c48;
  iVar3 = sub_1215012(DAT_051e8c48,puVar5);
  if (-1 < iVar3) {
    local_38 = 0xffffffff00000000;
    local_30 = local_30 & 0xffffff00;
    uStack_2c = 0;
    lStack_28 = SUB128(ZEXT812(0),4);
    uStack_20 = 0;
    uStack_1c = 0;
    sub_1234712(piVar2,iVar3,&local_38);
    uVar6 = 0xffffffff;
    lVar7 = 0;
    do {
      do {
        if (lVar7 == 0) goto LAB_0140bb73;
        iVar3 = *(int *)(lVar7 + -4);
        while (iVar3 == -1) {
LAB_0140bb73:
          uVar6 = uVar6 + 1;
          if (uStack_2c <= uVar6) {
            sub_121d00a(&local_38);
            (**(code **)(DAT_051e8880 + 0x48))(&local_38,&DAT_051e8880,&local_58,0);
            if (DAT_051fb810 == (uint)local_38) {
              if (DAT_051fb810 < 0) goto LAB_0140bc1f;
              sub_120b9ee(DAT_051e8c48);
              iVar3 = DAT_051fb810;
            }
            else {
              iVar3 = (uint)local_38;
              if (-1 < DAT_051fb810) {
                sub_120b9ee(DAT_051e8c48);
                iVar3 = (uint)local_38;
              }
            }
            DAT_051fb810 = iVar3;
            local_38 = CONCAT44(local_38._4_4_,0xffffffff);
            goto LAB_0140bc1f;
          }
          iVar3 = *(int *)(lStack_28 + (ulong)uVar6 * 4);
        }
        lVar7 = CONCAT44(uStack_1c,uStack_20) + 4 + (long)iVar3 * 8;
      } while (*(char *)((long)piVar2 + 0x139) == '\0');
      uVar1 = *(uint *)(CONCAT44(uStack_1c,uStack_20) + 4 + (long)iVar3 * 8);
    } while (((-1 < (int)uVar1) && ((int)uVar1 < *piVar2)) &&
            (*(char *)(*(long *)(piVar2 + 2) + 0x3e + (ulong)uVar1 * 0x40) != -1));
    sub_121d00a(&local_38);
  }
LAB_0140bc1f:
  if (local_48[0] < 0x40000000) {
    uVar4 = sub_14e69b();
    sub_14e70a(uVar4,local_40 + -0x10);
  }
  return;
}


