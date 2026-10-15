// sub_c2e2ba @ 00d2e2ba size=2084
// why: svc:ProfileService @00d2e4b6
// why: svc:ProfileService @00d2e4e7
// callers: 
// callees: sceRtcGetCurrentClock@plt, sceRtcGetWin32FileTime@plt, sub_15f45e, sub_c2fc4e, sub_146460, sub_15f3cc, sub_15e7f2, sub_c5aa84, sub_153da8, sub_14e69b, sub_14e70a, sub_9e3038, sub_c5aa00, sub_14fd10, sub_f259c8, sub_152a9e, sub_9e311a, sub_f24ec6, sub_137fbb0, _Znwm@plt, sub_f245d4, sub_c43302, sub_9e33a2, sub_16ee86, _ZSt19_Xbad_function_callv@plt
// lit 00d2e2c7	d_cond_timedwait@plt
// lit 00d2e339	ree@plt
// lit 00d2e352	PATH@
// lit 00d2e35d	s.1
// lit 00d2e385	PATH@
// lit 00d2e38d	s.1
// lit 00d2e3c0	ree@plt
// lit 00d2e3ed	PATH@
// lit 00d2e3f5	e@plt
// lit 00d2e3f5	PATH@
// lit 00d2e3f9	rodata
// lit 00d2e404	ree@plt
// lit 00d2e40d	PATH@
// lit 00d2e422	PATH@
// lit 00d2e468	entitlements
// lit 00d2e47f	rodata
// lit 00d2e47f	entitlements
// lit 00d2e48f	{0}/{1}/{2}
// lit 00d2e49f	rodata
// lit 00d2e49f	{0}/{1}/{2}
// lit 00d2e4b6	ProfileService
// lit 00d2e4e7	rodata
// lit 00d2e4e7	ProfileService
// lit 00d2e4eb	SetClientEntitlements
// lit 00d2e4f8	rodata
// lit 00d2e4ff	rodata
// lit 00d2e4ff	SetClientEntitlements
// lit 00d2e513	s.1
// lit 00d2e569	ree@plt
// lit 00d2e57f	data
// lit 00d2e584	free@plt
// lit 00d2e596	s.1
// lit 00d2e59b	free@plt
// lit 00d2e5ad	ree@plt
// lit 00d2e5b2	free@plt
// lit 00d2e63c	rodata
// lit 00d2e6f5	PATH@
// lit 00d2e6fe	data
// lit 00d2e721	utexattr_setprotocol@plt
// lit 00d2e728	PATH@
// lit 00d2e730	free@plt
// lit 00d2e73d	PATH@
// lit 00d2e773	free@plt
// lit 00d2e798	data
// lit 00d2e7a3	ee@plt
// lit 00d2e7c3	ee@plt
// lit 00d2e7d2	PATH@
// lit 00d2e83e	ta.1
// lit 00d2e854	data
// lit 00d2e86f	s.1
// lit 00d2e872	PATH@
// lit 00d2e895	s.1
// lit 00d2e898	PATH@
// lit 00d2e8bb	s.1
// lit 00d2e8be	PATH@
// lit 00d2e8ed	s.1
// lit 00d2e8f0	PATH@
// lit 00d2e90a	ta.1
// lit 00d2e917	ee@plt
// lit 00d2e917	PATH@
// lit 00d2e9a4	PATH@
// lit 00d2ea22	data
// lit 00d2ea25	s.1
// lit 00d2ea3a	s.1
// lit 00d2ea3d	PATH@
// lit 00d2ea60	s.1
// lit 00d2ea63	PATH@
// lit 00d2ea87	s.1
// lit 00d2ea8a	s.1
// lit 00d2ea8a	PATH@
// lit 00d2eab1	s.1
// lit 00d2eab4	s.1
// lit 00d2eab4	PATH@
// lit 00d2eac5	d_cond_timedwait@plt

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void sub_c2e2ba(undefined8 param_1,undefined8 *param_2,undefined8 *param_3)

{
  code *pcVar1;
  undefined8 *puVar2;
  undefined8 uVar3;
  long *plVar4;
  undefined8 **ppuVar5;
  undefined8 *puVar6;
  undefined **ppuVar7;
  undefined8 **ppuVar8;
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
  uint local_178 [2];
  long local_170;
  undefined8 *local_168;
  undefined **local_160;
  undefined1 auStack_158 [16];
  undefined8 **local_148 [2];
  undefined4 local_138;
  undefined1 auStack_130 [16];
  undefined1 auStack_120 [32];
  undefined4 local_100;
  undefined4 uStack_fc;
  undefined4 uStack_f8;
  uint uStack_f4;
  undefined8 local_f0;
  undefined *local_e8;
  long local_e0;
  undefined **local_c8;
  undefined8 *local_b8 [4];
  undefined8 **local_98;
  uint local_88 [2];
  long local_80;
  undefined1 local_78 [16];
  undefined8 *local_68 [4];
  undefined8 **local_48;
  undefined2 local_38 [4];
  
  if ((_DAT_03d6bbd8 & 0x3fffffff) != 0) {
    sceRtcGetCurrentClock_plt(&local_238,0);
    sceRtcGetWin32FileTime_plt(&local_238,&local_168);
    if (local_168 < DAT_03d6bc28) {
      sub_15f45e(local_78,0);
      (**(code **)(*DAT_051e8fb8 + 0x80))(&local_238);
      sub_c2fc4e(local_78,&local_238);
      sub_146460(&local_238);
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
        if (DAT_03d6d590 == (long *)0x0) {
          if (local_48 != (undefined8 **)0x0) {
            local_238._0_4_ = 0;
            (*(code *)(*local_48)[2])();
          }
        }
        else {
          local_218[0] = (long *)0x0;
          if (local_98 != (undefined8 **)0x0) {
            if (local_98 == local_b8) {
              ppuVar5 = local_b8;
              plVar4 = &local_238;
            }
            else {
              local_b8[0] = *local_98;
              plVar4 = (long *)0x0;
              ppuVar5 = local_98;
            }
            local_218[0] = (long *)(*(code *)*local_b8[0])(ppuVar5,plVar4);
          }
          local_148[0] = (undefined8 **)0x0;
          if (local_48 != (undefined8 **)0x0) {
            if (local_48 == local_68) {
              ppuVar5 = local_68;
              ppuVar8 = &local_168;
            }
            else {
              local_68[0] = *local_48;
              ppuVar8 = (undefined8 **)0x0;
              ppuVar5 = local_48;
            }
            local_148[0] = (undefined8 **)(*(code *)*local_68[0])(ppuVar5,ppuVar8);
          }
          local_e8 = &DAT_03d6bcf8;
          if (DAT_03d6d590 == (long *)0x0) {
            _ZSt19_Xbad_function_callv_plt();
                    /* WARNING: Does not return */
            pcVar1 = (code *)invalidInstructionException();
            (*pcVar1)();
          }
          (**(code **)(*DAT_03d6d590 + 0x10))
                    (DAT_03d6d590,local_78,&local_238,&local_168,&local_e8,&local_100);
          if (local_148[0] != (undefined8 **)0x0) {
            (*(code *)(*local_148[0])[4])(local_148[0],local_148[0] != &local_168);
            local_148[0] = (undefined8 **)0x0;
          }
          if (local_218[0] != (long *)0x0) {
            (**(code **)(*local_218[0] + 0x20))(local_218[0],local_218[0] != &local_238);
            local_218[0] = (long *)0x0;
          }
        }
      }
      else {
        sub_15f3cc(local_2b8,0);
        local_238._0_4_ = 0x8000000c;
        local_230 = "entitlements";
        sub_15e7f2(local_2b8,&local_238,local_78);
        local_238._0_4_ = 0x8000000b;
        local_230 = "{0}/{1}/{2}";
        sub_c5aa84(&local_e8);
        local_298 = &DAT_0267ca40;
        local_290 = "ProfileService";
        local_268 = &DAT_0267ca40;
        local_168 = &DAT_0267c968;
        local_260 = "SetClientEntitlements";
        local_160 = &local_e8;
        sub_153da8(local_88,&local_238,&local_168);
        if ((uint)local_e8 < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_e0 + -0x10);
        }
        sub_9e3038(&local_238);
        sub_c5aa00(local_178);
        sub_14fd10(local_2a8,local_88);
        sub_f259c8(&local_168,2,local_178,local_2a8);
        local_230 = (char *)CONCAT44(local_230._4_4_,local_160._0_4_);
        sub_152a9e(local_228,auStack_158);
        sub_152a9e(local_218,local_148);
        local_208 = local_138;
        sub_152a9e(local_200,auStack_130);
        sub_9e311a(auStack_1f0,auStack_120);
        sub_f24ec6(&local_168);
        if (local_2a8[0] < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_2a0 + -0x10);
        }
        if (local_178[0] < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_170 + -0x10);
        }
        sub_137fbb0(&local_168);
        if (local_1d8 < 0x40000000) {
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,local_1d0 + -2);
        }
        local_1a8 = CONCAT44(uStack_fc,local_100);
        uStack_1a0 = CONCAT44(uStack_f4,uStack_f8);
        local_1d8 = (uint)local_168;
        local_1d0 = local_160;
        local_1c8 = &DAT_03d6bcf8;
        local_198 = local_f0;
        local_148[0] = (undefined8 **)0x0;
        if (local_48 != (undefined8 **)0x0) {
          if (local_48 == local_68) {
            ppuVar5 = &local_168;
          }
          else {
            ppuVar5 = (undefined8 **)0x0;
          }
          local_148[0] = (undefined8 **)(*(code *)**local_48)(local_48,ppuVar5);
        }
        local_38[0] = 0;
        plVar4 = (long *)(**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,0x170,local_38,0);
        local_c8 = (undefined **)0x0;
        if (local_98 != (undefined8 **)0x0) {
          if (local_98 == local_b8) {
            ppuVar7 = &local_e8;
          }
          else {
            ppuVar7 = (undefined **)0x0;
          }
          local_c8 = (undefined **)(*(code *)**local_98)(local_98,ppuVar7);
        }
        local_278 = (undefined8 **)0x0;
        ppuVar5 = (undefined8 **)_Znwm_plt(0x40);
        *ppuVar5 = (undefined8 *)&DAT_025204e0;
        ppuVar5[6] = (undefined8 *)0x0;
        if (local_148[0] != (undefined8 **)0x0) {
          if (local_148[0] == &local_168) {
            ppuVar8 = ppuVar5 + 2;
          }
          else {
            ppuVar8 = (undefined8 **)0x0;
          }
          puVar6 = (undefined8 *)(*(code *)**local_148[0])(local_148[0],ppuVar8);
          ppuVar5[6] = puVar6;
        }
        local_248 = (undefined8 **)0x0;
        local_278 = ppuVar5;
        if (local_48 != (undefined8 **)0x0) {
          if (local_48 == local_68) {
            ppuVar5 = local_68;
            ppuVar8 = &local_268;
          }
          else {
            local_68[0] = *local_48;
            ppuVar8 = (undefined8 **)0x0;
            ppuVar5 = local_48;
          }
          local_248 = (undefined8 **)(*(code *)*local_68[0])(ppuVar5,ppuVar8);
        }
        sub_f245d4(plVar4,&local_e8,&local_298,&local_268);
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
        if (local_c8 != (undefined **)0x0) {
          (**(code **)(*local_c8 + 0x20))(local_c8,local_c8 != &local_e8);
          local_c8 = (undefined **)0x0;
        }
        sub_c43302(&local_238);
        if (local_148[0] != (undefined8 **)0x0) {
          (*(code *)(*local_148[0])[4])(local_148[0],local_148[0] != &local_168);
          local_148[0] = (undefined8 **)0x0;
        }
        if (local_190 != (long *)0x0) {
          (**(code **)(*local_190 + 0x18))();
        }
        local_190 = (long *)0x0;
        sub_9e33a2(local_1c0);
        if (local_1d8 < 0x40000000) {
          ppuVar7 = local_1d0 + -2;
          uVar3 = sub_14e69b();
          sub_14e70a(uVar3,ppuVar7);
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
      if (local_98 != (undefined8 **)0x0) {
        (*(code *)(*local_98)[4])(local_98,local_98 != local_b8);
        local_98 = (undefined8 **)0x0;
      }
      sub_16ee86(local_78);
    }
  }
  return;
}


