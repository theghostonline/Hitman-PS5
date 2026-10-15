// sub_9b0e92 @ 00ab0e92 size=2089
// why: svc:ChallengesService @00ab107f
// why: svc:ChallengesService @00ab10b0
// callers: 
// callees: sub_15f45e, sub_9b16ca, sub_16f272, sub_16ee86, sub_15f3cc, sub_143474, sub_15e7f2, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, _ZSt19_Xbad_function_callv@plt
// lit 00ab0e9f	d_cond_timedwait@plt
// lit 00ab0ea6	@plt
// lit 00ab0eb1	PATH@
// lit 00ab0ec4	ree@plt
// lit 00ab0ed0	@plt
// lit 00ab0ed9	@plt
// lit 00ab0ef1	free@plt
// lit 00ab0f04	ta.1
// lit 00ab0f13	rodata
// lit 00ab0f25	rodata
// lit 00ab0f56	rodata
// lit 00ab0f61	s.1
// lit 00ab0f68	ree@plt
// lit 00ab0f9b	e@plt
// lit 00ab0f9b	PATH@
// lit 00ab0f9f	rodata
// lit 00ab0faa	ree@plt
// lit 00ab0fb3	PATH@
// lit 00ab0fc8	PATH@
// lit 00ab1007	profileid
// lit 00ab1021	rodata
// lit 00ab1021	profileid
// lit 00ab102a	challengeids
// lit 00ab103e	rodata
// lit 00ab103e	challengeids
// lit 00ab1058	{0}/{1}/{2}
// lit 00ab1068	rodata
// lit 00ab1068	{0}/{1}/{2}
// lit 00ab107f	ChallengesService
// lit 00ab10b0	rodata
// lit 00ab10b0	ChallengesService
// lit 00ab10b4	GetProgression
// lit 00ab10c1	rodata
// lit 00ab10c8	rodata
// lit 00ab10c8	GetProgression
// lit 00ab10dc	s.1
// lit 00ab1132	ree@plt
// lit 00ab1148	data
// lit 00ab114d	free@plt
// lit 00ab115f	s.1
// lit 00ab1164	free@plt
// lit 00ab1176	ree@plt
// lit 00ab117b	free@plt
// lit 00ab1205	rodata
// lit 00ab12be	PATH@
// lit 00ab12c7	data
// lit 00ab12ea	utexattr_setprotocol@plt
// lit 00ab12f1	PATH@
// lit 00ab12f9	free@plt
// lit 00ab1306	PATH@
// lit 00ab133c	free@plt
// lit 00ab1361	data
// lit 00ab136c	ee@plt
// lit 00ab138c	ee@plt
// lit 00ab139b	PATH@
// lit 00ab1407	ta.1
// lit 00ab141d	data
// lit 00ab1438	s.1
// lit 00ab143b	PATH@
// lit 00ab1465	s.1
// lit 00ab1468	PATH@
// lit 00ab148b	s.1
// lit 00ab148e	PATH@
// lit 00ab14bd	s.1
// lit 00ab14c0	PATH@
// lit 00ab14da	ta.1
// lit 00ab14e7	ee@plt
// lit 00ab14e7	PATH@
// lit 00ab1574	PATH@
// lit 00ab15cc	rodata
// lit 00ab15ff	data
// lit 00ab1602	data
// lit 00ab1606	s.1
// lit 00ab161b	s.1
// lit 00ab161e	PATH@
// lit 00ab1641	s.1
// lit 00ab1644	PATH@
// lit 00ab1668	s.1
// lit 00ab166b	s.1
// lit 00ab166b	PATH@
// lit 00ab168b	s.1
// lit 00ab168e	PATH@
// lit 00ab16a2	d_cond_timedwait@plt

void sub_9b0e92(long param_1)

{
  code *pcVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  long *plVar4;
  undefined8 **ppuVar5;
  undefined **ppuVar6;
  undefined8 **ppuVar7;
  undefined1 local_2b8 [16];
  uint local_2a8 [2];
  long local_2a0;
  undefined8 *local_298;
  char *local_290;
  undefined8 **local_278;
  undefined8 *local_268;
  char *local_260;
  undefined8 **local_248;
  long local_238;
  char *local_230;
  undefined1 local_228 [16];
  long *local_218 [2];
  undefined4 local_208;
  undefined1 local_200 [16];
  undefined1 auStack_1f0 [24];
  uint local_1d8;
  undefined **local_1d0;
  undefined *local_1c8;
  undefined1 local_1c0 [24];
  undefined8 local_1a8;
  undefined8 uStack_1a0;
  undefined8 local_198;
  long *local_190;
  undefined8 *local_178;
  long local_170;
  undefined8 **local_158;
  undefined8 *local_148;
  undefined **local_140;
  undefined1 auStack_138 [16];
  undefined8 **local_128 [2];
  undefined4 local_118;
  undefined1 auStack_110 [16];
  undefined1 auStack_100 [32];
  uint local_e0 [2];
  long local_d8;
  undefined4 local_d0;
  undefined4 uStack_cc;
  undefined4 uStack_c8;
  uint uStack_c4;
  undefined8 local_c0;
  undefined *local_b8;
  long local_b0;
  undefined **local_98;
  uint local_88 [2];
  long local_80;
  undefined8 *local_78;
  undefined8 local_70;
  undefined8 *local_68 [4];
  undefined8 **local_48;
  undefined2 local_38 [4];
  
  if (((*(char *)(param_1 + 0x29) != '\0') && (DAT_03d3f168 == '\0')) && (DAT_03d6b7d8 != 2)) {
    if ((DAT_03d6b7d8 - 0x26U < 6) && ((0x29U >> (DAT_03d6b7d8 - 0x26U & 0x1f) & 1) != 0)) {
      DAT_03d3f168 = *(char *)(param_1 + 0x29);
      sub_15f45e(&local_238,1);
      puVar2 = (undefined8 *)sub_9b16ca(&local_238,param_1 + 0x18);
      local_70 = 0;
      local_78 = &DAT_0267c998;
      sub_16f272(&local_78,*puVar2,puVar2[1]);
      sub_16ee86(&local_238);
      local_48 = (undefined8 **)0x0;
      local_178 = &DAT_024cda30;
      local_170 = param_1;
      local_158 = &local_178;
      local_d0 = (**(code **)(DAT_03dcce20 + 0x80))(&DAT_03dcce20);
      uStack_cc = 0;
      uStack_c8 = 0;
      uStack_c4 = uStack_c4 & 0xffffff00;
      local_c0 = 0x100000001;
      (**(code **)(DAT_03dcce20 + 0x90))(&DAT_03dcce20);
      if (DAT_03d6b7d8 == 2) {
        if (DAT_03d6c960 == (long *)0x0) {
          if (local_48 != (undefined8 **)0x0) {
            local_238._0_4_ = 0;
            (*(code *)(*local_48)[2])();
          }
        }
        else {
          local_218[0] = (long *)0x0;
          if (local_158 != (undefined8 **)0x0) {
            if (local_158 == &local_178) {
              ppuVar5 = &local_178;
              plVar4 = &local_238;
              puVar2 = local_178;
            }
            else {
              plVar4 = (long *)0x0;
              puVar2 = *local_158;
              ppuVar5 = local_158;
            }
            local_218[0] = (long *)(*(code *)*puVar2)(ppuVar5,plVar4);
          }
          local_128[0] = (undefined8 **)0x0;
          if (local_48 != (undefined8 **)0x0) {
            if (local_48 == local_68) {
              ppuVar5 = local_68;
              ppuVar7 = &local_148;
            }
            else {
              local_68[0] = *local_48;
              ppuVar7 = (undefined8 **)0x0;
              ppuVar5 = local_48;
            }
            local_128[0] = (undefined8 **)(*(code *)*local_68[0])(ppuVar5,ppuVar7);
          }
          local_b8 = &DAT_03d6bcf8;
          if (DAT_03d6c960 == (long *)0x0) {
            _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
            pcVar1 = (code *)invalidInstructionException();
            (*pcVar1)();
          }
          (**(code **)(*DAT_03d6c960 + 0x10))
                    (DAT_03d6c960,&DAT_03d6bc58,&local_78,&local_238,&local_148,&local_b8,&local_d0)
          ;
          if (local_128[0] != (undefined8 **)0x0) {
            (*(code *)(*local_128[0])[4])(local_128[0],local_128[0] != &local_148);
            local_128[0] = (undefined8 **)0x0;
          }
          if (local_218[0] != (long *)0x0) {
            (**(code **)(*local_218[0] + 0x20))(local_218[0],local_218[0] != &local_238);
            local_218[0] = (long *)0x0;
          }
        }
      }
      else {
        sub_15f3cc(local_2b8,0);
        local_238._0_4_ = 0x80000009;
        local_230 = "profileid";
        sub_143474(local_2b8,&local_238,&DAT_03d6bc58);
        local_238._0_4_ = 0x8000000c;
        local_230 = "challengeids";
        sub_15e7f2(local_2b8,&local_238,&local_78);
        local_238._0_4_ = 0x8000000b;
        local_230 = "{0}/{1}/{2}";
        sub_c5aa84(&local_b8);
        local_298 = &DAT_0267ca40;
        local_290 = "ChallengesService";
        local_268 = &DAT_0267ca40;
        local_148 = &DAT_0267c968;
        local_260 = "GetProgression";
        local_140 = &local_b8;
        sub_153da8(local_88,&local_238,&local_148);
        if ((uint)local_b8 < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_b0 + -0x10);
        }
        sub_9e3038(&local_238);
        sub_c5aa00(local_e0);
        sub_14fd10(local_2a8,local_88);
        sub_f259c8(&local_148,2,local_e0,local_2a8);
        local_230 = (char *)CONCAT44(local_230._4_4_,local_140._0_4_);
        sub_152a9e(local_228,auStack_138);
        sub_152a9e(local_218,local_128);
        local_208 = local_118;
        sub_152a9e(local_200,auStack_110);
        sub_9e311a(auStack_1f0,auStack_100);
        sub_f24ec6(&local_148);
        if (local_2a8[0] < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_2a0 + -0x10);
        }
        if (local_e0[0] < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_d8 + -0x10);
        }
        sub_137fbb0(&local_148);
        if (local_1d8 < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_1d0 + -2);
        }
        local_1a8 = CONCAT44(uStack_cc,local_d0);
        uStack_1a0 = CONCAT44(uStack_c4,uStack_c8);
        local_1d8 = (uint)local_148;
        local_1d0 = local_140;
        local_1c8 = &DAT_03d6bcf8;
        local_198 = local_c0;
        local_128[0] = (undefined8 **)0x0;
        if (local_48 != (undefined8 **)0x0) {
          if (local_48 == local_68) {
            ppuVar5 = &local_148;
          }
          else {
            ppuVar5 = (undefined8 **)0x0;
          }
          local_128[0] = (undefined8 **)(*(code *)**local_48)(local_48,ppuVar5);
        }
        local_38[0] = 0;
        plVar4 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,local_38,0);
        local_98 = (undefined **)0x0;
        if (local_158 != (undefined8 **)0x0) {
          if (local_158 == &local_178) {
            ppuVar6 = &local_b8;
          }
          else {
            ppuVar6 = (undefined **)0x0;
          }
          local_98 = (undefined **)(*(code *)**local_158)(local_158,ppuVar6);
        }
        local_278 = (undefined8 **)0x0;
        ppuVar5 = (undefined8 **)_Znwm_plt(0x40);
        *ppuVar5 = (undefined8 *)&DAT_0251fb20;
        ppuVar5[6] = (undefined8 *)0x0;
        if (local_128[0] != (undefined8 **)0x0) {
          if (local_128[0] == &local_148) {
            ppuVar7 = ppuVar5 + 2;
          }
          else {
            ppuVar7 = (undefined8 **)0x0;
          }
          puVar2 = (undefined8 *)(*(code *)**local_128[0])(local_128[0],ppuVar7);
          ppuVar5[6] = puVar2;
        }
        local_248 = (undefined8 **)0x0;
        local_278 = ppuVar5;
        if (local_48 != (undefined8 **)0x0) {
          if (local_48 == local_68) {
            ppuVar5 = local_68;
            ppuVar7 = &local_268;
          }
          else {
            local_68[0] = *local_48;
            ppuVar7 = (undefined8 **)0x0;
            ppuVar5 = local_48;
          }
          local_248 = (undefined8 **)(*(code *)*local_68[0])(ppuVar5,ppuVar7);
        }
        sub_f245d4(plVar4,&local_b8,&local_298,&local_268);
        if (local_190 != (long *)0x0) {
          (**(code **)(*local_190 + 0x18))();
        }
        local_190 = plVar4;
        if (plVar4 != (long *)0x0) {
          (**(code **)(*plVar4 + 0x10))(plVar4);
        }
        if (local_248 != (undefined8 **)0x0) {
          (*(code *)(*local_248)[4])(local_248,local_248 != &local_268);
          local_248 = (undefined8 **)0x0;
        }
        if (local_278 != (undefined8 **)0x0) {
          (*(code *)(*local_278)[4])(local_278,local_278 != &local_298);
          local_278 = (undefined8 **)0x0;
        }
        if (local_98 != (undefined **)0x0) {
          (**(code **)(*local_98 + 0x20))(local_98,local_98 != &local_b8);
          local_98 = (undefined **)0x0;
        }
        sub_c43302(&local_238);
        if (local_128[0] != (undefined8 **)0x0) {
          (*(code *)(*local_128[0])[4])(local_128[0],local_128[0] != &local_148);
          local_128[0] = (undefined8 **)0x0;
        }
        if (local_190 != (long *)0x0) {
          (**(code **)(*local_190 + 0x18))();
        }
        local_190 = (long *)0x0;
        sub_9e33a2(local_1c0);
        if (local_1d8 < 0x40000000) {
          ppuVar6 = local_1d0 + -2;
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,ppuVar6);
        }
        sub_f24ec6(&local_238);
        if (local_88[0] < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_80 + -0x10);
        }
        sub_16ee86(local_2b8);
      }
      if (local_48 != (undefined8 **)0x0) {
        (*(code *)(*local_48)[4])(local_48,local_48 != local_68);
        local_48 = (undefined8 **)0x0;
      }
      if (local_158 != (undefined8 **)0x0) {
        (*(code *)(*local_158)[4])(local_158,local_158 != &local_178);
        local_158 = (undefined8 **)0x0;
      }
      sub_16ee86(&local_78);
    }
  }
  return;
}


