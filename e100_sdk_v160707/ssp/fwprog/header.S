/**
 * header.S
 */

.text

.globl begin_bst_image
.globl end_bst_image
.globl begin_bld_image
.globl end_bld_image
.globl begin_pba_image
.globl end_pba_image
.globl begin_sys_image
.globl end_sys_image
.globl begin_dsp_image
.globl end_dsp_image
.globl begin_rom_image
.globl end_rom_image
.globl begin_lnx_image
.globl end_lnx_image
.globl begin_dtb_image
.globl end_dtb_image
.globl begin_rfs_image
.globl end_rfs_image


#ifdef __LINK_BST__
begin_bst_image:	.word	__BEGIN_BST_IMAGE__
#else
begin_bst_image:	.word	0x0
#endif

#ifdef __LINK_BLD__
begin_bld_image:	.word	__BEGIN_BLD_IMAGE__
#else
begin_bld_image:	.word	0x0
#endif

#ifdef __LINK_PBA__
begin_pba_image:	.word	__BEGIN_PBA_IMAGE__
#else
begin_pba_image:	.word	0x0
#endif

#ifdef __LINK_SYS__
begin_sys_image:	.word	__BEGIN_SYS_IMAGE__
#else
begin_sys_image:	.word	0x0
#endif

#ifdef __LINK_ROM__
begin_rom_image:	.word	__BEGIN_ROM_IMAGE__
#else
begin_rom_image:	.word	0x0
#endif

#ifdef __LINK_DSP__
begin_dsp_image:	.word	__BEGIN_DSP_IMAGE__
#else
begin_dsp_image:	.word	0x0
#endif

#ifdef __LINK_LNX__
begin_lnx_image:	.word	__BEGIN_LNX_IMAGE__
#else
begin_lnx_image:	.word	0x0
#endif

#ifdef __LINK_DTB__
begin_dtb_image:	.word	__BEGIN_DTB_IMAGE__
#else
begin_dtb_image:	.word	0x0
#endif

#ifdef __LINK_RFS__
begin_rfs_image:	.word	__BEGIN_RFS_IMAGE__
#else
begin_rfs_image:	.word	0x0
#endif

// end image

#ifdef __LINK_BST__
end_bst_image:	.word	__END_BST_IMAGE__
#else
end_bst_image:	.word	0x0
#endif

#ifdef __LINK_BLD__
end_bld_image:	.word	__END_BLD_IMAGE__
#else
end_bld_image:	.word	0x0
#endif

#ifdef __LINK_PBA__
end_pba_image:	.word	__END_PBA_IMAGE__
#else
end_pba_image:	.word	0x0
#endif

#ifdef __LINK_SYS__
end_sys_image:	.word	__END_SYS_IMAGE__
#else
end_sys_image:	.word	0x0
#endif

#ifdef __LINK_ROM__
end_rom_image:	.word	__END_ROM_IMAGE__
#else
end_rom_image:	.word	0x0
#endif

#ifdef __LINK_DSP__
end_dsp_image:	.word	__END_DSP_IMAGE__
#else
end_dsp_image:	.word	0x0
#endif

#ifdef __LINK_LNX__
end_lnx_image:	.word	__END_LNX_IMAGE__
#else
end_lnx_image:	.word	0x0
#endif

#ifdef __LINK_DTB__
end_dtb_image:	.word	__END_DTB_IMAGE__
#else
end_dtb_image:	.word	0x0
#endif

#ifdef __LINK_RFS__
end_rfs_image:	.word	__END_RFS_IMAGE__
#else
end_rfs_image:	.word	0x0
#endif

