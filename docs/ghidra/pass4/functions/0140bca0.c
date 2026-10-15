// sub_130bca0 @ 0140bca0 size=207
// why: direct:0x0140bca0
// callers: sub_c49776@00d49776, sub_c7c3f2@00d7c3f2
// callees: sub_130b82a, sub_120b9ee
// lit 0140bcaa	PATH@
// lit 0140bccc	plt
// lit 0140bcd0	rodata
// lit 0140bcd4	PATH@
// lit 0140bcd9	free@plt
// lit 0140bcd9	s.1
// lit 0140bce0	ree@plt
// lit 0140bceb	e@plt
// lit 0140bceb	PATH@
// lit 0140bcf2	free@plt
// lit 0140bcf2	.rodata.1
// lit 0140bd2a	e@plt
// lit 0140bd2a	PATH@
// lit 0140bd58	free@plt
// lit 0140bd5a	PATH@
// lit 0140bd64	rodata

undefined8 sub_130bca0(undefined1 *param_1)

{
  int *piVar1;
  uint uVar2;
  undefined1 uVar3;
  undefined8 uVar4;
  
  uVar2 = DAT_051fb810;
  if (DAT_051fb818 == '\0') {
    uVar3 = 0;
    uVar4 = 0;
  }
  else {
    if ((int)DAT_051fb810 < 0) {
      uVar3 = sub_130b82a();
    }
    else {
      uVar3 = 0;
      uVar4 = 0;
      if (1 < *(int *)(*(long *)(DAT_051e8c48 + 8) + 0x20 + (ulong)DAT_051fb810 * 0x40) - 3U)
      goto LAB_0140bd61;
      LOCK();
      piVar1 = (int *)(*(long *)(DAT_051e8c48 + 8) + 0x24 + (ulong)DAT_051fb810 * 0x40);
      *piVar1 = *piVar1 + 1;
      UNLOCK();
      uVar3 = sub_130b82a();
      if (-1 < (int)uVar2) {
        sub_120b9ee(DAT_051e8c48,uVar2);
      }
    }
    uVar2 = DAT_051fb810;
    DAT_051fb810 = 0xffffffff;
    if (-1 < (int)uVar2) {
      sub_120b9ee(DAT_051e8c48);
    }
    uVar4 = 1;
    DAT_051fb818 = '\0';
  }
LAB_0140bd61:
  *param_1 = uVar3;
  return uVar4;
}


