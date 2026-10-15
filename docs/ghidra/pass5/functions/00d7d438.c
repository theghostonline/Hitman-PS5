// sub_c7d438 @ 00d7d438 size=1896
// why: svc:AuthenticationService @00d7d599
// why: svc:AuthenticationService @00d7d5ca
// callers: 
// callees: sub_15f3cc, sub_143474, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d7d445	ofday@plt
// lit 00d7d44c	E:/p4/compile_del05/del05/code/_out/ps5/Retail/engine.elf
// lit 00d7d452	rodata
// lit 00d7d452	PATH@
// lit 00d7d460	data
// lit 00d7d48f	rodata
// lit 00d7d496	rodata
// lit 00d7d49d	s.1
// lit 00d7d4a1	s.1
// lit 00d7d4a5	ree@plt
// lit 00d7d4d4	PATH@
// lit 00d7d4dc	e@plt
// lit 00d7d4dc	PATH@
// lit 00d7d4e0	rodata
// lit 00d7d4eb	ree@plt
// lit 00d7d4f4	PATH@
// lit 00d7d509	PATH@
// lit 00d7d548	productVersion
// lit 00d7d562	rodata
// lit 00d7d562	productVersion
// lit 00d7d572	{0}/{1}/{2}
// lit 00d7d582	rodata
// lit 00d7d582	{0}/{1}/{2}
// lit 00d7d599	AuthenticationService
// lit 00d7d5ca	rodata
// lit 00d7d5ca	AuthenticationService
// lit 00d7d5ce	GetBlobOfflineCacheDatabaseDiff
// lit 00d7d5db	rodata
// lit 00d7d5e2	rodata
// lit 00d7d5e2	GetBlobOfflineCacheDatabaseDiff
// lit 00d7d5f6	s.1
// lit 00d7d64c	ree@plt
// lit 00d7d662	data
// lit 00d7d667	free@plt
// lit 00d7d679	s.1
// lit 00d7d67e	free@plt
// lit 00d7d690	ree@plt
// lit 00d7d695	free@plt
// lit 00d7d723	rodata
// lit 00d7d7d1	PATH@
// lit 00d7d7da	data
// lit 00d7d7fd	utexattr_setprotocol@plt
// lit 00d7d804	PATH@
// lit 00d7d80c	free@plt
// lit 00d7d819	PATH@
// lit 00d7d849	free@plt
// lit 00d7d86e	data
// lit 00d7d879	ee@plt
// lit 00d7d899	ee@plt
// lit 00d7d8a8	PATH@
// lit 00d7d910	ta.1
// lit 00d7d926	data
// lit 00d7d941	s.1
// lit 00d7d944	PATH@
// lit 00d7d96e	s.1
// lit 00d7d971	PATH@
// lit 00d7d991	s.1
// lit 00d7d994	PATH@
// lit 00d7d9c4	s.1
// lit 00d7d9c7	PATH@
// lit 00d7d9e1	ta.1
// lit 00d7d9ee	ee@plt
// lit 00d7d9ee	PATH@
// lit 00d7da7b	PATH@
// lit 00d7dab5	PATH@
// lit 00d7daf5	data
// lit 00d7daf8	s.1
// lit 00d7db0d	s.1
// lit 00d7db10	PATH@
// lit 00d7db33	s.1
// lit 00d7db36	PATH@
// lit 00d7db56	s.1
// lit 00d7db59	PATH@
// lit 00d7db79	s.1
// lit 00d7db7c	PATH@
// lit 00d7db87	ofday@plt

void sub_c7d438(long param_1)

{
  code *pcVar1;
  undefined8 uVar2;
  long *plVar3;
  undefined8 **ppuVar4;
  undefined8 *puVar5;
  long *plVar6;
  undefined8 **ppuVar7;
  undefined1 local_2a8 [16];
  uint local_298 [2];
  long local_290;
  undefined8 *local_288;
  char *local_280;
  undefined8 **local_268;
  long local_258;
  char *local_250;
  undefined1 local_248 [16];
  long *local_238 [2];
  undefined4 local_228;
  undefined1 local_220 [16];
  undefined1 auStack_210 [24];
  uint local_1f8;
  long *local_1f0;
  undefined8 local_1e8;
  undefined1 local_1e0 [24];
  undefined8 local_1c8;
  undefined8 uStack_1c0;
  undefined8 local_1b8;
  long *local_1b0;
  undefined8 *local_198;
  char *local_190;
  undefined8 **local_178;
  undefined8 *local_168;
  long local_160;
  undefined8 **local_148;
  undefined8 *local_138;
  long *local_130;
  undefined1 auStack_128 [16];
  undefined8 **local_118 [2];
  undefined4 local_108;
  undefined1 auStack_100 [16];
  undefined1 auStack_f0 [32];
  uint local_d0 [2];
  long local_c8;
  undefined4 local_c0;
  undefined4 uStack_bc;
  undefined4 uStack_b8;
  uint uStack_b4;
  undefined8 local_b0;
  long local_a8;
  long local_a0;
  long *local_88;
  undefined8 *local_78;
  long local_70;
  undefined8 **local_58;
  uint local_48 [2];
  long local_40;
  undefined2 local_38 [4];
  
  *(undefined2 *)(param_1 + 0xc) = 0x100;
  *(undefined4 *)(param_1 + 8) = 0;
  local_168 = &DAT_025212d0;
  local_78 = &DAT_02521310;
  local_160 = param_1;
  local_148 = &local_168;
  local_70 = param_1;
  local_58 = &local_78;
  local_c0 = (**(code **)(DAT_03dcce20 + 0x80))(&DAT_03dcce20);
  uStack_bc = 0;
  uStack_b8 = 0;
  uStack_b4 = uStack_b4 & 0xffffff00;
  local_b0 = 0x100000001;
  (**(code **)(DAT_03dcce20 + 0x90))(&DAT_03dcce20);
  if (DAT_03d6b7d8 == 2) {
    if (DAT_03d6c930 == (long *)0x0) {
      if (local_58 != (undefined8 **)0x0) {
        local_258._0_4_ = 0;
        (*(code *)(*local_58)[2])();
      }
    }
    else {
      local_238[0] = (long *)0x0;
      if (local_148 != (undefined8 **)0x0) {
        if (local_148 == &local_168) {
          ppuVar4 = &local_168;
          plVar3 = &local_258;
          puVar5 = local_168;
        }
        else {
          plVar3 = (long *)0x0;
          puVar5 = *local_148;
          ppuVar4 = local_148;
        }
        local_238[0] = (long *)(*(code *)*puVar5)(ppuVar4,plVar3);
      }
      local_118[0] = (undefined8 **)0x0;
      if (local_58 != (undefined8 **)0x0) {
        if (local_58 == &local_78) {
          ppuVar4 = &local_78;
          ppuVar7 = &local_138;
          puVar5 = local_78;
        }
        else {
          ppuVar7 = (undefined8 **)0x0;
          puVar5 = *local_58;
          ppuVar4 = local_58;
        }
        local_118[0] = (undefined8 **)(*(code *)*puVar5)(ppuVar4,ppuVar7);
      }
      local_a8 = 0;
      if (DAT_03d6c930 == (long *)0x0) {
        _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
        pcVar1 = (code *)invalidInstructionException();
        (*pcVar1)();
      }
      (**(code **)(*DAT_03d6c930 + 0x10))
                (DAT_03d6c930,&DAT_03d6bdc8,&local_258,&local_138,&local_a8,&local_c0);
      if (local_118[0] != (undefined8 **)0x0) {
        (*(code *)(*local_118[0])[4])(local_118[0],local_118[0] != &local_138);
        local_118[0] = (undefined8 **)0x0;
      }
      if (local_238[0] != (long *)0x0) {
        (**(code **)(*local_238[0] + 0x20))(local_238[0],local_238[0] != &local_258);
        local_238[0] = (long *)0x0;
      }
    }
  }
  else {
    sub_15f3cc(local_2a8,0);
    local_258._0_4_ = 0x8000000e;
    local_250 = "productVersion";
    sub_143474(local_2a8,&local_258,&DAT_03d6bdc8);
    local_258._0_4_ = 0x8000000b;
    local_250 = "{0}/{1}/{2}";
    sub_c5aa84(&local_a8);
    local_288 = &DAT_0267ca40;
    local_280 = "AuthenticationService";
    local_198 = &DAT_0267ca40;
    local_138 = &DAT_0267c968;
    local_190 = "GetBlobOfflineCacheDatabaseDiff";
    local_130 = &local_a8;
    sub_153da8(local_48,&local_258,&local_138);
    if ((uint)local_a8 < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_a0 + -0x10);
    }
    sub_9e3038(&local_258);
    sub_c5aa00(local_d0);
    sub_14fd10(local_298,local_48);
    sub_f259c8(&local_138,2,local_d0,local_298);
    local_250 = (char *)CONCAT44(local_250._4_4_,local_130._0_4_);
    sub_152a9e(local_248,auStack_128);
    sub_152a9e(local_238,local_118);
    local_228 = local_108;
    sub_152a9e(local_220,auStack_100);
    sub_9e311a(auStack_210,auStack_f0);
    sub_f24ec6(&local_138);
    if (local_298[0] < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_290 + -0x10);
    }
    if (local_d0[0] < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_c8 + -0x10);
    }
    sub_137fbb0(&local_138);
    if (local_1f8 < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_1f0 + -2);
    }
    local_1c8 = CONCAT44(uStack_bc,local_c0);
    uStack_1c0 = CONCAT44(uStack_b4,uStack_b8);
    local_1f8 = (uint)local_138;
    local_1f0 = local_130;
    local_1e8 = 0;
    local_1b8 = local_b0;
    local_118[0] = (undefined8 **)0x0;
    if (local_58 != (undefined8 **)0x0) {
      if (local_58 == &local_78) {
        ppuVar4 = &local_138;
      }
      else {
        ppuVar4 = (undefined8 **)0x0;
      }
      local_118[0] = (undefined8 **)(*(code *)**local_58)(local_58,ppuVar4);
    }
    local_38[0] = 0;
    plVar3 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,local_38,0);
    local_88 = (long *)0x0;
    if (local_148 != (undefined8 **)0x0) {
      if (local_148 == &local_168) {
        plVar6 = &local_a8;
      }
      else {
        plVar6 = (long *)0x0;
      }
      local_88 = (long *)(*(code *)**local_148)(local_148,plVar6);
    }
    local_268 = (undefined8 **)0x0;
    ppuVar4 = (undefined8 **)_Znwm_plt(0x40);
    *ppuVar4 = (undefined8 *)&DAT_0251fae0;
    ppuVar4[6] = (undefined8 *)0x0;
    if (local_118[0] != (undefined8 **)0x0) {
      if (local_118[0] == &local_138) {
        ppuVar7 = ppuVar4 + 2;
      }
      else {
        ppuVar7 = (undefined8 **)0x0;
      }
      puVar5 = (undefined8 *)(*(code *)**local_118[0])(local_118[0],ppuVar7);
      ppuVar4[6] = puVar5;
    }
    local_178 = (undefined8 **)0x0;
    local_268 = ppuVar4;
    if (local_58 != (undefined8 **)0x0) {
      if (local_58 == &local_78) {
        ppuVar4 = &local_78;
        ppuVar7 = &local_198;
        puVar5 = local_78;
      }
      else {
        ppuVar7 = (undefined8 **)0x0;
        puVar5 = *local_58;
        ppuVar4 = local_58;
      }
      local_178 = (undefined8 **)(*(code *)*puVar5)(ppuVar4,ppuVar7);
    }
    sub_f245d4(plVar3,&local_a8,&local_288,&local_198);
    if (local_1b0 != (long *)0x0) {
      (**(code **)(*local_1b0 + 0x18))();
    }
    local_1b0 = plVar3;
    if (plVar3 != (long *)0x0) {
      (**(code **)(*plVar3 + 0x10))(plVar3);
    }
    if (local_178 != (undefined8 **)0x0) {
      (*(code *)(*local_178)[4])(local_178,local_178 != &local_198);
      local_178 = (undefined8 **)0x0;
    }
    if (local_268 != (undefined8 **)0x0) {
      (*(code *)(*local_268)[4])(local_268,local_268 != &local_288);
      local_268 = (undefined8 **)0x0;
    }
    if (local_88 != (long *)0x0) {
      (**(code **)(*local_88 + 0x20))(local_88,local_88 != &local_a8);
      local_88 = (long *)0x0;
    }
    sub_c43302(&local_258);
    if (local_118[0] != (undefined8 **)0x0) {
      (*(code *)(*local_118[0])[4])(local_118[0],local_118[0] != &local_138);
      local_118[0] = (undefined8 **)0x0;
    }
    if (local_1b0 != (long *)0x0) {
      (**(code **)(*local_1b0 + 0x18))();
    }
    local_1b0 = (long *)0x0;
    sub_9e33a2(local_1e0);
    if (local_1f8 < 0x40000000) {
      plVar3 = local_1f0 + -2;
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,plVar3);
    }
    sub_f24ec6(&local_258);
    if (local_48[0] < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_40 + -0x10);
    }
    sub_16ee86(local_2a8);
  }
  if (local_58 != (undefined8 **)0x0) {
    (*(code *)(*local_58)[4])(local_58,local_58 != &local_78);
    local_58 = (undefined8 **)0x0;
  }
  if (local_148 != (undefined8 **)0x0) {
    (*(code *)(*local_148)[4])(local_148,local_148 != &local_168);
  }
  return;
}


