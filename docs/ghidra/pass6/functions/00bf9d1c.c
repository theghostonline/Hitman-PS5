// sub_af9d1c @ 00bf9d1c size=2073
// why: svc:ContractsService @00bf9ef7
// why: svc:ContractsService @00bf9f28
// callers: sub_afb206@00bfb206, sub_af5bf4@00bf5bf4
// callees: sub_a6c028, _Znwm@plt, sub_15f3cc, sub_15e7f2, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00bf9d29	d_cond_timedwait@plt
// lit 00bf9d3d	e@plt
// lit 00bf9d47	rodata
// lit 00bf9d56	data
// lit 00bf9d59	ta.1
// lit 00bf9d79	rodata
// lit 00bf9d7d	data
// lit 00bf9d8c	data
// lit 00bf9d96	rodata
// lit 00bf9da5	data
// lit 00bf9da8	ta.1
// lit 00bf9dc5	rodata
// lit 00bf9dc9	data
// lit 00bf9dd8	data
// lit 00bf9ded	data
// lit 00bf9dfb	ree@plt
// lit 00bf9e21	PATH@
// lit 00bf9e29	e@plt
// lit 00bf9e29	PATH@
// lit 00bf9e2d	rodata
// lit 00bf9e3e	ree@plt
// lit 00bf9e4b	PATH@
// lit 00bf9e60	PATH@
// lit 00bf9eaa	creationData
// lit 00bf9ec0	rodata
// lit 00bf9ec0	creationData
// lit 00bf9ed0	{0}/{1}/{2}
// lit 00bf9ee0	rodata
// lit 00bf9ee0	{0}/{1}/{2}
// lit 00bf9ef7	ContractsService
// lit 00bf9f28	rodata
// lit 00bf9f28	ContractsService
// lit 00bf9f2c	CreateFromParams
// lit 00bf9f3a	rodata
// lit 00bf9f41	rodata
// lit 00bf9f41	CreateFromParams
// lit 00bf9f55	s.1
// lit 00bf9faa	ree@plt
// lit 00bf9fc0	data
// lit 00bf9fc5	free@plt
// lit 00bf9fd7	s.1
// lit 00bf9fdc	free@plt
// lit 00bf9fee	ree@plt
// lit 00bf9ff3	free@plt
// lit 00bfa08d	rodata
// lit 00bfa135	PATH@
// lit 00bfa13e	data
// lit 00bfa161	utexattr_setprotocol@plt
// lit 00bfa168	PATH@
// lit 00bfa170	free@plt
// lit 00bfa17d	PATH@
// lit 00bfa1b3	free@plt
// lit 00bfa1d1	data
// lit 00bfa1e3	ee@plt
// lit 00bfa207	ee@plt
// lit 00bfa216	PATH@
// lit 00bfa282	ta.1
// lit 00bfa298	data
// lit 00bfa2b3	s.1
// lit 00bfa2b6	PATH@
// lit 00bfa2d9	s.1
// lit 00bfa2dc	PATH@
// lit 00bfa2ff	s.1
// lit 00bfa302	PATH@
// lit 00bfa338	s.1
// lit 00bfa33b	PATH@
// lit 00bfa355	ta.1
// lit 00bfa362	ee@plt
// lit 00bfa362	PATH@
// lit 00bfa3ef	PATH@
// lit 00bfa465	data
// lit 00bfa468	s.1
// lit 00bfa47d	s.1
// lit 00bfa480	PATH@
// lit 00bfa4a3	s.1
// lit 00bfa4a6	PATH@
// lit 00bfa4ca	s.1
// lit 00bfa4cd	s.1
// lit 00bfa4cd	PATH@
// lit 00bfa4e1	ta.1
// lit 00bfa503	s.1
// lit 00bfa506	s.1
// lit 00bfa506	PATH@
// lit 00bfa519	ta.1
// lit 00bfa51c	d_cond_timedwait@plt

void sub_af9d1c(long *param_1)

{
  code *pcVar1;
  undefined8 **ppuVar2;
  undefined8 uVar3;
  long *plVar4;
  undefined8 *puVar5;
  undefined4 *puVar6;
  undefined8 **ppuVar7;
  long *plVar8;
  uint local_2b8 [2];
  long local_2b0;
  undefined8 *local_2a8;
  char *local_2a0;
  undefined8 **local_288;
  undefined4 local_278 [2];
  char *local_270;
  undefined1 local_268 [16];
  long *local_258 [2];
  undefined4 local_248;
  undefined1 local_240 [16];
  undefined1 auStack_230 [24];
  uint local_218;
  undefined8 **local_210;
  undefined8 *local_208;
  undefined1 local_200 [24];
  undefined8 local_1e8;
  undefined8 uStack_1e0;
  undefined8 local_1d8;
  long *local_1d0;
  undefined8 *local_1b8;
  char *local_1b0;
  undefined8 **local_198;
  undefined1 local_188 [16];
  uint local_178 [2];
  long local_170;
  undefined8 *local_168;
  undefined8 **local_160;
  undefined1 auStack_158 [16];
  undefined8 **local_148 [2];
  undefined4 local_138;
  undefined1 auStack_130 [16];
  undefined1 auStack_120 [24];
  undefined8 *local_108 [4];
  undefined8 **local_e8;
  undefined4 local_d0;
  undefined4 uStack_cc;
  undefined4 uStack_c8;
  uint uStack_c4;
  undefined8 local_c0;
  undefined8 *local_b8;
  long local_b0;
  undefined8 **local_98;
  uint local_88 [2];
  long local_80;
  long *local_78;
  long *local_70;
  undefined8 *local_68 [4];
  undefined8 **local_48;
  undefined2 local_38 [4];
  
  puVar5 = (undefined8 *)*param_1;
  sub_a6c028(&DAT_03d451c0,4);
  plVar8 = (long *)param_1[1];
  if (plVar8 != (long *)0x0) {
    (**(code **)(*plVar8 + 0x10))(plVar8);
  }
  local_e8 = (undefined8 **)0x0;
  ppuVar2 = (undefined8 **)_Znwm_plt(0x18);
  *ppuVar2 = &DAT_024eb6f8;
  ppuVar2[1] = puVar5;
  ppuVar2[2] = plVar8;
  if (plVar8 != (long *)0x0) {
    (**(code **)(*plVar8 + 0x10))(plVar8);
  }
  plVar4 = (long *)param_1[1];
  local_e8 = ppuVar2;
  if (plVar4 != (long *)0x0) {
    (**(code **)(*plVar4 + 0x10))(plVar4);
  }
  local_48 = (undefined8 **)0x0;
  ppuVar2 = (undefined8 **)_Znwm_plt(0x18);
  *ppuVar2 = &DAT_024eb738;
  ppuVar2[1] = puVar5;
  ppuVar2[2] = plVar4;
  if (plVar4 != (long *)0x0) {
    (**(code **)(*plVar4 + 0x10))(plVar4);
  }
  puVar5 = puVar5 + 0x35;
  local_48 = ppuVar2;
  local_d0 = (**(code **)(DAT_03dcce20 + 0x80))(&DAT_03dcce20);
  uStack_cc = 0;
  uStack_c8 = 0;
  uStack_c4 = uStack_c4 & 0xffffff00;
  local_c0 = 0x100000001;
  (**(code **)(DAT_03dcce20 + 0x90))(&DAT_03dcce20);
  local_78 = plVar4;
  if (DAT_03d6b7d8 == 2) {
    if (DAT_03d6cba0 == (long *)0x0) {
      if (local_48 != (undefined8 **)0x0) {
        local_278[0] = 0;
        (*(code *)(*local_48)[2])();
      }
    }
    else {
      local_258[0] = (long *)0x0;
      if (local_e8 != (undefined8 **)0x0) {
        if (local_e8 == local_108) {
          ppuVar2 = local_108;
          puVar6 = local_278;
        }
        else {
          local_108[0] = *local_e8;
          puVar6 = (undefined4 *)0x0;
          ppuVar2 = local_e8;
        }
        local_258[0] = (long *)(*(code *)*local_108[0])(ppuVar2,puVar6);
      }
      local_148[0] = (undefined8 **)0x0;
      if (local_48 != (undefined8 **)0x0) {
        if (local_48 == local_68) {
          ppuVar2 = local_68;
          ppuVar7 = &local_168;
        }
        else {
          local_68[0] = *local_48;
          ppuVar7 = (undefined8 **)0x0;
          ppuVar2 = local_48;
        }
        local_148[0] = (undefined8 **)(*(code *)*local_68[0])(ppuVar2,ppuVar7);
      }
      local_b8 = puVar5;
      if (DAT_03d6cba0 == (long *)0x0) {
        _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
        pcVar1 = (code *)invalidInstructionException();
        (*pcVar1)();
      }
      (**(code **)(*DAT_03d6cba0 + 0x10))
                (DAT_03d6cba0,param_1 + 2,local_278,&local_168,&local_b8,&local_d0);
      if (local_148[0] != (undefined8 **)0x0) {
        (*(code *)(*local_148[0])[4])(local_148[0],local_148[0] != &local_168);
        local_148[0] = (undefined8 **)0x0;
      }
      if (local_258[0] != (long *)0x0) {
        (**(code **)(*local_258[0] + 0x20))(local_258[0],local_258[0] != (long *)local_278);
        local_258[0] = (long *)0x0;
      }
    }
  }
  else {
    local_70 = plVar8;
    sub_15f3cc(local_188,0);
    local_278[0] = 0x8000000c;
    local_270 = "creationData";
    sub_15e7f2(local_188,local_278,param_1 + 2);
    local_278[0] = 0x8000000b;
    local_270 = "{0}/{1}/{2}";
    sub_c5aa84(&local_b8);
    local_2a8 = &DAT_0267ca40;
    local_2a0 = "ContractsService";
    local_1b8 = &DAT_0267ca40;
    local_168 = &DAT_0267c968;
    local_1b0 = "CreateFromParams";
    local_160 = &local_b8;
    sub_153da8(local_88,local_278,&local_168);
    if ((uint)local_b8 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_b0 + -0x10);
    }
    sub_9e3038(local_278);
    sub_c5aa00(local_178);
    sub_14fd10(local_2b8,local_88);
    sub_f259c8(&local_168,2,local_178,local_2b8);
    local_270 = (char *)CONCAT44(local_270._4_4_,local_160._0_4_);
    sub_152a9e(local_268,auStack_158);
    sub_152a9e(local_258,local_148);
    local_248 = local_138;
    sub_152a9e(local_240,auStack_130);
    sub_9e311a(auStack_230,auStack_120);
    sub_f24ec6(&local_168);
    if (local_2b8[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_2b0 + -0x10);
    }
    if (local_178[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_170 + -0x10);
    }
    sub_137fbb0(&local_168);
    if (local_218 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_210 + -2);
    }
    local_1e8 = CONCAT44(uStack_cc,local_d0);
    uStack_1e0 = CONCAT44(uStack_c4,uStack_c8);
    local_218 = (uint)local_168;
    local_210 = local_160;
    local_1d8 = local_c0;
    local_148[0] = (undefined8 **)0x0;
    local_208 = puVar5;
    if (local_48 != (undefined8 **)0x0) {
      if (local_48 == local_68) {
        ppuVar2 = &local_168;
      }
      else {
        ppuVar2 = (undefined8 **)0x0;
      }
      local_148[0] = (undefined8 **)(*(code *)**local_48)(local_48,ppuVar2);
    }
    local_38[0] = 0;
    plVar4 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,local_38,0);
    local_98 = (undefined8 **)0x0;
    if (local_e8 != (undefined8 **)0x0) {
      if (local_e8 == local_108) {
        ppuVar2 = &local_b8;
      }
      else {
        ppuVar2 = (undefined8 **)0x0;
      }
      local_98 = (undefined8 **)(*(code *)**local_e8)(local_e8,ppuVar2);
    }
    local_288 = (undefined8 **)0x0;
    ppuVar2 = (undefined8 **)_Znwm_plt(0x40);
    plVar8 = local_70;
    *ppuVar2 = (undefined8 *)&DAT_0251fce0;
    ppuVar2[6] = (undefined8 *)0x0;
    if (local_148[0] != (undefined8 **)0x0) {
      if (local_148[0] == &local_168) {
        ppuVar7 = ppuVar2 + 2;
      }
      else {
        ppuVar7 = (undefined8 **)0x0;
      }
      puVar5 = (undefined8 *)(*(code *)**local_148[0])(local_148[0],ppuVar7);
      ppuVar2[6] = puVar5;
    }
    local_198 = (undefined8 **)0x0;
    local_288 = ppuVar2;
    if (local_48 != (undefined8 **)0x0) {
      if (local_48 == local_68) {
        ppuVar2 = local_68;
        ppuVar7 = &local_1b8;
      }
      else {
        local_68[0] = *local_48;
        ppuVar7 = (undefined8 **)0x0;
        ppuVar2 = local_48;
      }
      local_198 = (undefined8 **)(*(code *)*local_68[0])(ppuVar2,ppuVar7);
    }
    sub_f245d4(plVar4,&local_b8,&local_2a8,&local_1b8);
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
    if (local_98 != (undefined8 **)0x0) {
      (*(code *)(*local_98)[4])(local_98,local_98 != &local_b8);
      local_98 = (undefined8 **)0x0;
    }
    sub_c43302(local_278);
    if (local_148[0] != (undefined8 **)0x0) {
      (*(code *)(*local_148[0])[4])(local_148[0],local_148[0] != &local_168);
      local_148[0] = (undefined8 **)0x0;
    }
    if (local_1d0 != (long *)0x0) {
      (**(code **)(*local_1d0 + 0x18))();
    }
    local_1d0 = (long *)0x0;
    sub_9e33a2(local_200);
    if (local_218 < 0x40000000) {
      ppuVar2 = local_210 + -2;
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,ppuVar2);
    }
    sub_f24ec6(local_278);
    if (local_88[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_80 + -0x10);
    }
    sub_16ee86(local_188);
  }
  if (local_48 != (undefined8 **)0x0) {
    (*(code *)(*local_48)[4])(local_48,local_48 != local_68);
    local_48 = (undefined8 **)0x0;
  }
  if (local_78 != (long *)0x0) {
    (**(code **)(*local_78 + 0x18))();
  }
  if (local_e8 != (undefined8 **)0x0) {
    (*(code *)(*local_e8)[4])(local_e8,local_e8 != local_108);
    local_e8 = (undefined8 **)0x0;
  }
  if (plVar8 != (long *)0x0) {
    (**(code **)(*plVar8 + 0x18))(plVar8);
  }
  return;
}


