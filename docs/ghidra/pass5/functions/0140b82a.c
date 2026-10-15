// sub_130b82a @ 0140b82a size=301
// why: direct:0x0140b82a
// callers: sub_130beda@0140beda, sub_130c08c@0140c08c, sub_130bca0@0140bca0
// callees: sub_120c0f2
// lit 0140b82f	e@plt
// lit 0140b847	plt
// lit 0140b84b	rodata
// lit 0140b84f	free@plt
// lit 0140b84f	s.1
// lit 0140b84f	e@plt
// lit 0140b85a	free@plt
// lit 0140b85a	rodata
// lit 0140b85a	PATH@
// lit 0140b870	plt
// lit 0140b874	free@plt
// lit 0140b874	s.1
// lit 0140b874	e@plt
// lit 0140b87b	free@plt
// lit 0140b87b	rodata
// lit 0140b87b	PATH@
// lit 0140b8a8	ta.1
// lit 0140b8ac	data
// lit 0140b8ac	PATH@
// lit 0140b8b8	free@plt
// lit 0140b8c6	t_env@plt
// lit 0140b8e0	free@plt
// lit 0140b8ed	plt
// lit 0140b8f1	rodata
// lit 0140b8f5	free@plt
// lit 0140b8f5	s.1
// lit 0140b8f5	e@plt
// lit 0140b8fc	free@plt
// lit 0140b8fc	rodata
// lit 0140b8fc	PATH@
// lit 0140b926	ta.1
// lit 0140b92a	data
// lit 0140b92a	PATH@
// lit 0140b936	free@plt
// lit 0140b944	plt
// lit 0140b950	e@plt

ulong sub_130b82a(uint *param_1)

{
  long lVar1;
  uint *unaff_RBX;
  ulong uVar2;
  
  uVar2 = (ulong)*param_1;
  if (-1 < (int)*param_1) {
    lVar1 = *(long *)(DAT_051e8c48 + 8);
    if ((*(int *)(lVar1 + 0x20 + uVar2 * 0x40) == 4) && (*(long *)(lVar1 + 8 + uVar2 * 0x40) != 0))
    {
      if (((-1 < (int)DAT_051fb808) && (*(int *)(lVar1 + 0x20 + (ulong)DAT_051fb808 * 0x40) == 4))
         && (*(long *)(lVar1 + 8 + (ulong)DAT_051fb808 * 0x40) != 0)) {
        (**(code **)(DAT_051e8880 + 0xf8))(&DAT_051e8880);
        uVar2 = (ulong)*param_1;
        unaff_RBX = param_1;
      }
      sub_120c0f2(&DAT_051fb808,uVar2);
      uVar2 = CONCAT71((int7)((ulong)unaff_RBX >> 8),1);
      if (-1 < (int)DAT_051fb808) {
        if ((*(int *)(*(long *)(DAT_051e8c48 + 8) + 0x20 + (ulong)DAT_051fb808 * 0x40) == 4) &&
           (*(long *)(*(long *)(DAT_051e8c48 + 8) + 8 + (ulong)DAT_051fb808 * 0x40) != 0)) {
          (**(code **)(DAT_051e8880 + 0xf0))(&DAT_051e8880);
        }
      }
      goto LAB_0140b94e;
    }
  }
  uVar2 = 0;
LAB_0140b94e:
  return uVar2 & 0xffffffff;
}


