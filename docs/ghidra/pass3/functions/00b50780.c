// sub_a50780 @ 00b50780 size=116
// why: svc:HubPagesService @00b507d5
// callers: sub_a49938@00b49938
// callees: sub_a4b400, sub_143364, sub_14e69b, sub_14e70a, sub_a4b490
// lit 00b50789	ta.1
// lit 00b5079b	OnlineDisableAutoReconnect
// lit 00b507ae	rodata
// lit 00b507ae	OnlineDisableAutoReconnect
// lit 00b507b7	free@plt
// lit 00b507d5	HubPagesService
// lit 00b507e9	ta.1

void sub_a50780(undefined8 param_1)

{
  byte bVar1;
  byte bVar2;
  undefined8 uVar3;
  char *pcVar4;
  uint local_30 [2];
  char *local_28;
  
  bVar1 = sub_a4b400();
  local_30[0] = 0x8000001a;
  local_28 = "OnlineDisableAutoReconnect";
  bVar2 = sub_143364(local_30,0);
  if (local_30[0] < 0x40000000) {
    pcVar4 = local_28 + -0x10;
    uVar3 = sub_14e69b();
    sub_14e70a(uVar3,pcVar4);
  }
  if (((bVar1 ^ 1) & bVar2) != 0) {
    sub_a4b490(param_1);
  }
  return;
}


