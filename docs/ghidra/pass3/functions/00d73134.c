// sub_c73134 @ 00d73134 size=1589
// why: svc:ProfileService @00d731d8
// why: svc:ProfileService @00d73202
// callers: sub_b77244@00c77244, sub_af39b8@00bf39b8
// callees: sub_15f3cc, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d73141	@plt
// lit 00d73148	ree@plt
// lit 00d7315a	PATH@
// lit 00d73168	PATH@
// lit 00d73173	s.1
// lit 00d731b1	{0}/{1}/{2}
// lit 00d731c1	rodata
// lit 00d731c1	{0}/{1}/{2}
// lit 00d731d8	ProfileService
// lit 00d73202	rodata
// lit 00d73202	ProfileService
// lit 00d73206	GetFriendsCount
// lit 00d73213	rodata
// lit 00d7321a	rodata
// lit 00d7321a	GetFriendsCount
// lit 00d7322e	s.1
// lit 00d7327d	ree@plt
// lit 00d73293	data
// lit 00d73298	free@plt
// lit 00d732aa	s.1
// lit 00d732af	free@plt
// lit 00d732c1	ree@plt
// lit 00d732c6	free@plt
// lit 00d7335b	rodata
// lit 00d733bb	data
// lit 00d733cd	s.1
// lit 00d733e5	s.1
// lit 00d733f9	PATH@
// lit 00d73402	data
// lit 00d73425	utexattr_setprotocol@plt
// lit 00d7342c	PATH@
// lit 00d73434	free@plt
// lit 00d7343e	PATH@
// lit 00d73449	s.1
// lit 00d73471	free@plt
// lit 00d73496	data
// lit 00d734a1	ee@plt
// lit 00d734c1	ee@plt
// lit 00d734cc	PATH@
// lit 00d734d7	s.1
// lit 00d7353b	ta.1
// lit 00d73550	data
// lit 00d7356b	s.1
// lit 00d7356e	PATH@
// lit 00d73591	s.1
// lit 00d73594	PATH@
// lit 00d735b7	s.1
// lit 00d735ba	PATH@
// lit 00d735e9	s.1
// lit 00d735ec	PATH@
// lit 00d73606	ta.1
// lit 00d73613	ee@plt
// lit 00d73613	PATH@
// lit 00d73695	PATH@
// lit 00d736a0	s.1
// lit 00d736d3	PATH@
// lit 00d73704	data
// lit 00d73707	s.1
// lit 00d7371c	s.1
// lit 00d7371f	PATH@
// lit 00d73742	s.1
// lit 00d73745	PATH@
// lit 00d73750	@plt

void sub_c73134(undefined8 *param_1,undefined8 *param_2,undefined8 *param_3)

{
  undefined8 *puVar1;
  code *pcVar2;
  undefined8 uVar3;
  long *plVar4;
  long *plVar5;
  long lVar6;
  undefined8 *puVar7;
  undefined4 *puVar8;
  undefined8 **ppuVar9;
  long *plVar10;
  long local_2a8 [4];
  long *local_288;
  long local_278 [4];
  long *local_258;
  undefined8 local_240;
  char *local_238;
  undefined1 local_230 [16];
  undefined1 local_220 [16];
  undefined4 local_210;
  undefined1 local_208 [16];
  undefined1 auStack_1f8 [24];
  uint local_1e0;
  undefined4 *local_1d8;
  undefined8 local_1d0;
  undefined1 local_1c8 [24];
  undefined8 local_1b0;
  undefined8 uStack_1a8;
  undefined8 local_1a0;
  long *local_198;
  uint local_188 [2];
  long local_180;
  long local_178 [4];
  long *local_158;
  undefined8 *local_148;
  undefined4 *local_140;
  undefined1 auStack_138 [16];
  undefined8 **local_128 [2];
  undefined4 local_118;
  undefined1 auStack_110 [16];
  undefined1 auStack_100 [24];
  long local_e8 [4];
  long *local_c8;
  long local_b8 [4];
  long *local_98;
  undefined1 local_80 [16];
  undefined4 local_70;
  long local_68;
  uint local_60 [2];
  long local_58;
  uint local_50 [2];
  long local_48;
  undefined8 *local_40;
  undefined8 *local_38;
  
  if (DAT_03d6b7d8 == 2) {
    if (DAT_03d6d3e0 == (long *)0x0) {
      if ((long *)param_2[4] != (long *)0x0) {
        local_240 = (ulong)local_240._4_4_ << 0x20;
        (**(code **)(*(long *)param_2[4] + 0x10))();
      }
    }
    else {
      local_158 = (long *)0x0;
      puVar1 = (undefined8 *)param_1[4];
      if (puVar1 != (undefined8 *)0x0) {
        if (puVar1 == param_1) {
          puVar7 = (undefined8 *)*param_1;
          plVar4 = local_178;
        }
        else {
          puVar7 = (undefined8 *)*puVar1;
          plVar4 = (long *)0x0;
          param_1 = puVar1;
        }
        local_158 = (long *)(*(code *)*puVar7)(param_1,plVar4);
      }
      local_288 = (long *)0x0;
      puVar1 = (undefined8 *)param_2[4];
      if (puVar1 != (undefined8 *)0x0) {
        if (puVar1 == param_2) {
          puVar7 = (undefined8 *)*param_2;
          plVar4 = local_2a8;
        }
        else {
          puVar7 = (undefined8 *)*puVar1;
          plVar4 = (long *)0x0;
          param_2 = puVar1;
        }
        local_288 = (long *)(*(code *)*puVar7)(param_2,plVar4);
      }
      local_240 = 0;
      if (DAT_03d6d3e0 == (long *)0x0) {
        _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
        pcVar2 = (code *)invalidInstructionException();
        (*pcVar2)();
      }
      (**(code **)(*DAT_03d6d3e0 + 0x10))(DAT_03d6d3e0,local_178,local_2a8,&local_240,param_3);
      if (local_288 != (long *)0x0) {
        (**(code **)(*local_288 + 0x20))(local_288,local_288 != local_2a8);
        local_288 = (long *)0x0;
      }
      if (local_158 != (long *)0x0) {
        (**(code **)(*local_158 + 0x20))(local_158,local_158 != local_178);
      }
    }
  }
  else {
    local_40 = param_3;
    local_38 = param_1;
    sub_15f3cc(local_80,0);
    local_240 = CONCAT44(local_240._4_4_,0x8000000b);
    local_238 = "{0}/{1}/{2}";
    sub_c5aa84(&local_70);
    local_148 = &DAT_0267c968;
    local_140 = &local_70;
    sub_153da8(local_50,&local_240,&local_148);
    if (local_70 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_68 + -0x10);
    }
    sub_9e3038(&local_240);
    sub_c5aa00(local_60);
    sub_14fd10(local_188,local_50);
    sub_f259c8(&local_148,2,local_60,local_188);
    local_238 = (char *)CONCAT44(local_238._4_4_,local_140._0_4_);
    sub_152a9e(local_230,auStack_138);
    sub_152a9e(local_220,local_128);
    local_210 = local_118;
    sub_152a9e(local_208,auStack_110);
    sub_9e311a(auStack_1f8,auStack_100);
    sub_f24ec6(&local_148);
    if (local_188[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_180 + -0x10);
    }
    puVar1 = local_40;
    if (local_60[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_58 + -0x10);
    }
    sub_137fbb0(&local_148);
    if (local_1e0 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,(long)local_1d8 + -0x10);
    }
    local_1e0 = (uint)local_148;
    local_1d8 = local_140;
    local_1d0 = 0;
    local_1b0 = *puVar1;
    uStack_1a8 = puVar1[1];
    local_1a0 = puVar1[2];
    local_128[0] = (undefined8 **)0x0;
    puVar1 = (undefined8 *)param_2[4];
    if (puVar1 != (undefined8 *)0x0) {
      if (puVar1 == param_2) {
        ppuVar9 = &local_148;
      }
      else {
        ppuVar9 = (undefined8 **)0x0;
      }
      local_128[0] = (undefined8 **)(**(code **)*puVar1)(puVar1,ppuVar9);
    }
    local_70 = (uint)local_70._2_2_ << 0x10;
    plVar4 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,&local_70,0);
    local_c8 = (long *)0x0;
    puVar1 = (undefined8 *)local_38[4];
    if (puVar1 != (undefined8 *)0x0) {
      if (puVar1 == local_38) {
        puVar7 = (undefined8 *)*puVar1;
        plVar5 = local_e8;
      }
      else {
        puVar7 = (undefined8 *)*puVar1;
        plVar5 = (long *)0x0;
      }
      local_c8 = (long *)(*(code *)*puVar7)(puVar1,plVar5);
    }
    local_258 = (long *)0x0;
    plVar5 = (long *)_Znwm_plt(0x40);
    *plVar5 = (long)&DAT_02520420;
    plVar5[6] = 0;
    if (local_128[0] != (undefined8 **)0x0) {
      if (local_128[0] == &local_148) {
        plVar10 = plVar5 + 2;
      }
      else {
        plVar10 = (long *)0x0;
      }
      lVar6 = (*(code *)**local_128[0])(local_128[0],plVar10);
      plVar5[6] = lVar6;
    }
    local_98 = (long *)0x0;
    puVar1 = (undefined8 *)param_2[4];
    local_258 = plVar5;
    if (puVar1 != (undefined8 *)0x0) {
      if (puVar1 == param_2) {
        puVar7 = (undefined8 *)*param_2;
        plVar5 = local_b8;
      }
      else {
        puVar7 = (undefined8 *)*puVar1;
        plVar5 = (long *)0x0;
        param_2 = puVar1;
      }
      local_98 = (long *)(*(code *)*puVar7)(param_2,plVar5);
    }
    sub_f245d4(plVar4,local_e8,local_278,local_b8);
    if (local_198 != (long *)0x0) {
      (**(code **)(*local_198 + 0x18))();
    }
    local_198 = plVar4;
    if (plVar4 != (long *)0x0) {
      (**(code **)(*plVar4 + 0x10))(plVar4);
    }
    if (local_98 != (long *)0x0) {
      (**(code **)(*local_98 + 0x20))(local_98,local_98 != local_b8);
      local_98 = (long *)0x0;
    }
    if (local_258 != (long *)0x0) {
      (**(code **)(*local_258 + 0x20))(local_258,local_258 != local_278);
      local_258 = (long *)0x0;
    }
    if (local_c8 != (long *)0x0) {
      (**(code **)(*local_c8 + 0x20))(local_c8,local_c8 != local_e8);
      local_c8 = (long *)0x0;
    }
    sub_c43302(&local_240);
    if (local_128[0] != (undefined8 **)0x0) {
      (*(code *)(*local_128[0])[4])(local_128[0],local_128[0] != &local_148);
      local_128[0] = (undefined8 **)0x0;
    }
    if (local_198 != (long *)0x0) {
      (**(code **)(*local_198 + 0x18))();
    }
    local_198 = (long *)0x0;
    sub_9e33a2(local_1c8);
    if (local_1e0 < 0x40000000) {
      puVar8 = local_1d8 + -4;
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,puVar8);
    }
    sub_f24ec6(&local_240);
    if (local_50[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_48 + -0x10);
    }
    sub_16ee86(local_80);
  }
  return;
}


