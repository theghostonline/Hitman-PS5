// sub_c703ae @ 00d703ae size=1743
// why: svc:MultiplayerService @00d704ca
// why: svc:MultiplayerService @00d704fb
// callers: sub_6138ce@007138ce, sub_a04c22@00b04c22
// callees: sub_15f3cc, sub_143474, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d703bb	@plt
// lit 00d703c2	ree@plt
// lit 00d703da	PATH@
// lit 00d703e8	PATH@
// lit 00d703f3	s.1
// lit 00d70434	matchId
// lit 00d7044a	rodata
// lit 00d7044a	matchId
// lit 00d70453	key
// lit 00d70466	rodata
// lit 00d70466	key
// lit 00d70479	value
// lit 00d7048c	rodata
// lit 00d7048c	value
// lit 00d704a3	{0}/{1}/{2}
// lit 00d704b3	rodata
// lit 00d704b3	{0}/{1}/{2}
// lit 00d704ca	MultiplayerService
// lit 00d704fb	rodata
// lit 00d704fb	MultiplayerService
// lit 00d704ff	SetMatchData
// lit 00d7050c	rodata
// lit 00d70513	rodata
// lit 00d70513	SetMatchData
// lit 00d70527	s.1
// lit 00d70576	ree@plt
// lit 00d7058c	data
// lit 00d70591	free@plt
// lit 00d705a3	s.1
// lit 00d705a8	free@plt
// lit 00d705ba	ree@plt
// lit 00d705bf	free@plt
// lit 00d70655	rodata
// lit 00d706b8	data
// lit 00d706ca	s.1
// lit 00d706e2	s.1
// lit 00d706f6	PATH@
// lit 00d706ff	data
// lit 00d70722	utexattr_setprotocol@plt
// lit 00d70729	PATH@
// lit 00d70731	free@plt
// lit 00d7073b	PATH@
// lit 00d70746	s.1
// lit 00d7076e	free@plt
// lit 00d70793	data
// lit 00d7079e	ee@plt
// lit 00d707be	ee@plt
// lit 00d707c9	PATH@
// lit 00d707d4	s.1
// lit 00d70838	ta.1
// lit 00d7084e	data
// lit 00d70869	s.1
// lit 00d7086c	PATH@
// lit 00d7088f	s.1
// lit 00d70892	PATH@
// lit 00d708b5	s.1
// lit 00d708b8	PATH@
// lit 00d708e7	s.1
// lit 00d708ea	PATH@
// lit 00d70904	ta.1
// lit 00d70911	ee@plt
// lit 00d70911	PATH@
// lit 00d70993	PATH@
// lit 00d7099e	s.1
// lit 00d709d0	PATH@
// lit 00d70a14	data
// lit 00d70a17	data
// lit 00d70a1b	s.1
// lit 00d70a30	s.1
// lit 00d70a33	PATH@
// lit 00d70a56	s.1
// lit 00d70a59	PATH@
// lit 00d70a64	@plt

void sub_c703ae(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 *param_4,
               undefined8 *param_5,undefined8 *param_6)

{
  code *pcVar1;
  undefined8 uVar2;
  long *plVar3;
  long *plVar4;
  long lVar5;
  undefined8 *puVar6;
  undefined8 *puVar7;
  undefined4 *puVar8;
  undefined8 **ppuVar9;
  long *plVar10;
  undefined8 *puVar11;
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
  undefined1 local_88 [16];
  undefined4 local_78;
  long local_70;
  uint local_68 [2];
  long local_60;
  uint local_58 [2];
  long local_50;
  undefined8 *local_48;
  undefined8 *local_40;
  undefined8 *local_38;
  
  if (DAT_03d6b7d8 == 2) {
    if (DAT_03d6cfc0 == (long *)0x0) {
      if ((long *)param_5[4] != (long *)0x0) {
        local_240 = (ulong)local_240._4_4_ << 0x20;
        (**(code **)(*(long *)param_5[4] + 0x10))();
      }
    }
    else {
      local_158 = (long *)0x0;
      puVar11 = (undefined8 *)param_4[4];
      if (puVar11 != (undefined8 *)0x0) {
        if (puVar11 == param_4) {
          puVar7 = (undefined8 *)*param_4;
          plVar3 = local_178;
        }
        else {
          puVar7 = (undefined8 *)*puVar11;
          plVar3 = (long *)0x0;
          param_4 = puVar11;
        }
        local_158 = (long *)(*(code *)*puVar7)(param_4,plVar3);
      }
      local_288 = (long *)0x0;
      puVar11 = (undefined8 *)param_5[4];
      if (puVar11 != (undefined8 *)0x0) {
        if (puVar11 == param_5) {
          puVar7 = (undefined8 *)*param_5;
          plVar3 = local_2a8;
        }
        else {
          puVar7 = (undefined8 *)*puVar11;
          plVar3 = (long *)0x0;
          param_5 = puVar11;
        }
        local_288 = (long *)(*(code *)*puVar7)(param_5,plVar3);
      }
      local_240 = 0;
      if (DAT_03d6cfc0 == (long *)0x0) {
        _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
        pcVar1 = (code *)invalidInstructionException();
        (*pcVar1)();
      }
      (**(code **)(*DAT_03d6cfc0 + 0x10))
                (DAT_03d6cfc0,param_1,param_2,param_3,local_178,local_2a8,&local_240,param_6);
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
    local_48 = param_6;
    local_40 = param_4;
    local_38 = param_5;
    sub_15f3cc(local_88,0);
    local_240._0_4_ = 0x80000007;
    local_238 = "matchId";
    sub_143474(local_88,&local_240,param_1);
    local_240._0_4_ = 0x80000003;
    local_238 = "key";
    sub_143474(local_88,&local_240,param_2);
    local_240._0_4_ = 0x80000005;
    local_238 = "value";
    sub_143474(local_88,&local_240,param_3);
    local_240 = CONCAT44(local_240._4_4_,0x8000000b);
    local_238 = "{0}/{1}/{2}";
    sub_c5aa84(&local_78);
    local_148 = &DAT_0267c968;
    local_140 = &local_78;
    sub_153da8(local_58,&local_240,&local_148);
    if (local_78 < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_70 + -0x10);
    }
    sub_9e3038(&local_240);
    sub_c5aa00(local_68);
    sub_14fd10(local_188,local_58);
    sub_f259c8(&local_148,2,local_68,local_188);
    local_238 = (char *)CONCAT44(local_238._4_4_,local_140._0_4_);
    sub_152a9e(local_230,auStack_138);
    sub_152a9e(local_220,local_128);
    local_210 = local_118;
    sub_152a9e(local_208,auStack_110);
    sub_9e311a(auStack_1f8,auStack_100);
    sub_f24ec6(&local_148);
    if (local_188[0] < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_180 + -0x10);
    }
    puVar11 = local_38;
    if (local_68[0] < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_60 + -0x10);
    }
    sub_137fbb0(&local_148);
    if (local_1e0 < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,(long)local_1d8 + -0x10);
    }
    local_1e0 = (uint)local_148;
    local_1d8 = local_140;
    local_1d0 = 0;
    local_1b0 = *local_48;
    uStack_1a8 = local_48[1];
    local_1a0 = local_48[2];
    local_128[0] = (undefined8 **)0x0;
    puVar7 = (undefined8 *)puVar11[4];
    if (puVar7 != (undefined8 *)0x0) {
      if (puVar7 == puVar11) {
        ppuVar9 = &local_148;
      }
      else {
        ppuVar9 = (undefined8 **)0x0;
      }
      local_128[0] = (undefined8 **)(**(code **)*puVar7)(puVar7,ppuVar9);
    }
    local_78 = (uint)local_78._2_2_ << 0x10;
    plVar3 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,&local_78,0);
    local_c8 = (long *)0x0;
    puVar7 = (undefined8 *)local_40[4];
    if (puVar7 != (undefined8 *)0x0) {
      if (puVar7 == local_40) {
        puVar6 = (undefined8 *)*puVar7;
        plVar4 = local_e8;
      }
      else {
        puVar6 = (undefined8 *)*puVar7;
        plVar4 = (long *)0x0;
      }
      local_c8 = (long *)(*(code *)*puVar6)(puVar7,plVar4);
    }
    local_258 = (long *)0x0;
    plVar4 = (long *)_Znwm_plt(0x40);
    *plVar4 = (long)&DAT_0251ffe0;
    plVar4[6] = 0;
    if (local_128[0] != (undefined8 **)0x0) {
      if (local_128[0] == &local_148) {
        plVar10 = plVar4 + 2;
      }
      else {
        plVar10 = (long *)0x0;
      }
      lVar5 = (*(code *)**local_128[0])(local_128[0],plVar10);
      plVar4[6] = lVar5;
    }
    local_98 = (long *)0x0;
    puVar7 = (undefined8 *)puVar11[4];
    local_258 = plVar4;
    if (puVar7 != (undefined8 *)0x0) {
      if (puVar7 == puVar11) {
        puVar6 = (undefined8 *)*puVar11;
        plVar4 = local_b8;
      }
      else {
        puVar6 = (undefined8 *)*puVar7;
        plVar4 = (long *)0x0;
        puVar11 = puVar7;
      }
      local_98 = (long *)(*(code *)*puVar6)(puVar11,plVar4);
    }
    sub_f245d4(plVar3,local_e8,local_278,local_b8);
    if (local_198 != (long *)0x0) {
      (**(code **)(*local_198 + 0x18))();
    }
    local_198 = plVar3;
    if (plVar3 != (long *)0x0) {
      (**(code **)(*plVar3 + 0x10))(plVar3);
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
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,puVar8);
    }
    sub_f24ec6(&local_240);
    if (local_58[0] < 0x40000000) {
      uVar2 = sub_14e69b();
      sub_14e70a(uVar2,local_50 + -0x10);
    }
    sub_16ee86(local_88);
  }
  return;
}


