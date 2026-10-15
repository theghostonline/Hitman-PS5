// sub_c80ed8 @ 00d80ed8 size=164
// why: direct:0x00d80ed8
// callers: 
// callees: sceNpEntitlementAccessGetAddcontEntitlementInfoList@plt
// lit 00d80ee1	ta.1
// lit 00d80ee5	rodata
// lit 00d80eec	data
// lit 00d80ef1	lt
// lit 00d80ef8	plt
// lit 00d80f07	@plt
// lit 00d80f24	rodata
// lit 00d80f28	PATH@
// lit 00d80f2d	ree@plt
// lit 00d80f39	free@plt
// lit 00d80f41	lt
// lit 00d80f4b	PATH@
// lit 00d80f51	data
// lit 00d80f55	plt
// lit 00d80f60	@plt
// lit 00d80f66	ree@plt
// lit 00d80f6b	free@plt
// lit 00d80f71	ta.1

undefined4 sub_c80ed8(long param_1)

{
  long lVar1;
  int iVar2;
  undefined4 uVar3;
  ulong uVar4;
  undefined8 uVar5;
  undefined2 local_28 [4];
  
  lVar1 = *(long *)(param_1 + 8);
  iVar2 = sceNpEntitlementAccessGetAddcontEntitlementInfoList_plt
                    (*(undefined4 *)(param_1 + 0x10),*(undefined8 *)(lVar1 + 0xa0),0,
                     (uint *)(lVar1 + 0xa8));
  *(int *)(lVar1 + 0xb0) = iVar2;
  if ((iVar2 == 0) && (uVar4 = (ulong)*(uint *)(lVar1 + 0xa8), uVar4 != 0)) {
    local_28[0] = 0;
    uVar5 = (**(code **)(*DAT_032f6d40 + 0x40))(DAT_032f6d40,uVar4 * 0x1c,local_28,0);
    *(undefined8 *)(lVar1 + 0xa0) = uVar5;
    uVar3 = sceNpEntitlementAccessGetAddcontEntitlementInfoList_plt
                      (*(undefined4 *)(param_1 + 0x10),uVar5,*(undefined4 *)(lVar1 + 0xa8));
    *(undefined4 *)(lVar1 + 0xb0) = uVar3;
  }
  LOCK();
  uVar3 = *(undefined4 *)(lVar1 + 0xac);
  *(undefined4 *)(lVar1 + 0xac) = 2;
  UNLOCK();
  return uVar3;
}


