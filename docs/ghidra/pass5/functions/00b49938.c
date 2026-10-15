// sub_a49938 @ 00b49938 size=539
// why: svc:HubPagesService @00b49aae
// callers: 
// callees: sub_14fd10, sub_a6b1fc, sub_a6ba8e, sub_a4de14, sub_9e5ada, sub_a4f12a, sub_143364, sub_14e69b, sub_14e70a, sub_a4b9de, sceKernelReadTsc@plt, sceKernelGetTscFrequency@plt, sub_a50780
// lit 00b4996d	PATH@
// lit 00b49973	PATH@
// lit 00b49983	rodata
// lit 00b49988	data
// lit 00b49994	ree@plt
// lit 00b499c4	free@plt
// lit 00b499cd	e@plt
// lit 00b499de	e@plt
// lit 00b499fa	ree@plt
// lit 00b49a00	ee@plt
// lit 00b49a06	@plt
// lit 00b49a10	lt
// lit 00b49a22	PATH@
// lit 00b49a2b	free@plt
// lit 00b49a41	free@plt
// lit 00b49a60	chReturnFromMain@plt
// lit 00b49a6f	OnlineDisableAutoReconnect
// lit 00b49a76	PATH@
// lit 00b49a8a	rodata
// lit 00b49a8a	OnlineDisableAutoReconnect
// lit 00b49aae	HubPagesService
// lit 00b49ad1	free@plt
// lit 00b49ae7	free@plt

void sub_a49938(long param_1,uint param_2,undefined8 param_3,undefined4 param_4,undefined8 param_5)

{
  char cVar1;
  long lVar2;
  undefined8 uVar3;
  char *pcVar4;
  undefined8 local_70;
  undefined2 local_68;
  undefined1 local_66;
  uint local_60 [2];
  long local_58;
  undefined4 local_50;
  uint local_48 [2];
  char *local_40;
  float local_34;
  
  local_70 = 0x300000008;
  local_68 = 0;
  local_66 = 0;
  sub_14fd10(local_60,param_3);
  local_50 = 0xbf800000;
  sub_a6b1fc(&DAT_03d451c0,8);
  if ((param_2 < 0x27) && ((0x4000000016U >> ((ulong)param_2 & 0x3f) & 1) != 0)) {
    local_50 = 0x3f800000;
  }
  sub_a6ba8e(&DAT_03d451c0,&local_70);
  if ((int)param_2 < 0x26) {
    switch(param_2) {
    case 1:
      sub_a4de14(param_1,param_4,param_5);
      break;
    case 2:
      goto switchD_00b499e5_caseD_2;
    case 4:
      *(undefined4 *)(param_1 + 0x48) = 0;
      local_48[0] = 0x8000001a;
      local_40 = "OnlineDisableAutoReconnect";
      cVar1 = sub_143364(local_48,0);
      if (local_48[0] < 0x40000000) {
        pcVar4 = local_40 + -0x10;
        uVar3 = sub_14e69b();
        sub_14e70a(uVar3,pcVar4);
      }
      if (cVar1 == '\0') {
        sub_a4f12a(param_1);
      }
      sub_a4b9de(param_1,param_4,param_5);
      if (*(int *)(param_1 + 0x1c) != -1) {
        sub_9e5ada(&DAT_03d45260);
        *(undefined4 *)(param_1 + 0x1c) = 0xffffffff;
      }
      break;
    case 5:
      lVar2 = sceKernelReadTsc_plt();
      local_34 = (float)lVar2;
      lVar2 = sceKernelGetTscFrequency_plt();
      *(float *)(param_1 + 0x48) = local_34 / (float)lVar2;
LAB_00b49b1b:
      sub_a50780(param_1);
    }
  }
  else {
    if (param_2 != 0x26) {
      if (((param_2 != 0x27) && (param_2 != 0x29)) || (DAT_03d6b7dc == 0x2b))
      goto switchD_00b499e5_caseD_3;
      goto LAB_00b49b1b;
    }
switchD_00b499e5_caseD_2:
    *(undefined4 *)(param_1 + 0x48) = 0;
    if (*(int *)(param_1 + 0x1c) != -1) {
      sub_9e5ada(&DAT_03d45260);
      *(undefined4 *)(param_1 + 0x1c) = 0xffffffff;
    }
    sub_a4f12a(param_1);
    (**(code **)(DAT_03d6b7c0 + 0x110))(&DAT_03d6b7c0);
  }
switchD_00b499e5_caseD_3:
  if (local_60[0] < 0x40000000) {
    uVar3 = sub_14e69b();
    sub_14e70a(uVar3,local_58 + -0x10);
  }
  return;
}


