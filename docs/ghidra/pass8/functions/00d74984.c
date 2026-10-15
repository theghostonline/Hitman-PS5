// sub_c74984 @ 00d74984 size=229
// why: svc:HubPagesService @00d74a3d
// callers: 
// callees: sub_c51c94, sceKernelReadTsc@plt, sceKernelGetTscFrequency@plt, sub_c75444, sub_c754ac, sub_143364, sub_14e69b, sub_14e70a, sub_16ee86
// lit 00d7498b	s.1
// lit 00d7498f	E:/p4/compile_del05/del05/code/_out/ps5/Retail/engine.elf
// lit 00d74995	rodata
// lit 00d74995	PATH@
// lit 00d749ad	texattr_init@plt
// lit 00d74a05	OnlineDisableAutoReconnect
// lit 00d74a18	rodata
// lit 00d74a18	OnlineDisableAutoReconnect
// lit 00d74a21	free@plt
// lit 00d74a3d	HubPagesService
// lit 00d74a50	rodata
// lit 00d74a50	PATH@
// lit 00d74a60	s.1

void sub_c74984(long param_1)

{
  undefined1 uVar1;
  long lVar2;
  undefined8 uVar3;
  char *pcVar4;
  undefined8 *local_30;
  char *local_28;
  float local_1c;
  
  *(undefined2 *)(param_1 + 0xc) = 0x100;
  *(undefined4 *)(param_1 + 8) = 0;
  (**(code **)(DAT_03d6b7c0 + 0x138))(&DAT_03d6b7c0);
  sub_c51c94();
  DAT_03d6bcf0 = 5.0;
  lVar2 = sceKernelReadTsc_plt();
  local_1c = (float)lVar2;
  lVar2 = sceKernelGetTscFrequency_plt();
  DAT_03d6bcec = local_1c / (float)lVar2 + DAT_03d6bcf0;
  sub_c75444(param_1);
  sub_c754ac();
  local_30 = (undefined8 *)CONCAT44(local_30._4_4_,0x8000001a);
  local_28 = "OnlineDisableAutoReconnect";
  uVar1 = sub_143364(&local_30,0);
  *(undefined1 *)(param_1 + 0x1c) = uVar1;
  if ((uint)local_30 < 0x40000000) {
    pcVar4 = local_28 + -0x10;
    uVar3 = sub_14e69b();
    sub_14e70a(uVar3,pcVar4);
  }
  local_28 = (char *)0x0;
  local_30 = &DAT_0267c998;
  sub_16ee86();
  return;
}


