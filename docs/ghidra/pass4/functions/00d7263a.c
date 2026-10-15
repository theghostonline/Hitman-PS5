// sub_c7263a @ 00d7263a size=1650
// why: svc:ProfileService @00d7270a
// why: svc:ProfileService @00d7273b
// callers: sub_b33c5e@00c33c5e, sub_a9d0ee@00b9d0ee
// callees: sub_15f3cc, sub_143474, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d72647	@plt
// lit 00d7264e	ree@plt
// lit 00d72666	PATH@
// lit 00d72674	PATH@
// lit 00d7267f	s.1
// lit 00d726c0	email
// lit 00d726d6	rodata
// lit 00d726d6	email
// lit 00d726e3	{0}/{1}/{2}
// lit 00d726f3	rodata
// lit 00d726f3	{0}/{1}/{2}
// lit 00d7270a	ProfileService
// lit 00d7273b	rodata
// lit 00d7273b	ProfileService
// lit 00d7273f	SubmitSemEmail
// lit 00d7274d	rodata
// lit 00d72755	rodata
// lit 00d72755	SubmitSemEmail
// lit 00d72769	s.1
// lit 00d727b8	ree@plt
// lit 00d727ce	data
// lit 00d727d3	free@plt
// lit 00d727e6	s.1
// lit 00d727eb	free@plt
// lit 00d727fe	ree@plt
// lit 00d72803	free@plt
// lit 00d72899	rodata
// lit 00d728fb	data
// lit 00d728ff	PATH@
// lit 00d72911	s.1
// lit 00d72929	s.1
// lit 00d7293d	PATH@
// lit 00d72946	data
// lit 00d72969	utexattr_setprotocol@plt
// lit 00d72970	PATH@
// lit 00d72978	free@plt
// lit 00d72982	PATH@
// lit 00d7298d	s.1
// lit 00d729b5	free@plt
// lit 00d729da	data
// lit 00d729e5	ee@plt
// lit 00d72a05	ee@plt
// lit 00d72a10	PATH@
// lit 00d72a1b	s.1
// lit 00d72a7b	ta.1
// lit 00d72a91	data
// lit 00d72aac	s.1
// lit 00d72aaf	PATH@
// lit 00d72ad2	s.1
// lit 00d72ad5	PATH@
// lit 00d72af8	s.1
// lit 00d72afb	PATH@
// lit 00d72b2a	s.1
// lit 00d72b2d	PATH@
// lit 00d72b47	ta.1
// lit 00d72b54	ee@plt
// lit 00d72b54	PATH@
// lit 00d72bd6	PATH@
// lit 00d72be1	s.1
// lit 00d72c47	data
// lit 00d72c4a	s.1
// lit 00d72c5f	s.1
// lit 00d72c62	PATH@
// lit 00d72c85	s.1
// lit 00d72c88	PATH@
// lit 00d72c93	@plt

void sub_c7263a(undefined8 param_1,undefined8 *param_2,undefined8 *param_3,undefined8 param_4,
               undefined8 *param_5)

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
  undefined1 local_88 [16];
  undefined4 local_78;
  long local_70;
  uint local_68 [2];
  long local_60;
  uint local_58 [2];
  long local_50;
  undefined8 *local_48;
  undefined8 *local_40;
  undefined8 local_38;
  
  if (DAT_03d6b7d8 == 2) {
    if (DAT_03d6d230 == (long *)0x0) {
      if ((long *)param_3[4] != (long *)0x0) {
        local_240 = (ulong)local_240._4_4_ << 0x20;
        (**(code **)(*(long *)param_3[4] + 0x10))();
      }
    }
    else {
      local_158 = (long *)0x0;
      puVar1 = (undefined8 *)param_2[4];
      if (puVar1 != (undefined8 *)0x0) {
        if (puVar1 == param_2) {
          puVar7 = (undefined8 *)*param_2;
          plVar4 = local_178;
        }
        else {
          puVar7 = (undefined8 *)*puVar1;
          plVar4 = (long *)0x0;
          param_2 = puVar1;
        }
        local_158 = (long *)(*(code *)*puVar7)(param_2,plVar4);
      }
      local_288 = (long *)0x0;
      puVar1 = (undefined8 *)param_3[4];
      if (puVar1 != (undefined8 *)0x0) {
        if (puVar1 == param_3) {
          puVar7 = (undefined8 *)*param_3;
          plVar4 = local_2a8;
        }
        else {
          puVar7 = (undefined8 *)*puVar1;
          plVar4 = (long *)0x0;
          param_3 = puVar1;
        }
        local_288 = (long *)(*(code *)*puVar7)(param_3,plVar4);
      }
      local_240 = param_4;
      if (DAT_03d6d230 == (long *)0x0) {
        _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
        pcVar2 = (code *)invalidInstructionException();
        (*pcVar2)();
      }
      (**(code **)(*DAT_03d6d230 + 0x10))
                (DAT_03d6d230,param_1,local_178,local_2a8,&local_240,param_5);
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
    local_48 = param_5;
    local_40 = param_2;
    local_38 = param_4;
    sub_15f3cc(local_88,0);
    local_240._0_4_ = 0x80000005;
    local_238 = "email";
    sub_143474(local_88,&local_240,param_1);
    local_240 = CONCAT44(local_240._4_4_,0x8000000b);
    local_238 = "{0}/{1}/{2}";
    sub_c5aa84(&local_78);
    local_148 = &DAT_0267c968;
    local_140 = &local_78;
    sub_153da8(local_58,&local_240,&local_148);
    if (local_78 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_70 + -0x10);
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
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_180 + -0x10);
    }
    puVar1 = local_48;
    if (local_68[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_60 + -0x10);
    }
    sub_137fbb0(&local_148);
    if (local_1e0 < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,(long)local_1d8 + -0x10);
    }
    local_1e0 = (uint)local_148;
    local_1d8 = local_140;
    local_1d0 = local_38;
    local_1b0 = *puVar1;
    uStack_1a8 = puVar1[1];
    local_1a0 = puVar1[2];
    local_128[0] = (undefined8 **)0x0;
    puVar1 = (undefined8 *)param_3[4];
    if (puVar1 != (undefined8 *)0x0) {
      if (puVar1 == param_3) {
        ppuVar9 = &local_148;
      }
      else {
        ppuVar9 = (undefined8 **)0x0;
      }
      local_128[0] = (undefined8 **)(**(code **)*puVar1)(puVar1,ppuVar9);
    }
    local_78 = (uint)local_78._2_2_ << 0x10;
    plVar4 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,&local_78,0);
    local_c8 = (long *)0x0;
    puVar1 = (undefined8 *)local_40[4];
    if (puVar1 != (undefined8 *)0x0) {
      if (puVar1 == local_40) {
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
    *plVar5 = (long)&DAT_02520320;
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
    puVar1 = (undefined8 *)param_3[4];
    local_258 = plVar5;
    if (puVar1 != (undefined8 *)0x0) {
      if (puVar1 == param_3) {
        puVar7 = (undefined8 *)*param_3;
        plVar5 = local_b8;
      }
      else {
        puVar7 = (undefined8 *)*puVar1;
        plVar5 = (long *)0x0;
        param_3 = puVar1;
      }
      local_98 = (long *)(*(code *)*puVar7)(param_3,plVar5);
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
    if (local_58[0] < 0x40000000) {
      uVar3 = sub_14e69b();
      sub_14e70a(uVar3,local_50 + -0x10);
    }
    sub_16ee86(local_88);
  }
  return;
}


