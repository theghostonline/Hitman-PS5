// sub_12127da @ 013127da size=1777
// why: direct:0x013127da
// callers: sub_c49776@00d49776, sub_c7c3f2@00d7c3f2, sub_1211384@01311384
// callees: sub_14fd10, sub_153fb6, sub_1540da, sub_14e69b, sub_14e70a, sub_db50b8, sub_151d1e, sub_14fe08, strstr@plt, sub_17a288, sub_14fe52, memmove@plt, memset@plt, sub_166e34
// lit 013127e7	plt
// lit 013127fc	System.IO.FileInputStream
// lit 01312811	rodata
// lit 01312811	System.IO.FileInputStream
// lit 01312828	rodata
// lit 0131282c	data
// lit 01312830	free@plt
// lit 0131287c	ee@plt
// lit 0131288d	System.IO.BinaryReader
// lit 0131289a	rodata
// lit 0131289a	System.IO.BinaryReader
// lit 013128b1	PATH@
// lit 013128c0	free@plt
// lit 013128e9	data
// lit 013128ed	.shstrtab
// lit 01312932	PATH@
// lit 01312941	PATH@
// lit 0131294a	free@plt
// lit 01312961	free@plt
// lit 01312971	free@plt
// lit 01312982	free@plt
// lit 01312989	ree@plt
// lit 01312995	ree@plt
// lit 01312a05	free@plt
// lit 01312a05	ree@plt
// lit 01312a0b	free@plt
// lit 01312a6f	patch
// lit 01312a76	System.IO.FileInputStream
// lit 01312a9a	StaticShadowMap
// lit 01312af9	free@plt
// lit 01312b09	free@plt
// lit 01312b19	free@plt
// lit 01312b33	PATH@
// lit 01312b41	free@plt
// lit 01312b47	ee@plt
// lit 01312b5d	free@plt
// lit 01312b73	ee@plt
// lit 01312b77	ree@plt
// lit 01312b81	data
// lit 01312b9c	rodata
// lit 01312bad	ree@plt
// lit 01312be6	ree@plt
// lit 01312c09	rodata
// lit 01312c24	ree@plt
// lit 01312c28	ree@plt
// lit 01312c41	e@plt
// lit 01312c49	e@plt
// lit 01312c4d	data
// lit 01312c66	rodata
// lit 01312c8a	e@plt
// lit 01312c92	PATH@
// lit 01312cad	data
// lit 01312ce0	rodata
// lit 01312cf0	data
// lit 01312d52	e@plt
// lit 01312d56	e@plt
// lit 01312d5f	rodata
// lit 01312dab	e@plt
// lit 01312db2	e@plt
// lit 01312db6	rodata
// lit 01312dba	e@plt
// lit 01312dbe	e@plt
// lit 01312dc2	data
// lit 01312dc6	data
// lit 01312dca	e@plt
// lit 01312dd6	e@plt
// lit 01312ddd	e@plt
// lit 01312de1	e@plt
// lit 01312de8	e@plt
// lit 01312dec	e@plt
// lit 01312df2	ree@plt
// lit 01312dff	e@plt
// lit 01312e03	e@plt
// lit 01312e07	rodata
// lit 01312e0b	e@plt
// lit 01312e12	e@plt
// lit 01312e1e	data
// lit 01312e42	data
// lit 01312e48	e@plt
// lit 01312e4c	ree@plt
// lit 01312e53	rodata
// lit 01312e6e	ree@plt
// lit 01312e74	ta.1
// lit 01312e8a	e@plt
// lit 01312ea0	ree@plt
// lit 01312eaa	s.1
// lit 01312eb4	s.1
// lit 01312eb7	free@plt
// lit 01312eb9	plt

undefined8 sub_12127da(undefined8 param_1,undefined1 (*param_2) [16])

{
  int iVar1;
  undefined1 (*pauVar2) [16];
  char *pcVar3;
  char cVar4;
  uint uVar5;
  undefined8 uVar6;
  long *plVar7;
  long *plVar8;
  ulong uVar9;
  ulong uVar10;
  long lVar11;
  uint *puVar12;
  long lVar13;
  byte bVar16;
  undefined1 (*pauVar14) [16];
  long *plVar15;
  int iVar17;
  long lVar18;
  uint uVar19;
  uint uVar20;
  ulong uVar21;
  bool bVar22;
  undefined1 auVar23 [16];
  uint local_d8;
  undefined4 uStack_d4;
  char *local_d0;
  uint *local_c8;
  uint local_80 [2];
  long local_78;
  long local_70;
  undefined1 (*local_68) [16];
  long *local_60;
  ulong local_58;
  uint local_50 [2];
  char *local_48;
  uint local_3c;
  uint local_38;
  char local_31;
  
  local_50[0] = 0x80000019;
  local_48 = "System.IO.FileInputStream";
  sub_14fd10(local_80,param_1);
  local_d0 = (char *)&DAT_0267c968;
  local_d8 = 1;
  local_c8 = local_80;
  uVar6 = sub_153fb6();
  plVar7 = (long *)sub_1540da(uVar6,local_50,&DAT_032f62e8,&local_d8);
  if (local_80[0] < 0x40000000) {
    uVar6 = sub_14e69b();
    sub_14e70a(uVar6,local_78 + -0x10);
  }
  cVar4 = (**(code **)(*plVar7 + 0x30))(plVar7);
  if (cVar4 == '\0') {
    return 0;
  }
  local_d8 = 0x80000016;
  local_d0 = "System.IO.BinaryReader";
  plVar8 = (long *)sub_db50b8(&local_d8,plVar7);
  local_80[0] = 0;
  (**(code **)(*plVar8 + 0x40))(plVar8,local_80,&DAT_0267cb20);
  local_60 = plVar7;
  if (local_80[0] == 0x52504b32) {
    local_d8 = 0;
    local_3c = local_3c & 0xffffff00;
    local_38 = local_38 & 0xffffff00;
    local_31 = '\0';
    local_50[0] = local_50[0] & 0xffff0000;
    local_68 = param_2;
    (**(code **)(*plVar8 + 0x40))(plVar8,&local_d8,&DAT_0267cb20);
    (**(code **)(*plVar8 + 0x40))(plVar8,&local_3c,&DAT_0267cbc8);
    (**(code **)(*plVar8 + 0x40))(plVar8,&local_38,&DAT_0267cbc8);
    (**(code **)(*plVar8 + 0x40))(plVar8,&local_31,&DAT_0267cbc8);
    (**(code **)(*plVar8 + 0x38))(plVar8,local_50,2);
    bVar22 = local_31 != '\0';
  }
  else {
    if (local_80[0] != 0x52504b47) {
      return 0;
    }
    sub_151d1e(&local_d8,param_1);
    uVar5 = (uint)local_c8;
    iVar1 = (int)local_d0;
    if (((ulong)local_c8 >> 0x3e & 1) == 0) {
      uVar19 = iVar1 - local_d8;
    }
    else {
      uVar19 = uVar5 & 0xff;
    }
    local_68 = param_2;
    if (uVar19 != 0) {
      if (((ulong)local_c8 & 0x4000000000000000) == 0) {
        uVar19 = iVar1 - local_d8;
      }
      else {
        uVar19 = uVar5 & 0xff;
      }
      uVar20 = uVar19 - 1;
      if (-1 < (int)uVar20) {
        lVar18 = 0;
        do {
          puVar12 = &local_d8;
          if (((ulong)local_c8 & 0x4000000000000000) == 0) {
            puVar12 = (uint *)CONCAT44(uStack_d4,local_d8);
          }
          if (*(char *)((long)puVar12 + lVar18 + (int)uVar20) == '/') {
            iVar17 = (int)lVar18;
            if (-1 < (int)(uVar20 + iVar17)) {
              uVar5 = uVar5 & 0xff;
              if (((ulong)local_c8 & 0x4000000000000000) == 0) {
                uVar5 = iVar1 - local_d8;
              }
              if (~uVar20 + uVar5 == iVar17) {
                local_50[0] = 0x80000000;
                local_48 = "";
              }
              else {
                puVar12 = &local_d8;
                if (((ulong)local_c8 & 0x4000000000000000) == 0) {
                  puVar12 = (uint *)CONCAT44(uStack_d4,local_d8);
                }
                sub_14fe52(local_50,(long)puVar12 + (long)(int)(uVar19 + iVar17),
                           (long)(int)((~uVar20 + uVar5) - iVar17));
              }
              goto LAB_01312a6b;
            }
            break;
          }
          lVar13 = (int)uVar20 + lVar18;
          lVar18 = lVar18 + -1;
        } while (0 < lVar13);
      }
    }
    sub_14fe08(local_50,&local_d8);
LAB_01312a6b:
    pcVar3 = local_48;
    lVar18 = strstr_plt(local_48,"patch");
    bVar22 = lVar18 != 0;
    if (local_50[0] < 0x40000000) {
      uVar6 = sub_14e69b();
      sub_14e70a(uVar6,pcVar3 + -0x10);
    }
    if ((local_c8 < &DAT_4000000000000000) && (CONCAT44(uStack_d4,local_d8) != 0)) {
      (**(code **)(*DAT_032f6d40 + 0x50))();
    }
  }
  local_d8 = 0;
  local_50[0] = 0;
  local_3c = 0;
  (**(code **)(*plVar8 + 0x40))(plVar8,&local_d8,&DAT_0267cbf8);
  (**(code **)(*plVar8 + 0x40))(plVar8,local_50,&DAT_0267cbf8);
  (**(code **)(*plVar8 + 0x40))(plVar8,&local_3c,&DAT_0267cbf8);
  pauVar2 = local_68;
  if (bVar22) {
    local_38 = 0;
    (**(code **)(*plVar8 + 0x40))(plVar8,&local_38,&DAT_0267cbf8);
    pauVar14 = pauVar2 + 3;
    sub_17a288(pauVar14,local_38);
    if ((*(ulong *)pauVar2[4] & 0x4000000000000000) == 0) {
      pauVar14 = *(undefined1 (**) [16])*pauVar14;
    }
    (**(code **)(*plVar8 + 0x38))(plVar8,pauVar14,(ulong)local_38 << 3);
  }
  uVar21 = (ulong)local_d8;
  uVar9 = *(ulong *)pauVar2[1];
  if ((uVar9 & 0x4000000000000000) == 0) {
    uVar5 = (int)(*(long *)(*pauVar2 + 8) - *(long *)*pauVar2 >> 2) * -0x33333333;
  }
  else {
    uVar5 = (uint)uVar9 & 0xff;
  }
  lVar18 = uVar21 - uVar5;
  if (lVar18 == 0 || uVar21 < uVar5) goto LAB_01312e33;
  bVar16 = (byte)(uVar9 >> 8);
  if ((uVar9 & 0x4000000000000000) == 0) {
    uVar5 = (int)((long)((uVar9 & 0x3fffffffffffffff) - *(long *)*pauVar2) >> 2) * -0x33333333;
  }
  else {
    uVar5 = (uint)bVar16;
  }
  if (uVar5 < uVar21) {
    if ((uVar9 & 0x4000000000000000) == 0) {
      uVar10 = (ulong)(uint)((int)((ulong)(*(long *)(*pauVar2 + 8) - *(long *)*pauVar2) >> 2) *
                            -0x33333333);
      uVar5 = (int)((long)((uVar9 & 0x3fffffffffffffff) - *(long *)*pauVar2) >> 2) * -0x33333333;
    }
    else {
      uVar10 = uVar9 & 0xff;
      uVar5 = (uint)bVar16;
    }
    if (uVar5 < uVar21) {
      if (uVar21 * 0x14 < 0x11) {
        if ((uVar9 & 0x4000000000000000) == 0) {
          local_58 = *(ulong *)*pauVar2;
          lVar13 = *(long *)(*pauVar2 + 8);
          auVar23._0_12_ = ZEXT812(0);
          auVar23._12_4_ = 0;
          *pauVar2 = auVar23;
          *(undefined8 *)pauVar2[1] = 0x4000000000000000;
          memmove_plt(pauVar2,local_58,lVar13 - local_58);
          if ((-1 < (long)uVar9) && (local_58 != 0)) {
            (**(code **)(*DAT_032f6d40 + 0x50))();
          }
          goto LAB_01312dc6;
        }
        uVar9 = uVar21 << 8 | uVar9 & 0xffffffffffff00ff;
      }
      else {
        local_58 = uVar10;
        if (local_d8 == 0) {
          local_70 = 0;
          uVar10 = uVar9;
        }
        else {
          local_38 = local_38 & 0xffff0000;
          local_70 = (**(code **)(*DAT_032f6d40 + 0x48))(DAT_032f6d40,uVar21 * 0x14,4,&local_38,0);
          uVar10 = *(ulong *)pauVar2[1];
        }
        if ((uVar10 & 0x4000000000000000) == 0) {
          pauVar14 = *(undefined1 (**) [16])*pauVar2;
          lVar13 = *(long *)(*pauVar2 + 8);
        }
        else {
          lVar13 = (long)pauVar2 + (uVar10 & 0xff) * 0x14;
          pauVar14 = pauVar2;
        }
        memmove_plt(local_70,pauVar14,lVar13 - (long)pauVar14);
        if ((uVar9 < 0x4000000000000000) && (*(long *)*pauVar2 != 0)) {
          (**(code **)(*DAT_032f6d40 + 0x50))();
        }
        *(long *)*pauVar2 = local_70;
        *(ulong *)(*pauVar2 + 8) = local_70 + local_58 * 0x14;
        uVar9 = local_70 + uVar21 * 0x14;
      }
      *(ulong *)pauVar2[1] = uVar9;
    }
LAB_01312dc6:
    uVar9 = *(ulong *)pauVar2[1];
  }
  if ((uVar9 & 0x4000000000000000) == 0) {
    lVar13 = uVar21 * 0x14 + *(long *)*pauVar2 + lVar18 * -0x14;
    lVar11 = *(long *)(*pauVar2 + 8);
  }
  else {
    lVar13 = (long)pauVar2 + lVar18 * -0x14 + uVar21 * 0x14;
    lVar11 = (long)pauVar2 + (uVar9 & 0xff) * 0x14;
  }
  memset_plt(lVar13,0,(lVar11 + lVar18 * 0x14) - lVar13);
  uVar9 = *(ulong *)pauVar2[1];
LAB_01312e33:
  plVar7 = local_60;
  if ((uVar9 & 0x4000000000000000) == 0) {
    *(ulong *)(*pauVar2 + 8) = uVar21 * 0x14 + *(long *)*pauVar2;
  }
  else {
    uVar9 = uVar9 & 0xffffffffffffff00 | uVar21;
    *(ulong *)pauVar2[1] = uVar9;
  }
  pauVar14 = pauVar2;
  if ((uVar9 & 0x4000000000000000) == 0) {
    pauVar14 = *(undefined1 (**) [16])*pauVar2;
  }
  (**(code **)(*plVar8 + 0x38))(plVar8,pauVar14,local_50[0]);
  plVar15 = (long *)(pauVar2[1] + 8);
  sub_166e34(plVar15,local_3c);
  if ((*(ulong *)(pauVar2[2] + 8) & 0x4000000000000000) == 0) {
    plVar15 = (long *)*plVar15;
  }
  (**(code **)(*plVar8 + 0x38))(plVar8,plVar15,local_3c);
  (**(code **)(*plVar8 + 0x20))(plVar8);
  (**(code **)(*plVar7 + 0x20))(plVar7);
  return 1;
}


