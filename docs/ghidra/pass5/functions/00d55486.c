// sub_c55486 @ 00d55486 size=2021
// why: svc:HubPagesService @00d55b36
// why: svc:HubPagesService @00d55b60
// callers: 
// callees: sub_15eb9a, sub_15ecca, sub_196712, sub_c35568, sub_1c159a, sub_15f4f0, sub_14fd10, sub_c4e160, sub_14e69b, sub_14e70a, sub_15eb42, strcmp@plt, sub_162ad4, sub_15f45e, sub_16ee86, sub_15dea0, sub_c34b16, sub_c352f8, sub_c42970, sub_143364, sub_15e7f2, sub_146460
// lit 00d55493	t@plt
// lit 00d554ab	Inventory
// lit 00d554c5	rodata
// lit 00d554c5	Inventory
// lit 00d554f8	rodata
// lit 00d55506	data
// lit 00d5550f	rodata
// lit 00d5551d	e@plt
// lit 00d55534	Unlockable.Id
// lit 00d5555e	Unlockable.Id
// lit 00d555a1	Unlockable
// lit 00d555b3	Unlockable
// lit 00d555bc	Properties
// lit 00d555d4	Properties
// lit 00d555e0	RepositoryId
// lit 00d555fb	RepositoryId
// lit 00d55616	Unlockable
// lit 00d55637	Unlockable
// lit 00d55643	Type
// lit 00d5565e	Type
// lit 00d556ef	vailability_msg
// lit 00d55705	Unlockable.Id
// lit 00d5574f	 modules first.
// lit 00d55757	data
// lit 00d55794	rodata
// lit 00d55798	data
// lit 00d557a1	rodata
// lit 00d557ae	e@plt
// lit 00d557c5	UnlockableId
// lit 00d557e1	UnlockableId
// lit 00d55833	ree@plt
// lit 00d55837	e@plt
// lit 00d5583e	free@plt
// lit 00d55859	ee@plt
// lit 00d55862	rodata
// lit 00d55879	Unlockable.Id
// lit 00d558fe	vailability_msg
// lit 00d55906	data
// lit 00d55967	PATH@
// lit 00d559a3	rodata
// lit 00d559a3	PATH@
// lit 00d559cb	rodata
// lit 00d559d9	data
// lit 00d559e2	rodata
// lit 00d559ef	e@plt
// lit 00d55a0a	data
// lit 00d55a31	PATH@
// lit 00d55a39	free@plt
// lit 00d55a45	s.1
// lit 00d55a5f	PATH@
// lit 00d55a6a	e@plt
// lit 00d55a96	ree@plt
// lit 00d55aba	s.1
// lit 00d55acf	s.1
// lit 00d55ad2	PATH@
// lit 00d55af2	s.1
// lit 00d55af5	PATH@
// lit 00d55b01	OnlineDisableAutoReconnect
// lit 00d55b10	rodata
// lit 00d55b10	OnlineDisableAutoReconnect
// lit 00d55b36	HubPagesService
// lit 00d55b60	HubPagesService
// lit 00d55b7c	Stats
// lit 00d55b89	rodata
// lit 00d55b89	Stats
// lit 00d55bb8	rodata
// lit 00d55bc6	data
// lit 00d55bd4	rodata
// lit 00d55bdb	@plt
// lit 00d55bdf	rodata
// lit 00d55bed	data
// lit 00d55bfb	rodata
// lit 00d55c02	@plt
// lit 00d55c1b	rodata
// lit 00d55c1b	Stats
// lit 00d55c46	free@plt
// lit 00d55c59	t@plt

void sub_c55486(long param_1,undefined8 param_2)

{
  uint *puVar1;
  long *plVar2;
  ulong uVar3;
  char cVar4;
  char cVar5;
  int iVar6;
  uint uVar7;
  undefined8 *puVar8;
  long lVar9;
  undefined8 uVar10;
  undefined8 **ppuVar11;
  uint uVar12;
  uint *puVar13;
  undefined1 *puVar14;
  uint *puVar15;
  uint *puVar16;
  char *pcVar17;
  undefined8 *puVar18;
  undefined8 *puVar19;
  bool bVar20;
  undefined1 auVar21 [16];
  undefined4 local_140 [2];
  char *local_138;
  undefined4 local_130 [2];
  char *local_128;
  uint local_120 [2];
  long local_118;
  undefined4 local_110 [2];
  char *local_108;
  undefined4 local_100 [2];
  undefined *local_f8;
  long local_f0;
  undefined1 local_e8 [16];
  ulong local_d8;
  long *local_c8;
  undefined8 *local_b8;
  uint *local_b0;
  undefined8 *local_a8;
  undefined8 local_a0;
  undefined1 local_98 [16];
  undefined8 local_88;
  undefined8 *local_78;
  char *pcStack_70;
  undefined1 local_68 [16];
  undefined8 **local_58;
  long local_50;
  uint *local_48;
  uint local_40 [2];
  char *local_38;
  
  local_f0 = param_1;
  local_a0 = param_2;
  puVar8 = (undefined8 *)sub_15eb9a(param_2);
  auVar21._8_8_ = local_68._8_8_;
  auVar21._0_8_ = local_68._0_8_;
  local_e8._0_12_ = ZEXT812(0);
  local_e8._12_4_ = 0;
  local_d8 = 0x4000000000000000;
  if ((undefined *)*puVar8 == &DAT_032f7c00) {
    puVar15 = (uint *)puVar8[1];
    if ((*(ulong *)(puVar15 + 4) & 0x4000000000000000) == 0) {
      puVar13 = *(uint **)(puVar15 + 2);
      puVar15 = *(uint **)puVar15;
    }
    else {
      puVar13 = puVar15 + (*(ulong *)(puVar15 + 4) & 0xff) * 4;
    }
    puVar16 = puVar13;
    local_68 = auVar21;
    if (puVar15 != puVar13) {
      do {
        local_48 = puVar16;
        local_98._0_4_ = 0x80000000;
        local_78 = (undefined8 *)CONCAT44(local_78._4_4_,0x8000000d);
        local_98._8_8_ = &DAT_01d8bb7b;
        pcStack_70 = "Unlockable.Id";
        local_68 = auVar21;
        lVar9 = sub_15ecca(puVar15,&local_78);
        if (lVar9 != 0) {
          cVar4 = sub_196712(lVar9,local_98);
          if (cVar4 != '\0') {
            cVar4 = sub_c35568(local_98);
            if (cVar4 == '\0') {
              local_40[0] = 0x8000000a;
              local_38 = "Unlockable";
              uVar10 = sub_15eb9a(puVar15,local_40);
              local_140[0] = 0x8000000a;
              local_138 = "Properties";
              uVar10 = sub_15eb9a(uVar10,local_140);
              local_130[0] = 0x8000000c;
              local_128 = "RepositoryId";
              uVar10 = sub_15eb9a(uVar10,local_130);
              auVar21 = sub_1c159a(uVar10,&DAT_051fb770);
              local_110[0] = 0x8000000a;
              local_108 = "Unlockable";
              uVar10 = sub_15eb9a(puVar15,local_110);
              local_100[0] = 0x80000004;
              local_f8 = &DAT_01d42a5d;
              uVar10 = sub_15eb9a(uVar10,local_100);
              sub_15f4f0(local_120,uVar10,&DAT_01d8bb7b);
              sub_14fd10(&local_78,local_98);
              local_68 = auVar21;
              sub_14fd10(&local_58,local_120);
              sub_c4e160(local_e8,&local_78);
              if ((uint)local_58 < 0x40000000) {
                lVar9 = local_50 + -0x10;
                uVar10 = sub_14e69b();
                sub_14e70a(uVar10,lVar9);
              }
              if ((uint)local_78 < 0x40000000) {
                pcVar17 = pcStack_70 + -0x10;
                uVar10 = sub_14e69b();
                sub_14e70a(uVar10,pcVar17);
              }
              puVar13 = local_48;
              if (local_120[0] < 0x40000000) {
                lVar9 = local_118 + -0x10;
                uVar10 = sub_14e69b();
                sub_14e70a(uVar10,lVar9);
              }
            }
          }
        }
        if ((uint)local_98._0_4_ < 0x40000000) {
          lVar9 = local_98._8_8_ + -0x10;
          uVar10 = sub_14e69b();
          sub_14e70a(uVar10,lVar9);
        }
        puVar15 = puVar15 + 4;
        puVar16 = local_48;
        auVar21 = local_68;
      } while (puVar13 != puVar15);
    }
  }
  local_98._0_12_ = ZEXT812(0);
  local_98._12_4_ = 0;
  local_88 = 0x4000000000000000;
  lVar9 = sub_15eb42(&DAT_03e2e488,&DAT_03e2e4c0);
  puVar8 = *(undefined8 **)(lVar9 + 8);
  if ((puVar8[2] & 0x4000000000000000) == 0) {
    puVar18 = (undefined8 *)puVar8[1];
    puVar8 = (undefined8 *)*puVar8;
  }
  else {
    puVar18 = puVar8 + (puVar8[2] & 0xff) * 2;
  }
  if (puVar8 != puVar18) {
    do {
      local_a8 = puVar18;
      local_40[0] = 0x8000000c;
      local_38 = "UnlockableId";
      local_b8 = puVar8;
      uVar10 = sub_15eb9a(puVar8,local_40);
      sub_15f4f0(&local_78,uVar10,&DAT_01d8bb7b);
      pcVar17 = pcStack_70;
      uVar3 = local_d8;
      uVar12 = (uint)local_78;
      local_b0 = (uint *)local_e8._8_8_;
      bVar20 = (local_d8 & 0x4000000000000000) != 0;
      puVar15 = (uint *)(local_e8._0_8_ + -0x30);
      if (bVar20) {
        puVar15 = (uint *)&local_118;
      }
      local_48 = (uint *)(local_e8 + (local_d8 & 0xff) * 0x30);
      puVar13 = (uint *)(local_e8 + (local_d8 & 0xff) * 0x30);
      if (!bVar20) {
        puVar13 = (uint *)local_e8._8_8_;
      }
      do {
        do {
          puVar16 = puVar15 + 0xc;
          if (puVar13 == puVar16) {
            puVar16 = local_b0;
            if ((uVar3 & 0x4000000000000000) == 0) goto LAB_00d558c8;
            goto LAB_00d558d9;
          }
          puVar1 = puVar15 + 0xe;
          if (*(char **)puVar1 == pcVar17) goto LAB_00d55885;
          puVar15 = puVar16;
        } while (((*puVar16 ^ uVar12) & 0x3fffffff) != 0);
        iVar6 = strcmp_plt(*(char **)puVar1,pcVar17);
      } while (iVar6 != 0);
LAB_00d55885:
      if ((uVar3 & 0x4000000000000000) == 0) {
LAB_00d558c8:
        local_48 = local_b0;
      }
      puVar8 = local_b8;
      puVar19 = local_a8;
      if (puVar16 == local_48) {
LAB_00d558d9:
        puVar19 = local_a8;
        puVar8 = local_b8;
        sub_162ad4(local_98,&local_78);
      }
      if (uVar12 < 0x40000000) {
        uVar10 = sub_14e69b();
        sub_14e70a(uVar10,pcVar17 + -0x10);
      }
      puVar8 = puVar8 + 2;
      puVar18 = local_a8;
    } while (puVar8 != puVar19);
  }
  sub_15f45e(&local_78,0);
  DAT_03e2e4a8 = DAT_03e2e4a9;
  ppuVar11 = (undefined8 **)sub_15eb42(&DAT_03e2e488,&DAT_03e2e4d0);
  if (ppuVar11 != &local_78) {
    sub_16ee86(ppuVar11);
    *ppuVar11 = &DAT_0267c998;
    *ppuVar11 = local_78;
    ppuVar11[1] = (undefined8 *)pcStack_70;
    local_78 = &DAT_0267c998;
    pcStack_70 = (char *)0x0;
  }
  sub_16ee86(&local_78);
  sub_15f45e(&local_78,0);
  sub_16ee86(&DAT_03e2e498);
  DAT_03e2e4a0 = pcStack_70;
  DAT_03e2e498 = local_78;
  local_78 = &DAT_0267c998;
  pcStack_70 = (char *)0x0;
  sub_16ee86(&local_78);
  lVar9 = sub_15eb42(&DAT_03e2e488,&DAT_03e2e4c0);
  puVar8 = *(undefined8 **)(lVar9 + 8);
  if ((puVar8[2] & 0x4000000000000000) == 0) {
    puVar18 = (undefined8 *)puVar8[1];
    puVar8 = (undefined8 *)*puVar8;
  }
  else {
    puVar18 = puVar8 + (puVar8[2] & 0xff) * 2;
  }
  for (; puVar8 != puVar18; puVar8 = puVar8 + 2) {
    sub_15dea0(DAT_03e2e4a0,puVar8);
  }
  sub_c34b16(local_e8);
  sub_c352f8(local_e8);
  lVar9 = local_f0;
  local_58 = (undefined8 **)0x0;
  puVar8 = *(undefined8 **)(local_f0 + 0x40);
  if (puVar8 != (undefined8 *)0x0) {
    if (puVar8 == (undefined8 *)(local_f0 + 0x20)) {
      ppuVar11 = &local_78;
    }
    else {
      ppuVar11 = (undefined8 **)0x0;
    }
    local_58 = (undefined8 **)(**(code **)*puVar8)(puVar8,ppuVar11);
  }
  local_c8 = (long *)0x0;
  puVar8 = *(undefined8 **)(lVar9 + 0x70);
  if (puVar8 != (undefined8 *)0x0) {
    if (puVar8 == (undefined8 *)(lVar9 + 0x50)) {
      puVar14 = local_e8;
    }
    else {
      puVar14 = (undefined1 *)0x0;
    }
    local_c8 = (long *)(**(code **)*puVar8)(puVar8,puVar14);
  }
  sub_c42970(local_98,&local_78,local_e8,*(undefined8 *)(lVar9 + 0x80));
  if (local_c8 != (long *)0x0) {
    (**(code **)(*local_c8 + 0x20))(local_c8,local_c8 != (long *)local_e8);
    local_c8 = (long *)0x0;
  }
  if (local_58 != (undefined8 **)0x0) {
    (*(code *)(*local_58)[4])(local_58,local_58 != &local_78);
    local_58 = (undefined8 **)0x0;
  }
  local_40[0] = 0x8000001a;
  local_38 = "OnlineDisableAutoReconnect";
  cVar5 = sub_143364(local_40,0);
  cVar4 = DAT_03e2e4a9;
  if (cVar5 == '\0') {
    if (local_40[0] < 0x40000000) {
      pcVar17 = local_38 + -0x10;
      uVar10 = sub_14e69b();
      sub_14e70a(uVar10,pcVar17);
    }
    if (cVar4 == '\0') goto LAB_00d55c4d;
  }
  else if (local_40[0] < 0x40000000) {
    pcVar17 = local_38 + -0x10;
    uVar10 = sub_14e69b();
    sub_14e70a(uVar10,pcVar17);
  }
  local_40[0] = 0x80000005;
  local_38 = "Stats";
  puVar8 = (undefined8 *)sub_15eb9a(local_a0);
  lVar9 = sub_15eb42(&DAT_03e2e488,&DAT_03e2e4e0);
  if ((undefined *)*puVar8 == &DAT_032f7c18) {
    plVar2 = (long *)puVar8[1];
    if ((plVar2[2] & 0x4000000000000000U) == 0) {
      uVar12 = (uint)((ulong)(plVar2[1] - *plVar2) >> 5);
    }
    else {
      uVar12 = (uint)plVar2[2] & 0xff;
    }
    plVar2 = *(long **)(lVar9 + 8);
    if ((plVar2[2] & 0x4000000000000000U) == 0) {
      uVar7 = (uint)((ulong)(plVar2[1] - *plVar2) >> 5);
    }
    else {
      uVar7 = (uint)plVar2[2] & 0xff;
    }
    if (uVar7 <= uVar12) {
      local_40[0] = 0x80000005;
      local_38 = "Stats";
      uVar10 = sub_15eb9a(local_a0);
      sub_15e7f2(&DAT_03e2e488,&DAT_03e2e4e0,uVar10);
      DAT_03e2e4a8 = DAT_03e2e4a9;
      DAT_03e2e4aa = 1;
    }
  }
LAB_00d55c4d:
  sub_146460(local_98);
  return;
}


