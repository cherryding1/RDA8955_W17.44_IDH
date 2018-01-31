XCPU_OVERLAY_BASE_FIRST = .;

OVERLAY XCPU_OVERLAY_BASE_FIRST : NOCROSSREFS AT (_compressed_code_load_start)
{
    /* .init section is used to list all init function which is only execlute once when power on*/
    .xcpu_overlay_init_func
    {
        xcpu_overlay_init_func_start = .;
        *(.init)

        *UCS2.o(.text .text.* .rodata .rodata.*)
        *FileSystemSrc.o(.text .text.* .rodata .rodata.*)
        *PwronCharger.o(.text .text.* .rodata .rodata.*)
        *SimDetection.o(.text .text.* .rodata .rodata.*)
        *NvramInterface.o(.text .text.* .rodata .rodata.*)
        *nvram_cust_pack.o(.text .text.* .rodata .rodata.*)
        *nvram_user_config.o(.text .text.* .rodata .rodata.*)

        /****************** GDI (MMI) ********************/
        /*  About 80Kytes*/
		/*
        *drm_file.o(.text .text.* .rodata .rodata.*)
        *gdi.o(.text .text.* .rodata .rodata.*)
        *gdi_2d_engine.o(.text .text.* .rodata .rodata.*)
        *gdi_animate.o(.text .text.* .rodata .rodata.*)
        *gdi_bytestream.o(.text .text.* .rodata .rodata.*)
        *gdi_font.o(.text .text.* .rodata .rodata.*)
        *gdi_image.o(.text .text.* .rodata .rodata.*)
        *gdi_image_bmp.o(.text .text.* .rodata .rodata.*)
        *gdi_image_decoder.o(.text .text.* .rodata .rodata.*)
        *gdi_image_dev_bmp.o(.text .text.* .rodata .rodata.*)
        *gdi_image_gif.o(.text .text.* .rodata .rodata.*)
        *gdi_image_hwgif.o(.text .text.* .rodata .rodata.*)
        *gdi_image_jpeg.o(.text .text.* .rodata .rodata.*)
        *gdi_image_m3d.o(.text .text.* .rodata .rodata.*)
        *gdi_image_png.o(.text .text.* .rodata .rodata.*)
        *gdi_image_sequence.o(.text .text.* .rodata .rodata.*)
        *gdi_image_svg.o(.text .text.* .rodata .rodata.*)
        *gdi_image_virtualfile.o(.text .text.* .rodata .rodata.*)
        *gdi_image_wbmp.o(.text .text.* .rodata .rodata.*)
        *gdi_layer.o(.text .text.* .rodata .rodata.*)
        *gdi_layermerge.o(.text .text.* .rodata .rodata.*)
        *gdi_lcd.o(.text .text.* .rodata .rodata.*)
        *gdi_mutex.o(.text .text.* .rodata .rodata.*)
        *gdi_primitive.o(.text .text.* .rodata .rodata.*)
        *gdi_util.o(.text .text.* .rodata .rodata.*)
        *gd_2d_engine.o(.text .text.* .rodata .rodata.*)
        *gd_primitive.o(.text .text.* .rodata .rodata.*)
        *gd_primitive16.o(.text .text.* .rodata .rodata.*)
        *gd_primitive24.o(.text .text.* .rodata .rodata.*)
        *gd_primitive32.o(.text .text.* .rodata .rodata.*)
        *gd_primitive8.o(.text .text.* .rodata .rodata.*)
        *gd_primitive_arm.o(.text .text.* .rodata .rodata.*)
		*/

        /********************stack********************************/
        /* about 250K Bytes  */

        *usim_prot.o(.text .text.* .rodata .rodata.*)
        *cb_util.o(.text .text.* .rodata .rodata.*)
        *mm_func.o(.text .text.* .rodata .rodata.*)
        *l1s_gctk_221.o(.text .text.* .rodata .rodata.*)
        *cb_trs.o(.text .text.* .rodata .rodata.*)
        *sim_proc.o(.text .text.* .rodata .rodata.*)
        *l1s_gncp_221.o(.text .text.* .rodata .rodata.*)
        *l1s_gnbp_221.o(.text .text.* .rodata .rodata.*)
        *rri_ctrs.o(.text .text.* .rodata .rodata.*)
        *cc_u01.o(.text .text.* .rodata .rodata.*)
        *l1s_pnbp_221.o(.text .text.* .rodata .rodata.*)
        *rri_ptrs.o(.text .text.* .rodata .rodata.*)
        *smr_proc.o(.text .text.* .rodata .rodata.*)
        *mmp_pip.o(.text .text.* .rodata .rodata.*)
        *rld_trsf.o(.text .text.* .rodata .rodata.*)
        *rrp_cond.o(.text .text.* .rodata .rodata.*)
        *mm_mmul.o(.text .text.* .rodata .rodata.*)
        *mmp_reg.o(.text .text.* .rodata .rodata.*)
        *sm_trs.o(.text .text.* .rodata .rodata.*)
        *rlu_gene.o(.text .text.* .rodata .rodata.*)
        *l1a_gnbp_221.o(.text .text.* .rodata .rodata.*)
        *snd_uti.o(.text .text.* .rodata .rodata.*)
        *mm_mmdl.o(.text .text.* .rodata .rodata.*)
        *rri_dec.o(.text .text.* .rodata .rodata.*)
        *rri_scan.o(.text .text.* .rodata .rodata.*)
        *l1a_pnbp_221.o(.text .text.* .rodata .rodata.*)
        *rri_rssi.o(.text .text.* .rodata .rodata.*)
        *cb_fsm.o(.text .text.* .rodata .rodata.*)
        *smc_proc.o(.text .text.* .rodata .rodata.*)
        *snd_rout.o(.text .text.* .rodata .rodata.*)
        *mm_out.o(.text .text.* .rodata .rodata.*)
        *ss_u1.o(.text .text.* .rodata .rodata.*)
        *rr_dec.o(.text .text.* .rodata .rodata.*)
        *ss_u01.o(.text .text.* .rodata .rodata.*)
        *rrp_acci.o(.text .text.* .rodata .rodata.*)
        *rr_out.o(.text .text.* .rodata .rodata.*)
        *l1a_gtls_221.o(.text .text.* .rodata .rodata.*)
        *rlu_tst.o(.text .text.* .rodata .rodata.*)
        *l1a_gncp_221.o(.text .text.* .rodata .rodata.*)


        /**********************WGUI****************************/
        /*  About 200K Bytes
        *wgui_softkeys.o(.text .text.* .rodata .rodata.*)
        *wgui_status_icons.o(.text .text.* .rodata .rodata.*)
        *wgui_menu_shortcuts.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_inputs.o(.text .text.* .rodata .rodata.*)
        *wgui_categories.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_email.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_fmgr.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_IMPS.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_PE.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_popup.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_sublcd.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_tab.o(.text .text.* .rodata .rodata.*)
        *wgui_datetime.o(.text .text.* .rodata .rodata.*)
        *wgui_dynamic_menuitems.o(.text .text.* .rodata .rodata.*)
        *wgui_dynamic_menus.o(.text .text.* .rodata .rodata.*)
        *wgui_fixed_menuitems.o(.text .text.* .rodata .rodata.*)
        *wgui_fixed_menus.o(.text .text.* .rodata .rodata.*)
        *wgui_inline_edit.o(.text .text.* .rodata .rodata.*)
        *wgui_inputs.o(.text .text.* .rodata .rodata.*)
        *wgui_tab.o(.text .text.* .rodata .rodata.*)
        *wgui_tab_bars.o(.text .text.* .rodata .rodata.*)
        *wgui_touch_screen.o(.text .text.* .rodata .rodata.*)
        *wgui_virtual_keyboard.o(.text .text.* .rodata .rodata.*)
        *wgui_draw_manager.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_util.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_idlescreen.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_MM.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_CM.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_MMS.o(.text .text.* .rodata .rodata.*)
        *wgui_calendar.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_mediamenu_screen.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_stopwatch.o(.text .text.* .rodata .rodata.*)
        *wgui_categories_multimedia.o(.text .text.* .rodata .rodata.*)
         */
        . = ALIGN(0x20);
        xcpu_overlay_init_func_end = .;
    }
}

#define FLASH_SIZE_OF_XCPU_OVERLAY_0  SIZEOF(.xcpu_overlay_init_func)
#define MAX_SIZE_OF_XCPU_OVERLAY_0 MAX(SIZEOF(.xcpu_overlay_init_func), 0)
_xcpu_overlay_first_max_section = MAX_SIZE_OF_XCPU_OVERLAY_0;
_xcpu_overlay_first_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_0;
XCPU_OVERLAY_BASE_SECOND = XCPU_OVERLAY_BASE_FIRST + _xcpu_overlay_first_max_section;
xcpu_overlay_flash_second = _compressed_code_load_start + _xcpu_overlay_first_flash_size;
OVERLAY XCPU_OVERLAY_BASE_SECOND : NOCROSSREFS AT (xcpu_overlay_flash_second)
{
    .overlay_2_section_1
    {
        xcpu_overlay_2_section_1_start = .;
        LONG(0)
        . = ALIGN(0x20);
        xcpu_overlay_2_section_1_end = .;
    }
    .overlay_2_section_2
    {
        xcpu_overlay_2_section_2_start = .;

        *CustImgRes.o(.text .text.* .rodata .rodata.*)
        *CustImgMap.o(.text .text.* .rodata .rodata.*)
        *CustImgMapExt.o(.text .text.* .rodata .rodata.*)
        *CustImgResExt.o(.text .text.* .rodata .rodata.*)
        *CustMenuRes.o(.text .text.* .rodata .rodata.*)
        *CustStrMap.o(.text .text.* .rodata .rodata.*)
        *FontRes.o(.text .text.* .rodata .rodata.*)
        *resource_audio.o(.text .text.* .rodata .rodata.*)
        *reverie*.o(.text .text.* .rodata .rodata.*)
        *revKeypad.o(.text .text.* .rodata .rodata.*)
        *revlib.o(.text .text.* .rodata .rodata.*)
        *RevDictionary.o(.text .text.* .rodata .rodata.*)


        *vpp_speech_asm.o(.text .text.* .rodata .rodata.*)              /*  130KBytes */
        *vpp_audiojpeg_dec.o(.text .text.* .rodata .rodata.*)       /*  70KBytes  */
        *vpp_audiojpegenc_asm.o(.text .text.* .rodata .rodata.*)  /*  30KBytes  */
        *vpp_audiojpegenc.o(.text .text.* .rodata .rodata.*)
        *vpp_audio_dec_tab_asm.o(.text .text.* .rodata .rodata.*)  


        . = ALIGN(0x20);
        xcpu_overlay_2_section_2_end = .;
    }
    .overlay_2_section_3
    {
        xcpu_overlay_2_section_3_start = .;
        LONG(0)
        . = ALIGN(0x20);
        xcpu_overlay_2_section_3_end = .;
    }
}
#define FLASH_SIZE_OF_XCPU_OVERLAY_1 0 + SIZEOF(.overlay_2_section_1) + SIZEOF(.overlay_2_section_2) + SIZEOF(.overlay_2_section_3)
#define MAX_SIZE_OF_XCPU_OVERLAY_1 MAX(SIZEOF(.overlay_2_section_1), MAX(SIZEOF(.overlay_2_section_2), MAX(SIZEOF(.overlay_2_section_3),0)))
_xcpu_overlay_second_max_section = MAX_SIZE_OF_XCPU_OVERLAY_1;
_xcpu_overlay_second_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_1;
_xcpu_overlay_section_2_1_size = SIZEOF(.overlay_2_section_1) ;
_xcpu_overlay_section_2_2_size = SIZEOF(.overlay_2_section_2) ;
_xcpu_overlay_section_2_3_size = SIZEOF(.overlay_2_section_3) ;
XCPU_OVERLAY_BASE_THIRD = XCPU_OVERLAY_BASE_SECOND + _xcpu_overlay_second_max_section;
xcpu_overlay_flash_third = xcpu_overlay_flash_second + _xcpu_overlay_second_flash_size;
OVERLAY XCPU_OVERLAY_BASE_THIRD : NOCROSSREFS AT (xcpu_overlay_flash_third)
{
    .overlay_3_section_1
    {
        xcpu_overlay_3_section_1_start = .;

        /********************* can not put here *********************/
        /*

        *audio_api.o(.text .text.* .rodata .rodata.*)
        *lily_stateMachine.o(.text .text.* .rodata .rodata.*)
        *mmc_audiompeg.o(.text .text.* .rodata .rodata.*)
        *mmc_dafrtpss.o(.text .text.* .rodata .rodata.*)
        *mmc_sbc_drv.o(.text .text.* .rodata .rodata.*)
        *mmf_vid_avi.o(.text .text.* .rodata .rodata.*)
        *cam_api.o(.text .text.* .rodata .rodata.*)
        *cam_ilm.o(.text .text.* .rodata .rodata.*)

        *FMRadioMsgHdlr.o(.text .text.* .rodata .rodata.*)
        *fm_radio.o(.text .text.* .rodata .rodata.*)
        *NetworkSetup.o(.text .text.* .rodata .rodata.*)
        *SettingSrc.o(.text .text.* .rodata .rodata.*)
        *ScheduledPowerOnOff.o(.text .text.* .rodata .rodata.*)
        *FactoryModeSrc.o(.text .text.* .rodata .rodata.*)
        *Restore.o(.text .text.* .rodata .rodata.*)

        */


        /************************ MDI **************************/
/*        *apbs.o(.text .text.* .rodata .rodata.*)*/
        *apfs.o(.text .text.* .rodata .rodata.*)
        *av_parsers.o(.text .text.* .rodata .rodata.*)
        *box_code_base.o(.text .text.* .rodata .rodata.*)
        *box_func.o(.text .text.* .rodata .rodata.*)
        *descriptors.o(.text .text.* .rodata .rodata.*)
        *fmrecord_api.o(.text .text.* .rodata .rodata.*)
        *getblk.o(.text .text.* .rodata .rodata.*)
        *gethdr.o(.text .text.* .rodata .rodata.*)
        *getpic.o(.text .text.* .rodata .rodata.*)
        *getvlc.o(.text .text.* .rodata .rodata.*)
        *idct.o(.text .text.* .rodata .rodata.*)
        *idct_mips.o(.text .text.* .rodata .rodata.*)
        *interpolate8x8.o(.text .text.* .rodata .rodata.*)
        *isom_intern.o(.text .text.* .rodata .rodata.*)
        *isom_read.o(.text .text.* .rodata .rodata.*)
        *lily_staMacLists.o(.text .text.* .rodata .rodata.*)
        *mbcoding.o(.text .text.* .rodata .rodata.*)
        *mbprediction.o(.text .text.* .rodata .rodata.*)
        *mmc_adpcm.o(.text .text.* .rodata .rodata.*)
/*        *mmc_audioamr.o(.text .text.* .rodata .rodata.*)  */
        *mmc_audiomp3stream.o(.text .text.* .rodata .rodata.*)
        *mmc_audiowav.o(.text .text.* .rodata .rodata.*)
        *mmc_audio_overlay.o(.text .text.* .rodata .rodata.*)
        *odf_codec.o(.text .text.* .rodata .rodata.*)
        *quant_h263.o(.text .text.* .rodata .rodata.*)
        *record_api.o(.text .text.* .rodata .rodata.*)
        *resamplesubs.o(.text .text.* .rodata .rodata.*)
        *wmaudio.o(.text .text.* .rodata .rodata.*)
        *wmaudio_parse.o(.text .text.* .rodata .rodata.*)
        *xvid.o(.text .text.* .rodata .rodata.*)
        *xvid_decraw.o(.text .text.* .rodata .rodata.*)

        /****************** AudioPlayer (MMI) ********************/
        *AudioPlayerMainScreen.o(.text .text.* .rodata .rodata.*)
        *AudioPlayerPlayList.o(.text .text.* .rodata .rodata.*)

        /****************** CameraApp (MMI) ********************/
        *CameraMenu.o(.text .text.* .rodata .rodata.*)
        *ImageViewer.o(.text .text.* .rodata .rodata.*)

        /**********************mp4*************************************/
        *Mmc_sndRec.o(.text .text.* .rodata .rodata.*)
        *mmc_vdochat.o(.text .text.* .rodata .rodata.*)
        *bitstream.o(.text .text.* .rodata .rodata.*)
        *mpeg4_decoder.o(.text .text.* .rodata .rodata.*)
        *mpeg4_idct.o(.text .text.* .rodata .rodata.*)
        *mpeg4_idct_mips.o(.text .text.* .rodata .rodata.*)
        *yuv2rgb.o(.text .text.* .rodata .rodata.*)
        *yuv2rgb_asm.o(.text .text.* .rodata .rodata.*)
        *yuv2rgb_blend.o(.text .text.* .rodata .rodata.*)
        *yuv420_bilinear_resample.o(.text .text.* .rodata .rodata.*)
        *zoom.o(.text .text.* .rodata .rodata.*)
        *h263_idct_mips.o(.text .text.* .rodata .rodata.*)
        *mmc_isom_video.o(.text .text.* .rodata .rodata.*)
        *mmc_previewvoc.o(.text .text.* .rodata .rodata.*)
        *mmc_video.o(.text .text.* .rodata .rodata.*)
        *vpp_preview.o(.text .text.* .rodata .rodata.*)
        *video_api.o(.text .text.* .rodata .rodata.*)

        /**********************MEDIA-camera*************************************/
        *cam_msg_handler.o(.text .text.* .rodata .rodata.*)
        *usbvideo_cam_if.o(.text .text.* .rodata .rodata.*)

        /************sound jpg enc****************/
        *vpp_audiojpegenc_asm_amrjpegenc.o(.text .text.* .rodata .rodata.*)
        *vpp_audiojpegenc_asm.o(.text .text.* .rodata .rodata.*)
        *vpp_audiojpegenc_asm_main.o(.text .text.* .rodata .rodata.*)
        *vpp_audiojpegenc_asm_mp3enc.o(.text .text.* .rodata .rodata.*)
        *yuvrotate.o(.text .text.* .rodata .rodata.*)

        /**********************video*************************************/
        *mmf_JpegAmrring_avi_enc.o(.text .text.* .rodata .rodata.*)
        *vpp_video_dec_asm.o(.text .text.* .rodata .rodata.*)
        *VdoPlyApp.o(.text .text.* .rodata .rodata.*)
        *VdoRecApp.o(.text .text.* .rodata .rodata.*)

        /****************** aac_dec_c ********************/
        *aac_buffers.o(.text .text.* .rodata .rodata.*)
        *aac_dec_risk_main.o(.text .text.* .rodata .rodata.*)
        *aac_dequant.o(.text .text.* .rodata .rodata.*)
        *aac_fft.o(.text .text.* .rodata .rodata.*)
        *aac_huffman.o(.text .text.* .rodata .rodata.*)
        *aac_hufftabs.o(.text .text.* .rodata .rodata.*)
        *aac_imdct.o(.text .text.* .rodata .rodata.*)
        *aac_stproc.o(.text .text.* .rodata .rodata.*)
        *aac_trigtabs.o(.text .text.* .rodata .rodata.*)
        *aacdec.o(.text .text.* .rodata .rodata.*)
        *aactabs.o(.text .text.* .rodata .rodata.*)
        *dct4.o(.text .text.* .rodata .rodata.*)
        *decelmnt.o(.text .text.* .rodata .rodata.*)
        *noiseless.o(.text .text.* .rodata .rodata.*)
        *pns.o(.text .text.* .rodata .rodata.*)
        *resample.o(.text .text.* .rodata .rodata.*)
        *timing.o(.text .text.* .rodata .rodata.*)
        *tns.o(.text .text.* .rodata .rodata.*)
        *trigtabs_fltgen.o(.text .text.* .rodata .rodata.*)
        *userint.o(.text .text.* .rodata .rodata.*)
        *filefmt.o(.text .text.* .rodata .rodata.*)

        /****************** amr_dec_c ********************/
        *amr_decode.o(.text .text.* .rodata .rodata.*)
        *amrfile.o(.text .text.* .rodata .rodata.*)
        *bitparse.o(.text .text.* .rodata .rodata.*)
        *frmbtord.o(.text .text.* .rodata .rodata.*)
        *interf_dec/o(.text .text.* .rodata .rodata.*)
        *sp_dec.o(.text .text.* .rodata .rodata.*)
        *mp3_fixed.o(.text .text.* .rodata .rodata.*)
        *mp3_frame.o(.text .text.* .rodata .rodata.*)
        *mp3_huffman.o(.text .text.* .rodata .rodata.*)
        *mp3_layer12.o(.text .text.* .rodata .rodata.*)
        *mp3_layer3.o(.text .text.* .rodata .rodata.*)
        *mmf_mp3_test.o(.text .text.* .rodata .rodata.*)
        *mp3_player.o(.text .text.* .rodata .rodata.*)
        *mp3_stream.o(.text .text.* .rodata .rodata.*)
        *mp3_synth.o(.text .text.* .rodata .rodata.*)
        *mp3_timer.o(.text .text.* .rodata .rodata.*)

        /****************** sbc_c ********************/
        *sbc.o(.text .text.* .rodata .rodata.*)
        *sbcenc.o(.text .text.* .rodata .rodata.*)

        /****************** funandgames ********************/
        *FunAndGamesSrc.o(.text .text.* .rodata .rodata.*)
        *Game.o(.text .text.* .rodata .rodata.*)
        *GameFramework.o(.text .text.* .rodata .rodata.*)
        *Panda.o(.text .text.* .rodata .rodata.*)
        *pushbox.o(.text .text.* .rodata .rodata.*)
        *Notepad.o(.text .text.* .rodata .rodata.*)
        *worldclock.o(.text .text.* .rodata .rodata.*)
        *unitconvertor.o(.text .text.* .rodata .rodata.*)
        *healthmenstural.o(.text .text.* .rodata .rodata.*)
        *bmi.o(.text .text.* .rodata .rodata.*)
        *Calorie.o(.text .text.* .rodata .rodata.*)
        *MultiPurposeStopwatch.o(.text .text.* .rodata .rodata.*)
        *TypicalStopwatch.o(.text .text.* .rodata .rodata.*)

        /************** EngineerMode & FactoryMode ****************/
        *EngineerModeDeviceSrc.o(.text .text.* .rodata .rodata.*)
        *EngineerModeDummySrc.o(.text .text.* .rodata .rodata.*)
        *EngineerModeSocketApp.o(.text .text.* .rodata .rodata.*)
        *EngineerModeSocketDemoApp.o(.text .text.* .rodata .rodata.*)
        *EngineerModeTracer.o(.text .text.* .rodata .rodata.*)
        *FactoryModeSrc.o(.text .text.* .rodata .rodata.*)

        /************** BBAT Diag & AT  ****************/
        *diag_*.o(.text .text.* .rodata .rodata.*)
        *at_cmd_*.o(.text .text.* .rodata .rodata.*)
        *at_cmdlist.o(.text .text.* .rodata .rodata.*)
        *at_command_table.o(.text .text.* .rodata .rodata.*)
 /*       *at_common.o(.text .text.* .rodata .rodata.*) */
        *at_mux.o(.text .text.* .rodata .rodata.*)
        *at_module.o(.text .text.* .rodata .rodata.*)
        *at_parser.o(.text .text.* .rodata .rodata.*)
        *at_shal.o(.text .text.* .rodata .rodata.*)
        *at_uart.o(.text .text.* .rodata .rodata.*)
        *at_utility.o(.text .text.* .rodata .rodata.*)
        *upg_write.o(.text .text.* .rodata .rodata.*)

        /****************** resource skins ********************/
        *resource_audply_skins.o(.text .text.* .rodata .rodata.*)
        *resource_calc_coordinates.o(.text .text.* .rodata .rodata.*)
        *resource_camera_skins.o(.text .text.* .rodata .rodata.*)
        *resource_fmradio_skins.o(.text .text.* .rodata .rodata.*)
        *resource_fmschedulerec_skins.o(.text .text.* .rodata .rodata.*)
        *resource_font_jtbl.o(.text .text.* .rodata .rodata.*)
        *resource_image_jtbl.o(.text .text.* .rodata .rodata.*)
        *resource_lang_pack_jtbl.o(.text .text.* .rodata .rodata.*)
        *resource_shortcuts.o(.text .text.* .rodata .rodata.*)
        *resource_str_jtbl.o(.text .text.* .rodata .rodata.*)
        *resource_video_skins.o(.text .text.* .rodata .rodata.*)
        *resource_world_clock_city.o(.text .text.* .rodata .rodata.*)
        *resource_audio_app.o(.text .text.* .rodata .rodata.*)

        /****************** FMRadio (MMI) ********************/
        *FMRadioMainScreen.o(.text .text.* .rodata .rodata.*)

        /****************** Setting (MMI) ********************/
        *SoundEffect.o(.text .text.* .rodata .rodata.*)
        /* *CallFixedDialList.o(.text .text.* .rodata .rodata.*) */

        /**********************WGUI****************************/
        *wgui_calendar.o(.text .text.* .rodata .rodata.*)

        . = ALIGN(0x20);
        xcpu_overlay_3_section_1_end = .;
    }
    .overlay_3_section_2
    {
        xcpu_overlay_3_section_2_start = .;
        /***********************  *WAP APP********************/
#include "libwapfiles.ld"

        . = ALIGN(0x20);
        xcpu_overlay_3_section_2_end = .;
    }
    .overlay_3_section_3
    {
        xcpu_overlay_3_section_3_start = .;
        LONG(0)
        . = ALIGN(0x20);
        xcpu_overlay_3_section_3_end = .;
    }
}
#define FLASH_SIZE_OF_XCPU_OVERLAY_2 0 + SIZEOF(.overlay_3_section_1) + SIZEOF(.overlay_3_section_2) + SIZEOF(.overlay_3_section_3)
#define MAX_SIZE_OF_XCPU_OVERLAY_2 MAX(SIZEOF(.overlay_3_section_1), MAX(SIZEOF(.overlay_3_section_2), MAX(SIZEOF(.overlay_3_section_3),0)))
_xcpu_overlay_section_3_1_size = SIZEOF(.overlay_3_section_1) ;
_xcpu_overlay_section_3_2_size = SIZEOF(.overlay_3_section_2) ;
_xcpu_overlay_section_3_3_size = SIZEOF(.overlay_3_section_3) ;
_xcpu_overlay_third_max_section = MAX_SIZE_OF_XCPU_OVERLAY_2;
_xcpu_overlay_third_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_2;

XCPU_OVERLAY_BASE_FOURTH = XCPU_OVERLAY_BASE_THIRD + _xcpu_overlay_third_max_section;
xcpu_overlay_flash_fourth = xcpu_overlay_flash_third + _xcpu_overlay_third_flash_size;
OVERLAY XCPU_OVERLAY_BASE_FOURTH : NOCROSSREFS AT (xcpu_overlay_flash_fourth)
{
    .overlay_4_section_1
    {
        . = ALIGN(0x20);
    }
    .overlay_4_section_2
    {
        . = ALIGN(0x20);
    }
}
#define FLASH_SIZE_OF_XCPU_OVERLAY_3 0 + SIZEOF(.overlay_4_section_1) + SIZEOF(.overlay_4_section_2)
#define MAX_SIZE_OF_XCPU_OVERLAY_3 MAX(SIZEOF(.overlay_4_section_1), MAX(SIZEOF(.overlay_4_section_2), 0))
_xcpu_overlay_fourth_max_section = MAX_SIZE_OF_XCPU_OVERLAY_3;
_xcpu_overlay_fourth_flash_size = FLASH_SIZE_OF_XCPU_OVERLAY_3;
_xcpu_overlay_total_ram_size =  _xcpu_overlay_first_max_section + _xcpu_overlay_second_max_section + _xcpu_overlay_third_max_section + _xcpu_overlay_fourth_max_section;
_xcpu_overlay_flash_end = xcpu_overlay_flash_fourth  + _xcpu_overlay_fourth_flash_size;

LD_ASSERT(ABSOLUTE(_xcpu_overlay_total_ram_size) < 0x120000, "FLAS", "_xcpu_overlay section overflow, should not big than 0x120000: Please check compress_code_section_3232.ld.")

