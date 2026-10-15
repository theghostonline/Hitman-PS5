// sub_c770ce @ 00d770ce size=2124
// why: svc:ProfileService @00d77255
// why: svc:ProfileService @00d77286
// callers: 
// callees: sub_15f3cc, sub_143474, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d770db	ofday@plt
// lit 00d770f5	data
// lit 00d7711f	ta.1
// lit 00d7712e	rodata
// lit 00d77135	rodata
// lit 00d7713c	s.1
// lit 00d77140	s.1
// lit 00d77144	ree@plt
// lit 00d77171	PATH@
// lit 00d77179	e@plt
// lit 00d77179	PATH@
// lit 00d7717d	rodata
// lit 00d77188	ree@plt
// lit 00d77191	PATH@
// lit 00d771a6	PATH@
// lit 00d771cd	PATH@
// lit 00d771dd	.shstrtab
// lit 00d77208	email
// lit 00d7721e	rodata
// lit 00d7721e	email
// lit 00d7722e	{0}/{1}/{2}
// lit 00d7723e	rodata
// lit 00d7723e	{0}/{1}/{2}
// lit 00d77255	ProfileService
// lit 00d77286	rodata
// lit 00d77286	ProfileService
// lit 00d7728a	UnconfirmedSignupIoIAccount
// lit 00d77298	rodata
// lit 00d772a0	rodata
// lit 00d772a0	UnconfirmedSignupIoIAccount
// lit 00d772b4	s.1
// lit 00d7730a	ree@plt
// lit 00d77320	data
// lit 00d77325	free@plt
// lit 00d77337	s.1
// lit 00d7733c	free@plt
// lit 00d7734e	ree@plt
// lit 00d77353	free@plt
// lit 00d773ee	rodata
// lit 00d7749b	@plt
// lit 00d774d1	@plt
// lit 00d774d5	free@plt
// lit 00d774d5	ta.1
// lit 00d774df	free@plt
// lit 00d774df	rodata
// lit 00d774e4	free@plt
// lit 00d774e8	free@plt
// lit 00d774ef	free@plt
// lit 00d774f8	free@plt
// lit 00d774f8	data
// lit 00d77529	63580
// lit 00d7754b	PATH@
// lit 00d77554	data
// lit 00d77577	utexattr_setprotocol@plt
// lit 00d7757e	PATH@
// lit 00d77586	free@plt
// lit 00d77593	PATH@
// lit 00d775c3	free@plt
// lit 00d775e8	data
// lit 00d775f3	ee@plt
// lit 00d77613	ee@plt
// lit 00d77622	PATH@
// lit 00d7768e	ta.1
// lit 00d776a4	data
// lit 00d776bf	s.1
// lit 00d776c2	PATH@
// lit 00d776ec	s.1
// lit 00d776ef	PATH@
// lit 00d7770f	s.1
// lit 00d77712	PATH@
// lit 00d7773e	s.1
// lit 00d77741	PATH@
// lit 00d7775b	ta.1
// lit 00d77768	ee@plt
// lit 00d77768	PATH@
// lit 00d777f5	PATH@
// lit 00d7786b	data
// lit 00d7786e	s.1
// lit 00d77883	s.1
// lit 00d77886	PATH@
// lit 00d778a9	s.1
// lit 00d778ac	PATH@
// lit 00d778d0	s.1
// lit 00d778d3	PATH@
// lit 00d778f3	s.1
// lit 00d778f6	PATH@
// lit 00d77901	ofday@plt

void sub_c770ce(long param_1,uint *param_2)

{
  short sVar1;
  undefined8 uVar2;
  code *pcVar3;
  long *plVar4;
  undefined8 **ppuVar5;
  undefined8 *puVar6;
  ushort uVar7;
  int iVar8;
  long *plVar9;
  undefined8 **ppuVar10;
  long lVar11;
  uint local_2a8 [2];
  long local_2a0;
  undefined8 *local_298;
  char *local_290;
  undefined8 **local_278;
  long local_268;
  char *local_260;
  undefined1 local_258 [16];
  long *local_248 [2];
  undefined4 local_238;
  undefined1 local_230 [16];
  undefined1 auStack_220 [24];
  uint local_208;
  long *local_200;
  long local_1f8;
  undefined1 local_1f0 [24];
  undefined8 local_1d8;
  undefined8 uStack_1d0;
  undefined8 local_1c8;
  long *local_1c0;
  undefined8 *local_1a8;
  char *local_1a0;
  undefined8 **local_188;
  undefined8 *local_178;
  long local_170;
  undefined8 **local_158;
  undefined8 *local_148;
  long *local_140;
  undefined1 auStack_138 [16];
  undefined8 **local_128 [2];
  undefined4 local_118;
  undefined1 auStack_110 [16];
  undefined1 auStack_100 [32];
  undefined1 local_e0 [16];
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
  uint local_78 [2];
  long local_70;
  undefined8 *local_68;
  long local_60;
  undefined8 **local_48;
  undefined2 local_38 [4];
  
  if ((*param_2 & 0x3fffffff) == 0) {
    if ((DAT_03d6b9a8 >> 0x3e & 1) == 0) {
      uVar7 = (ushort)((ulong)(DAT_03d6b9a0 - (long)DAT_03d6b998) >> 5);
    }
    else {
      uVar7 = (ushort)DAT_03d6b9a8 & 0xff;
    }
    DAT_03d6b9b0 = (uint)uVar7 << 0x10;
    if (0 < (short)uVar7) {
      iVar8 = 0;
      do {
        lVar11 = (long)(short)iVar8;
        puVar6 = &DAT_03d6b998;
        if ((DAT_03d6b9a8 & 0x4000000000000000) == 0) {
          puVar6 = DAT_03d6b998;
        }
        if (puVar6[lVar11 * 4 + 3] == 0) {
          (*(code *)puVar6[lVar11 * 4 + 2])();
        }
        else {
          pcVar3 = (code *)puVar6[lVar11 * 4];
          if (((ulong)pcVar3 & 1) != 0) {
            pcVar3 = *(code **)(pcVar3 + *(long *)(puVar6[lVar11 * 4 + 3] + puVar6[lVar11 * 4 + 1])
                                         + -1);
          }
          (*pcVar3)();
        }
        iVar8 = DAT_03d6b9b0 + 1;
        sVar1 = DAT_03d6b9b0._2_2_ + -1;
        DAT_03d6b9b0 = CONCAT22(sVar1,(short)iVar8);
      } while (0 < sVar1);
    }
    DAT_03d6b9b0 = CONCAT22(DAT_03d6b9b0._2_2_,0xfffe);
  }
  else {
    local_48 = &local_68;
    lVar11 = param_1 + 0x18;
    local_178 = &DAT_025208e0;
    local_68 = &DAT_02520920;
    local_170 = param_1;
    local_158 = &local_178;
    local_60 = param_1;
    local_c0 = (**(code **)(DAT_03dcce20 + 0x80))(&DAT_03dcce20);
    uStack_bc = 0;
    uStack_b8 = 0;
    uStack_b4 = uStack_b4 & 0xffffff00;
    local_b0 = 0x100000001;
    (**(code **)(DAT_03dcce20 + 0x90))(&DAT_03dcce20);
    if (DAT_03d6b7d8 == 2) {
      if (DAT_03d6d290 == (long *)0x0) {
        if (local_48 != (undefined8 **)0x0) {
          local_268._0_4_ = 0;
          (*(code *)(*local_48)[2])();
        }
      }
      else {
        local_248[0] = (long *)0x0;
        if (local_158 != (undefined8 **)0x0) {
          if (local_158 == &local_178) {
            ppuVar5 = &local_178;
            plVar4 = &local_268;
            puVar6 = local_178;
          }
          else {
            plVar4 = (long *)0x0;
            puVar6 = *local_158;
            ppuVar5 = local_158;
          }
          local_248[0] = (long *)(*(code *)*puVar6)(ppuVar5,plVar4);
        }
        local_128[0] = (undefined8 **)0x0;
        if (local_48 != (undefined8 **)0x0) {
          if (local_48 == &local_68) {
            ppuVar5 = &local_68;
            ppuVar10 = &local_148;
            puVar6 = local_68;
          }
          else {
            ppuVar10 = (undefined8 **)0x0;
            puVar6 = *local_48;
            ppuVar5 = local_48;
          }
          local_128[0] = (undefined8 **)(*(code *)*puVar6)(ppuVar5,ppuVar10);
        }
        local_a8 = lVar11;
        if (DAT_03d6d290 == (long *)0x0) {
          _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
          pcVar3 = (code *)invalidInstructionException();
          (*pcVar3)();
        }
        (**(code **)(*DAT_03d6d290 + 0x10))
                  (DAT_03d6d290,param_2,&local_268,&local_148,&local_a8,&local_c0);
        if (local_128[0] != (undefined8 **)0x0) {
          (*(code *)(*local_128[0])[4])(local_128[0],local_128[0] != &local_148);
          local_128[0] = (undefined8 **)0x0;
        }
        if (local_248[0] != (long *)0x0) {
          (**(code **)(*local_248[0] + 0x20))(local_248[0],local_248[0] != &local_268);
          local_248[0] = (long *)0x0;
        }
      }
    }
    else {
      sub_15f3cc(local_e0,0);
      local_268._0_4_ = 0x80000005;
      local_260 = "email";
      sub_143474(local_e0,&local_268,param_2);
      local_268._0_4_ = 0x8000000b;
      local_260 = "{0}/{1}/{2}";
      sub_c5aa84(&local_a8);
      local_298 = &DAT_0267ca40;
      local_290 = "ProfileService";
      local_1a8 = &DAT_0267ca40;
      local_148 = &DAT_0267c968;
      local_1a0 = "UnconfirmedSignupIoIAccount";
      local_140 = &local_a8;
      sub_153da8(local_78,&local_268,&local_148);
      if ((uint)local_a8 < 0x40000000) {
        uVar2 = sub_14e69b();
        sub_14e70a(uVar2,local_a0 + -0x10);
      }
      sub_9e3038(&local_268);
      sub_c5aa00(local_d0);
      sub_14fd10(local_2a8,local_78);
      sub_f259c8(&local_148,2,local_d0,local_2a8);
      local_260 = (char *)CONCAT44(local_260._4_4_,local_140._0_4_);
      sub_152a9e(local_258,auStack_138);
      sub_152a9e(local_248,local_128);
      local_238 = local_118;
      sub_152a9e(local_230,auStack_110);
      sub_9e311a(auStack_220,auStack_100);
      sub_f24ec6(&local_148);
      if (local_2a8[0] < 0x40000000) {
        uVar2 = sub_14e69b();
        sub_14e70a(uVar2,local_2a0 + -0x10);
      }
      if (local_d0[0] < 0x40000000) {
        uVar2 = sub_14e69b();
        sub_14e70a(uVar2,local_c8 + -0x10);
      }
      sub_137fbb0(&local_148);
      if (local_208 < 0x40000000) {
        uVar2 = sub_14e69b();
        sub_14e70a(uVar2,local_200 + -2);
      }
      local_1d8 = CONCAT44(uStack_bc,local_c0);
      uStack_1d0 = CONCAT44(uStack_b4,uStack_b8);
      local_208 = (uint)local_148;
      local_200 = local_140;
      local_1c8 = local_b0;
      local_128[0] = (undefined8 **)0x0;
      local_1f8 = lVar11;
      if (local_48 != (undefined8 **)0x0) {
        if (local_48 == &local_68) {
          ppuVar5 = &local_148;
        }
        else {
          ppuVar5 = (undefined8 **)0x0;
        }
        local_128[0] = (undefined8 **)(*(code *)**local_48)(local_48,ppuVar5);
      }
      local_38[0] = 0;
      plVar4 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,local_38,0);
      local_88 = (long *)0x0;
      if (local_158 != (undefined8 **)0x0) {
        if (local_158 == &local_178) {
          plVar9 = &local_a8;
        }
        else {
          plVar9 = (long *)0x0;
        }
        local_88 = (long *)(*(code *)**local_158)(local_158,plVar9);
      }
      local_278 = (undefined8 **)0x0;
      ppuVar5 = (undefined8 **)_Znwm_plt(0x40);
      *ppuVar5 = (undefined8 *)&DAT_02520360;
      ppuVar5[6] = (undefined8 *)0x0;
      if (local_128[0] != (undefined8 **)0x0) {
        if (local_128[0] == &local_148) {
          ppuVar10 = ppuVar5 + 2;
        }
        else {
          ppuVar10 = (undefined8 **)0x0;
        }
        puVar6 = (undefined8 *)(*(code *)**local_128[0])(local_128[0],ppuVar10);
        ppuVar5[6] = puVar6;
      }
      local_188 = (undefined8 **)0x0;
      local_278 = ppuVar5;
      if (local_48 != (undefined8 **)0x0) {
        if (local_48 == &local_68) {
          ppuVar5 = &local_68;
          ppuVar10 = &local_1a8;
          puVar6 = local_68;
        }
        else {
          ppuVar10 = (undefined8 **)0x0;
          puVar6 = *local_48;
          ppuVar5 = local_48;
        }
        local_188 = (undefined8 **)(*(code *)*puVar6)(ppuVar5,ppuVar10);
      }
      sub_f245d4(plVar4,&local_a8,&local_298,&local_1a8);
      if (local_1c0 != (long *)0x0) {
        (**(code **)(*local_1c0 + 0x18))();
      }
      local_1c0 = plVar4;
      if (plVar4 != (long *)0x0) {
        (**(code **)(*plVar4 + 0x10))(plVar4);
      }
      if (local_188 != (undefined8 **)0x0) {
        (*(code *)(*local_188)[4])(local_188,local_188 != &local_1a8);
        local_188 = (undefined8 **)0x0;
      }
      if (local_278 != (undefined8 **)0x0) {
        (*(code *)(*local_278)[4])(local_278,local_278 != &local_298);
        local_278 = (undefined8 **)0x0;
      }
      if (local_88 != (long *)0x0) {
        (**(code **)(*local_88 + 0x20))(local_88,local_88 != &local_a8);
        local_88 = (long *)0x0;
      }
      sub_c43302(&local_268);
      if (local_128[0] != (undefined8 **)0x0) {
        (*(code *)(*local_128[0])[4])(local_128[0],local_128[0] != &local_148);
        local_128[0] = (undefined8 **)0x0;
      }
      if (local_1c0 != (long *)0x0) {
        (**(code **)(*local_1c0 + 0x18))();
      }
      local_1c0 = (long *)0x0;
      sub_9e33a2(local_1f0);
      if (local_208 < 0x40000000) {
        plVar4 = local_200 + -2;
        uVar2 = sub_14e69b();
        sub_14e70a(uVar2,plVar4);
      }
      sub_f24ec6(&local_268);
      if (local_78[0] < 0x40000000) {
        uVar2 = sub_14e69b();
        sub_14e70a(uVar2,local_70 + -0x10);
      }
      sub_16ee86(local_e0);
    }
    if (local_48 != (undefined8 **)0x0) {
      (*(code *)(*local_48)[4])(local_48,local_48 != &local_68);
      local_48 = (undefined8 **)0x0;
    }
    if (local_158 != (undefined8 **)0x0) {
      (*(code *)(*local_158)[4])(local_158,local_158 != &local_178);
    }
  }
  return;
}


