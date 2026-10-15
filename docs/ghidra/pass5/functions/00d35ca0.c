// sub_c35ca0 @ 00d35ca0 size=1437
// why: svc:HubPagesService @00d35f30
// why: svc:HubPagesService @00d35f3d
// why: svc:HubPagesService @00d35d78
// callers: sub_c412c4@00d412c4, sub_c35ad6@00d35ad6
// callees: sub_c3623e, sub_143364, sub_14e69b, sub_14e70a, sub_c3641c, sub_c36368, sub_15eb42, sub_15f4f0, strcmp@plt, sub_15d7dc, sub_c367ec, sceRtcGetCurrentClockLocalTime@plt, sceRtcGetWin32FileTime@plt, sub_c373d0
// lit 00d35cad	ree@plt
// lit 00d35cb8	thread_cond_init@plt
// lit 00d35cd3	thread_cond_init@plt
// lit 00d35ce5	@plt
// lit 00d35cfb	PATH@
// lit 00d35d05	read_cond_broadcast@plt
// lit 00d35d05	PATH@
// lit 00d35d14	hread_cond_broadcast@plt
// lit 00d35d14	PATH@
// lit 00d35d1f	ead_cond_broadcast@plt
// lit 00d35d1f	PATH@
// lit 00d35d2a	nd_broadcast@plt
// lit 00d35d2a	PATH@
// lit 00d35d3e	OnlineDisableAutoReconnect
// lit 00d35d52	rodata
// lit 00d35d52	OnlineDisableAutoReconnect
// lit 00d35d78	HubPagesService
// lit 00d35d83	ond_broadcast@plt
// lit 00d35d97	s.1
// lit 00d35da4	PATH@
// lit 00d35dad	data
// lit 00d35db0	ree@plt
// lit 00d35de1	.shstrtab
// lit 00d35de6	e@plt
// lit 00d35e14	_destroy@plt
// lit 00d35e1c	plt
// lit 00d35e24	lt
// lit 00d35e3f	ex_lock@plt
// lit 00d35e50	s.1
// lit 00d35e58	ta.1
// lit 00d35e63	data
// lit 00d35e6f	ex_lock@plt
// lit 00d35e7c	s.1
// lit 00d35e84	ta.1
// lit 00d35e93	data
// lit 00d35eb5	e@plt
// lit 00d35ecb	s.1
// lit 00d35ee2	Name
// lit 00d35ef7	Name
// lit 00d35f13	OnlineDisableAutoReconnect
// lit 00d35f17	ContractFailed
// lit 00d35f1e	OnlineDisableAutoReconnect
// lit 00d35f30	HubPagesService
// lit 00d35f3d	HubPagesService
// lit 00d35f61	PATH@
// lit 00d35f7a	t_env@plt
// lit 00d35f90	s.1
// lit 00d35f99	plt
// lit 00d35fa1	lt
// lit 00d35fa9	@plt
// lit 00d35fad	atexit@plt
// lit 00d35fda	@plt
// lit 00d35fde	free@plt
// lit 00d35fde	ta.1
// lit 00d35fe8	free@plt
// lit 00d35fe8	rodata
// lit 00d35fed	free@plt
// lit 00d35ff1	free@plt
// lit 00d35ff8	free@plt
// lit 00d3600f	free@plt
// lit 00d3600f	data
// lit 00d36026	atexit@plt
// lit 00d36031	atexit@plt
// lit 00d3603f	t_env@plt
// lit 00d3604c	63580
// lit 00d3609b	hread_mutex_unlock@plt
// lit 00d360cf	hread_mutex_unlock@plt
// lit 00d360df	ond_broadcast@plt
// lit 00d360df	PATH@
// lit 00d360e8	ad_cond_broadcast@plt
// lit 00d360e8	PATH@
// lit 00d360f4	free@plt
// lit 00d360f7	free@plt
// lit 00d360fb	_cond_broadcast@plt
// lit 00d360fb	PATH@
// lit 00d36112	d_cond_broadcast@plt
// lit 00d36112	PATH@
// lit 00d3611d	thread_cond_init@plt
// lit 00d36138	read_cond_destroy@plt
// lit 00d3615d	cond_broadcast@plt
// lit 00d36165	PATH@
// lit 00d36170	s.1
// lit 00d36198	PATH@
// lit 00d361a3	s.1
// lit 00d361eb	s.1
// lit 00d36200	s.1
// lit 00d36203	PATH@
// lit 00d3622a	s.1
// lit 00d3622d	PATH@

void sub_c35ca0(long param_1,undefined8 *param_2,undefined8 *param_3,uint param_4)

{
  double dVar1;
  uint uVar2;
  undefined8 *puVar3;
  char cVar4;
  short sVar5;
  int iVar6;
  long *plVar7;
  ulong uVar8;
  code *pcVar9;
  undefined8 *puVar10;
  ushort uVar11;
  long lVar12;
  long lVar13;
  char *pcVar14;
  uint *puVar15;
  bool bVar16;
  bool bVar17;
  undefined8 uVar18;
  long local_f8 [4];
  long *local_d8;
  long local_c8 [4];
  long *local_a8;
  uint *local_90;
  undefined8 *local_88;
  undefined8 *local_80;
  undefined4 local_78;
  undefined4 uStack_74;
  undefined *local_70;
  int local_64;
  long local_60;
  uint local_58 [2];
  char *local_50;
  uint local_48;
  uint local_44;
  char *local_40;
  long local_38;
  
  if (*(double *)(param_1 + 0x238) < 0.0) {
    uVar18 = sub_c3623e();
    *(undefined8 *)(param_1 + 0x238) = uVar18;
  }
  bVar17 = false;
  if (((ulong)DAT_03d6b7d8 < 0x2a) &&
     (bVar17 = false, (0x24000000004U >> ((ulong)DAT_03d6b7d8 & 0x3f) & 1) != 0)) {
    bVar17 = DAT_03d6be08 == '\0';
  }
  if ((((*(char *)(param_1 + 0x269) == '\0') && (*(char *)(param_1 + 0x268) == '\0')) &&
      (*(char *)(param_1 + 0x26a) == '\0')) && ((bool)(bVar17 & *(char *)(param_1 + 0x270) == '\0'))
     ) {
    local_58[0] = 0x8000001a;
    local_50 = "OnlineDisableAutoReconnect";
    cVar4 = sub_143364(local_58,0);
    if (local_58[0] < 0x40000000) {
      local_38 = CONCAT71(local_38._1_7_,cVar4);
      pcVar14 = local_50 + -0x10;
      uVar18 = sub_14e69b();
      sub_14e70a(uVar18,pcVar14);
      cVar4 = (char)local_38;
    }
    if (cVar4 == '\0') {
      if (*(char *)(param_1 + 0x26f) != '\0') {
        uVar8 = *(ulong *)(param_1 + 0x1b8);
        bVar17 = (uVar8 >> 0x3e & 1) == 0;
        puVar15 = (uint *)(param_1 + 0x1a8);
        if (bVar17) {
          puVar15 = *(uint **)(param_1 + 0x1a8);
        }
        local_90 = (uint *)(param_1 + 0x1a8 + (uVar8 & 0xff) * 4);
        if (bVar17) {
          local_90 = *(uint **)(param_1 + 0x1b0);
        }
        local_44 = param_4;
        if (puVar15 != local_90) {
          local_60 = param_1 + 0x1e0;
          local_38 = param_1 + 0x200;
          local_88 = (undefined8 *)(param_1 + 0xe8);
          local_80 = param_2;
          do {
            puVar3 = local_88;
            uVar2 = *puVar15;
            lVar12 = *(long *)(param_1 + 0x1f8);
            lVar13 = local_38;
            local_48 = uVar2;
            if (lVar12 != 0) {
              do {
                if (*(uint *)(lVar12 + 0x20) < uVar2) {
                  plVar7 = (long *)(lVar12 + 0x18);
                }
                else {
                  if (*(uint *)(lVar12 + 0x20) <= uVar2) {
                    if (lVar12 + 0x20 != local_38) {
                      uVar18 = sub_c36368(local_60,&local_48);
                      local_78 = 0x80000004;
                      local_70 = &DAT_01d57258;
                      uVar18 = sub_15eb42(uVar18,&local_78);
                      sub_15f4f0(local_58,uVar18,&DAT_01d8bb7b);
                      local_40 = local_50;
                      iVar6 = strcmp_plt(local_50,"ContractFailed");
                      if (local_58[0] < 0x40000000) {
                        local_40 = local_40 + -0x10;
                        local_64 = iVar6;
                        uVar18 = sub_14e69b();
                        sub_14e70a(uVar18,local_40);
                        iVar6 = local_64;
                      }
                      if (iVar6 == 0) {
                        local_40 = (char *)sub_c36368(local_60,&local_48);
                        *(undefined2 *)(param_1 + 0x100) = 0;
                        uVar8 = *(ulong *)(param_1 + 0xf8);
                        if ((uVar8 & 0x4000000000000000) == 0) {
                          uVar11 = (ushort)((ulong)(*(long *)(param_1 + 0xf0) -
                                                   *(long *)(param_1 + 0xe8)) >> 5);
                        }
                        else {
                          uVar11 = (ushort)uVar8 & 0xff;
                        }
                        *(ushort *)(param_1 + 0x102) = uVar11;
                        if (0 < (short)uVar11) {
                          iVar6 = 0;
                          while( true ) {
                            lVar12 = (long)(short)iVar6;
                            puVar10 = puVar3;
                            if ((uVar8 & 0x4000000000000000) == 0) {
                              puVar10 = (undefined8 *)*puVar3;
                            }
                            if (puVar10[lVar12 * 4 + 3] == 0) {
                              (*(code *)puVar10[lVar12 * 4 + 2])(uVar2,local_40);
                            }
                            else {
                              plVar7 = (long *)(puVar10[lVar12 * 4 + 3] + puVar10[lVar12 * 4 + 1]);
                              pcVar9 = (code *)puVar10[lVar12 * 4];
                              if (((ulong)pcVar9 & 1) != 0) {
                                pcVar9 = *(code **)(pcVar9 + *plVar7 + -1);
                              }
                              (*pcVar9)(plVar7,uVar2,local_40);
                            }
                            iVar6 = *(int *)(param_1 + 0x100) + 1;
                            *(short *)(param_1 + 0x100) = (short)iVar6;
                            sVar5 = *(short *)(param_1 + 0x102) + -1;
                            *(short *)(param_1 + 0x102) = sVar5;
                            if (sVar5 < 1) break;
                            uVar8 = *(ulong *)(param_1 + 0xf8);
                          }
                        }
                        *(undefined2 *)(param_1 + 0x100) = 0xfffe;
                      }
                    }
                    break;
                  }
                  plVar7 = (long *)(lVar12 + 0x10);
                }
                lVar12 = *plVar7;
              } while (lVar12 != 0);
              lVar12 = *(long *)(param_1 + 0x1f8);
              while (lVar13 = local_38, lVar12 != 0) {
                if (*(uint *)(lVar12 + 0x20) < uVar2) {
                  plVar7 = (long *)(lVar12 + 0x18);
                }
                else {
                  if (*(uint *)(lVar12 + 0x20) <= uVar2) {
                    lVar13 = lVar12 + 0x20;
                    break;
                  }
                  plVar7 = (long *)(lVar12 + 0x10);
                }
                lVar12 = *plVar7;
              }
            }
            if (local_38 != lVar13) {
              sub_c3641c(local_58,local_60);
            }
            puVar15 = puVar15 + 1;
          } while (puVar15 != local_90);
          uVar8 = *(ulong *)(param_1 + 0x1b8);
          param_2 = local_80;
        }
        param_4 = local_44;
        if ((uVar8 & 0x4000000000000000) == 0) {
          *(undefined8 *)(param_1 + 0x1b0) = *(undefined8 *)(param_1 + 0x1a8);
        }
        else {
          *(ulong *)(param_1 + 0x1b8) = uVar8 & 0xffffffffffffff00;
        }
        sub_15d7dc(param_1 + 0x220);
        if ((*(ulong *)(param_1 + 0x230) & 0x4000000000000000) == 0) {
          *(undefined8 *)(param_1 + 0x228) = *(undefined8 *)(param_1 + 0x220);
        }
        else {
          *(ulong *)(param_1 + 0x230) = *(ulong *)(param_1 + 0x230) & 0xffffffffffffff00;
        }
        *(undefined1 *)(param_1 + 0x26f) = 0;
      }
    }
    else if (*(char *)(param_1 + 0x26f) != '\0') goto LAB_00d35d97;
    bVar16 = true;
    bVar17 = true;
    if (*(char *)(param_1 + 0x26b) == '\0') {
      bVar17 = *(char *)(param_1 + 0x26d) != '\0';
    }
    sub_c367ec(param_1,bVar17);
    if (*(char *)(param_1 + 0x26c) == '\0') {
      dVar1 = *(double *)(param_1 + 0x238);
      lVar12 = *(long *)(param_1 + 0x250);
      sceRtcGetCurrentClockLocalTime_plt(local_58);
      sceRtcGetWin32FileTime_plt(local_58,&local_78);
      bVar16 = (ulong)((long)((float)dVar1 * 1e+07) + lVar12) < CONCAT44(uStack_74,local_78);
    }
    *(bool *)(param_1 + 0x26e) = bVar16;
    local_a8 = (long *)0x0;
    puVar3 = (undefined8 *)param_2[4];
    if (puVar3 != (undefined8 *)0x0) {
      if (puVar3 == param_2) {
        puVar10 = (undefined8 *)*puVar3;
        plVar7 = local_c8;
      }
      else {
        puVar10 = (undefined8 *)*puVar3;
        plVar7 = (long *)0x0;
      }
      local_a8 = (long *)(*(code *)*puVar10)(puVar3,plVar7);
    }
    local_d8 = (long *)0x0;
    puVar3 = (undefined8 *)param_3[4];
    if (puVar3 != (undefined8 *)0x0) {
      if (puVar3 == param_3) {
        puVar10 = (undefined8 *)*puVar3;
        plVar7 = local_f8;
      }
      else {
        puVar10 = (undefined8 *)*puVar3;
        plVar7 = (long *)0x0;
      }
      local_d8 = (long *)(*(code *)*puVar10)(puVar3,plVar7);
    }
    sub_c373d0(param_1,local_c8,local_f8,param_4 & 0xff);
    if (local_d8 != (long *)0x0) {
      (**(code **)(*local_d8 + 0x20))(local_d8,local_d8 != local_f8);
      local_d8 = (long *)0x0;
    }
    if (local_a8 != (long *)0x0) {
      (**(code **)(*local_a8 + 0x20))(local_a8,local_a8 != local_c8);
    }
  }
  else {
LAB_00d35d97:
    if ((long *)param_3[4] != (long *)0x0) {
      local_58[0] = 0;
      (**(code **)(*(long *)param_3[4] + 0x10))();
    }
  }
  return;
}


