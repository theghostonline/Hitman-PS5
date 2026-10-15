// sub_c49776 @ 00d49776 size=713
// why: str:Loading Online Repository FAILED: Some repo resource references were not on the resource container or the actual data was malformed. @00d49979
// callers: sub_c480aa@00d480aa, sub_c484ba@00d484ba
// callees: sub_162ad4, sub_c49a40, sub_1468c6, sub_12127da, sub_12115aa, sub_130ba38, sub_120b9ee, sub_130bca0, sub_c495fe, sub_153da8, sub_14e69b, sub_14e70a, sub_c49d02, sub_146460
// lit 00d49783	ree@plt
// lit 00d497aa	data
// lit 00d497ed	data
// lit 00d497f5	data
// lit 00d497fd	data
// lit 00d4980e	free@plt
// lit 00d49813	.shstrtab
// lit 00d49842	e@plt
// lit 00d49846	data
// lit 00d498a8	ree@plt
// lit 00d498ac	@plt
// lit 00d498b0	free@plt
// lit 00d498e8	free@plt
// lit 00d49908	plt
// lit 00d49928	rodata
// lit 00d4992c	free@plt
// lit 00d4995f	PATH@
// lit 00d49968	PATH@
// lit 00d49979	Loading Online Repository FAILED: Some repo resource references were not on the resource container or the actual data was malformed.
// lit 00d49985	close@plt
// lit 00d499a6	Mounting dynamic resources in File '{0}' FAILED.
// lit 00d499b7	rodata
// lit 00d499c8	rodata
// lit 00d499c8	Mounting dynamic resources in File '{0}' FAILED.
// lit 00d499de	s.1
// lit 00d499e5	rodata
// lit 00d499ed	close@plt
// lit 00d49a2d	ree@plt

void sub_c49776(undefined8 param_1,undefined8 param_2)

{
  undefined4 uVar1;
  undefined8 uVar2;
  char cVar3;
  undefined8 uVar4;
  undefined1 *puVar5;
  undefined1 local_e8 [16];
  undefined8 local_d8;
  undefined1 local_c8 [16];
  ulong local_b8;
  undefined1 local_b0 [16];
  ulong local_a0;
  undefined1 local_98 [16];
  ulong local_88;
  undefined1 local_80 [16];
  undefined8 local_70;
  undefined8 local_68;
  char *local_60;
  undefined1 local_58 [16];
  ulong local_48;
  undefined8 local_38;
  
  local_e8._0_12_ = ZEXT812(0);
  local_e8._12_4_ = 0;
  local_d8 = 0x4000000000000000;
  local_38 = param_1;
  sub_162ad4();
  local_c8._0_12_ = ZEXT812(0);
  local_c8._12_4_ = 0;
  local_b8 = 0x4000000000000000;
  local_a0 = 0x4000000000000000;
  local_48 = 0x4000000000000000;
  local_80 = ZEXT416(0) << 0x20;
  local_88 = 0x4000000000000000;
  local_70 = 0x4000000000000000;
  local_b0 = local_c8;
  local_98 = local_c8;
  local_58 = local_c8;
  sub_c49a40(local_58,local_c8);
  sub_1468c6(local_80);
  if ((local_88 >> 0x3e == 0) && (local_98._0_8_ != 0)) {
    (**(code **)(*DAT_032f6d40 + 0x50))();
  }
  local_98._0_12_ = ZEXT812(0);
  local_98._12_4_ = 0;
  local_88 = 0x4000000000000000;
  if ((local_a0 < 0x4000000000000000) && (local_b0._0_8_ != 0)) {
    (**(code **)(*DAT_032f6d40 + 0x50))();
  }
  local_b0._0_12_ = ZEXT812(0);
  local_b0._12_4_ = 0;
  local_a0 = 0x4000000000000000;
  if ((local_b8 < 0x4000000000000000) && (local_c8._0_8_ != 0)) {
    (**(code **)(*DAT_032f6d40 + 0x50))();
  }
  if ((local_48 & 0x4000000000000000) == 0) {
    puVar5 = (undefined1 *)local_58._8_8_;
  }
  else {
    puVar5 = local_58 + (local_48 & 0xff) * 0x60;
  }
  cVar3 = sub_12127da(param_2,puVar5 + -0x60);
  if (cVar3 == '\0') {
    local_68 = CONCAT44(local_68._4_4_,0x80000030);
    local_60 = "Mounting dynamic resources in File \'{0}\' FAILED.";
    sub_153da8(local_c8);
    uVar1 = local_c8._0_4_;
    uVar2 = local_c8._8_8_;
    sub_c495fe(local_38,local_c8._0_4_,local_c8._8_8_,0x3ff);
    if ((uint)uVar1 < 0x40000000) {
      uVar4 = sub_14e69b();
      sub_14e70a(uVar4,uVar2 + -0x10);
    }
  }
  else {
    (**(code **)(*DAT_051e8fb8 + 0x50))();
    sub_12115aa(DAT_051e8c48,local_e8,local_58,1);
    sub_130ba38();
    if (-1 < (int)DAT_051fb810) {
      local_68 = *(undefined8 *)(*(long *)(DAT_051e8c48 + 8) + (ulong)DAT_051fb810 * 0x40);
      (**(code **)(DAT_051e8880 + 0x50))(local_c8,&DAT_051e8880);
      if (-1 < (int)local_c8._0_4_) {
        sub_120b9ee(DAT_051e8c48);
        local_c8._0_4_ = 0xffffffff;
      }
    }
    sub_130bca0(&local_68);
    if (((char)local_68 == '\0') && (DAT_03d6b4f0 != 0)) {
      sub_c495fe(local_38,0x80000084,
                 "Loading Online Repository FAILED: Some repo resource references were not on the resource container or the actual data was malformed."
                 ,0x3ff);
    }
  }
  sub_c49d02(local_58);
  sub_146460(local_e8);
  return;
}


