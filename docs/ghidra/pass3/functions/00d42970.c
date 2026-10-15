// sub_c42970 @ 00d42970 size=1994
// why: svc:ProfileService @00d42b14
// why: svc:ProfileService @00d42b45
// callers: sub_c55486@00d55486, sub_c42162@00d42162
// callees: sub_16f272, sub_15f3cc, sub_15e7f2, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d4297d	d_cond_timedwait@plt
// lit 00d429a5	rodata
// lit 00d429b8	s.1
// lit 00d429e0	PATH@
// lit 00d429e8	s.1
// lit 00d42a1b	ree@plt
// lit 00d42a48	PATH@
// lit 00d42a50	e@plt
// lit 00d42a50	PATH@
// lit 00d42a54	rodata
// lit 00d42a5f	ree@plt
// lit 00d42a68	PATH@
// lit 00d42a7d	PATH@
// lit 00d42ac6	unlockableIds
// lit 00d42add	rodata
// lit 00d42add	unlockableIds
// lit 00d42aed	{0}/{1}/{2}
// lit 00d42afd	rodata
// lit 00d42afd	{0}/{1}/{2}
// lit 00d42b14	ProfileService
// lit 00d42b45	rodata
// lit 00d42b45	ProfileService
// lit 00d42b49	SynchronizeOfflineUnlockables
// lit 00d42b57	rodata
// lit 00d42b5f	rodata
// lit 00d42b5f	SynchronizeOfflineUnlockables
// lit 00d42b73	s.1
// lit 00d42bc9	ree@plt
// lit 00d42bdf	data
// lit 00d42be4	free@plt
// lit 00d42bf6	s.1
// lit 00d42bfb	free@plt
// lit 00d42c0d	ree@plt
// lit 00d42c12	free@plt
// lit 00d42cb0	rodata
// lit 00d42d58	PATH@
// lit 00d42d61	data
// lit 00d42d84	utexattr_setprotocol@plt
// lit 00d42d8b	PATH@
// lit 00d42d93	free@plt
// lit 00d42da0	PATH@
// lit 00d42dd6	free@plt
// lit 00d42dfb	data
// lit 00d42e06	ee@plt
// lit 00d42e26	ee@plt
// lit 00d42e35	PATH@
// lit 00d42ea1	ta.1
// lit 00d42eb7	data
// lit 00d42ed2	s.1
// lit 00d42ed5	PATH@
// lit 00d42ef8	s.1
// lit 00d42efb	PATH@
// lit 00d42f1e	s.1
// lit 00d42f21	PATH@
// lit 00d42f50	s.1
// lit 00d42f53	PATH@
// lit 00d42f6d	ta.1
// lit 00d42f7a	ee@plt
// lit 00d42f7a	PATH@
// lit 00d43007	PATH@
// lit 00d4307e	data
// lit 00d43081	s.1
// lit 00d43096	s.1
// lit 00d43099	PATH@
// lit 00d430bc	s.1
// lit 00d430bf	PATH@
// lit 00d430e3	s.1
// lit 00d430e6	s.1
// lit 00d430e6	PATH@
// lit 00d4310d	s.1
// lit 00d43110	s.1
// lit 00d43110	PATH@
// lit 00d43121	d_cond_timedwait@plt

void sub_c42970(undefined8 param_1,undefined8 *param_2,undefined8 *param_3,long param_4)

{
  code *pcVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  long *plVar4;
  undefined8 **ppuVar5;
  undefined8 *puVar6;
  long *plVar7;
  undefined8 **ppuVar8;
  uint local_2b8 [2];
  long local_2b0;
  undefined8 *local_2a8;
  char *local_2a0;
  undefined8 **local_288;
  long local_278;
  char *local_270;
  undefined1 local_268 [16];
  long *local_258 [2];
  undefined4 local_248;
  undefined1 local_240 [16];
  undefined1 auStack_230 [24];
  uint local_218;
  long *local_210;
  long local_208;
  undefined1 local_200 [24];
  undefined8 local_1e8;
  undefined8 uStack_1e0;
  undefined8 local_1d8;
  long *local_1d0;
  undefined8 *local_1b8;
  char *local_1b0;
  undefined8 **local_198;
  undefined8 *local_188;
  long *local_180;
  undefined1 auStack_178 [16];
  undefined8 **local_168 [2];
  undefined4 local_158;
  undefined1 auStack_150 [16];
  undefined1 auStack_140 [32];
  undefined1 local_120 [16];
  uint local_110 [2];
  long local_108;
  undefined4 local_100;
  undefined4 uStack_fc;
  undefined4 uStack_f8;
  uint uStack_f4;
  undefined8 local_f0;
  long local_e8;
  long local_e0;
  long *local_c8;
  undefined8 *local_b8 [4];
  undefined8 **local_98;
  uint local_88 [2];
  long local_80;
  undefined8 *local_78;
  undefined8 local_70;
  undefined8 *local_68 [4];
  undefined8 **local_48;
  undefined2 local_38 [4];
  
  local_70 = 0;
  local_78 = &DAT_0267c998;
  sub_16f272(&local_78,&DAT_032fa628,param_1);
  local_98 = (undefined8 **)0x0;
  puVar6 = (undefined8 *)param_2[4];
  if (puVar6 != (undefined8 *)0x0) {
    if (puVar6 == param_2) {
      puVar2 = (undefined8 *)*puVar6;
      ppuVar5 = local_b8;
    }
    else {
      puVar2 = (undefined8 *)*puVar6;
      ppuVar5 = (undefined8 **)0x0;
    }
    local_98 = (undefined8 **)(*(code *)*puVar2)(puVar6,ppuVar5);
  }
  local_48 = (undefined8 **)0x0;
  puVar6 = (undefined8 *)param_3[4];
  if (puVar6 != (undefined8 *)0x0) {
    if (puVar6 == param_3) {
      puVar2 = (undefined8 *)*puVar6;
      ppuVar5 = local_68;
    }
    else {
      puVar2 = (undefined8 *)*puVar6;
      ppuVar5 = (undefined8 **)0x0;
    }
    local_48 = (undefined8 **)(*(code *)*puVar2)(puVar6,ppuVar5);
  }
  local_100 = (**(code **)(DAT_03dcce20 + 0x80))(&DAT_03dcce20);
  uStack_fc = 0;
  uStack_f8 = 0;
  uStack_f4 = uStack_f4 & 0xffffff00;
  local_f0 = 0x100000001;
  (**(code **)(DAT_03dcce20 + 0x90))(&DAT_03dcce20);
  if (DAT_03d6b7d8 == 2) {
    if (DAT_03d6d410 == (long *)0x0) {
      if (local_48 != (undefined8 **)0x0) {
        local_278._0_4_ = 0;
        (*(code *)(*local_48)[2])();
      }
    }
    else {
      local_258[0] = (long *)0x0;
      if (local_98 != (undefined8 **)0x0) {
        if (local_98 == local_b8) {
          ppuVar5 = local_b8;
          plVar4 = &local_278;
        }
        else {
          local_b8[0] = *local_98;
          plVar4 = (long *)0x0;
          ppuVar5 = local_98;
        }
        local_258[0] = (long *)(*(code *)*local_b8[0])(ppuVar5,plVar4);
      }
      local_168[0] = (undefined8 **)0x0;
      if (local_48 != (undefined8 **)0x0) {
        if (local_48 == local_68) {
          ppuVar5 = local_68;
          ppuVar8 = &local_188;
        }
        else {
          local_68[0] = *local_48;
          ppuVar8 = (undefined8 **)0x0;
          ppuVar5 = local_48;
        }
        local_168[0] = (undefined8 **)(*(code *)*local_68[0])(ppuVar5,ppuVar8);
      }
      local_e8 = param_4;
      if (DAT_03d6d410 == (long *)0x0) {
        _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
        pcVar1 = (code *)invalidInstructionException();
        (*pcVar1)();
      }
      (**(code **)(*DAT_03d6d410 + 0x10))
                (DAT_03d6d410,&local_78,&local_278,&local_188,&local_e8,&local_100);
      if (local_168[0] != (undefined8 **)0x0) {
        (*(code *)(*local_168[0])[4])(local_168[0],local_168[0] != &local_188);
        local_168[0] = (undefined8 **)0x0;
      }
      if (local_258[0] != (long *)0x0) {
        (**(code **)(*local_258[0] + 0x20))(local_258[0],local_258[0] != &local_278);
        local_258[0] = (long *)0x0;
      }
    }
  }
  else {
    sub_15f3cc(local_120,0);
    local_278._0_4_ = 0x8000000d;
    local_270 = "unlockableIds";
    sub_15e7f2(local_120,&local_278,&local_78);
    local_278._0_4_ = 0x8000000b;
    local_270 = "{0}/{1}/{2}";
    sub_c5aa84(&local_e8);
    local_2a8 = &DAT_0267ca40;
    local_2a0 = "ProfileService";
    local_1b8 = &DAT_0267ca40;
    local_188 = &DAT_0267c968;
    local_1b0 = "SynchronizeOfflineUnlockables";
    local_180 = &local_e8;
    sub_153da8(local_88,&local_278,&local_188);
    if ((uint)local_e8 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_e0 + -0x10);
    }
    sub_9e3038(&local_278);
    sub_c5aa00(local_110);
    sub_14fd10(local_2b8,local_88);
    sub_f259c8(&local_188,2,local_110,local_2b8);
    local_270 = (char *)CONCAT44(local_270._4_4_,local_180._0_4_);
    sub_152a9e(local_268,auStack_178);
    sub_152a9e(local_258,local_168);
    local_248 = local_158;
    sub_152a9e(local_240,auStack_150);
    sub_9e311a(auStack_230,auStack_140);
    sub_f24ec6(&local_188);
    if (local_2b8[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_2b0 + -0x10);
    }
    if (local_110[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_108 + -0x10);
    }
    sub_137fbb0(&local_188);
    if (local_218 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_210 + -2);
    }
    local_1e8 = CONCAT44(uStack_fc,local_100);
    uStack_1e0 = CONCAT44(uStack_f4,uStack_f8);
    local_218 = (uint)local_188;
    local_210 = local_180;
    local_1d8 = local_f0;
    local_168[0] = (undefined8 **)0x0;
    local_208 = param_4;
    if (local_48 != (undefined8 **)0x0) {
      if (local_48 == local_68) {
        ppuVar5 = &local_188;
      }
      else {
        ppuVar5 = (undefined8 **)0x0;
      }
      local_168[0] = (undefined8 **)(*(code *)**local_48)(local_48,ppuVar5);
    }
    local_38[0] = 0;
    plVar4 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,local_38,0);
    local_c8 = (long *)0x0;
    if (local_98 != (undefined8 **)0x0) {
      if (local_98 == local_b8) {
        plVar7 = &local_e8;
      }
      else {
        plVar7 = (long *)0x0;
      }
      local_c8 = (long *)(*(code *)**local_98)(local_98,plVar7);
    }
    local_288 = (undefined8 **)0x0;
    ppuVar5 = (undefined8 **)_Znwm_plt(0x40);
    *ppuVar5 = (undefined8 *)&DAT_02520460;
    ppuVar5[6] = (undefined8 *)0x0;
    if (local_168[0] != (undefined8 **)0x0) {
      if (local_168[0] == &local_188) {
        ppuVar8 = ppuVar5 + 2;
      }
      else {
        ppuVar8 = (undefined8 **)0x0;
      }
      puVar6 = (undefined8 *)(*(code *)**local_168[0])(local_168[0],ppuVar8);
      ppuVar5[6] = puVar6;
    }
    local_198 = (undefined8 **)0x0;
    local_288 = ppuVar5;
    if (local_48 != (undefined8 **)0x0) {
      if (local_48 == local_68) {
        ppuVar5 = local_68;
        ppuVar8 = &local_1b8;
      }
      else {
        local_68[0] = *local_48;
        ppuVar8 = (undefined8 **)0x0;
        ppuVar5 = local_48;
      }
      local_198 = (undefined8 **)(*(code *)*local_68[0])(ppuVar5,ppuVar8);
    }
    sub_f245d4(plVar4,&local_e8,&local_2a8,&local_1b8);
    if (local_1d0 != (long *)0x0) {
      (**(code **)(*local_1d0 + 0x18))();
    }
    local_1d0 = plVar4;
    if (plVar4 != (long *)0x0) {
      (**(code **)(*plVar4 + 0x10))(plVar4);
    }
    if (local_198 != (undefined8 **)0x0) {
      (*(code *)(*local_198)[4])(local_198,local_198 != &local_1b8);
      local_198 = (undefined8 **)0x0;
    }
    if (local_288 != (undefined8 **)0x0) {
      (*(code *)(*local_288)[4])(local_288,local_288 != &local_2a8);
      local_288 = (undefined8 **)0x0;
    }
    if (local_c8 != (long *)0x0) {
      (**(code **)(*local_c8 + 0x20))(local_c8,local_c8 != &local_e8);
      local_c8 = (long *)0x0;
    }
    sub_c43302(&local_278);
    if (local_168[0] != (undefined8 **)0x0) {
      (*(code *)(*local_168[0])[4])(local_168[0],local_168[0] != &local_188);
      local_168[0] = (undefined8 **)0x0;
    }
    if (local_1d0 != (long *)0x0) {
      (**(code **)(*local_1d0 + 0x18))();
    }
    local_1d0 = (long *)0x0;
    sub_9e33a2(local_200);
    if (local_218 < 0x40000000) {
      plVar4 = local_210 + -2;
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,plVar4);
    }
    sub_f24ec6(&local_278);
    if (local_88[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_80 + -0x10);
    }
    sub_16ee86(local_120);
  }
  if (local_48 != (undefined8 **)0x0) {
    (*(code *)(*local_48)[4])(local_48,local_48 != local_68);
    local_48 = (undefined8 **)0x0;
  }
  if (local_98 != (undefined8 **)0x0) {
    (*(code *)(*local_98)[4])(local_98,local_98 != local_b8);
    local_98 = (undefined8 **)0x0;
  }
  sub_16ee86(&local_78);
  return;
}


