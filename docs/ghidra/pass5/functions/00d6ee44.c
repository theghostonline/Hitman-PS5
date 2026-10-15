// sub_c6ee44 @ 00d6ee44 size=1743
// why: svc:GamePersistentDataService @00d6ef60
// why: svc:GamePersistentDataService @00d6ef91
// callers: sub_b020ba@00c020ba, sub_b76fcc@00c76fcc, sub_af3740@00bf3740, sub_b1e288@00c1e288, sub_b9506a@00c9506a, sub_b26656@00c26656, sub_c544a4@00d544a4, sub_b3ea62@00c3ea62, sub_b8cc5c@00c8cc5c, sub_c5767e@00d5767e
// callees: sub_15f3cc, sub_143474, sub_15e7f2, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d6ee51	@plt
// lit 00d6ee58	ree@plt
// lit 00d6ee70	PATH@
// lit 00d6ee7e	PATH@
// lit 00d6ee89	s.1
// lit 00d6eeca	userId
// lit 00d6eee0	rodata
// lit 00d6eee0	userId
// lit 00d6eee9	key
// lit 00d6eefc	rodata
// lit 00d6eefc	key
// lit 00d6ef0f	data
// lit 00d6ef22	rodata
// lit 00d6ef22	data
// lit 00d6ef39	{0}/{1}/{2}
// lit 00d6ef49	rodata
// lit 00d6ef49	{0}/{1}/{2}
// lit 00d6ef60	GamePersistentDataService
// lit 00d6ef91	rodata
// lit 00d6ef91	GamePersistentDataService
// lit 00d6ef95	SaveData
// lit 00d6efa2	rodata
// lit 00d6efa9	rodata
// lit 00d6efa9	SaveData
// lit 00d6efbd	s.1
// lit 00d6f00c	ree@plt
// lit 00d6f022	data
// lit 00d6f027	free@plt
// lit 00d6f039	s.1
// lit 00d6f03e	free@plt
// lit 00d6f050	ree@plt
// lit 00d6f055	free@plt
// lit 00d6f0eb	rodata
// lit 00d6f14e	data
// lit 00d6f160	s.1
// lit 00d6f178	s.1
// lit 00d6f18c	PATH@
// lit 00d6f195	data
// lit 00d6f1b8	utexattr_setprotocol@plt
// lit 00d6f1bf	PATH@
// lit 00d6f1c7	free@plt
// lit 00d6f1d1	PATH@
// lit 00d6f1dc	s.1
// lit 00d6f204	free@plt
// lit 00d6f229	data
// lit 00d6f234	ee@plt
// lit 00d6f254	ee@plt
// lit 00d6f25f	PATH@
// lit 00d6f26a	s.1
// lit 00d6f2ce	ta.1
// lit 00d6f2e4	data
// lit 00d6f2ff	s.1
// lit 00d6f302	PATH@
// lit 00d6f325	s.1
// lit 00d6f328	PATH@
// lit 00d6f34b	s.1
// lit 00d6f34e	PATH@
// lit 00d6f37d	s.1
// lit 00d6f380	PATH@
// lit 00d6f39a	ta.1
// lit 00d6f3a7	ee@plt
// lit 00d6f3a7	PATH@
// lit 00d6f429	PATH@
// lit 00d6f434	s.1
// lit 00d6f466	PATH@
// lit 00d6f4aa	data
// lit 00d6f4ad	data
// lit 00d6f4b1	s.1
// lit 00d6f4c6	s.1
// lit 00d6f4c9	PATH@
// lit 00d6f4ec	s.1
// lit 00d6f4ef	PATH@
// lit 00d6f4fa	@plt

void sub_c6ee44(undefined8 param_1,undefined8 param_2,undefined8 param_3,undefined8 *param_4,
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
    if (DAT_03d6cd50 == (long *)0x0) {
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
      if (DAT_03d6cd50 == (long *)0x0) {
        _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
        pcVar1 = (code *)invalidInstructionException();
        (*pcVar1)();
      }
      (**(code **)(*DAT_03d6cd50 + 0x10))
                (DAT_03d6cd50,param_1,param_2,param_3,local_178,local_2a8,&local_240,param_6);
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
    local_240._0_4_ = 0x80000006;
    local_238 = "userId";
    sub_143474(local_88,&local_240,param_1);
    local_240._0_4_ = 0x80000003;
    local_238 = "key";
    sub_143474(local_88,&local_240,param_2);
    local_240._0_4_ = 0x80000004;
    local_238 = "data";
    sub_15e7f2(local_88,&local_240,param_3);
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
    *plVar4 = (long)&DAT_0251fe20;
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


