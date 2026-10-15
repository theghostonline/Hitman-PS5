// sub_c75778 @ 00d75778 size=1788
// why: svc:ProfileService @00d75966
// why: svc:ProfileService @00d75997
// callers: 
// callees: sub_15f3cc, sub_15e7f2, sub_143474, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86
// lit 00d75785	@plt
// lit 00d757aa	friends
// lit 00d757bd	rodata
// lit 00d757bd	friends
// lit 00d757e8	ree@plt
// lit 00d75814	e@plt
// lit 00d75814	PATH@
// lit 00d75818	rodata
// lit 00d75823	ree@plt
// lit 00d75857	s.1
// lit 00d7585b	s.1
// lit 00d75865	rodata
// lit 00d7587e	data
// lit 00d75881	data
// lit 00d75885	s.1
// lit 00d7589a	s.1
// lit 00d7589d	PATH@
// lit 00d758c4	s.1
// lit 00d758c7	PATH@
// lit 00d758ef	id
// lit 00d75909	rodata
// lit 00d75909	id
// lit 00d75912	extensionsData
// lit 00d75925	rodata
// lit 00d75925	extensionsData
// lit 00d7593f	{0}/{1}/{2}
// lit 00d7594f	rodata
// lit 00d7594f	{0}/{1}/{2}
// lit 00d75966	ProfileService
// lit 00d75997	rodata
// lit 00d75997	ProfileService
// lit 00d7599b	UpdateExtensions
// lit 00d759a8	rodata
// lit 00d759af	rodata
// lit 00d759af	UpdateExtensions
// lit 00d759c3	s.1
// lit 00d75a19	ree@plt
// lit 00d75a2f	data
// lit 00d75a34	free@plt
// lit 00d75a46	s.1
// lit 00d75a4b	free@plt
// lit 00d75a5d	ree@plt
// lit 00d75a62	free@plt
// lit 00d75aec	rodata
// lit 00d75ba5	utexattr_setprotocol@plt
// lit 00d75bac	PATH@
// lit 00d75bb4	free@plt
// lit 00d75bc1	PATH@
// lit 00d75bf1	free@plt
// lit 00d75c16	data
// lit 00d75c21	ee@plt
// lit 00d75c41	ee@plt
// lit 00d75c53	PATH@
// lit 00d75cc5	ta.1
// lit 00d75cdb	data
// lit 00d75cf6	s.1
// lit 00d75cf9	PATH@
// lit 00d75d1c	s.1
// lit 00d75d1f	PATH@
// lit 00d75d3f	s.1
// lit 00d75d42	PATH@
// lit 00d75d6e	s.1
// lit 00d75d71	PATH@
// lit 00d75d8b	ta.1
// lit 00d75d98	ee@plt
// lit 00d75d98	PATH@
// lit 00d75e21	s.1
// lit 00d75e24	s.1
// lit 00d75e24	PATH@
// lit 00d75e4b	s.1
// lit 00d75e4e	s.1
// lit 00d75e4e	PATH@
// lit 00d75e62	@plt

void sub_c75778(undefined8 param_1,undefined8 param_2)

{
  undefined8 uVar1;
  long *plVar2;
  undefined8 **ppuVar3;
  undefined8 *puVar4;
  long *plVar5;
  undefined8 **ppuVar6;
  undefined4 local_2c8 [2];
  char *local_2c0;
  long local_2b8 [4];
  long *local_298;
  long local_288;
  char *local_280;
  undefined1 local_278 [16];
  long *local_268 [2];
  undefined4 local_258;
  undefined1 local_250 [16];
  undefined1 auStack_240 [24];
  uint local_228;
  long *local_220;
  undefined8 local_218;
  undefined1 local_210 [24];
  undefined8 local_1f8;
  undefined8 uStack_1f0;
  undefined8 local_1e8;
  long *local_1e0;
  undefined1 local_1c8 [16];
  uint local_1b8 [2];
  long local_1b0;
  undefined1 local_1a8 [16];
  undefined8 *local_198;
  char *local_190;
  undefined8 **local_178;
  undefined8 *local_168;
  long *local_160;
  undefined1 auStack_158 [16];
  undefined8 **local_148 [2];
  undefined4 local_138;
  undefined1 auStack_130 [16];
  undefined1 auStack_120 [24];
  undefined8 *local_108;
  char *local_100;
  undefined8 **local_e8;
  undefined8 *local_d8 [4];
  undefined8 **local_b8;
  uint local_a8 [2];
  long local_a0;
  long local_98;
  long local_90;
  long *local_78;
  undefined4 local_60;
  undefined4 uStack_5c;
  undefined4 uStack_58;
  uint uStack_54;
  undefined8 local_50;
  uint local_48 [2];
  long local_40;
  undefined2 local_38 [4];
  
  sub_15f3cc(local_1a8,0);
  local_2c8[0] = 0x80000007;
  local_2c0 = "friends";
  uVar1 = sub_15e7f2(local_1a8,local_2c8,param_2);
  local_298 = (long *)0x0;
  local_b8 = (undefined8 **)0x0;
  local_60 = (**(code **)(DAT_03dcce20 + 0x80))(&DAT_03dcce20);
  uStack_5c = 0;
  uStack_58 = 0;
  uStack_54 = uStack_54 & 0xffffff00;
  local_50 = 0x100000001;
  (**(code **)(DAT_03dcce20 + 0x90))(&DAT_03dcce20);
  if (DAT_03d6b7d8 == 2) {
    if (DAT_03d6d380 != (long *)0x0) {
      local_268[0] = (long *)0x0;
      local_148[0] = (undefined8 **)0x0;
      local_98 = 0;
      (**(code **)(*DAT_03d6d380 + 0x10))
                (DAT_03d6d380,&DAT_03d6bc58,uVar1,&local_288,&local_168,&local_98,&local_60);
      if (local_148[0] != (undefined8 **)0x0) {
        (*(code *)(*local_148[0])[4])(local_148[0],local_148[0] != &local_168);
        local_148[0] = (undefined8 **)0x0;
      }
      if (local_268[0] != (long *)0x0) {
        (**(code **)(*local_268[0] + 0x20))(local_268[0],local_268[0] != &local_288);
        local_268[0] = (long *)0x0;
      }
    }
  }
  else {
    sub_15f3cc(local_1c8,0);
    local_288._0_4_ = 0x80000002;
    local_280 = "id";
    sub_143474(local_1c8,&local_288,&DAT_03d6bc58);
    local_288._0_4_ = 0x8000000e;
    local_280 = "extensionsData";
    sub_15e7f2(local_1c8,&local_288,uVar1);
    local_288._0_4_ = 0x8000000b;
    local_280 = "{0}/{1}/{2}";
    sub_c5aa84(&local_98);
    local_198 = &DAT_0267ca40;
    local_190 = "ProfileService";
    local_108 = &DAT_0267ca40;
    local_168 = &DAT_0267c968;
    local_100 = "UpdateExtensions";
    local_160 = &local_98;
    sub_153da8(local_48,&local_288,&local_168);
    if ((uint)local_98 < 0x40000000) {
      uVar1 = sub_14e69b();
      sub_14e70a(uVar1,local_90 + -0x10);
    }
    sub_9e3038(&local_288);
    sub_c5aa00(local_a8);
    sub_14fd10(local_1b8,local_48);
    sub_f259c8(&local_168,2,local_a8,local_1b8);
    local_280 = (char *)CONCAT44(local_280._4_4_,local_160._0_4_);
    sub_152a9e(local_278,auStack_158);
    sub_152a9e(local_268,local_148);
    local_258 = local_138;
    sub_152a9e(local_250,auStack_130);
    sub_9e311a(auStack_240,auStack_120);
    sub_f24ec6(&local_168);
    if (local_1b8[0] < 0x40000000) {
      uVar1 = sub_14e69b();
      sub_14e70a(uVar1,local_1b0 + -0x10);
    }
    if (local_a8[0] < 0x40000000) {
      uVar1 = sub_14e69b();
      sub_14e70a(uVar1,local_a0 + -0x10);
    }
    sub_137fbb0(&local_168);
    if (local_228 < 0x40000000) {
      uVar1 = sub_14e69b();
      sub_14e70a(uVar1,local_220 + -2);
    }
    local_1f8 = CONCAT44(uStack_5c,local_60);
    uStack_1f0 = CONCAT44(uStack_54,uStack_58);
    local_228 = (uint)local_168;
    local_220 = local_160;
    local_218 = 0;
    local_1e8 = local_50;
    local_148[0] = (undefined8 **)0x0;
    if (local_b8 != (undefined8 **)0x0) {
      if (local_b8 == local_d8) {
        ppuVar3 = &local_168;
      }
      else {
        ppuVar3 = (undefined8 **)0x0;
      }
      local_148[0] = (undefined8 **)(*(code *)**local_b8)(local_b8,ppuVar3);
    }
    local_38[0] = 0;
    plVar2 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,local_38,0);
    local_78 = (long *)0x0;
    if (local_298 != (long *)0x0) {
      if (local_298 == local_2b8) {
        plVar5 = &local_98;
      }
      else {
        plVar5 = (long *)0x0;
      }
      local_78 = (long *)(**(code **)*local_298)(local_298,plVar5);
    }
    local_178 = (undefined8 **)0x0;
    ppuVar3 = (undefined8 **)_Znwm_plt(0x40);
    *ppuVar3 = (undefined8 *)&DAT_025203e0;
    ppuVar3[6] = (undefined8 *)0x0;
    if (local_148[0] != (undefined8 **)0x0) {
      if (local_148[0] == &local_168) {
        ppuVar6 = ppuVar3 + 2;
      }
      else {
        ppuVar6 = (undefined8 **)0x0;
      }
      puVar4 = (undefined8 *)(*(code *)**local_148[0])(local_148[0],ppuVar6);
      ppuVar3[6] = puVar4;
    }
    local_e8 = (undefined8 **)0x0;
    local_178 = ppuVar3;
    if (local_b8 != (undefined8 **)0x0) {
      if (local_b8 == local_d8) {
        ppuVar3 = local_d8;
        ppuVar6 = &local_108;
      }
      else {
        local_d8[0] = *local_b8;
        ppuVar6 = (undefined8 **)0x0;
        ppuVar3 = local_b8;
      }
      local_e8 = (undefined8 **)(*(code *)*local_d8[0])(ppuVar3,ppuVar6);
    }
    sub_f245d4(plVar2,&local_98,&local_198,&local_108);
    if (local_1e0 != (long *)0x0) {
      (**(code **)(*local_1e0 + 0x18))();
    }
    local_1e0 = plVar2;
    if (plVar2 != (long *)0x0) {
      (**(code **)(*plVar2 + 0x10))(plVar2);
    }
    if (local_e8 != (undefined8 **)0x0) {
      (*(code *)(*local_e8)[4])(local_e8,local_e8 != &local_108);
      local_e8 = (undefined8 **)0x0;
    }
    if (local_178 != (undefined8 **)0x0) {
      (*(code *)(*local_178)[4])(local_178,local_178 != &local_198);
      local_178 = (undefined8 **)0x0;
    }
    if (local_78 != (long *)0x0) {
      (**(code **)(*local_78 + 0x20))(local_78,local_78 != &local_98);
      local_78 = (long *)0x0;
    }
    sub_c43302(&local_288);
    if (local_148[0] != (undefined8 **)0x0) {
      (*(code *)(*local_148[0])[4])(local_148[0],local_148[0] != &local_168);
      local_148[0] = (undefined8 **)0x0;
    }
    if (local_1e0 != (long *)0x0) {
      (**(code **)(*local_1e0 + 0x18))();
    }
    local_1e0 = (long *)0x0;
    sub_9e33a2(local_210);
    if (local_228 < 0x40000000) {
      plVar2 = local_220 + -2;
      uVar1 = sub_14e69b();
      sub_14e70a(uVar1,plVar2);
    }
    sub_f24ec6(&local_288);
    if (local_48[0] < 0x40000000) {
      uVar1 = sub_14e69b();
      sub_14e70a(uVar1,local_40 + -0x10);
    }
    sub_16ee86(local_1c8);
  }
  if (local_b8 != (undefined8 **)0x0) {
    (*(code *)(*local_b8)[4])(local_b8,local_b8 != local_d8);
    local_b8 = (undefined8 **)0x0;
  }
  if (local_298 != (long *)0x0) {
    (**(code **)(*local_298 + 0x20))(local_298,local_298 != local_2b8);
    local_298 = (long *)0x0;
  }
  sub_16ee86(local_1a8);
  return;
}


